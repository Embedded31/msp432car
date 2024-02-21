# Project name
PROJECT = msp432car

# -- Embedded Toolchain --
# Modify the ARM_DIR in order to point to the base directory of the ARM toolchain in your system
ARM_DIR = /Applications/ArmGNUToolchain/13.2.Rel1/arm-none-eabi
CC = $(ARM_DIR)/bin/arm-none-eabi-gcc
LD = $(ARM_DIR)/bin/arm-none-eabi-ld
SIZE = $(ARM_DIR)/bin/arm-none-eabi-size
ARM_INC_DIR = $(ARM_DIR)/arm-none-eabi/include

# -- Local testing toolchain --
# We expect a local gcc installation

# -- Sources and Objects --
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/**/*.c)
INC_DIR = inc
HDRS = $(wildcard $(INC_DIR)/*.h)
OBJ_DIR = build/obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# -- Target and build --
BUILD_DIR = build
TARGET = build/$(PROJECT).elf

# -- Compiler options --
CFLAGS  = -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -D__MSP432P401R__ -DDeviceFamily_MSP432P401x -DARM_MATH_CM4 -D__FPU_PRESENT=1
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -Og -g -gstrict-dwarf
CFLAGS += -Wall
CFLAGS += $(addprefix -isystem, $(ARM_INC_DIR)) $(addprefix -I, $(INC_DIR))

# -- Linker options --
LDFLAGS  = -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -specs=nosys.specs
LDFLAGS += -T config/msp432p401r.lds
LDFLAGS += -Wl,--gc-sections

# -- Cppcheck for static analysis (optional) --
CPPCHECK_SRCS  = $(SRCS)
CPPCHECK_EXCLUDE = $(SRC_DIR)/drivers $(SRC_DIR)/lib
CPPCHECK_FLAGS = --quiet --enable=all --inline-suppr --std=c99 --error-exitcode=1
CPPCHECK_FLAGS += $(addprefix -I, $(INC_DIR))
CPPCHECK_FLAGS += $(addprefix -i, $(CPPCHECK_EXCLUDE))
CPPCHECK_FLAGS += $(addprefix --suppress=, checkersReport missingIncludeSystem *:inc/CMSIS/* *:inc/device/* *:inc/driverlib/*)
CPPCHECK_FLAGS += -D__MSP432P401R__ -DDeviceFamily_MSP432P401x -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__GNUC__

# -- Test sources and objects --
TEST_OBJ_DIR = build/tests
TEST_TARGET = build/test
TEST_SRCS = $(wildcard tests/*.c)
TEST_SRCS += $(wildcard tests/**/*.c)
TEST_HDRS_DIR = tests/
TEST_COMM_OBJS = $(patsubst $(SRC_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(addprefix $(SRC_DIR)/app/, powertrain_module.c remote_module.c state_machine.c sensing_module.c system.c telemetry_module.c))
TEST_COMM_OBJS += $(patsubst $(SRC_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(SRC_DIR)/lib/queue.c)
TEST_ONLY_OBJS = $(patsubst tests/%.c, $(TEST_OBJ_DIR)/%.o, $(TEST_SRCS))

# -- Test compiling and linking options --
TEST_GCC_FLAGS = -Wall -Og $(addprefix -I, $(TEST_HDRS_DIR) $(INC_DIR)) -DTEST

# Rules
all: static-analysis compile size

compile: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	@echo "\n$(FONT_RESET)$(FONT_BOLD)$(FONT_BLUE)\xc2\xbb Linking $(FONT_RESET)$(notdir $^)$(FONT_RED)"
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "$(FONT_RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(FONT_BOLD)$(FONT_YELLOW)\xc2\xbb Compiling $(FONT_RESET)$<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

test: $(TEST_OBJ_DIR) $(TEST_TARGET) test-size

$(TEST_OBJ_DIR):
	@mkdir -p $(TEST_OBJ_DIR)

$(TEST_TARGET) : $(TEST_COMM_OBJS) $(TEST_ONLY_OBJS)
	@echo "\n$(FONT_RESET)$(FONT_BOLD)$(FONT_BLUE)\xc2\xbb Linking Tests$(FONT_RESET)$(notdir $^)$(FONT_RED)"
	@gcc $(TEST_LD_FLAGS) -o $@ $^
	@echo "$(FONT_RESET)"

$(TEST_OBJ_DIR)/%.o : tests/%.c
	@echo "$(FONT_BOLD)$(FONT_YELLOW)\xc2\xbb Compiling Tests$(FONT_RESET)$<"
	@mkdir -p $(dir $@)
	@gcc $(TEST_GCC_FLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(FONT_BOLD)$(FONT_YELLOW)\xc2\xbb Compiling Tests$(FONT_RESET)$<"
	@mkdir -p $(dir $@)
	@gcc $(TEST_GCC_FLAGS) -c -o $@ $<


clean:
	@rm -rf $(BUILD_DIR)

size:
	@echo "$(FONT_BOLD)\xc2\xbb Target information$(FONT_RESET)"
	@$(SIZE) --format=berkeley $(TARGET)

test-size:
	@echo "$(FONT_BOLD)\xc2\xbb Target information$(FONT_RESET)"
	@size $(TEST_TARGET)

static-analysis:
	@echo "$(FONT_RESET)$(FONT_BOLD)\xc2\xbb Running static analysis$(FONT_RESET)$(FONT_RED)"
	@cppcheck $(CPPCHECK_FLAGS) $(CPPCHECK_SRCS)
	@if [ $$? -eq 0 ]; then \
  	  	echo "$(FONT_RESET)Good job! Analysis passed."; \
  	fi
	@echo "$(FONT_RESET)"

flash:
	@echo "$(FONT_RESET)$(FONT_BOLD)\xc2\xbb Flashing$(FONT_RESET)"
	@openocd -f config/ti_msp432_launchpad.cfg -c "program build/${PROJECT}.elf verify reset exit"

.PHONY: all clean format size cppcheck flash

# Utilities
FONT_RESET = \033[0m
FONT_BOLD = \033[1m
FONT_RED = \033[31m
FONT_GREEN = \033[32m
FONT_YELLOW = \033[33m
FONT_BLUE = \033[34m
FONT_GRAY = \033[90m
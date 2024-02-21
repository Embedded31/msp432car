# The MSP432car



|                           |                               |
:-------------------------:|:-------------------------:
![MSP432car](/.github/image_sideMSP432car.png?raw=true)  |  ![MSP432car](/.github/image_side2MSP432car.png?raw=true)

The MSP432Car embedded project is a battery-powered remotely IR-controlled car with both manual and autonomous driving capabilities, this project focuses on obstacle recognition and avoidance, remote control via IR signals and Bluetooth Low Energy communications.

The principal features are:
- **Obstacle recognition and avoidance**: thanks to the combined work of the ultrasonic sensor and the servo motor the car is capable of discover and avoid the obstacles present on the path.
- **Remote manual driving**: the car can be remotely controlled using an infrared controller or by sending specific bluetooth messages.
- **Logging**: every time that a relevant event occurs, the car will send (expose) a bluetooth message using the BLE technology.
- **Autonomous mode**: the car is provided with a self-driving mode in which it applies obstacle avoidance mechanisms.

---
<br>

## Requirements

### Hardware requirements
- **Texas Instruments MSP432P401R**: the microcontroller that runs the code
- **HC-SR04 ultrasonic sensor**: sends ultrasonic waves to detect objects near the car
- **SG90 servo motor**: moves the ultrasonic sensor from left to right 
- **HC08 Bluetooth Low Energy module**: exchanges information with the connected devices
- **infrared receiver & controller**: used to receive commands to pilot the car
- **L298N motor driver**: responsible of managing the motor's voltage
- **DC motors**: for allowing the car to move
- **Battery pack**: in our case a 2x 3.7V 2200mAh batteries
- **Voltage dividers**: we used two self-made voltage dividers in order to reduce the 5V outputs from the ultrasonic and infrared sensors
to a 3.3V signal manageable from the MSP432; a third voltage divider is used to scale down the battery voltage for ADC conversions.
- **Voltage converter**: in order to provide three power lines (5V, 3.3V and GND) to the system from the battery pack, in our case 
we used an old arduino one like board that handles the battery output and offers the three voltages as output.

|   |   |   |   |
|---|---|---|---|
|![MSP432401P401R](/.github/image_MSP432.png?raw=true) | ![HC-SR04](/.github/image_HCSR04.jpg?raw=true) | ![SG90](/.github/image_SG90.jpg?raw=true) | ![HC-08](/.github/image_HC08.png?raw=true)  |
| ![TL-1838](/.github/image_infrared.jpg?raw=true) | ![L298N](/.github/image_L298N.png?raw=true) | ![DCMotor](/.github/image_dc_motor.jpg?raw=true) | ![Battery](/.github/image_batteries.jpg?raw=true) |

#### Pin mapping 

|Port | Pin | Direction |        Device                   | Pin | Description         | 
|-----|-----|-----------|---------------------- |-----|---------------------|
| 1 | 6 | OUT |Ultrasonic sensor | Trig | Used to trigger a new ultrasonic measurement | 
| 1 | 7 | IN |Ultrasonic sensor | Echo | Result signal of the measurement |      
| 2 | 4 | OUT | Motor driver | ENB | PWM signal of the B channel | 
| 2 | 5 | OUT | Motor driver | ENA | PWM signal of the A channel | 
| 2 | 7 | IN | Infrared receiver | Signal | TTL signal of a IR reading | 
| 3 | 2 | IN | Bluetooth LE module | RX | Serial reception 
| 3 | 3 | OUT | Bluetooth LE module | TX | Serial transmission 
| 4 | 1 | OUT | Motor driver | IN1 | Channel A direction selector 0 
| 4 | 2 | OUT | Motor driver | IN2 | Channel A direction selector 1 
| 4 | 3 | OUT | Motor driver | IN3 | Channel B direction selector 0 
| 4 | 4 | OUT | Motor driver | IN4 | Channel B direction selector 1 
| 5 | 6 | OUT | Servo motor | Signal | PWM signal for a specific position
| 6 | 1 | IN | Battery voltage divider | - | Current voltage of the battery pack (scaled down) 

<br>

### Software requirements
- **ARM GNU Toolchain**: used to cross-compile the code for the microcontroller
- **GNU Make**: for an easy and automated approach to the building, flashing and testing process
- **Cppcheck**: completely optional, we used it for running static analysis of the code before compiling
- **OpenOCD**: for flashing the compiled executable into the microcontroller
- **Local Toolchain**: for compiling and executing the testing program

---
<br>

## Project Layout

```

├── config                      # msp432 config files to compile software to target device
│   ├── msp432p401r.lds
│   └── ti_msp432_launchpad.cfg
│
├── inc                         # headers, can be included in other files
│   ├── driverlib            
│   │   ├── driverlib.h
│   │   └── ...
│   │   
│   ├── infrared_hal.h
│   ├── motor_hal.h
│   ├── msp.h
│   ├── powertrain_module.h
│   ├── queue.h
│   ├── remote_module.h
│   ├── sensing_module.h
│   ├── servo_hal.h
│   ├── state_machine.h
│   ├── system.h
│   ├── telemetry_module.h
│   ├── timer_hal.h
│   └── ultrasonic_hal.h
│
├── src                     # our application source code
│   ├── app                 # main application, FSM and modules
│   │   ├── main.c
│   │   ├── powertrain_module.c
│   │   ├── remote_module.c
│   │   ├── sensing_module.c
│   │   ├── state_machine.c
│   │   ├── system.c
│   │   └── telemetry_module.c
│   ├── drivers
│   │   └── ...
│   ├── hal                 # hardware abstraction layer
│   │   ├── battery_hal.c
│   │   ├── bluetooth_hal.c
│   │   ├── infrared_hal.c
│   │   ├── motor_hal.c
│   │   ├── servo_hal.c
│   │   ├── timer_hal.c
│   │   └── ultrasonic_hal.c
│   └── lib
│       └── ...
├── tests                   # HAL scaffolding to perform tests
│   ├── battery_hal.c
│   ├── battery_hal.h
│   ├── bluetooth_hal.c
│   ├── bluetooth_hal.h
│   ├── infrared_hal.c
│   ├── infrared_hal.h
│   ├── motor_hal.c
│   ├── motor_hal.h
│   ├── servo_hal.c
│   ├── servo_hal.h
│   ├── ultrasonic_hal.c
│   ├── ultrasonic_hal.h
│   │
│   ├── integration-tests   
│   │   ├── it_state_machine.c
│   │   └── it_state_machine.h
│   │
│   └── unit-tests
│       ├── ut_powertrain_module.c
│       ├── ut_powertrain_module.h
│       ├── ut_sensing_module.c
│       └── ut_sensing_module.h
└── README.md

```


All hardware dependent components of the project are in the HAL (Hardware Abstraction Layer). This has allowed us
to easily test the rest of the system, as we can simulate the hardware with software, therefore eliminating all complications that come with working with real hardware.
In the 'test' folder all functions of the HAL have been replaced by mock functions
that return valid values and correctly set environment variables, but without interacting with the hardware. Unit tests have been developed to check whether the function behaves as expected under a wide range of circumstances.

Higher level functions performing related tasks are grouped into modules. Modules carry out specific, high level work, for
example checking if there is an object in front of the car, notify the host about general information and status of the car,
moving the car in a specified direction.

The main.c operates as a FSM, this is how the program knows what to do at any given time.

---
<br>

## How to build, burn and run
The first step is to modify the ARM_DIR variable inside the Makefile in order to point to the base folder of the ARM GNU toolchain in
your system, then the following commands are available:
- `make`: analyzes and compile the code, the final executable is build/msp432car.elf
- `make compile`: compiles the code
- `make flash`: calls openocd and flashes the code into the microcontroller
- `make test`: compiles the test program (build/test)

---
<br>

## User guide

The car can operate in two different modes:
- **Autonomous mode**: the car starts and goes forward until a object is detected in front of the car, here the car will check if the path on the right or 
on the left is clear, turn and repeat; if both left and right are unavailable the car will perform a 180 degrees rotation and restart. 
- **Remote mode**: in this mode the car waits for commands that can be sent both via infrared or BLE.

The supported commands are:

| IR controller button | IR decimal code | BLE command | Function |
|--|--|--|--|
| Up arrow | 70 | "FWD" | Sets the car in forward direction at default speed (30%) |
| Down arrow | 21 | "REV" | Sets the car in backward direction at default reverse speed (20%) |
| Left arrow | 68 | "LFT" | Performs a 45 degrees counterclockwise turn |
| Right arrow | 67 | "RGT" | Performs a 45 degrees clockwise turn |
| OK | 64 | "STP" | Stops the car |
| Num 2 | 25 | | Increase the speed of the motors by 10% (max 100%) |
| Num 8 | 28 | | Decrease the speed of the motors by 10% (min 20%) |
| Asterisk | 66 | "AUT" / "MAN" | Toggles the operating mode between |

---
<br>

## Links

+ [Video](https://youtu.be/ml64IiFhyTM)
+ [Slides](https://docs.google.com/presentation/d/1Rq2v0xtXG-0sWzcRDlUgwbvq_DOUz4xPYq6V3XFrePk/edit?usp=sharing)

---
<br>

## Team members contributions

- **Andrea Piccin**: battery HAL,  bluetooth HAL, infrared HAL, timer HAL, ultrasonic HAL, motor HAL, remote module, sensing module, some work in powertrain and testing. Many revisions and additions to all files, tested and debugged the code for the whole project.

- **Simone Rossi**: infrared HAL, motor HAL, servo HAL, main, system, state machine, powertrain module, unit and integration testing.

- **Matteo Frizzera**: sensing module, telemetry module; some work in servo hal. Presentation and readme


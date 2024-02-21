# The MSP432car



|                           |                               |
:-------------------------:|:-------------------------:
![MSP432car](/.github/image_sideMSP432car.png?raw=true)  |  ![MSP432car](/.github/image_side2MSP432car.png?raw=true)

The aim of this project was to create a battery powered, self driving car able to detect and avoid obstacles. The car can also be controlled via an infrared transmitter, and is 
able to switch modes between autonomous driving and remote contol.

## Hardware components

### TI MSP432401P401R
This is the microcontroller that pilots the car  

![MSP432401P401R](/.github/image_MSP432.png?raw=true)

<br>


### HC-SR04 ultrasonic sensor
Sends ultrasonic waves to detect objects near the car  

![HC-SR04](/.github/image_HCSR04.jpg?raw=true)

<br>

### SG90 servo motor
Moves the ultrasonic sensor from left to right  

![SG90](/.github/image_SG90.jpg?raw=true)

<br>

### HC08 ble module
Sends information about the status of the car to an external device

![HC-08](/.github/image_HC08.png?raw=true)

<br>

### TL1838 infrared module
Used to receive commands to pilot the car  

![TL-1838](/.github/image_TL1838.jpg?raw=true)

<br>

### L298N motor driver
It's respondible for sending power to the wheels

![L298N](/.github/image_L298N.png?raw=true)

<br>


## Code structure

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
to easily test the rest of the system, as we can simulate the hardware with with software, therefore eliminating all complications that come with working with real hardware.
In the 'test' folder all functions of the HAL have been replaced by mock functions
that return valid values and correctly set enviroment variables, but without interacting with the hardware. Unit tests have been developed to check whether the function behaves as expected under a wide range of circumstances.

Higher level functions performing related tasks are grouped into modules. Modules carry out specific, high level work, for
example checking if there is an object in front of the car, notify the host about general information and status of the car,
moving the car in a specified direction.

The main.c operates a FSM, this is how the program knows what to do at any given time. For each state there is a corresponding function to execute.


# How to build, burn and run the MSP432car

TODO andrea 

# User guide

The car has two main modes it can operate in. In remote mode the car listens for commands 
that are sent via infrared signals. Every signal corresponds to a command, for example increase speed, rotate in a particular direction, stop the car. This mode allows a person to manually control the vehicle, for example using a remote infrared controller
by giving it commands via a infrared remote controller.
In autonomous driving mode the car goes forward, using the ultrasonic sensor to detect objects in front. When it detects and obstacle, it check lateral clearance by moving the sensor from left to right, then moves in the direction 
in wich it didn't detect any object. If there is no clear path, meaning the sensor detected an object in front, on the left and on 
the right, then the msp432car will make a complete 180 degrees turn and go back.

We used UART 

wheel, motors 



# Pin mapping 

|PIN   |        DESCRIPTION                    |
| -----|-------------------------------------- |
| P6.1 | battery adc conversion                | 
| P3.2 | bluetooth receiver input pin          | 
| P3.2 | bluetooth transmitter output pin      |         
| P2.7 | interrupt infrared port               |     
| P2.4 | left motor PWM signal                 |     
| P2.5 | right motor PWM signal                |         
| P4.1 | right motor direction                 |         
| P4.2 | right motor direction                 |         
| P4.3 | left motor direction                  |             
| P4.4 | left motor direction                  |             
| P5.6 | servo PWM signal                      |         
| P1.6 | ultrasonic sensor trigger pin         |         
| P1.7 | interrupt for ultrasonic sensor echo pin  |             


# Links

+ [Video](https://youtu.be/ml64IiFhyTM)
+ [Slides](https://docs.google.com/presentation/d/1Rq2v0xtXG-0sWzcRDlUgwbvq_DOUz4xPYq6V3XFrePk/edit?usp=sharing)

# Team members contributions

Andrea Piccin: remote module, battery hal,  bluetooth hal, infrared hal, timer hal, ultrasonic hal; some work in powertrain, sensing module and motor hal. Many revisions and additions to all files, tested and debugged the code for the whole project.

Simone Rossi: main, system, state machine, powertrain module, motor hal, servo hal, unit and integration testing.

Matteo Frizzera: sensing module, telemetry module; some work in servo hal. Presentation and readme


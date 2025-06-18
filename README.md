# Automated Garden Project!

## Introduction

In this project, my goal is to automate my garden's irrigation system.
I'll be using a soil moisture sensor combined with a solenoid valve to control the irrigation process.
One garden, one goal.

The project is very simple — and it's also my first "big" project, combining hardware and code.

## Technologies & Components Used

- **Arduino Nano**.
- **Soil Moisture Sensor**.
- **Solenoid Valve** (220V AC, 1A).
- **Transistor** (to control power to the moisture sensor).
- **Relay Module** (to switch the solenoid valve).
- **Push Buttons** (for *Pause* and *Manual* modes).
- **LEDs** (to indicate system status: irrigating, paused, manual).
- **LCD Display** (16x2 with I2C interface).
- **PlatformIO** (for development and uploading).

## Optional/Support Hardware

- **Screw terminal breakout board** (for Arduino Nano).
- Breadboard & jumper wires (for tests before solder).
- Plastic enclosure or case.
  
## Code Features

- Checks soil moisture every hour (since a resistive sensor is used, it's powered only during readings to extend its lifespan).
- Maintains soil moisture within a range of 30% to 70%.
- **Pause mode**: instantly stops all irrigation activity when the pause switch is activated.
- **Manual mode**: allows manual activation of irrigation through a dedicated switch.
- Status LEDs indicate the current system mode (paused, manual, irrigating) in real time.
- LCD display shows real-time system status, including current mode, last soil moisture reading, and irrigation activity.

## Conclusion

This project not only helped me better understand how to combine electronics and software, but also gave me hands-on experience with real-world automation.
While simple, it lays the foundation for more complex systems in the future — such as weather-based irrigation, remote monitoring, or IoT integration.

There’s still a lot of room for improvement, but I’m proud of this first step toward smarter, more efficient gardening.

One garden, one goal — automated. 

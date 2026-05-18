# ZanaBlocks

## Introduction

A cross-platform desktop application that helps children learn programming through simple visual blocks and real hardware interaction using devices such as the ESP32.

## Overview

This application makes programming enjoyable and interactive for children by combining drag-and-drop visual programming with actual electronic hardware.

Children can create programs using vibrant blocks, deploy them directly to an ESP32 device, and instantly witness the results in the real world through LEDs, motors, sensors, and other electronic components.

The objective is to simplify, engage, and provide hands-on learning experiences for programming concepts.

[🎥 Demo Video](./assets/demo.webm)

## ⚠️ Safety Notice

This application is provided for educational purposes under the Apache 2.0 License.
Use with children must be under parental supervision.
Always use correct resistors and GPIO pins before powering any circuit.

## ✨ Features

* 🧩 Visual Block Programming
  * Easy drag-and-drop programming blocks
  * No complex syntax to memorize
  * Beginner-friendly interface
* 💻 Cross-Platform Desktop Application
  * Works on Windows, macOS, and Linux
  * Simple installation and setup
* ⚡ Direct Deployment to ESP32
  * Upload programs directly to connected hardware
  * Instant feedback and real-time results
* 🔌 Hardware Control
  * Control LEDs, motors, buzzers, and displays
  * Read sensors such as buttons, distance sensors, and temperature sensors
  * Support for additional actors and electronic modules
* 🔄 Learn Programming Logic
  * Understand loops and repetition
  * Learn conditions (if, else)
  * Explore variables and events
  * Practice logical thinking and problem solving
* 🎓 Easy to Learn
  * Designed specifically for children and beginners
  * Visual learning approach
  * Interactive and engaging
* 🛠️ Build Real Projects
  * Create robots and smart devices
  * Build games and interactive projects
  * Encourage creativity and experimentation

## 🎯 Educational Goals

This platform helps children:

* Learn the basics of programming
* Understand programming flow control
* Develop computational thinking skills
* Gain confidence through hands-on projects
* Connect software with real-world hardware

## 🧠 Example Learning Topics

Children can learn concepts such as:

* Turning LEDs on and off
* Using loops to repeat actions
* Reacting to button presses
* Reading sensor values
* Controlling motors
* Creating interactive behaviors

## 🤝 Contributing

Contributions are welcome.
Feel free to submit issues, feature requests, or pull requests.

## ❤️ Mission

Making programming and electronics accessible, fun, and inspiring for children through visual learning and real-world interaction.

## 🏗️ Building from source

### Build Requirements

* CMake >= 3.31
* Qt6
* protobuf
* libserialport
* C++17 compiler

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## License

This project is licensed under the Apache License 2.0.
See the LICENSE file for details.

### Third-Party Licenses

This application uses third-party open-source libraries:

| Library             |  License            |
|---------------------|---------------------|
| Qt                  |  LGPLv3 / GPLv3     |
| Protocol Buffers    |  BSD 3-Clause       |
| esp-serial-flasher  |  Apache-2.0 license |
| libserialport       |  LGPLv3               |

See the LICENSES directory for full license texts.

## Disclaimer

This project is not affiliated with The Qt Company or Google.

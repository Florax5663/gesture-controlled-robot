# Wireless Gesture-Controlled Robot with Obstacle Avoidance

A dual-microcontroller robotics project that translates human hand gestures into real-time mechanical movement using radio frequency communication, featuring built-in ultrasonic collision prevention. 

## ⚙️ How It Works
This system operates on a master-slave architecture utilizing two Arduino boards:
1. **The Transmitter (The Glove):** An Arduino Nano reads X, Y, and Z-axis orientation data from an MPU6050 accelerometer/gyroscope attached to the user's hand. It packages this directional data and transmits it wirelessly via an nRF24L01 radio module.
2. **The Receiver (The Chassis):** An Arduino Uno receives the radio payload and processes the orientation data to dictate motor logic. It commands an L298N motor driver to move the robotic chassis mirroring the user's hand tilt. 
3. **Obstacle Avoidance:** The Receiver chassis is equipped with an HC-SR04 Ultrasonic Sensor facing forward. If the sensor detects an object within a critical distance threshold, it acts as an emergency brake—automatically halting the motors and ignoring forward movement commands from the glove until the path is clear.

## 🛠️ Hardware Components
* **Microcontrollers:** Arduino Uno & Arduino Nano
* **Sensors:** MPU6050 (6-axis IMU) & HC-SR04 (Ultrasonic Distance Sensor)
* **Communication:** nRF24L01 Transceiver Modules (2.4GHz)
* **Actuation:** L298N Motor Driver Module & DC Motors

## 📂 Repository Structure
* `/Transmitter` - Contains the `transmitter.ino` code for the Arduino Nano (MPU6050 data reading and nRF24L01 transmission).
* `/Receiver` - Contains the `receiver.ino` code for the Arduino Uno (nRF24L01 reception, Ultrasonic distance polling, and L298N motor mapping).

## 🚀 Setup & Installation
1. Clone this repository: `git clone https://github.com/Florax5663/gesture-controlled-robot.git`
2. Open the `/Transmitter` sketch in the Arduino IDE, select your Arduino Nano board, and upload.
3. Open the `/Receiver` sketch, select your Arduino Uno board, and upload.
4. Ensure all hardware pins match the configurations defined at the top of each sketch.

---
---
*Engineered in C++ | Created by [Piuse Samanta](https://florax5663.github.io/my-first-project/)*
# ClimateNode-360

![C++](https://img.shields.io/badge/Language-C++-blue.svg)
![PHP](https://img.shields.io/badge/Language-PHP-indigo.svg)
![Hardware](https://img.shields.io/badge/Hardware-ESP32_S3_%7C_ESP32_C3-green.svg)

## Overview
**ClimateNode-360** is a comprehensive, scalable environmental monitoring system designed around a Master-Slave IoT architecture. Utilizing the power of **ESP32-S3** and **ESP32-C3** microcontrollers, the system collects distributed sensor data and routes it to a custom PHP-based backend for real-time processing, storage, and visualization.

This project is intended for applications such as smart agriculture, distributed weather stations, and industrial environmental tracking.

## System Architecture

The project is divided into three main components:

1. **The Sensor Nodes (Slaves):** Powered by the low-cost **ESP32-C3**, these nodes interface directly with environmental sensors. They acquire data and transmit it wirelessly to the central master node.
2. **The Central Hub (Master):** Powered by the robust **ESP32-S3**, this hub acts as the data aggregator. It receives environmental metrics from multiple C3 slave nodes and pushes the consolidated data to the web server.
3. **The Web Platform (Backend/Frontend):** A custom-built **PHP/SQL** backend that handles API requests from the Master node, logs the data into a database, and displays it via a responsive, CSS-styled frontend dashboard.

## Repository Structure

* `/ESP32_S3_Master/` - Firmware for the ESP32-S3 Central Hub (Data aggregator & HTTP client).
* `/ESP32_C3_Slave/` - Firmware for the ESP32-C3 Sensor Nodes (Data collection & transmission).
* `/ESP32_S3_V1.0/` & `/ESP32_C3_V1.0/` - Hardware design files and specific versioned firmware iterations.
* `/PHP_BackEnd_FrontEnd/ClimateNode_360/` - The web application. Includes the PHP API endpoints for the ESP32, database schemas, and the frontend dashboard (HTML/CSS/PHP).
* `/Sensor_Testing_Codes/` - Isolated C/C++ scripts used for testing individual sensors before integrating them into the main firmware.

## Hardware Requirements

* **Master Node:** ESP32-S3 Development Board
* **Slave Nodes:** ESP32-C3 Development Boards
* **Sensors:** Compatible environmental sensors (e.g., DHT11/22, BME280, Soil Moisture sensors, depending on the specific implementation detailed in the `Sensor_Testing_Codes` directory).

## Getting Started

### 1. Web Server Setup
1. Upload the contents of `/PHP_BackEnd_FrontEnd/ClimateNode_360/` to your web server (Apache/Nginx) or local environment like XAMPP.
2. Configure your SQL database using the provided schema.
3. Update the database connection strings in the PHP configuration files.

### 2. Microcontroller Setup
1. Open the `.ino` or `.cpp` files in the `/ESP32_S3_Master/` and `/ESP32_C3_Slave/` directories using **Arduino IDE** or **PlatformIO**.
2. Update the Wi-Fi credentials (`SSID` and `PASSWORD`) in the firmware.
3. Update the API endpoint URL in the ESP32-S3 Master code to point to your hosted PHP backend.
4. Compile and flash the respective boards.

## Author

**Dinindu Kaluarachchi**
* GitHub: [@Dmxddrx](https://github.com/Dmxddrx)

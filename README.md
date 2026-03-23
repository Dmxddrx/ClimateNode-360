# ClimateNode 360

![Language](https://img.shields.io/badge/Language-C++_%7C_PHP-blue.svg)
![Hardware](https://img.shields.io/badge/Hardware-ESP32--C3-green.svg)
![Interface](https://img.shields.io/badge/Interface-I2C_%7C_HTTP-orange.svg)

## Overview
**ClimateNode 360** is a distributed IoT-based environmental monitoring system designed for real-time measurement of temperature, humidity, and airborne dust concentration. Built for workshops, laboratories, and semi-industrial environments, the system utilizes multiple ESP32-C3 nodes to collect and transmit data over a local Wi-Fi network to a centralized XAMPP web server. 

Data is stored in a MySQL database and visualized on a custom PHP-based web dashboard, providing real-time metrics, daily averages, and weekly trends.

## Hardware Architecture

The sensor nodes are designed for reliability, high precision, and low-cost deployment.

* **Microcontroller:** ESP32-C3 module providing decentralized sensing and Wi-Fi connectivity.
* **Temperature & Humidity Sensor:** SHT30 (Digital I2C). Provides factory-calibrated readings with an accuracy of ±0.3°C and ±2% RH.
* **Optical Dust Sensor:** Sharp GP2Y1010. Measures airborne particulate matter via infrared light scattering.
* **Analog-to-Digital Converter:** ADS1115 (16-bit). The analog output of the dust sensor is routed through this external ADC to improve measurement resolution and reduce electrical noise compared to the ESP32's internal ADC.

### Hardware Pin Mapping

| Component | Pin / Connection | Description |
| :--- | :--- | :--- |
| **I2C SDA** | GPIO 6 | Shared bus for SHT30 and ADS1115 |
| **I2C SCL** | GPIO 7 | Shared bus for SHT30 and ADS1115 |
| **Dust Sensor LED** | GPIO 4 | Controls the GP2Y1010 internal infrared LED |
| **Status LED** | GPIO 10 | Non-blocking visual indicator for system states |
| **Dust Analog Out** | ADS1115 (A1) | Routed to the 16-bit ADC for precision reading |

## Firmware Features

The C++ firmware is optimized for stability and uninterrupted long-term operation:

* **Data Averaging:** The node samples the environment every 15 seconds. It accumulates 5 samples and calculates an average before transmitting, reducing sensor noise and network traffic.
* **I2C Muting:** Sensor communication is temporarily paused during Wi-Fi transmission to prevent timing conflicts and bus lockups.
* **Power Optimization:** The dust sensor requires significant energy to power its internal LED. To minimize wear and power draw, dust is measured only once every five upload cycles.
* **System Reliability:** Implements a 30-second Task Watchdog Timer (WDT) to automatically recover from software hangs, alongside continuous reset reason detection for debugging.
* **Multi-Wi-Fi Fallback:** Stores multiple network credentials and automatically attempts sequential connections if the primary network drops.

## Web Dashboard & Server API

The backend is built on a standard XAMPP stack (Apache, PHP, MySQL) and uses asynchronous API calls to update the frontend without page reloads.

* **Data Ingestion (`log_data.php`):** Receives HTTP POST requests from the nodes containing `node_id`, `temp`, `hum`, and `dust`.
* **Data Retrieval APIs:** * `api_nodes.php`: Fetches the latest reading from each node (refreshed every 10 seconds).
    * `api_today.php` & `api_week.php`: Fetches daily measurements and weekly averages for Chart.js visualization (refreshed every 30 seconds).
* **Visualization:** A dark-themed, responsive dashboard utilizing Chart.js to map temperature, humidity, and dust concentration trends over time.

## Repository Structure

* `/ESP32_C3_Slave/` - Firmware for the ESP32-C3 Sensor Nodes (Data collection, averaging, & transmission).
* `/ESP32_S3_Master/` - Firmware for the ESP32-S3 Central Hub (Used for extended multi-tier architectures).
* `/PHP_BackEnd_FrontEnd/ClimateNode_360/` - The web application, including API endpoints, database schemas, and the HTML/CSS/JS frontend.
* `/Sensor_Testing_Codes/` - Isolated C/C++ scripts for calibrating and testing the SHT30, GP2Y1010, and ADS1115 independently.

## Getting Started

1.  **Server Deployment:** Copy the contents of `/PHP_BackEnd_FrontEnd/` into your Apache `htdocs` directory. 
2.  **Database Setup:** Create a MySQL database and execute the SQL schema to build the `sensor_network` table. Update the database credentials in the PHP files.
3.  **Firmware Flashing:** Open the ESP32-C3 firmware in your IDE. Update the Wi-Fi credentials and point the HTTP POST target to your server's IP address (e.g., `http://192.168.1.X/ClimateNode_360/log_data.php`).
4.  **Hardware Assembly:** Wire the SHT30, GP2Y1010, and ADS1115 to the ESP32-C3 according to the pin mapping table. Ensure a stable 5V 2A power supply is used.

## Author

**Dinindu Kaluarachchi**
* GitHub: [@Dmxddrx](https://github.com/Dmxddrx)
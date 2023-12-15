# Smart Irrigation System

This project implements a smart irrigation system that efficiently monitors and controls soil moisture, air humidity, air temperature, water container level, and pumps water accordingly. It comprises sensors, a controller (Yolobit), a gateway (ESP32), a server (Adafruit IO), and an application for intuitive monitoring and control.

## Components

### Sensors

- **Soil Moisture Sensor**: Collects soil moisture data.
- **Air Humidity and Temperature Sensor**: Measures air humidity and temperature.
- **Water Container Depth Sensor**: Determines the water level in the container and sends signals to the controller.

### Controller (Yolobit)

- Gathers information from the sensors to regulate the pump's operation.
- Displays relevant data on an LCD screen.
- Sends signals to the Gateway for data transmission.

### Gateway (ESP32)

- Receives and transmits data between the Yolobit and the server.
- Facilitates seamless communication between the internal components and the server.

### Server (Adafruit IO)

- Manages and stores various data received from the Gateway.
- Serves as a central repository for storing sensor readings and system status.

### Application

- Provides a user-friendly interface to visualize sensor data.
- Allows users to monitor the system's activity and device statuses conveniently.

## How it Works

1. **Sensors Gather Data**: Soil moisture, air humidity, temperature, and water level are collected.
2. **Yolobit Controls Operations**: Based on sensor inputs, the Yolobit manages the pump and displays information on the LCD.
3. **Data Transmission**: Information is transmitted to and from the ESP32 Gateway.
4. **Server Management**: Adafruit IO manages, stores, and organizes the data received from the Gateway.
5. **User Interface**: The application offers an intuitive interface to monitor sensor readings and system status.

## Usage

- Clone the repository.
- Set up the sensors, Yolobit controller, ESP32 Gateway, and the server following the provided documentation.
- Run the application to visualize and control the smart irrigation system.

## Link Repo

- [GitHub](https://github.com/Vypboy/Embedded_System_Assignment)
- [Video]()
- [Slide](https://www.canva.com/design/DAFSvUCivEY/SHxznl-BgYQJNTkfXhJ3WA/edit?utm_content=DAFSvUCivEY&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)

## License

This project is licensed under the [MIT License](LICENSE).

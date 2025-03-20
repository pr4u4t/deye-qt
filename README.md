# deye-qt

# Deye Integration for Home Assistant

This project integrates a Deye inverter system with Home Assistant using Qt. The integration supports communication over serial ports, Modbus, and HTTP server with REST API for controlling and monitoring the inverter system.

## Features

- **Serial Communication**: Interface with the Deye inverter via a serial port.
- **Modbus**: Support for Modbus communication protocol for data exchange.
- **HTTP Server**: Provides a REST API to interact with the Deye inverter.
- **Data Collection**: Collects and processes inverter data periodically.
- **Customizable Options**: Configurable communication parameters for serial communication.

## Setup

### Prerequisites

Before setting up the integration, make sure you have:

- Home Assistant running.
- Qt development environment set up (Qt 5.15 or higher recommended).
- A Deye inverter connected to your system.

### Installation

1. Clone this repository to your local machine.
   
   ```bash
   git clone https://github.com/your-repository/deye-home-assistant.git
   cd deye-home-assistant
Build the project using Qt:

    qmake
    make

    Follow the instructions for configuring the Home Assistant integration (detailed below).

Configuration

To configure the Deye integration, you need to specify the communication settings. The following options are available when running the Qt-based data collector:
Command-Line Options

The data collector can be configured via command-line options. The following options are available:

    -d, --device: Open serial port <device> (e.g., /dev/ttyUSB0 on Linux or COM3 on Windows).

    Example:

--device /dev/ttyUSB0

-p, --parity: Set the parity for serial communication. Options include none, even, odd, mark, space.

Example:

--parity none

-b, --baud: Set the baud rate for serial communication. Common values include 9600 and 115200.

Example:

--baud 115200

-l, --dataBits: Set the number of data bits. Available options are 5, 6, 7, or 8.

Example:

--dataBits 8

-s, --stopBits: Set the number of stop bits. Available options are 1 or 2.

Example:

--stopBits 1

-t, --responseTime: Set the response timeout in milliseconds.

Example:

--responseTime 1000

-r, --numberOfRetries: Set the number of retries for communication errors.

Example:

--numberOfRetries 3

-i, --interval: Set the loop interval in milliseconds (how often data is collected and sent).

Example:

    --interval 5000

Example Command

To start the data collector with custom serial settings:

./deye_data_collector --device /dev/ttyUSB0 --parity none --baud 115200 --dataBits 8 --stopBits 1 --responseTime 1000 --numberOfRetries 3 --interval 5000

This command configures the data collector to:

    Use the /dev/ttyUSB0 serial port.
    Set parity to none.
    Set the baud rate to 115200.
    Use 8 data bits.
    Set 1 stop bit.
    Set a response timeout of 1000 milliseconds.
    Retry 3 times on communication errors.
    Collect data every 5 seconds.

Home Assistant Integration
REST API

The Deye integration exposes a REST API that allows Home Assistant to interact with the inverter. You can use this API to fetch inverter data, send commands, and more.

The API can be accessed via:

http://<IP_ADDRESS>:<PORT>/api/v1/inverter

Refer to the API documentation for detailed endpoints and usage.
Home Assistant Setup

To integrate the Deye inverter with Home Assistant, you will need to add the appropriate configuration to your configuration.yaml file.

sensor:
  - platform: rest
    resource: http://<IP_ADDRESS>:<PORT>/api/v1/inverter/status
    name: "Deye Inverter Status"
    value_template: "{{ value_json.status }}"
    scan_interval: 60

Replace <IP_ADDRESS> and <PORT> with the actual IP address and port of your running Deye data collector.
Troubleshooting

    Serial Connection Issues: Ensure the correct serial port and communication parameters are configured.
    API Errors: Double-check the REST API configuration in Home Assistant.

License

This project is licensed under the MIT License.
Contributing

Feel free to open issues or submit pull requests for enhancements or bug fixes!

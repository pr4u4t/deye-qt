# Deye Integration for Home Assistant

A Qt-based integration for connecting Deye inverter systems with Home Assistant, supporting serial communication, Modbus protocol, and REST API functionality.

## Features

- **Serial Communication**: Interface with Deye inverters via serial ports
- **Modbus Protocol**: Support for Modbus data exchange
- **HTTP Server**: REST API for inverter interaction
- **Data Collection**: Periodic inverter data collection and processing
- **Customizable Options**: Configurable communication parameters

## Prerequisites

- Home Assistant installation
- Qt development environment (Qt 5.15 or higher recommended)
- Deye inverter with physical connection to your system

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/your-repository/deye-home-assistant.git
   cd deye-home-assistant
   ```

2. Build the project:
   ```bash
   qmake
   make
   ```

3. Configure the integration as detailed below

## Configuration

### Command-Line Options

The data collector accepts the following command-line parameters:

| Option | Description | Example |
|--------|-------------|---------|
| `-d, --device` | Serial port device path | `--device /dev/ttyUSB0` |
| `-p, --parity` | Parity setting (none, even, odd, mark, space) | `--parity none` |
| `-b, --baud` | Baud rate (e.g., 9600, 115200) | `--baud 115200` |
| `-l, --dataBits` | Number of data bits (5, 6, 7, 8) | `--dataBits 8` |
| `-s, --stopBits` | Number of stop bits (1, 2) | `--stopBits 1` |
| `-t, --responseTime` | Response timeout in milliseconds | `--responseTime 1000` |
| `-r, --numberOfRetries` | Number of retries for communication errors | `--numberOfRetries 3` |
| `-i, --interval` | Data collection interval in milliseconds | `--interval 5000` |

### Example Command

```bash
./deye_data_collector --device /dev/ttyUSB0 --parity none --baud 115200 --dataBits 8 --stopBits 1 --responseTime 1000 --numberOfRetries 3 --interval 5000
```

## Home Assistant Integration

### REST API

The integration exposes a REST API endpoint at:
```
http://<IP_ADDRESS>:<PORT>/api/v1/inverter
```

### Home Assistant Configuration

Add the following to your `configuration.yaml`:

```yaml
sensor:
  - platform: rest
    resource: http://<IP_ADDRESS>:<PORT>/api/v1/inverter/status
    name: "Deye Inverter Status"
    value_template: "{{ value_json.status }}"
    scan_interval: 60
```

Replace `<IP_ADDRESS>` and `<PORT>` with your actual values.

## Troubleshooting

- **Serial Connection Issues**: Verify serial port settings and device connections
- **API Errors**: Confirm REST API configuration in Home Assistant

## Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.

[![Docker Build Status](https://github.com/pr4u4t/deye-qt/actions/workflows/docker-build.yml/badge.svg)](https://github.com/pr4u4t/deye-qt/actions)

# Deye Integration for Home Assistant

A Qt-based integration for connecting Deye inverter systems with Home Assistant, supporting serial communication, Modbus protocol, and REST API functionality.

## Features

- **Serial Communication**: Interface with Deye inverters via serial ports
- **Modbus Protocol**: Support for Modbus data exchange
- **HTTP Server**: REST API for inverter interaction
- **Data Collection**: Periodic inverter data collection and processing
- **Customizable Options**: Configurable communication parameters
- **Comprehensive Monitoring**: Track battery, grid, solar, and consumption metrics

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

## Available Sensors

The integration provides access to the following sensor categories and metrics:

### Battery Metrics
- **Battery Capacity**: Overall battery capacity percentage
- **Battery SOC**: State of charge percentage 
- **Battery Voltage**: Battery voltage in V (divided by 10)
- **Battery Current**: Battery current in mA
- **Battery Power**: Battery power in W
- **Battery Temperature**: Battery temperature in °C (divided by 10)
- **Daily Battery Charge**: Daily energy charged to battery in kWh (divided by 100)
- **Daily Battery Discharge**: Daily energy discharged from battery in kWh (divided by 100)
- **Total Battery Charge**: Total energy charged to battery in kWh (divided by 100)
- **Total Battery Discharge**: Total energy discharged from battery in kWh (divided by 100)

### Grid Metrics
- **Grid Voltage L1/L2/L3**: Grid voltage for each phase in V (divided by 10)
- **Current L1/L2/L3**: Current for each phase in A (divided by 10)
- **Total Grid Power**: Total grid power in W
- **GridSell Power**: Power being sold to grid in W
- **Daily Energy Bought**: Daily energy purchased from grid in kWh (divided by 100)
- **Daily Energy Sold**: Daily energy sold to grid in kWh (divided by 100)
- **Total Energy Bought**: Total energy purchased from grid in kWh (divided by 100)
- **Total Energy Sold**: Total energy sold to grid in kWh (divided by 100)

### Solar Production Metrics
- **PV1/PV2 Voltage**: Voltage from solar panels in V (divided by 10)
- **PV1/PV2 Current**: Current from solar panels in A (divided by 10)
- **PV1 Input Power**: Power from PV1 in W
- **PV2 Power**: Power from PV2 in W
- **Daily Production**: Daily solar production in kWh (divided by 100)
- **Total Production**: Total solar production in kWh (divided by 100)

### Load Consumption Metrics
- **Load Voltage L1/L2/L3**: Load voltage for each phase in V (divided by 10)
- **Load L1/L2/L3 Power**: Load power for each phase in W
- **Total Load Power**: Total power consumption in W
- **Day Load Power**: Current day's load power in W
- **Daily Load Consumption**: Daily energy consumption in kWh (divided by 100)
- **Total Load Consumption**: Total energy consumption in kWh (divided by 100)

### Inverter Status Metrics
- **Inverter L1/L2/L3 Power**: Inverter power for each phase in W
- **AC Temperature**: Inverter AC temperature in °C (divided by 10)
- **DC Temperature**: Inverter DC temperature in °C (divided by 10)
- **Total Active IN Power**: Total active input power in W

### CT Sensor Metrics
- **External CT L1/L2/L3 Power**: External CT power measurements in W
- **Internal CT L1/L2/L3 Power**: Internal CT power measurements in W
- **Gen Port Power**: Generator port power in W

## Lovelace UI

The integration can be visualized in Home Assistant's Lovelace UI. A sample dashboard configuration is available in the `lovelace-ui.yaml` file with cards for:

- Battery status monitoring
- Grid metrics visualization
- Solar production tracking
- Load consumption analysis
- Inverter status display
- Power flow visualization
- Historical data analysis

## Troubleshooting

- **Serial Connection Issues**: Verify serial port settings and device connections
- **API Errors**: Confirm REST API configuration in Home Assistant
- **Sensor Reading Issues**: Check scaling factors in template sensors (some values need to be divided by 10 or 100)

## Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.

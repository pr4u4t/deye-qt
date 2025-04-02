# Home Assistant Add-on: Deye Data Collector

Collect data from Deye inverters via serial Modbus and expose it via REST API.

![Supports aarch64 Architecture][aarch64-shield] ![Supports amd64 Architecture][amd64-shield] ![Supports arm64 Architecture][arm64-shield]

## About

This Home Assistant add-on collects real-time data from Deye inverters using serial Modbus communication. The collected data is then exposed via a REST API, allowing Home Assistant and other systems to retrieve and analyze inverter metrics efficiently.

### Features:
- Supports multiple architectures (aarch64, amd64, arm64)
- Communicates with Deye inverters via serial Modbus
- Exposes inverter data as REST API endpoints
- Compatible with Home Assistant entity discovery
- Lightweight and efficient

For more information, please see the [Deye Inverter Documentation][deye-docs].

[deye-docs]: https://github.com/pr4u4t/deye-qt
[aarch64-shield]: https://img.shields.io/badge/aarch64-yes-green.svg
[amd64-shield]: https://img.shields.io/badge/amd64-yes-green.svg
[arm64-shield]: https://img.shields.io/badge/arm64-yes-green.svg

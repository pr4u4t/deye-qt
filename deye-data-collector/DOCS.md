# Deye Data Collector Add-on Documentation

## Overview
This add-on collects data from Deye inverters via serial Modbus and exposes it via a REST API.

## Installing Mosquitto MQTT Broker in Home Assistant

To use MQTT with your Deye Data Collector add-on, you will need to install the **Mosquitto MQTT Broker** add-on in Home Assistant.

### Steps to Install Mosquitto MQTT Broker:
1. **Go to Home Assistant's Add-on Store**:
   - Open your Home Assistant UI.
   - Go to **Settings** > **Add-ons** > **Add-on Store**.

2. **Add the Mosquitto Add-on Repository**:
   - If you don't already have the Mosquitto add-on, you can add the official Home Assistant repository.
   - In the **Add-on Store**, click on the **three dots menu** (top right) and select **Repositories**.
   - Add the official Home Assistant add-on repository URL:
     ```
     https://github.com/home-assistant/addons
     ```

3. **Install the Mosquitto Add-on**:
   - Search for "Mosquitto MQTT" in the Add-on Store.
   - Click on **Mosquitto MQTT** and press **Install**.

4. **Start the Mosquitto Add-on**:
   - After installation, click **Start** to run the Mosquitto MQTT broker.

---

## Configure Mosquitto MQTT Broker with Home Assistant

To configure the MQTT broker with a username and password:

1. **Configure the MQTT Broker**:
   - After starting the Mosquitto add-on, go to the **Mosquitto Configuration** section in the add-on settings.
   - Add the following to the **Configuration** tab to define your MQTT username and password:

     ```yaml
     logins:
       - username: "homeassistant"
         password: "your_password"
     anonymous: false  # Disable anonymous access
     ```

     Replace `"your_password"` with a secure password. You can create a random strong password or use something memorable.

2. **Enable MQTT Discovery**:
   - To allow Home Assistant to automatically discover devices using MQTT, ensure that `mqtt.discovery` is enabled in your Home Assistant configuration.

     Add this to your `configuration.yaml` file:

     ```yaml
     mqtt:
       broker: "localhost"  # Assuming Mosquitto is running on the same device as Home Assistant
       username: "homeassistant"  # This matches the username configured in Mosquitto
       password: "your_password"  # Use the same password you set above
       discovery: true  # Enable entity discovery
       discovery_prefix: homeassistant  # (Optional) Set the discovery prefix for all devices
     ```

     After adding this configuration, Home Assistant will automatically discover devices that are publishing to the MQTT broker.

---

## Restart Home Assistant

- After adding the configuration for both the Mosquitto MQTT broker and the Home Assistant integration, **restart Home Assistant** to apply the changes.
- Go to **Settings** > **System** > **Restart** to restart Home Assistant.

---

## Verifying MQTT Connection

1. **Check MQTT Integration**:
   - Go to **Settings** > **Devices & Services** > **Integrations** in Home Assistant.
   - Search for **MQTT** and ensure the integration is connected and no errors are present.

2. **Verify Device Discovery**:
   - Check the **Devices** section in Home Assistant. Your devices should automatically appear once they publish data to the MQTT broker.

---

## Features of Deye Data Collector Add-on
- Supports **multiple architectures** (`amd64`, `arm64`, `aarch64`).
- Provides **real-time inverter monitoring**.
- **MQTT & REST API** support for seamless integration.

For more details, check [GitHub Repository](https://github.com/pr4u4t/deye-qt).

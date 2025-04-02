#include "dummy.h"

Dummy::Dummy(const Settings& settings, QObject* parent)
    : Inverter(settings, "Dummy", "Random") {
    Q_UNUSED(parent)
    setSensors(createSensorList());
}

Dummy::~Dummy() {}

bool Dummy::connectDevice() {
    return true;
}

QVector<Sensor> Dummy::createSensorList() const {
    return {
        // Temperature Sensors (scaling /10)
        {"AC Temperature", "C", "temperature", "ac_temp", "deye_ac_temp_001", 0.01f, 541, SensorDataType::SHORT},
        {"Battery Temperature", "C", "temperature", "battery_temp", "deye_battery_temp_002", 0.01f, 586, SensorDataType::SHORT},
        {"DC Temperature", "C", "temperature", "dc_temp", "deye_dc_temp_003", 0.01f, 540, SensorDataType::SHORT},

        // Battery Sensors
        {"Battery SOC", "%", "battery", "battery_soc", "deye_battery_capacity_004", 1.0f, 588, SensorDataType::SHORT},
        //{"Battery SOH", "%", "battery", "battery_soh", "deye_battery_soh_005", 1.0, 10006, SensorDataType::SHORT},
        {"Battery Current", "mA", "current", "battery_current", "deye_battery_current_006", 0.01f, 591, SensorDataType::SSHORT},
        {"Battery Power", "W", "power", "battery_power", "deye_battery_power_007", 1.0f, 590, SensorDataType::SSHORT},
        {"Battery Voltage", "V", "voltage", "battery_voltage", "deye_battery_voltage_008", 0.01f, 587, SensorDataType::USHORT},

        // Current Sensors (scaling /10)
        {"Current L1", "A", "current", "current_l1", "deye_current_l1_009", 0.01f, 630, SensorDataType::SSHORT},
        {"Current L2", "A", "current", "current_l2", "deye_current_l2_010", 0.01f, 631, SensorDataType::SSHORT},
        {"Current L3", "A", "current", "current_l3", "deye_current_l3_011", 0.01f, 632, SensorDataType::SSHORT},

        // Voltage Sensors (scaling /10)
        {"Grid Voltage L1", "V", "voltage", "grid_voltage_l1", "deye_grid_voltage_l1_012", 0.1f, 598, SensorDataType::SHORT},
        {"Grid Voltage L2", "V", "voltage", "grid_voltage_l2", "deye_grid_voltage_l2_013", 0.1f, 599, SensorDataType::SHORT},
        {"Grid Voltage L3", "V", "voltage", "grid_voltage_l3", "deye_grid_voltage_l3_014", 0.1f, 600, SensorDataType::SHORT},
        {"Load Voltage L1", "V", "voltage", "load_voltage_l1", "deye_load_voltage_l1_015", 0.1f, 644, SensorDataType::USHORT},
        {"Load Voltage L2", "V", "voltage", "load_voltage_l2", "deye_load_voltage_l2_016", 0.1f, 645, SensorDataType::USHORT},
        {"Load Voltage L3", "V", "voltage", "load_voltage_l3", "deye_load_voltage_l3_017", 0.1f, 646, SensorDataType::USHORT},
        {"PV1 Voltage", "V", "voltage", "pv1_voltage", "deye_pv1_voltage_018", 0.1f, 676, SensorDataType::SHORT},
        {"PV2 Voltage", "V", "voltage", "pv2_voltage", "deye_pv2_voltage_019", 0.1f, 678, SensorDataType::SHORT},

        // Current Sensors (scaling /10)
        {"PV1 Current", "A", "current", "pv1_current", "deye_pv1_current_020", 0.1f, 677, SensorDataType::SHORT},
        {"PV2 Current", "A", "current", "pv2_current", "deye_pv2_current_021", 0.1f, 679, SensorDataType::SHORT},

        // Power Sensors
        {"PV1 Power", "W", "power", "pv1_input_power", "deye_pv1_power_022", 1.0f, 672, SensorDataType::SHORT},
        {"PV2 Power", "W", "power", "pv2_power", "deye_pv2_power_023", 1.0f, 673, SensorDataType::SHORT},
        {"Day Load Power", "W", "power", "day_load_power", "deye_day_load_power_024", 1.0f, 526, SensorDataType::SHORT},
        {"External CT L1 Power", "W", "power", "external_ct_l1_power", "deye_external_ct_l1_power_025", 1.0f, 616, SensorDataType::SSHORT},
        {"External CT L2 Power", "W", "power", "external_ct_l2_power", "deye_external_ct_l2_power_026", 1.0f, 617, SensorDataType::SSHORT},
        {"External CT L3 Power", "W", "power", "external_ct_l3_power", "deye_external_ct_l3_power_027", 1.0f, 618, SensorDataType::SSHORT},
        {"Gen Port Power", "W", "power", "gen_port_power", "deye_gen_port_power_028", 1.0f, 667, SensorDataType::SHORT},
        {"GridSell Power", "W", "power", "gridsell_power", "deye_gridsell_power_029", 1.0f, 525, SensorDataType::SHORT},
        {"Internal CT L1 Power", "W", "power", "internal_ct_l1_power", "deye_internal_ct_l1_power_030", 1.0f, 604, SensorDataType::SSHORT},
        {"Internal CT L2 Power", "W", "power", "internal_ct_l2_power", "deye_internal_ct_l2_power_031", 1.0f, 605, SensorDataType::SSHORT},
        {"Internal CT L3 Power", "W", "power", "internal_ct_l3_power", "deye_internal_ct_l3_power_032", 1.0f, 606, SensorDataType::SSHORT},
        {"Inverter L1 Power", "W", "power", "inverter_l1_power", "deye_inverter_l1_power_033", 1.0f, 633, SensorDataType::SSHORT},
        {"Inverter L2 Power", "W", "power", "inverter_l2_power", "deye_inverter_l2_power_034", 1.0f, 634, SensorDataType::SSHORT},
        {"Inverter L3 Power", "W", "power", "inverter_l3_power", "deye_inverter_l3_power_035", 1.0f, 635, SensorDataType::SSHORT},
        {"Load L1 Power", "W", "power", "load_l1_power", "deye_load_l1_power_036", 1.0f, 650, SensorDataType::SSHORT},
        {"Load L2 Power", "W", "power", "load_l2_power", "deye_load_l2_power_037", 1.0f, 651, SensorDataType::SSHORT},
        {"Load L3 Power", "W", "power", "load_l3_power", "deye_load_l3_power_038", 1.0f, 652, SensorDataType::SSHORT},
        {"Total Grid Power", "W", "power", "total_grid_power", "deye_total_grid_power_039", 1.0f, 625, SensorDataType::SHORT},
        {"Total Load Power", "W", "power", "total_load_power", "deye_total_load_power_040", 1.0f, 653, SensorDataType::SSHORT},
        {"Total Active IN Power", "W", "power", "total_active_in_power", "deye_total_active_in_power_041", 1.0f, 607, SensorDataType::SSHORT},

        // Energy Sensors (scaling /100)
        {"Daily Battery Charge", "kWh", "energy", "daily_battery_charge", "deye_daily_battery_charge_042", 0.1f, 514, SensorDataType::SHORT},
        {"Daily Battery Discharge", "kWh", "energy", "daily_battery_discharge", "deye_daily_battery_discharge_043", 0.1f, 515, SensorDataType::SHORT},
        {"Daily Energy Bought", "kWh", "energy", "daily_energy_bought", "deye_daily_energy_bought_044", 0.1f, 520, SensorDataType::SHORT},
        {"Daily Energy Sold", "kWh", "energy", "daily_energy_sold", "deye_daily_energy_sold_045", 0.1f, 521, SensorDataType::SHORT},
        {"Daily Load Consumption", "kWh", "energy", "daily_load_consumption", "deye_daily_load_consumption_046", 0.01f, 526, SensorDataType::SHORT},
        {"Daily Production", "kWh", "energy", "daily_production", "deye_daily_production_047", 0.1f, 529, SensorDataType::SHORT},
        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_048", 0.01f, 516, SensorDataType::DWORD},
        //{"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_049", 0.01, 517},
        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_050", 0.01f, 518, SensorDataType::DWORD},
        //{"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_051", 0.01, 519, SensorDataType::SHORT},
        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_052", 0.1f, 522, SensorDataType::DWORD},
        //{"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_053", 0.01, 523},
        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01f, 524, SensorDataType::DWORD},
        //{"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01, 525},
        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_055", 0.1f, 527, SensorDataType::DWORD},
        //{"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_056", 0.01, 528},
        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_057", 0.1f, 534, SensorDataType::DWORD},
        //{"Total Production", "kWh", "energy", "total_production", "deye_total_production_058", 0.01, 535}
    };
}

float Dummy::sensorValue(const /*signed*/ qint16 data, float scale) const {
    return static_cast<float>(data) * scale;
}

float Dummy::sensorValue(const /*unsigned*/ quint16 data, float scale) const {
    return static_cast<float>(data) * scale;
}

float Dummy::sensorValue(const /*signed*/ qint16 low, const qint16 high, float scale) const {
    SensorData tmp;
    tmp.words.low = low;
    tmp.words.high = high;
    return static_cast<float>(tmp.data) * scale;
}

void Dummy::disconnectDevice() {}

void Dummy::readReport() {
    qDebug() << "Performing read request";
    for (const auto& sensor : sensors()) {
        const auto inst = getNumberFromEnd(settings().instance());
		const auto sens = getNumberFromEnd(sensor.uniqueId());

        //data()[sensor.uniqueId()] = QString("%1.%2").arg(inst).arg(sens).toDouble();

        switch (sensor.type()) {
        case SensorDataType::SHORT:
            data()[sensor.uniqueId()] = sensorValue(generateRandom<short>(), sensor.scalingFactor());
            break;
        case SensorDataType::USHORT:
            data()[sensor.uniqueId()] = sensorValue(generateRandom<unsigned short>(), sensor.scalingFactor());
            break;
        case SensorDataType::SSHORT:
            data()[sensor.uniqueId()] = sensorValue(generateRandom<short>(), sensor.scalingFactor());
            break;
        case SensorDataType::DWORD:
            data()[sensor.uniqueId()] = sensorValue(generateRandom<short>(), generateRandom<short>(), sensor.scalingFactor());
            break;
        }
    }

    emit reportReady(data());
}

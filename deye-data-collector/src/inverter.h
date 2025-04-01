#ifndef INVERTER_H
#define INVERTER_H

#include <QObject>

#include "settings.h"

enum class SensorDataType {
    SHORT = 0,
    USHORT,
    SSHORT,
    DWORD
};

struct dshort {
    qint16 low;
    qint16 high;
};

union SensorData {
    float result;
    qint32 data;
    dshort words;
};

struct Sensor {
    Q_GADGET

public:
    QString m_name;
    QString m_unit;
    QString m_deviceClass;
    QString m_topicSuffix;
    QString m_uniqueId;
    float m_scalingFactor;
    int m_address;
    SensorDataType m_type;

    QString name() const { return m_name; }
    QString unit() const { return m_unit; }
    QString deviceClass() const { return m_deviceClass; }
    QString topicSuffix() const { return m_topicSuffix; }
    QString uniqueId() const { return m_uniqueId; }

    // Numeric and enum getters returning by value
    float scalingFactor() const { return m_scalingFactor; }
    int address() const { return m_address; }
    SensorDataType type() const { return m_type; }

    QString typeString(SensorDataType type) const {
        switch (type) {
        case SensorDataType::SSHORT:
            return "signed short";
        case SensorDataType::SHORT:
            return "short";
        case SensorDataType::USHORT:
            return "unsigned short";
        case SensorDataType::DWORD:
            return "dword";
        }

        return "unknown type";
    }

    // Add this method
    QString toString() const {
        return QString("DeyeSensor { "
            "name: '%1', "
            "unit: '%2', "
            "deviceClass: '%3', "
            "topicSuffix: '%4', "
            "uniqueId: '%5', "
            "scalingFactor: %6, "
            "address: %7, "
            "type: %8 }")
            .arg(name())
            .arg(unit())
            .arg(deviceClass())
            .arg(topicSuffix())
            .arg(uniqueId())
            .arg(scalingFactor())
            .arg(address())
            .arg(typeString(type()));
    }

    friend QDebug operator<<(QDebug debug, const Sensor& sensor) {
        QDebugStateSaver saver(debug);
        debug.nospace() << sensor.toString();
        return debug;
    }

    QJsonObject toJson() const {
        return {
            {"name", name()},
            {"unit", unit()},
            {"device_class", deviceClass()},
            {"topic_suffix", topicSuffix()},
            {"unique_id", uniqueId()},
            {"scaling_factor", scalingFactor()},
            {"address", address()},
            {"type", typeString(type())}
        };
    }
};

class Inverter : public QObject {
    Q_OBJECT

public:
    Inverter(const Settings& settings, const QString& manufacturer, const QString& model)
        : m_manufacturer(manufacturer)
        , m_model(model)
        , m_settings(settings){
    }
    
    virtual ~Inverter() = default;

    virtual bool connectDevice() = 0;

    virtual void disconnectDevice() = 0;

    virtual void readReport() = 0;

    inline QString manufacturer() const{
        return m_manufacturer;
    }

    inline QString model() const{
        return m_model;
    }

    const QJsonObject& report() const {
        return m_report;
    }

    const QVector<Sensor>& sensors() const {
        return m_dict;
    }

	Settings settings() const {
		return m_settings;
	}

signals:
    void reportReady(const QJsonObject& report);

protected:
    
    QJsonObject& data() {
        return m_report;
    }

    void setSensors(const QVector<Sensor>& sensors) {
        m_dict = sensors;
    }

private:
    QString m_manufacturer;
    QString m_model;
    QJsonObject m_report;
    QVector<Sensor> m_dict;
	Settings m_settings;
};

//Q_DECLARE_INTERFACE(Inverter, "pl.ntsystems.Inverter/1.0")

#endif // INVERTER_H
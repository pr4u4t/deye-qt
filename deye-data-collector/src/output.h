#ifndef OUTPUT_H
#define OUTPUT_H

#include <QObject>

#include "settings.h"

class Output {
public:
    Output(const Settings& settings)
        : m_settings(settings) {}

    virtual ~Output() = default;

    virtual bool init() = 0;

    virtual void deinit() = 0;
    
public slots:
    virtual void update(const QJsonObject& report) = 0;

protected:

    Settings settings() const {
        return m_settings;
    }

private:
    Settings m_settings;
};

#endif // OUTPUT_H
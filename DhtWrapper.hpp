#ifndef DHTWRAPPER_HPP
#define DHTWRAPPER_HPP

#include <QObject>

#include <QTimer>

#include <memory>

#include "dhtxxd.h"

#define GPIO_PIN 4

class DhtWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DhtWrapper(int pi, QObject *parent = nullptr);
    ~DhtWrapper();

    void callback(DHTXXD_data_t data);
    void read_sensor();
    void run();

signals:
    void temp_update(float temperature);
    void humid_update(float humididity);

private:

    DHTXXD_t* dht;

    std::shared_ptr<QTimer> timer;
};

#endif // DHTWRAPPER_HPP

#include "DhtWrapper.hpp"

#include <pigpiod_if2.h>

DhtWrapper::DhtWrapper(int pi, QObject *parent) : QObject(parent)
{
//    auto pi = pigpio_start(nullptr, nullptr); /* Connect to local Pi. */
    dht = DHTXXD(pi, GPIO_PIN, DHTXX, nullptr);
}

DhtWrapper::~DhtWrapper()
{
    DHTXXD_cancel(dht);
}

void DhtWrapper::read_sensor()
{
    if(DHTXXD_ready(dht))
    {
        auto reading = DHTXXD_data(dht);
        if(reading.status == 0)
        {
            temp_update(reading.temperature);
            humid_update(reading.humidity);
        }
    }
}

void DhtWrapper::run()
{
    DHTXXD_auto_read(dht, 5.0);

    timer = std::make_shared<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &DhtWrapper::read_sensor);
    timer->start();
}

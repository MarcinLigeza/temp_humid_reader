#include "SensorsAggregate.hpp"

#include <pigpiod_if2.h>

SensorsAggregate::SensorsAggregate()
{
    auto pi = pigpio_start(nullptr, nullptr); /* Connect to local Pi. */
    dht = std::make_shared<DhtWrapper>(pi, this);
    tsl = std::make_shared<TSL_Wrapper>(pi, this);
}

std::shared_ptr<DhtWrapper> SensorsAggregate::get_dht()
{
    return dht;
}

std::shared_ptr<TSL_Wrapper> SensorsAggregate::get_tsl()
{
    return tsl;
}

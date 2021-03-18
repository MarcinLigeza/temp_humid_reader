#ifndef SENSORSAGGREGATE_HPP
#define SENSORSAGGREGATE_HPP

#include <QObject>

#include "DhtWrapper.hpp"
#include "TSL_Wrapper.hpp"

class SensorsAggregate : public QObject
{
    Q_OBJECT;
public:
    SensorsAggregate();

    std::shared_ptr<DhtWrapper> get_dht();
    std::shared_ptr<TSL_Wrapper> get_tsl();

private:
    std::shared_ptr<DhtWrapper> dht;
    std::shared_ptr<TSL_Wrapper> tsl;
};

#endif // SENSORSAGGREGATE_HPP

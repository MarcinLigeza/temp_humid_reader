#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "SensorsAggregate.hpp"

int main(int argc, char *argv[])
{
    system("sudo pigpiod");

    QApplication a(argc, argv);
    MainWindow w;
    SensorsAggregate sensors;
    auto dht = sensors.get_dht();
    w.set_tsl(sensors.get_tsl());
    QObject::connect(dht.get(), &DhtWrapper::temp_update, &w, &MainWindow::update_temp);
    QObject::connect(dht.get(), &DhtWrapper::humid_update, &w, &MainWindow::update_humid);
    dht->run();
    w.show();
    return a.exec();
}

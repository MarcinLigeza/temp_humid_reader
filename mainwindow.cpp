#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QTimer>

#include <QUrlQuery>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "TSL_Config.hpp"
#include "TSL2581.hpp"


const QString KEY = "522b3cd11994164b77884ee6fc2af8a7";
const QString CITY = "Krakow,pl";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    network_access_manager = new QNetworkAccessManager(this);
    connect(network_access_manager, &QNetworkAccessManager::finished, this, &MainWindow::update_outdoor);

    QTimer *network_timer = new QTimer(this);
    connect(network_timer, &QTimer::timeout, this, &MainWindow::query_weather_api);
    network_timer->start(5000);

    QTimer *time_timer = new QTimer(this);
    connect(time_timer, &QTimer::timeout, this, &MainWindow::show_time);
    time_timer->start(100);

    show_time();

    showFullScreen();

    scene = new QGraphicsScene(this);

    scene->addLine(1, 2, 5, 10, QPen());
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_tsl(std::shared_ptr<TSL_Wrapper> tsl_)
{
    tsl = tsl_;
    QTimer *light_timer = new QTimer(this);
    connect(light_timer, &QTimer::timeout, this, &MainWindow::query_light_sensor);
    light_timer->start(500);
}

void MainWindow::update_temp(float temp)
{
    QString text(QString::number(temp,'f', 1) + " °C");
    ui->temp_value_label->setText(text);
}

void MainWindow::update_humid(float humid)
{
    QString text(QString::number(humid, 'f', 1)+ " %");
    ui->humid_value_label->setText(text);
}

void MainWindow::update_outdoor(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray result = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(result);
        QJsonObject obj = jsonResponse.object();

        float temp = obj["main"].toObject().find("temp").value().toDouble() - 273.15;
        float humid = obj["main"].toObject().find("humidity").value().toDouble();

        ui->temp_out_value_label->setText(QString::number(temp, 'f', 1) + " °C");
        ui->humid_out_value_label->setText(QString::number(humid, 'f', 1) + " %");
    }
    else
        ui->debug_label->setText("ERROR: " + reply->errorString());
    reply->deleteLater();
}

void MainWindow::show_time()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");

    ui->lcdNumber->display(text);
}

void MainWindow::query_weather_api()
{
    QUrlQuery query;
    query.addQueryItem("q", CITY);
    query.addQueryItem("APPID", KEY);

    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    url.setQuery(query);

    qDebug()<< "url: "<< url.toString(QUrl::FullyEncoded);

    network_access_manager->get(QNetworkRequest(url));
    ui->slider_label->setText(QString::number(get_api_key().length()));
}

void MainWindow::query_light_sensor()
{
    auto lux = tsl->calculate_lux(0, NOM_INTEG_CYCLE);
    ui->debug_label->setText(QString("light sensor reading: ") + QString::number(lux));
}

QString MainWindow::get_api_key()
{
    //TODO: nie działa - skopiować secrets na raspberry
    QFile file("secrets/api_keys");
    if(!file.open(QIODevice::ReadOnly)) {
        std::runtime_error("Cannot open file containing api key");
    }

    QTextStream in(&file);
    QString key = in.readAll().split("=")[1];

    file.close();

    return key;
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (not auto_brightness_control)
        brightness_controller.set_brightness(value);
}

void MainWindow::on_auto_backlight_toggled(bool checked)
{
    auto_brightness_control = checked;
    if(checked)
        ui->horizontalSlider->hide();
    else
        ui->horizontalSlider->show();
}

void MainWindow::on_close_button_clicked()
{
    close();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>

#include <QGraphicsScene>

#include "BrightnessController.hpp"
#include "TSL_Wrapper.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void set_tsl(std::shared_ptr<TSL_Wrapper> tsl_);

public slots:
    void update_temp(float temp);
    void update_humid(float humid);
    void update_outdoor(QNetworkReply *reply);

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_power_off_button_clicked();

    void on_auto_backlight_toggled(bool checked);

    void on_close_button_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *network_access_manager;

    BrightnessController brightness_controller{};
    std::shared_ptr<TSL_Wrapper> tsl;

    bool auto_brightness_control{false};

    void show_time();
    void query_weather_api();
    void query_light_sensor();

    QString get_api_key();

    QGraphicsScene *scene;
};
#endif // MAINWINDOW_H

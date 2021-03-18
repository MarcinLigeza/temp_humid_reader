#include "BrightnessController.hpp"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#define POWER_FILE "/sys/class/backlight/rpi_backlight/bl_power"
#define POWER_ON 0
#define POWER_OFF 1

#define BRIGHTNESS_FILE "/sys/class/backlight/rpi_backlight/brightness"
#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_MIN 10

BrightnessController::BrightnessController()
{

}

int BrightnessController::get_brightness()
{
    QFile file (BRIGHTNESS_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
        return -1;
    }
    else
    {
        QTextStream in(&file);
        file.close();
        return in.readLine().toInt();
    }
}

void BrightnessController::set_brightness(int brightness_value)
{
    if (brightness_value > BRIGHTNESS_MAX)
        brightness_value = BRIGHTNESS_MAX;

    if (brightness_value < BRIGHTNESS_MIN)
        brightness_value = BRIGHTNESS_MIN;

    if (brightness_value == get_brightness())
        return;

    QFile file (BRIGHTNESS_FILE);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream out(&file);
        out << QString::number(brightness_value);
        file.flush();
        file.close();
    }
}

int BrightnessController::get_power()
{
    QFile file (POWER_FILE);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
        return -1;
    }
    else
    {
        QTextStream in(&file);
        file.close();
        return in.readLine().toInt();
    }
}

void BrightnessController::set_power(int power_value)
{
    if (power_value > POWER_OFF) power_value = POWER_ON;
    if (power_value < POWER_ON) power_value = POWER_OFF;
    if (power_value == get_power()) return;

    QFile file (POWER_FILE);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream out(&file);
        out << QString::number(power_value);
        file.flush();
        file.close();
    }
}

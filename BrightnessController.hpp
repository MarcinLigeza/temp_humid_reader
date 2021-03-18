#ifndef BRIGHTNESSCONTROLLER_HPP
#define BRIGHTNESSCONTROLLER_HPP

class BrightnessController
{
public:
    BrightnessController();
    void set_brightness(int brightness_value);
    void set_power(int power_value);

private:
    int get_brightness();
    int get_power();

};

#endif // BRIGHTNESSCONTROLLER_HPP

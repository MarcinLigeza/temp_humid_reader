#ifndef TSL_WRAPPER_HPP
#define TSL_WRAPPER_HPP

#include <QObject>

class TSL_Wrapper : public QObject
{
    Q_OBJECT
public:
    explicit TSL_Wrapper(int pi, QObject *parent = nullptr);

    uint32_t calculate_lux(uint16_t iGain, uint16_t tIntCycles);

private:
    int pi;
    int handle{};

    uint16_t channel_0{};
    uint16_t channel_1{};

    void init_TSL2581(void);
    void reload_register(void);
    void set_interrupt_threshold(uint16_t min, uint16_t max);
    void read_channel(void);
    uint8_t read_ID();

    // Helper functions
    void i2c_write(uint8_t address, uint8_t data);
    uint8_t i2c_read(uint8_t address);
};

#endif // TSL_WRAPPER_HPP

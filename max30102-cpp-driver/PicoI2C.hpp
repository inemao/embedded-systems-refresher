#ifndef PICO_I2C_HPP
#define PICO_I2C_HPP

#include "I2CInterface.hpp"
#include "hardware/i2c.h"

class PicoI2C : public I2CInterface {
public:
    explicit PicoI2C(i2c_inst_t* i2c);

    int write(uint8_t address,
              const uint8_t* data,
              size_t length,
              bool nostop) override;

    int read(uint8_t address,
             uint8_t* data,
             size_t length,
             bool nostop) override;

private:
    i2c_inst_t* i2c_;
};

#endif
#include "PicoI2C.hpp"

PicoI2C::PicoI2C(i2c_inst_t* i2c)
    : i2c_(i2c) {
}

int PicoI2C::write(uint8_t address,
                   const uint8_t* data,
                   size_t length,
                   bool nostop) {
    return i2c_write_timeout_us(i2c_, address, data, length, nostop, 10000);;  
}

int PicoI2C::read(uint8_t address,
                  uint8_t* data,
                  size_t length,
                  bool nostop) {
    return i2c_read_timeout_us(i2c_, address, data, length, nostop, 10000);
}
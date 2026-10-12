#ifndef I2C_INTERFACE_HPP
#define I2C_INTERFACE_HPP

#include <cstdint>
#include <cstddef>

class I2CInterface {
public:
    virtual ~I2CInterface() = default;

    virtual int write(uint8_t address,
                      const uint8_t* data,
                      size_t length,
                      bool nostop) = 0;

    virtual int read(uint8_t address,
                     uint8_t* data,
                     size_t length,
                     bool nostop) = 0;
};

#endif
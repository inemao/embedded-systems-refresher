#include <cstdio>
#include "MAX30102.hpp"

MAX30102::MAX30102(I2CInterface& i2c, uint8_t address)
    : i2c_(i2c),
      address_(address) {
}


int MAX30102::read_register(Register reg, uint8_t* value){
    uint8_t reg_value = static_cast<uint8_t>(reg);

    int written = i2c_.write(address_, &reg_value, 1, true);
    if (written != 1) return -1;

    int read = i2c_.read(address_, value, 1, false);
    if (read != 1) return -2;

    return 1;
}

int MAX30102::write_register(Register reg, uint8_t value) {
    uint8_t data[2] = {static_cast<uint8_t>(reg), value};
    
    return i2c_.write(address_, data, 2, false);
}

bool MAX30102::initialize(const Config& config) {
    uint8_t part_id = 0;
    if(read_register(Register::PART_ID, &part_id) != 1) return false;
    if(part_id != EXPECTED_PART_ID) return false;

    uint8_t spo2_config_value =
    ((static_cast<uint8_t>(config.adc_range)  << 5) & 0b01100000) |
    ((static_cast<uint8_t>(config.sample_rate) << 2) & 0b00011100) |
    ((static_cast<uint8_t>(config.pulse_width) << 0) & 0b00000011);
    
    if (write_register(Register::SPO2_CONFIG, spo2_config_value) != 2) return false;
    if (write_register(Register::LED1_PA, config.red_led_current) != 2) return false;
    if (write_register(Register::LED2_PA, config.ir_led_current) != 2) return false;

    if (write_register(Register::FIFO_WR_PTR, 0x00) != 2) return false;
    if (write_register(Register::OVF_COUNTER, 0x00) != 2) return false;
    if (write_register(Register::FIFO_RD_PTR, 0x00) != 2) return false;
    if (write_register(Register::FIFO_CONFIG, 0x00) != 2) return false;

    if (write_register(Register::MODE_CONFIG, SPO2_MODE_VALUE) != 2) return false;

    return true;
}

bool MAX30102::read_sample(Sample& sample){
    uint8_t reg_value = static_cast<uint8_t>(Register::FIFO_DATA);
    uint8_t data[6];

    int bytes_written = i2c_.write(address_, &reg_value, 1, true);
    if (bytes_written != 1) return false;

    int bytes_read = i2c_.read(address_, data, 6, false);
    if (bytes_read != 6) return false;

    sample.red = (((uint32_t)data[0] << 16) |
        ((uint32_t)data[1] << 8)  |
        (uint32_t)data[2]) & 0x3FFFF;
    sample.ir = (((uint32_t)data[3] << 16) |
        ((uint32_t)data[4] << 8)  |
        (uint32_t)data[5]) & 0x3FFFF;
    return true;
}

 int MAX30102::available(){
    uint8_t write_ptr = 0;
    uint8_t read_ptr = 0;

    if(read_register(Register::FIFO_WR_PTR, &write_ptr) != 1) return -1;
    if(read_register(Register::FIFO_RD_PTR, &read_ptr) != 1) return -1;

    return (write_ptr - read_ptr) & 0x1F;
 }
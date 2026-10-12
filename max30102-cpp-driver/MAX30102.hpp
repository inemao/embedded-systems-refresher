#ifndef MAX30102_HPP
#define MAX30102_HPP

#include <cstdint>
#include "I2CInterface.hpp"

class MAX30102 {
public:
    struct Sample {
        uint32_t red;
        uint32_t ir;
    };

    enum class SampleRate : uint8_t {
    SPS_50   = 0b000,
    SPS_100  = 0b001,
    SPS_200  = 0b010,
    SPS_400  = 0b011,
    SPS_800  = 0b100,
    SPS_1000 = 0b101,
    SPS_1600 = 0b110,
    SPS_3200 = 0b111
    };

    enum class AdcRange : uint8_t {
    RANGE_2048  = 0b00,
    RANGE_4096  = 0b01,
    RANGE_8192  = 0b10,
    RANGE_16384 = 0b11
    };

    enum class PulseWidth : uint8_t {
    US_69  = 0b00,
    US_118 = 0b01,
    US_215 = 0b10,
    US_411 = 0b11
    };


    struct Config {
        AdcRange adc_range;
        SampleRate sample_rate;
        PulseWidth pulse_width;
        uint8_t red_led_current;
        uint8_t ir_led_current;
    };

    MAX30102(I2CInterface& i2c, uint8_t address);

    bool initialize(const Config& config);
    int available();
    bool read_sample(Sample& sample);

private:

    enum class Register : uint8_t {
    FIFO_WR_PTR = 0x04,
    OVF_COUNTER = 0x05,
    FIFO_RD_PTR = 0x06,
    FIFO_DATA = 0x07,
    FIFO_CONFIG = 0x08,
    MODE_CONFIG = 0x09,
    SPO2_CONFIG = 0x0A,
    LED1_PA = 0x0C,
    LED2_PA = 0x0D,
    PART_ID = 0xFF
    };

    static constexpr uint8_t EXPECTED_PART_ID = 0x15;
    static constexpr uint8_t SPO2_MODE_VALUE = 0x03;
    
    I2CInterface& i2c_;
    uint8_t address_;

    int read_register(Register reg, uint8_t* value);
    int write_register(Register reg, uint8_t value);
};

#endif
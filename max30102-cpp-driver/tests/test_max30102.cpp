#include <cassert>
#include <iostream>
#include "FakeI2C.hpp"
#include "../MAX30102.hpp"

void test_read_sample_success() {
    FakeI2C fake_i2c;

    fake_i2c.set_read_data({
        0x01, 0x23, 0x45,
        0x02, 0x34, 0x56
    });

    

    MAX30102 sensor(fake_i2c, 0x57);

    MAX30102::Sample sample;
    bool success = sensor.read_sample(sample);

    assert(success);
    assert(sample.red == 0x012345);
    assert(sample.ir == 0x023456);

    assert(fake_i2c.writes().size() == 1);

    const auto& write = fake_i2c.writes()[0];

    assert(write.address == 0x57);
    assert(write.data.size() == 1);
    assert(write.data[0] == 0x07);
    assert(write.nostop == true);
}


void test_read_sample_write_failure() {
    FakeI2C fake_i2c;

    fake_i2c.set_write_result(-1);

    MAX30102 sensor(fake_i2c, 0x57);

    MAX30102::Sample sample;
    bool success = sensor.read_sample(sample);

    assert(!success);
}

void test_read_sample_read_failure() {
    FakeI2C fake_i2c;
    
    fake_i2c.set_read_result(-1);

    MAX30102 sensor(fake_i2c, 0x57);

    MAX30102::Sample sample;
    bool success = sensor.read_sample(sample);



    assert(!success);

    assert(fake_i2c.writes().size() == 1);

    const auto& write = fake_i2c.writes()[0];

    assert(write.address == 0x57);
    assert(write.data.size() == 1);
    assert(write.data[0] == 0x07);
    assert(write.nostop == true);

    
}

void test_initialize_success() {
    FakeI2C fake_i2c;

    fake_i2c.set_read_data({0x15});

    MAX30102 sensor(fake_i2c, 0x57);

    MAX30102::Config config;
    config.adc_range = MAX30102::AdcRange::RANGE_4096;
    config.sample_rate = MAX30102::SampleRate::SPS_100;
    config.pulse_width = MAX30102::PulseWidth::US_411;
    config.red_led_current = 0x32;
    config.ir_led_current = 0x32;

    bool success = sensor.initialize(config);

    assert(success);

    const auto& writes = fake_i2c.writes();

    assert(writes[0].address == 0x57);
    assert(writes[0].data.size() == 1);
    assert(writes[0].data[0] == 0xFF);
    assert(writes[0].nostop == true);

    assert(writes.size() >= 2);

    assert(writes[1].address == 0x57);
    assert(writes[1].data.size() == 2);
    assert(writes[1].data[0] == 0x0A);
    assert(writes[1].data[1] == 0x27);
    assert(writes[1].nostop == false);

    assert(writes.size() == 9);

    assert(writes[2].data.size() == 2);
    assert(writes[2].data[0] == 0x0C);
    assert(writes[2].data[1] == 0x32);
    assert(writes[2].nostop == false);

    assert(writes[3].data.size() == 2);
    assert(writes[3].data[0] == 0x0D);
    assert(writes[3].data[1] == 0x32);
    assert(writes[3].nostop == false);

    // Reset FIFO write pointer
    assert(writes[4].data.size() == 2);
    assert(writes[4].data[0] == 0x04);
    assert(writes[4].data[1] == 0x00);
    assert(writes[4].nostop == false);

    // Reset FIFO overflow counter
    assert(writes[5].data.size() == 2);
    assert(writes[5].data[0] == 0x05);
    assert(writes[5].data[1] == 0x00);
    assert(writes[5].nostop == false);

    // Reset FIFO read pointer
    assert(writes[6].data.size() == 2);
    assert(writes[6].data[0] == 0x06);
    assert(writes[6].data[1] == 0x00);
    assert(writes[6].nostop == false);

    assert(writes[7].data.size() == 2);
    assert(writes[7].data[0] == 0x08);
    assert(writes[7].data[1] == 0x00);
    assert(writes[7].nostop == false);

    assert(writes[8].data.size() == 2);
    assert(writes[8].data[0] == 0x09);
    assert(writes[8].data[1] == 0x03);
    assert(writes[8].nostop == false);
}

void test_initialize_wrong_part_id() {
    FakeI2C fake_i2c;

    fake_i2c.set_read_data({0x16});

    MAX30102 sensor(fake_i2c, 0x57);

    // Create the same Config used in test_initialize_success()
    MAX30102::Config config;
    config.adc_range = MAX30102::AdcRange::RANGE_4096;
    config.sample_rate = MAX30102::SampleRate::SPS_100;
    config.pulse_width = MAX30102::PulseWidth::US_411;
    config.red_led_current = 0x32;
    config.ir_led_current = 0x32;

    // Call sensor.initialize(config)
    bool success = sensor.initialize(config);

    // Assert that initialization failed
    assert(!success);
    assert(fake_i2c.writes().size() == 1);

    const auto& write = fake_i2c.writes()[0];

    assert(write.address == 0x57);
    assert(write.data.size() == 1);
    assert(write.data[0] == 0xFF);
    assert(write.nostop == true);
}

void test_available_wraparound() {
    FakeI2C fake_i2c;

    fake_i2c.queue_read_data({3});   // FIFO write pointer
    fake_i2c.queue_read_data({29});  // FIFO read pointer

    MAX30102 sensor(fake_i2c, 0x57);

    int unread = sensor.available();

    assert(unread == 6);

    const auto& writes = fake_i2c.writes();

    assert(writes.size() == 2);

    assert(writes[0].data.size() == 1);
    assert(writes[0].data[0] == 0x04);
    assert(writes[0].nostop == true);

    assert(writes[1].data.size() == 1);
    assert(writes[1].data[0] == 0x06);
    assert(writes[1].nostop == true);
}

void test_available_write_pointer_failure() {
    FakeI2C fake_i2c;

    // Make the first I²C read fail.

    MAX30102 sensor(fake_i2c, 0x57);
    fake_i2c.set_read_result(-1);
    // Call available().
    int result = sensor.available();
    // Verify that the result is -1.
    assert(result == -1);

    assert(fake_i2c.writes().size() == 1);

    const auto& write = fake_i2c.writes()[0];

    assert(write.address == 0x57);
    assert(write.data.size() == 1);
    assert(write.data[0] == 0x04);
    assert(write.nostop == true);       
}

int main() {
    test_read_sample_success();
    test_read_sample_write_failure();
    test_read_sample_read_failure();
    test_initialize_success();
    test_initialize_wrong_part_id();
    test_available_wraparound();
    test_available_write_pointer_failure();

    std::cout << "All tests passed\n";
}
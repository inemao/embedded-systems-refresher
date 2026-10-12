#ifndef FAKE_I2C_HPP
#define FAKE_I2C_HPP

#include "../I2CInterface.hpp"
#include <vector>
#include <deque>

class FakeI2C : public I2CInterface {
public:

    struct WriteTransaction {
        uint8_t address;
        std::vector<uint8_t> data;
        bool nostop;
    };

    int write(uint8_t address,
              const uint8_t* data,
              size_t length,
              bool nostop) override;

    int read(uint8_t address,
             uint8_t* data,
             size_t length,
             bool nostop) override;

    void set_read_data(const std::vector<uint8_t>& data);
    const std::vector<WriteTransaction>& writes() const;
    void set_write_result(int result);
    void set_read_result(int result);

    void queue_read_data(const std::vector<uint8_t>& data);

private:
    bool override_write_result_ = false;
    int write_result_ = 0;

    bool override_read_result_ = false;
    int read_result_ = 0;

    std::vector<uint8_t> read_data_;
    std::vector<WriteTransaction> writes_;

    std::deque<std::vector<uint8_t>> read_responses_;
    

};

#endif
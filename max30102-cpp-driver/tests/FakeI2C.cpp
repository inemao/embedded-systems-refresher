#include "FakeI2C.hpp"

void FakeI2C::set_read_data(const std::vector<uint8_t>& data) {
    read_data_ = data;
}

int FakeI2C::write(uint8_t address,
                   const uint8_t* data,
                   size_t length,
                   bool nostop) {

    WriteTransaction transaction;

    transaction.address = address;
    transaction.nostop = nostop;

    for (size_t i = 0; i < length; i++) {
        transaction.data.push_back(data[i]);
    }

    writes_.push_back(transaction);

    if (override_write_result_) {
        return write_result_;
    }

    return length;
}

int FakeI2C::read(uint8_t,
                  uint8_t* data,
                  size_t length,
                  bool) {

    if (override_read_result_) {
        return read_result_;
    }

    std::vector<uint8_t> response;

    if (!read_responses_.empty()) {
        response = read_responses_.front();
        read_responses_.pop_front();
    } else {
        response = read_data_;
    }

    if (response.size() < length) {
        return -1;
    }

    for (size_t i = 0; i < length; i++) {
        data[i] = response[i];
    }

    return static_cast<int>(length);
}


const std::vector<FakeI2C::WriteTransaction>& FakeI2C::writes() const {
    return writes_;
}

void FakeI2C::set_write_result(int result) {
    write_result_ = result;
    override_write_result_ = true;
}

void FakeI2C::set_read_result(int result) {
    read_result_ = result;
    override_read_result_ = true;
}

void FakeI2C::queue_read_data(const std::vector<uint8_t>& data) {
    read_responses_.push_back(data);
}
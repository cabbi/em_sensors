#ifndef __ADS1X15_H_
#define __ADS1X15_H_


#include <iostream>
#include <system_error>
#include "i2c.hpp"
#include "ads1x15.hpp"

/*
espp::Ads1x15::Gain::TWOTHIRDS: Range di ±6.144V
espp::Ads1x15::Gain::ONE: Range di ±4.096V
espp::Ads1x15::Gain::TWO: Range di ±2.048V
espp::Ads1x15::Gain::FOUR: Range di ±1.024V
espp::Ads1x15::Gain::EIGHT: Range di ±0.512V
espp::Ads1x15::Gain::SIXTEEN: Range di ±0.256V
*/

class EmAds1x115 {
public:
    EmAds1x115(gpio_num_t sda_pin, gpio_num_t scl_pin, 
                  espp::Ads1x15::Gain gain = espp::Ads1x15::Gain::ONE)
        : m_i2c({
              .port = I2C_NUM_1,
              .sda_io_num = sda_pin,
              .scl_io_num = scl_pin,
              .clk_speed = 400000
          }),
          m_ads(espp::Ads1x15::Ads1115Config{
              .device_address = espp::Ads1x15::DEFAULT_ADDRESS,
              .write = [this](uint8_t addr, const uint8_t *data, size_t len) { 
                  return i2c_bus_.write(addr, data, len); 
              },
              .read = [this](uint8_t addr, uint8_t *data, size_t len) { 
                  return i2c_bus_.read(addr, data, len); 
              },
              .gain = gain
          }) 
    {}

    float readChannelVoltage(int channel) {
        if (channel < 0 || channel > 3) return 0.0f;

        std::error_code ec;
        float millivolts = ads_.sample_mv(channel, ec);
        if (ec) {
            return 0.0f;
        }
        return millivolts / 1000.0f;
    }

private:
    espp::I2c m_i2c;
    espp::Ads1x15 m_ads;
};


#endif __ADS1X15_H_
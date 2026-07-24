#ifndef __ADS1X15_H_
#define __ADS1X15_H_


#include <iostream>
#include <system_error>

#include <ads1x15.hpp>

#include "em_i2c.h"

class EmAds1x15 {
public:
    enum class Gain {
        Range_6v144 = static_cast<int>(espp::Ads1x15::Gain::TWOTHIRDS),
        Range_4v096 = static_cast<int>(espp::Ads1x15::Gain::ONE),
        Range_2v048 = static_cast<int>(espp::Ads1x15::Gain::TWO),
        Range_1v024 = static_cast<int>(espp::Ads1x15::Gain::FOUR),
        Range_0v512 = static_cast<int>(espp::Ads1x15::Gain::EIGHT),
        Range_0v256 = static_cast<int>(espp::Ads1x15::Gain::SIXTEEN)
    };

    EmAds1x15(EmI2c& sharedI2c, 
              Gain gain,
              uint8_t address = espp::Ads1x15::DEFAULT_ADDRESS)
        : m_sharedI2c(sharedI2c),
          m_ads(espp::Ads1x15::Ads1115Config{
              .device_address = address,
              .write = [this](uint8_t addr, const uint8_t *data, size_t len) { 
                  return this->m_sharedI2c.write(addr, data, len); 
              },
              .read = [this](uint8_t addr, uint8_t *data, size_t len) { 
                  return this->m_sharedI2c.read(addr, data, len); 
              },
              .gain = static_cast<espp::Ads1x15::Gain>(gain)
          }) 
    {}

    bool readChannelVoltage(int channel, float& voltage) {
        if (channel < 0 || channel > 3) return 0.0f;

        std::error_code ec;
        voltage = m_ads.sample_mv(channel, ec);
        if (ec) {
            voltage = 0.0f;
            return false;
        }
        voltage /= 1000.0f;
        return true;
    }

private:
    EmI2c& m_sharedI2c;
    espp::Ads1x15 m_ads;
};


#endif //__ADS1X15_H_
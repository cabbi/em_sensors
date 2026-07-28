#ifndef _I2C_BUS_H__
#define _I2C_BUS_H__

#include <stdint.h>

#include <i2c.hpp>

// A I2C wrapper around the ESP's one just to have a layer in case
// of future support of new frameworks (e.g. Arduino)
class EmI2c: public espp::I2c {
public:
    EmI2c(gpio_num_t sdaPin,  
          gpio_num_t sclPin, 
          bool autoInit = true,
          i2c_port_t port = (i2c_port_t)-1, 
          uint32_t timeoutMs = 10,
          uint32_t clkSpeed = 100000) :
        espp::I2c(Config {
            .isr_core_id = -1,
            .port = port,
            .sda_io_num = sdaPin,
            .scl_io_num = sclPin,
            .sda_pullup_en = GPIO_PULLUP_DISABLE,
            .scl_pullup_en = GPIO_PULLUP_DISABLE,
            .timeout_ms = timeoutMs,
            .clk_speed = clkSpeed,
            .auto_init = autoInit,
            .log_level = espp::Logger::Verbosity::WARN
        }) { }

    virtual ~EmI2c() = default;

    virtual bool begin() {
        std::error_code err;
        init(err);
        return err.value() == ESP_OK;
    }

    virtual bool end() {
        std::error_code err;
        deinit(err);
        return err.value() == ESP_OK;
    }

    virtual bool isInitialized() const { return initialized(); }

    virtual i2c_port_t getPort() const { return config_.port; }
    virtual gpio_num_t getSdaGpio() const { return config_.sda_io_num; }
    virtual gpio_num_t getSclGpio() const { return config_.scl_io_num; }
};

#endif //_I2C_BUS_H__

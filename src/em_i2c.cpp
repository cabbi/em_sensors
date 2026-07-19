#include "em_i2c.h"


EmI2c::EmI2c() 
    : m_port(I2C_NUM_MAX), m_initialized(false) {}

EmI2c::~EmI2c() {
    end();
}

bool EmI2c::begin(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed) {
    if (m_initialized) {
        return true;
    }

    m_port = port;

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_pin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl_pin;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = clk_speed;

    esp_err_t err = i2c_param_config(m_port, &conf);
    if (err != ESP_OK) {
        return false;
    }

    err = i2c_driver_install(m_port, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        return false;
    }

    m_initialized = true;
    return true;
}

void EmI2c::end() {
    if (m_initialized && m_port < I2C_NUM_MAX) {
        i2c_driver_delete(m_port);
    }
}
	
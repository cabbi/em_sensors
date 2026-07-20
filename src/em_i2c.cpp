#include "em_i2c.h"

EmI2C::EmI2C() 
    :  m_port(I2C_NUM_MAX), 
       m_bus_handle(nullptr), 
       m_initialized(false) {}

EmI2C::~EmI2C() {
    end();
}

bool EmI2C::begin(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed) {
    if (m_initialized) {
        return true;
    }
    m_port = port;
    i2c_master_bus_config_t bus_config = {};
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.i2c_port = m_port;
    bus_config.scl_io_num = scl_pin;
    bus_config.sda_io_num = sda_pin;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = true; // Sostituisce GPIO_PULLUP_ENABLE

    esp_err_t err = i2c_new_master_bus(&bus_config, &m_bus_handle);
    if (err != ESP_OK) {
        m_bus_handle = nullptr;
        m_port = I2C_NUM_MAX;
        return false;
    }

    m_initialized = true;
    return true;
}

void EmI2C::end() {
    if (m_initialized && m_bus_handle != nullptr) {
        if (i2c_del_master_bus(m_bus_handle) == ESP_OK) {
            m_bus_handle = nullptr;
            m_port = I2C_NUM_MAX;
            m_initialized = false;
        }
    }
}

#ifndef _I2C_BUS_H__
#define _I2C_BUS_H__

#include <driver/i2c_master.h>
#include <hal/gpio_types.h>

class EmI2C {
public:
    EmI2C();
    ~EmI2C();

    bool begin(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed = 100000);    
    void end();

    i2c_port_t getPort() const { return m_port; }
    i2c_master_bus_handle_t getBusHandle() const { return m_bus_handle; }
    bool isInitialized() const { return m_initialized; }

private:
    i2c_port_t m_port;
    i2c_master_bus_handle_t m_bus_handle; 
    bool m_initialized;
};

#endif //_I2C_BUS_H__

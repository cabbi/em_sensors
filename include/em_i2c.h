#ifndef _I2C_BUS_H__
#define _I2C_BUS_H__

#include <driver/i2c.h>

class EmI2c {
public:
    EmI2c();
    ~EmI2c();

    bool begin(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed = 100000);    
	void end();

    i2c_port_t getPort() const { return m_port; }
    bool isInitialized() const { return m_initialized; }

private:
    i2c_port_t m_port; 
    bool m_initialized;
};

#endif //_I2C_BUS__

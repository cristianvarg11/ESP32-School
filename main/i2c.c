///--------- Inclusion de librerias -----------//
#include "driver/i2c.h"
#include "driver/gpio.h"
//--------------------------------------------//
#define pinSDA 19
#define pinSCL 18
#define i2c_clock 100000
//------------ Func iniciar_i2c -------------//
void iniciar_i2c(){
    i2c_config_t configuracion;
    configuracion.mode = I2C_MODE_MASTER;
    configuracion.sda_io_num = pinSDA;
    configuracion.scl_io_num = pinSCL;
    configuracion.scl_pullup_en = GPIO_PULLUP_ENABLE;
    configuracion.sda_pullup_en = GPIO_PULLUP_ENABLE;
    configuracion.master.clk_speed = i2c_clock;
    i2c_param_config(I2C_NUM_0, &configuracion);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0,0,0);
}
//-------------------------------------------//

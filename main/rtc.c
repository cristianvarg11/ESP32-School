//--------- Inclusion de librerias ----------//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <string.h>
//------- Macros y Definiciones-------------//
#define ds3231 0x68
#define SecZ   0x00  // 0 seg
#define MinZ   0x18  // 18 min
#define HorZ   0x48  // Formato para 8am
#define DiaZ   0x02  // Dia lunes
#define FecZ   0x15  // Fecha 15
#define MesZ   0x06  // junio
#define AniZ   0x20  // 2020
//-----------------------------------------//
//======== Func enviar_ds3231 =============//
static void enviar_ds3231(){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start (cmd);
    i2c_master_write_byte (cmd, (ds3231 << 1) | I2C_MASTER_WRITE,1);
    i2c_master_write_byte (cmd, 0x0,1);
    i2c_master_write_byte (cmd, SecZ,1);
    i2c_master_write_byte (cmd, MinZ,1);
    i2c_master_write_byte (cmd, HorZ,1);
    i2c_master_write_byte (cmd, DiaZ,1);
    i2c_master_write_byte (cmd, FecZ,1);
    i2c_master_write_byte (cmd, MesZ,1);
    i2c_master_write_byte (cmd, AniZ,1);
    i2c_master_stop (cmd);
    i2c_master_cmd_begin (I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
    i2c_cmd_link_delete (cmd);
}
//========Func leer_ds3231 ==================//
char* leer_ds3231(uint8_t* seg, uint8_t* min, uint8_t* hor, uint8_t* fec, uint8_t* mes, uint8_t* ani, uint8_t* ampm){
    char* diax[]= {"Domingo","Lunes","Martes","Miercoles","Jueves","Viernes","Sabado"};
    uint8_t segundos, minutos, horas, dia, fecha, meses, anios;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start (cmd);
    i2c_master_write_byte (cmd, (ds3231 << 1) |I2C_MASTER_WRITE,1);
    i2c_master_write_byte (cmd, 0x0, 1);
    i2c_master_start (cmd);
    i2c_master_write_byte (cmd, (ds3231 >> 1) |I2C_MASTER_READ, 1);
    i2c_master_read (cmd, &segundos, 1, 0);
    i2c_master_read (cmd, &minutos, 1, 0);
    i2c_master_read (cmd, &horas, 1, 0);
    i2c_master_read (cmd, &dia, 1, 0);
    i2c_master_read (cmd, &fecha, 1, 0);
    i2c_master_read (cmd, &meses, 1, 0);
    i2c_master_read (cmd, &anios, 1, 1);
    i2c_master_stop (cmd);
    i2c_master_cmd_begin (I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
    i2c_cmd_link_delete (cmd);
    *seg = segundos;
    *min = minutos;
    *hor = 0x1F & horas;
    *fec = fecha;
    *mes = meses;
    *ani = anios;
    if (horas & 0x20)*ampm = 1; else *ampm = 0;
    return diax[dia - 1];
}
//========= Task RTOS para DS3231 ==========//
void TareaDS3231(void *P){
    uint8_t seg,min,hor,fec,mes,ani,ampm;
    char diax[20], pmam[2];
    enviar_ds3231();  // Solo se deja la primera vez a usar para determinar la hora cero, luego, se comenta
    for(;;){
        strcpy(diax, leer_ds3231(&seg,&min,&hor,&fec,&mes,&ani,&ampm));
        if(ampm)
            memcpy(pmam, "PM", 2);
        else
            memcpy(pmam, "AM", 2);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
//===========================================//

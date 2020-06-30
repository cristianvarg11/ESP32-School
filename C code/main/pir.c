//--------- Inclusion de librerias -------//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "string.h"
//----------Macros y Definiciones--------//
#define DATA_PIR GPIO_NUM_4
#define BUZZER GPIO_NUM_22 // se empleara un led
// #define PULSADOR GPIO_NUM_0
//--------------------------------------//
//-------Declaraciones------------------//
uint8_t cont_alarma = 0;
struct ALARMA{
    uint8_t anio;
    uint8_t mes;
    uint8_t fecha;
    uint8_t seg;
    uint8_t min;
    uint8_t hora;
    uint8_t formato_hora;
    char dia[10];
    char ampm[2];
} Registros_alarma[20];
extern char* leer_ds3231(uint8_t* seg, uint8_t* min, uint8_t* hor, uint8_t* fec, uint8_t* mes, uint8_t* ani, uint8_t* ampm);
extern xQueueHandle Alarmas;
//======= Task RTOS TareaPIR(se detecto algun mov?)===========//
void TareaPir (void *P){
    gpio_set_direction(DATA_PIR, GPIO_MODE_INPUT);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    leer_ds3231(&Registros_alarma[0].seg,
                &Registros_alarma[0].min,
                &Registros_alarma[0].hora,
                &Registros_alarma[0].fecha,
                &Registros_alarma[0].mes,
                &Registros_alarma[0].anio,
                &Registros_alarma[0].formato_hora);
    for (;;){
        if (gpio_get_level(DATA_PIR) || (Registros_alarma[cont_alarma].hora >= 6 &&  Registros_alarma[cont_alarma].hora <= 12 &&
            strcmp(Registros_alarma[cont_alarma].ampm, "PM") == 0) || (Registros_alarma[cont_alarma].hora >= 1
            && Registros_alarma[cont_alarma].hora <= 7 && strcmp(Registros_alarma[cont_alarma].ampm,"AM") == 0) ||
            (strcmp(Registros_alarma[cont_alarma].dia, "Sabado") == 0) || (strcmp(Registros_alarma[cont_alarma].dia, "Domingo") == 0)){
            //--------- obterner lectura del RTC-------------//
            strcpy(Registros_alarma[cont_alarma].dia,
                    leer_ds3231(&Registros_alarma[cont_alarma].seg,
                    &Registros_alarma[cont_alarma].min,
                    &Registros_alarma[cont_alarma].hora,
                    &Registros_alarma[cont_alarma].fecha,
                    &Registros_alarma[cont_alarma].mes,
                    &Registros_alarma[cont_alarma].anio,
                    &Registros_alarma[cont_alarma].formato_hora));
            //--------obtener siglas formato hora------------//
            if(Registros_alarma[cont_alarma].formato_hora)
                strcpy(Registros_alarma[cont_alarma].ampm,"PM");
            else
                strcpy(Registros_alarma[cont_alarma].ampm,"AM");
            //------- Encender buzzer por 2 seg ------------//
            gpio_set_level(BUZZER,1);
            vTaskDelay(2000/portTICK_PERIOD_MS);
            gpio_set_level(BUZZER,0);
            //----------------------------------------------//
            cont_alarma++;
            if(cont_alarma>=20)cont_alarma=0;
        }
        xQueueOverwrite(Alarmas, &cont_alarma);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

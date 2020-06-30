//--------------- Includion de librerias ------------//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <string.h>
//-------------------Definiciones--------------------//
#define LED_A 21
#define LED_B 22
#define LED_C 23
extern xQueueHandle Cola_leds;
extern xQueueHandle topicox;
char mensaje_led[];
char topico_led[];
char aula_a[] = {"AA"}; char aula_b[] = {"AB"}; char aula_c[] = {"AC"};
//---------------------------------------------------//
//-------------Config GPIO ---------------------------//
static void configGPIO(){
    gpio_set_direction(LED_A, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(LED_B, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(LED_C, GPIO_MODE_INPUT_OUTPUT);
}
//------------ On/Off -------------------------------//
static void statex_led(char Aula, int Led){
    vTaskDelay(60000/portTICK_PERIOD_MS);  // solo tomar el payload de cada 60 seg
	xQueueReceive(Cola_leds, &mensaje_led, portMAX_DELAY);  // recibir datos de payload
	xQueueReceive(topicox, &topico_led, portMAX_DELAY);  // recibir nombre del topic
	if ((strncmp(mensaje_led, "on",2) == 0) || (strncmp(topico_led, Aula,2) == 0)){
        gpio_set_level(Led, 1); }
        else{
            if((strncmp(mensaje_led, "oo",2) == 0) || (strncmp(topico_led, Aula,2) == 0)){
                gpio_set_level(Led, 0);
            }
        }
}
//------------- Bloque e ilum -----------------------//
void  Tareilum(void *P){
    configGPIO();
    for (;;){
        statex_led(aula_a, LED_A);
        statex_led(aula_b, LED_B);
        statex_led(aula_c, LED_C);
    }
}

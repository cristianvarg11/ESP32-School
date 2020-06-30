//------------ Inclusion de librerias ---------------//
#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <string.h>
#include "esp_log.h"
//------------ Definiciones ------------------------//
#define us_retardo 1
#define numDHT_bits 40
#define numDHT_bytes 5
#define DHTpin 4
//------------ Variables globales ------------------//
extern xQueueHandle Cola_sensor;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
//--------------------------------------------------//

//------------ Func Tiempo de espera ---------------//
static esp_err_t TiempoDeEspera(gpio_num_t pin, uint32_t timeout, int valor_esperado, uint32_t *contador_us){
    gpio_set_direction(pin,GPIO_MODE_INPUT);
    for(uint32_t i = 0; i < timeout; i+= us_retardo){
        ets_delay_us(us_retardo);
        if(gpio_get_level(pin) == valor_esperado){
            if(contador_us) *contador_us = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}
//---------------------------------------------------//

//---------------Func capturar dato-nums ---------------//
static esp_err_t CapturarDatos(gpio_num_t pin, uint8_t datos[numDHT_bytes]){
    uint32_t tiempo_low;
    uint32_t tiempo_high;
    //Inicio de la secuencia de lectura
    gpio_set_direction(pin,GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 0);
    ets_delay_us(20000);
    gpio_set_level(pin,1);
    //se espera hasta 40us por un flanco de bajada desde el DHT
    if(TiempoDeEspera(pin, 40, 0, NULL) != ESP_OK)return ESP_ERR_TIMEOUT;
    //se espera hasta 90us por un flanco de subida
    if(TiempoDeEspera(pin, 90, 1, NULL) != ESP_OK)return ESP_ERR_TIMEOUT;
    //se espera hasta 90us por un flanco de bajada
    if(TiempoDeEspera(pin, 90, 1, NULL) != ESP_OK)return ESP_ERR_TIMEOUT;
    //Si la resp fue satisfactoria, empieza la captuda de datos
    for(int i=0; i < numDHT_bits; i++){
        if(TiempoDeEspera(pin, 60, 1, &tiempo_low) != ESP_OK)return ESP_ERR_TIMEOUT;
        if(TiempoDeEspera(pin, 75, 0, &tiempo_high) != ESP_OK)return ESP_ERR_TIMEOUT;
            uint8_t b = i/8 , m = i % 8;
            if(!m)datos[b] = 0;
            datos[b] |= (tiempo_high > tiempo_low) << (7-m);
    }
    return ESP_OK;
}
//---------------------------------------------------------//

//----------- Func Leer DHT11 -----------------------------//
static esp_err_t leerDHT(gpio_num_t pin,uint8_t *humedad, uint8_t *temperatura, uint8_t *decimal){
    uint8_t datos[numDHT_bytes] = {0,0,0,0,0};
    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 1);

    portENTER_CRITICAL(&mux);
    esp_err_t resultado = CapturarDatos(pin, datos);
    portEXIT_CRITICAL(&mux);

    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(pin, 1);
    if(resultado != ESP_OK)return resultado;
    if(datos[4] != ((datos[0] + datos[1] + datos[2] + datos[3]) & 0xFF)){
        ESP_LOGE("Sensor_DHT11","Error en verificacion de Checksum");
        return ESP_ERR_INVALID_CRC;
    }
    *humedad = datos[0];
    *temperatura = datos[2];
    *decimal = datos[3];
    return ESP_OK;
}
//-------------------------------------------------------//

//--------- Task DHT11 ---------------------------------//
void TareaDHT(void *P){
    uint8_t temperatura = 0, decimal = 0;
    uint8_t humedad = 0;
    char datos_sensor[] = {"00-000"};
    for(;;){
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        if (leerDHT(DHTpin, &humedad, &temperatura, &decimal) == ESP_OK){
            ESP_LOGI("DHT11","Humedad %d%% Temperatura: %d.%dC",humedad,temperatura,decimal);
            datos_sensor[0] = (humedad / 10)+'0';
            datos_sensor[1] = (humedad % 10)+'0';
            datos_sensor[3] = (temperatura / 10)+'0';
            datos_sensor[4] = (temperatura % 10)+'0';
            datos_sensor[5] = (decimal % 10)+'0';
            xQueueOverwrite(Cola_sensor,&datos_sensor);  // mismos parametros que xQueueSend
        }
        else
            ESP_LOGE("DHT11","No fue posible leer datos del DHT11");
    }
}

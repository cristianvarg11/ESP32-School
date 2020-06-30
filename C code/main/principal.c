//--------- Inclusion de librerias ------------//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "freertos/queue.h"
#include "esp_log.h"
//--------- Headers ---------------------------//
#include "Wificonfig.h"
#include "mqtt.h"
#include "dht.h"
#include "rtc.h"
#include "pir.h"
#include "Ilum.h"
//-------- Definiciones y macros --------------//
#define Pila 1024
#define punteros 1
#define bytes 6
//-------- Variables globales -----------------//
xQueueHandle Cola_sensor;  // cola para DHT11
xQueueHandle Cola_leds;    // cola para almacenar mensaje de on/off
xQueueHandle topicox;      // cola para almacenar el topico
xQueueHandle Alarmas;      // cola para alarmas del PIR
//---------------------------------------------//
//============ APP main =======================//
void app_main(){
    ESP_LOGW("inicio", "iniciando aplicacion...");
    ESP_LOGW("inicio", "Memoria disponible: %d bytes", esp_get_free_heap_size());
    ESP_LOGW("inicio", "Version ESP-IDF: %s", esp_get_idf_version());
        Cola_sensor = xQueueCreate(punteros, bytes);
        nvs_flash_init();
        iniciar_wifi();
        xTaskCreatePinnedToCore(TareaDHT, "Temp & Humd", Pila*4, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(TareaMQTT, "MQTT", Pila*4, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(TareaPir, "Seguridad", Pila*4, NULL, 2, NULL, 0);
        xTaskCreatePinnedToCore(TareaDS3231, "Iniciar rtc", Pila*4, NULL, 2, NULL, 0); // solo la primera vez para inicializar el RTC
        xTaskCreatePinnedToCore(Tareilum, "Estado de LEDS", Pila*4, NULL, 2, NULL, 0 );
}

//---------- Inclusion de librerias ------------//
#include <string.h>
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "mqtt_client.h"
//---------- Definiciones y macros -------------//
#define cliente_i     "ciexpro_03"
#define direccionIP "192.168.43.214"
#define puertoMQTT  5050
#define General    "general"  //topico general de todas las aulas
#define AA         "AA"       // Topico aula A
#define AB         "AB"       // Topico aula B
#define AC         "AC"    	  // Topico Aula C
static const char *TAG = "MQTT";
extern xQueueHandle Cola_sensor;
extern xQueueHandle Cola_leds;
extern xQueueHandle topicox;
extern xQueueHandle Alarmas;
char mensaje_led[] = {"dd"};  		 // almacenar el mensaje de on/off recibido en MQTT
char topico_led[]= {"dd"};  		 	// almacenar el topico desde el cual llego el mensaje de MQTT
//----------- Manejador de eventos MQTT --------//
static esp_err_t eventosMQTT(esp_mqtt_event_handle_t event){
    esp_mqtt_client_handle_t client = event -> client;
    int msg_id;
    switch(event -> event_id){

        case MQTT_EVENT_CONNECTED:  // Conexion
            ESP_LOGI(TAG, "MQTT EVENT CONNECTED");
            //----- Suscripcion a topics -----//
            msg_id = esp_mqtt_client_subscribe(client, General, 0);
            msg_id = esp_mqtt_client_subscribe(client, AA, 0);
            msg_id = esp_mqtt_client_subscribe(client, AB, 0);
            msg_id = esp_mqtt_client_subscribe(client, AC, 0);
            ESP_LOGI(TAG, "Suscripcion exitosa, id = %d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:  // Desconexion
            ESP_LOGI(TAG, "MQTT desconectado.");
            break;
        case MQTT_EVENT_SUBSCRIBED:  //Suscripcion
            ESP_LOGI(TAG, "Suscripcion de topic, id = %d", event -> msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:  // Suscripcion finalizada
            ESP_LOGI(TAG, "Suscripcion finalizada, id = %d", event -> msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:  // Publicacion
            ESP_LOGI(TAG, "Publicacion realizada, id = %d", event -> msg_id);
            break;
        case MQTT_EVENT_DATA:  // Recepcion de informacion
            ESP_LOGW(TAG, "Datp recibido");
            printf("Topic = %.*s\r\n", event -> topic_len, event -> topic);
            printf("Data = %.*s\r\n", event -> data_len, event -> data);
            //--------------------------------------------//
            strncpy(mensaje_led, event -> data, 2);  // copiar solo los dos primeros char de payload
            strncpy(topico_led, event -> data,2);   // copiar solo los dos primeros  char de topico del payload
            xQueueOverwrite(Cola_leds, &mensaje_led);
            xQueueOverwrite(topicox, &topico_led);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT EVENT ERROR");
            break;
        default:
            ESP_LOGE(TAG, "Otro evento, id = %d ", event -> event_id);
            break;
    }
    return ESP_OK;
}
//---------------- Manejador MQTT --------------------//
static void ManejadorMQTT(void *handle_args, esp_event_base_t base, int32_t event_id, void *event_data){
    ESP_LOGD(TAG, "Evento activo de tipo: %s, id: %d", base, event_id);
    eventosMQTT(event_data);
}
//--------------- Task MQTT -------------------------//
void TareaMQTT(void *P){
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = direccionIP,
        .port = puertoMQTT,
        .client_id = cliente_i,
    };
    //--- Inicializacion de APIs ---//
    esp_mqtt_client_handle_t cliente = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(cliente, ESP_EVENT_ANY_ID, ManejadorMQTT, cliente);
    esp_mqtt_client_start(cliente);
    // ---- Variables locales ---- //
    char datosDHT[6];
    char datos_al[2];
    char HR_dht[] = {"Humedad R.: 00%"};
    char T_dht[] = {"Temperatura: 00.0C"};
    char alarms[] = {"Alarmas registradas: 00"};
    //---------------------------//
    while(1){
        //----- temp/humd -----//
        xQueueReceive (Cola_sensor, &datosDHT, portMAX_DELAY);  // Sensor de temp&humd
            HR_dht[12] = datosDHT[0];
            HR_dht[13] = datosDHT[1];
            T_dht[13] = datosDHT[3];
            T_dht[14] = datosDHT[4];
            T_dht[16] = datosDHT[5];
        esp_mqtt_client_publish(cliente, General, HR_dht, strlen(HR_dht), 0, 0);  // Humedad relativa
        esp_mqtt_client_publish(cliente, General, T_dht, strlen(T_dht), 0, 0);  // Temperatura
        //----- Alarmas PIR ---//
        xQueueReceive (Alarmas, &datos_al, portMAX_DELAY); // PIR
            alarms[21] = datos_al[0];
            alarms[22] = datos_al[1];
        esp_mqtt_client_publish(cliente, General, alarms, strlen(alarms), 0, 0); // cantidad de alarmas
    }
}

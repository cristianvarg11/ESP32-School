//---------- Inclusion de librerias ------------//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
//--------- SSID & Password --------------------//
#define nombreWiFi "red"
#define claveWiFi "clave"

//-------- Manejador de eventos ----------------//
static esp_err_t ManejadorEventos(void *ctx, system_event_t *evento){
    switch(evento -> event_id){
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            printf("\n\n Conexion perdida con el punto de acceso \n\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            printf("\nReconectando...\n");
            esp_wifi_connect();
            break;

        default:
            break;
    }
    return ESP_OK;
}
//----------------------------------------------//

//------- Func start wifi como STA -------------//
void iniciar_wifi(){
    tcpip_adapter_init();
        esp_event_loop_init(ManejadorEventos,NULL);
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);
        esp_wifi_set_storage(WIFI_STORAGE_RAM);
        wifi_config_t wifi_config = {
            .sta = {
                .ssid = nombreWiFi,
                .password = claveWiFi,
            },
        };
        esp_wifi_set_mode(WIFI_MODE_STA);
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        esp_wifi_start();
}

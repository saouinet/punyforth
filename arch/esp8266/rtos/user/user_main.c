#include "user_config.h"
#include "espressif/esp_common.h"
#include "espressif/esp_softap.h"
#include "freertos/task.h"
#include "uart.h"
#include "tcp_shell.h"
#include "punyforth.h"
#include "wifisettings.h"

static xTaskHandle tasks[8];

void wifi_init(void) {  
    struct softap_config config;
    wifi_set_opmode(STATION_MODE);
    wifi_softap_get_config(&config);
    memset(config.ssid, 0, 32);
    memset(config.password, 0, 64);
    memcpy(config.ssid, WIFI_SSID, WIFI_SSID_LEN);
    memcpy(config.password, WIFI_PASS, WIFI_PASS_LEN);
    config.authmode = AUTH_WPA_WPA2_PSK;
    config.ssid_len = 0;
    config.max_connection = 4;
    wifi_softap_set_config(&config);
}

static void ICACHE_FLASH_ATTR forth_init(void* dummy) {
    forth_start();   
}

int ICACHE_FLASH_ATTR forth_div(int a, int b) { return a / b; }
int ICACHE_FLASH_ATTR forth_mod(int a, int b) { return a % b; }

void ICACHE_FLASH_ATTR forth_putchar(char c) { 
//    uart_tx_one_char(0, c);
    printf("%c", c);
//    if (tcp_shell_is_connected()) {
//        tcp_shell_put_char(c);
//    }
}

char ICACHE_FLASH_ATTR forth_getchar() { 
    return tcp_shell_read_char();
}

void ICACHE_FLASH_ATTR user_init(void) {
    printf("Initializing UART ...\n");
//uart_init_new();    
//UART_SetPrintPort(1);
    printf("Initializing WIFI\n");	
    wifi_init();
    tcp_shell_init();
    printf("Starting PunyForth task...\n");
    xTaskCreate(forth_init, "punyforth", 256, NULL, 2, &tasks[0]); 
    printf("PunyForth started\n");
}

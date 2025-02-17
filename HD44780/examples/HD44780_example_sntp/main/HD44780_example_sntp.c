/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"
#include "HD44780.h"

// Definitions for custom display character RAM locations
#define TOP_RIGHT_L 0
#define TOP_LEFT_L 1
#define BOTTOM_RIGHT_L 2
#define BOTTOM_LEFT_L 3
#define BOTTOM_DASH 4

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

// Function predefinitions
static void obtain_time(void);
void updateTimeAfterInit();
void setupDisplay(HD44780_FOUR_BIT_BUS *bus);
void updateDisplay(struct tm *timeinfo);


void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { 2, 16, 18, 19, 21, 22, 16, 17 };
    setupDisplay(&bus);

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }

    // Set timezone to Central Standard Time 
    setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1);
    tzset();

    while (true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        updateTimeAfterInit();
    }
}


static void obtain_time(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /*
     * This is the basic default config with one server and starting the service
     */
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
    //config.sync_cb = time_sync_notification_cb;     // Note: This is only needed if we want

    esp_netif_sntp_init(&config);

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 15;

    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count);
    time(&now);
    localtime_r(&now, &timeinfo);

    ESP_ERROR_CHECK( example_disconnect() );
    esp_netif_sntp_deinit();
}


void updateTimeAfterInit() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    updateDisplay(&timeinfo);
}


void setupDisplay(HD44780_FOUR_BIT_BUS *bus) {
    HD44780_initFourBitBus(bus);

    uint8_t topLeftL[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00111,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    };

    uint8_t bottomLeftL[8] = {
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00111,
        0b00000,
        0b00000,
        0b00000
    };

    uint8_t bottomRightL[8] = {
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11100,
        0b00000,
        0b00000,
        0b00000
    };

    uint8_t topRightL[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11100,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    };

    uint8_t bottomDash[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b00000,
        0b00000,
        0b00000
    };
    
    HD44780_createChar(TOP_LEFT_L, topLeftL);
    HD44780_createChar(TOP_RIGHT_L, topRightL);
    HD44780_createChar(BOTTOM_RIGHT_L, bottomRightL);
    HD44780_createChar(BOTTOM_LEFT_L, bottomLeftL);
    HD44780_createChar(BOTTOM_DASH, bottomDash);

    // Print a square pattern across the entire screen.
    // The characters holding data will be overwritten when they first update.
    HD44780_homeCursor();
    HD44780_writeChar(TOP_LEFT_L);
    for(int i = 0; i < 14; i++) {
        HD44780_print("-");
    }
    HD44780_writeChar(TOP_RIGHT_L);

    HD44780_setCursorPos(0, 1);
    HD44780_writeChar(BOTTOM_LEFT_L);
    for(int i = 0; i < 14; i++) {
        HD44780_writeChar(BOTTOM_DASH);
    }
    HD44780_writeChar(BOTTOM_RIGHT_L);
}


void updateDisplay(struct tm *timeinfo) {
    
    char strftime_buf[16];

    // String format the date and print to the first row
    strftime(strftime_buf, sizeof(strftime_buf), "%d %b, %Y", timeinfo);
    HD44780_setCursorPos(2, 0);
    HD44780_print(strftime_buf);

    // String format the time and print to the second row
    strftime(strftime_buf, sizeof(strftime_buf), "%X", timeinfo);
    HD44780_setCursorPos(4, 1);
    HD44780_print(strftime_buf);
}
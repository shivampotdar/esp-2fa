#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"
#include "ntp.h"
#include "totp.h"
#include "nvs.h"
#include "display.h"
// SDA - GPIO21
#define PIN_SDA 21

// SCL - GPIO22
#define PIN_SCL 22

static const char *TAG = "ssd1306";

void task_test_SSD1306i2c(char* buf) {
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = PIN_SDA;
	u8g2_esp32_hal.scl  = PIN_SCL;
	u8g2_esp32_hal_init(u8g2_esp32_hal);


	u8g2_t u8g2; // a structure which will contain all the data for one display
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(
		&u8g2,
		U8G2_R0,
		//u8x8_byte_sw_i2c,
		u8g2_esp32_i2c_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
	u8x8_SetI2CAddress(&u8g2.u8x8,0x78);

	ESP_LOGI(TAG, "u8g2_InitDisplay");
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

	ESP_LOGI(TAG, "u8g2_SetPowerSave");
	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	ESP_LOGI(TAG, "u8g2_ClearBuffer");
	u8g2_ClearBuffer(&u8g2);
	ESP_LOGI(TAG, "u8g2_DrawBox");
	u8g2_DrawBox(&u8g2, 0, 26, 80,6);
	u8g2_DrawFrame(&u8g2, 0,26,100,6);

	ESP_LOGI(TAG, "u8g2_SetFont");
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
	ESP_LOGI(TAG, "u8g2_DrawStr");
    u8g2_DrawStr(&u8g2, 2,17,buf);
	ESP_LOGI(TAG, "u8g2_SendBuffer");
	u8g2_SendBuffer(&u8g2);

	ESP_LOGI(TAG, "All done!");

}

// void app_main()
// {
// 	time_t now;
//     struct tm timeinfo;

//     obtain_time();

// 	setenv("TZ", "IST-05:30:00", 1);
//     tzset();

// 	time(&now);
//     localtime_r(&now, &timeinfo);
//     char *buffer = (char*)malloc(sizeof(char)*30);
//     memset(buffer, '\0', 100);
	
// 	sprintf(buffer, "%d:%d:%d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

// 	task_test_SSD1306i2c(buffer);
// }

// void app_main()
// {
//   char *key = "secretKey";
//   char *payload = "Hello HMAC SHA 256!";
//   unsigned char hmacResult[32];
//   char message_hash[64];

//   mbedtls_md_context_t ctx;
//   mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
 
//   const size_t payloadLength = strlen(payload);
//   const size_t keyLength = strlen(key);            
 
//   mbedtls_md_init(&ctx);
//   mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
//   mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, keyLength);
//   mbedtls_md_hmac_update(&ctx, (const unsigned char *) payload, payloadLength);
//   mbedtls_md_hmac_finish(&ctx, hmacResult);
//   mbedtls_md_free(&ctx);
 
//   for(int i= 0; i< sizeof(hmacResult); i++){
//     char str[3];
 
//     sprintf(str, "%02x", (int)hmacResult[i]);
//     strcat(message_hash, str);
//   }

//   	ESP_LOGI("hmac", "%s", message_hash);

// }

void app_main()
{
	time_t now;

    obtain_time();

	setenv("TZ", "IST-05:30:00", 1);
    tzset();

	time(&now);
    ESP_LOGI("time", "%d", (unsigned)time(&now));

	char res[10];
	char *key = NULL;

	get_from_nvs(&key);
	save_to_nvs("yfloo4rknli2o227obq33hbph4ygfq4gvkn5su3664x6a4lohxlhmg7y");
	
	if (key == NULL)
	{
		save_to_nvs("yfloo4rknli2o227obq33hbph4ygfq4gvkn5su3664x6a4lohxlhmg7y");
		get_from_nvs(&key);
	}
	display_init();

	while(1)
	{
		// get_from_nvs(&key);
		totp_init(MBEDTLS_MD_SHA1);
		// ESP_LOGI("time", "%d", ((unsigned)time(NULL))/30);
		totp_generate(key, ((unsigned)time(NULL))/30, 6, res);

	
		ESP_LOGI("hmac", "%s: %s", key, res);
		totp_free();
		display_write(res);
		// task_test_SSD1306i2c(res);
		vTaskDelay(100);
	}
}
#include "driver/uart.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#define BUF_SIZE (1024)

uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

const char* ssid = "Galaxy A719BC3";
const char* password =  "ogoz7599";

AsyncWebServer server(80);

uart_config_t uart_config = {
  .baud_rate = 9600,
  .data_bits = UART_DATA_8_BITS,
  .parity    = UART_PARITY_DISABLE,
  .stop_bits = UART_STOP_BITS_1,
  .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

void setup() {
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      AsyncWebParameter* p = request->getParam(0);
      uart_write_bytes(UART_NUM_0, (const char *) p->value().c_str(), 55);
      request->send(200, "text/plain", "Configuration string received");
  });

    uart_write_bytes(UART_NUM_0, (const char *) "Y-0500-0125-H@O-0500-0250-H@R-0500-0375-H@B-0500-0000-H", 55);
    delay(3000);
    uart_write_bytes(UART_NUM_0, (const char *) "Y-0500-0125-S@O-0500-0250-S@R-0500-0375-S@B-0500-0000-S", 55);
 
  server.begin();
}

void loop(){
    /*uart_write_bytes(UART_NUM_0, (const char *) "Y-0500-0125-H+O-0500-0250-H+R-0500-0375-H+B-0500-0000-H", 55);
    delay(3000);
    uart_write_bytes(UART_NUM_0, (const char *) "Y-0500-0125-S+O-0500-0250-S+R-0500-0375-S+B-0500-0000-S", 55);
    delay(3000);*/
}

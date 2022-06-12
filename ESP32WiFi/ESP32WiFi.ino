// USed liraries
#include "driver/uart.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

// Define Buffer
#define BUF_SIZE (1024)

// Memory handling
uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

// Wi-fi name and password
const char* ssid = "moto e20 6363";
const char* password =  "12456789";

// Declare web server instance on port 80
AsyncWebServer server(80);

// Uart configuration structure
uart_config_t uart_config = {
  .baud_rate = 9600,
  .data_bits = UART_DATA_8_BITS,
  .parity    = UART_PARITY_DISABLE,
  .stop_bits = UART_STOP_BITS_1,
  .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

void setup() {
  // Configure uart peripherals
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

  // Connect to wi-fi
  WiFi.begin(ssid, password);

  // Wait for wi-fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Web server execution program waiting for GET HTTP request
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      // Get the parameters from the URL
      AsyncWebParameter* p = request->getParam(0);
      // Send the received parameters to the stm32 via uart
      uart_write_bytes(UART_NUM_0, (const char *) p->value().c_str(), 55);
      // Send response to request
      request->send(200, "text/plain", "Configuration string received");
  });

  // Initiates server
  server.begin();
}

// Does nothing. Just prevents end of program whilw server is running.
void loop(){
}

#define WIFI_SSID ""
#define WIFI_PASS ""
#define ADDRESSES_PRESS {"192.168.0.69", "192.168.0.69"}
#define ADDRESSES_HOLD {"192.168.0.420", "192.168.0.420"}
#define BUTTON_HOLD_TIME_MS 500
#define BUTTON_DEBOUCE_TIME_MS 20
#define BUTTON_PIN D2
#define BUTTON_ENABLE_PULLUP true
#define BUTTON_ACTIVE_LOW true

#define OPEN_SERVER_BUTTON_PIN 0
#define OPEN_SERVER_BUTTON_LONGPRESS_TIME_MS 5000

#define USE_LOGGING



#ifdef USE_LOGGING
  #define Log(x) Serial.print(x)
#else
  #define Log(x)
#endif
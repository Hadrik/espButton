#define WIFI_SSID ""
#define WIFI_PASS ""
#define ADDRESSES {"192.168.0.69", "192.168.0.420"}
#define BUTTON_DEBOUCE_TIME_MS 20
#define BUTTON_LONGPRESS_TIME_MS 5000
#define BUTTON_PIN D2
#define BUTTON_ENABLE_PULLUP true
#define BUTTON_ACTIVE_LOW true

#define USE_LOGGING



#ifdef USE_LOGGING
  #define Log(x) Serial.print(x)
#else
  #define Log(x)
#endif
#define WIFI_SSID "TurboWifi.2"
#define WIFI_PASS "HovnoKleslo"
#define ADDRESSES {"test", "ss"}

#define BUTTON_PIN 13

#define USE_LOGGING



#ifdef USE_LOGGING
  #define Log(x) Serial.print(x)
#else
  #define Log(x)
#endif
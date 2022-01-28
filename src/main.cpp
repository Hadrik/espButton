#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EasyButton.h>
#include <vector>
#include "custom.h"
#include "webpage.h"
EasyButton button(BUTTON_PIN, BUTTON_DEBOUCE_TIME_MS,
                  BUTTON_ENABLE_PULLUP, BUTTON_ACTIVE_LOW);
EasyButton button2(OPEN_SERVER_BUTTON_PIN);
ESP8266WebServer server(80);
std::vector<String> addressesSingle = ADDRESSES_PRESS;
std::vector<String> addressesDouble = ADDRESSES_HOLD;
bool openServer = false;

void beginWifi() {
  Log("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (!WiFi.isConnected()) {
    delay(500);
    Log(".");
  }
  Log("\nConnected to wifi, address: ");
  Log(WiFi.localIP());
}

int makeGetRequest(String address) {
  HTTPClient http;
  int code = -1;

  Log("\nMaking GET request to: " + address + " ... ");

  if(http.begin(address)){
    code = http.GET();
    http.end();
    Log("success!");
  }
  else {
    Log("fail!");
  }
  return code;
}

void showWebpage() {
  Log("\nWeb interface accessed");
  server.send(200, "text/html", index_html);
}

void updateList() {
  digitalWrite(LED_BUILTIN, LOW);
  Log("\nUpdating list of addresses.");
  String json = server.arg("plain");
  /*
  {
    "single": {
      "list": [
        "192.168.0."
      ],
      "size": 1
    },
    "double": {
      "list": [
        "192.168.0.",
        "192.168.0."
      ],
      "size": 2
    }
  } */
  StaticJsonDocument<1024> doc;
  DeserializationError dszErr = deserializeJson(doc, json.c_str());

  if(dszErr) {
    Log("Deserialization error!\n");
    server.send(500);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }

  int sizeSingle = doc["single"]["size"].as<int>();
  int sizeDouble = doc["double"]["size"].as<int>();
  addressesSingle.resize(sizeSingle);
  addressesDouble.resize(sizeDouble);

  Log("Single press: \n");
  for(int i = 0; i < sizeSingle; i++) {
    addressesSingle[i] = doc["single"]["list"][i].as<String>();

    Log(addressesSingle[i].c_str());
    Log("\n");
  }
  Log("Double press: \n");
  for(int i = 0; i < sizeDouble; i++) {
    addressesDouble[i] = doc["double"]["list"][i].as<String>();

    Log(addressesDouble[i].c_str());
    Log("\n");
  }
  server.send(202);
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);
}

void sendList() {
  Log("\nSending list of addresses.");
  StaticJsonDocument<1024> doc;

  int sizeSingle = addressesSingle.size();
  int sizeDouble = addressesDouble.size();
  doc["single"]["size"] = sizeSingle;
  doc["double"]["size"] = sizeDouble;
  JsonArray listSingle = doc["single"].createNestedArray("list");
  JsonArray listDouble = doc["double"].createNestedArray("list");

  for(int i = 0; i < sizeSingle; i++) {
    listSingle.add(addressesSingle[i]);
  }
  for(int i = 0; i < sizeDouble; i++) {
    listDouble.add(addressesDouble[i]);
  }

  String out;
  serializeJson(doc, out);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", out);
}

void pressedSingle() {
  digitalWrite(LED_BUILTIN, LOW);
  Log("\n\nShort press!");
  for (String address : addressesSingle) {
    makeGetRequest(address);
  }
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);
}
void pressedDouble() {
  digitalWrite(LED_BUILTIN, LOW);
  Log("\n\nLong press!");
  for (String address : addressesDouble) {
    makeGetRequest(address);
  }
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);
}
void held() {
  Log("\n\nLong press!");
  openServer = !openServer;
  if(openServer) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH);
    Log("\nServer opened");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH);
    Log("\nServer closed");
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  beginWifi();
  server.begin();
  server.on("/", HTTP_GET, showWebpage);
  server.on("/list", HTTP_GET, sendList);
  server.on("/", HTTP_POST, updateList);

  button.begin();
  button2.begin();
  button.onPressed(pressedSingle);
  button.onPressedFor(BUTTON_HOLD_TIME_MS, pressedDouble);
  button2.onPressedFor(OPEN_SERVER_BUTTON_LONGPRESS_TIME_MS, held);
}

void loop() {
  button.read();
  button2.read();
  if(openServer)
    server.handleClient();
}

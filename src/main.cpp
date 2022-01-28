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
ESP8266WebServer server(80);
std::vector<String> addresses = ADDRESSES;
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
  Log("\nUpdating list of addresses.");
  String json = server.arg("plain");
  /* {
        "list": [
          "a",
          "b",
          ...
          ],
        "size": 2
      } */
  StaticJsonDocument<1024> doc;
  DeserializationError dszErr = deserializeJson(doc, json.c_str());

  if(dszErr) {
    Log("Deserialization error!");
    server.send(500);
    return;
  }

  int size = doc["size"].as<int>();
  addresses.resize(size);

  char buffer[50];
  for(int i = 0; i < size; i++) {
    addresses[i] = doc["list"][i].as<String>();

    sprintf(buffer, "%s", addresses[i].c_str());
    Log("\n");
    Log(buffer);
  }
  server.send(202);
}

void sendList() {
  Log("\nSending list of addresses.");
  StaticJsonDocument<500> doc;

  int size = addresses.size();
  doc["size"] = size;
  JsonArray list = doc.createNestedArray("list");

  for(int i = 0; i < size; i++) {
    list.add(addresses[i]);
  }

  String out;
  serializeJson(doc, out);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", out);
}

void pressed() {
  Log("\n\nShort press!");
  for (String address : addresses) {
    makeGetRequest(address);
  }
}
void held() {
  Log("\n\nLong press!");
  openServer = !openServer;
  if(openServer)
    Log("\nServer opened");
  else
    Log("\nServer closed");
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  beginWifi();
  server.begin();
  server.on("/", HTTP_GET, showWebpage);
  server.on("/list", HTTP_GET, sendList);
  server.on("/", HTTP_POST, updateList);

  button.begin();
  button.onPressed(pressed);
  button.onPressedFor(BUTTON_LONGPRESS_TIME_MS, held);
}

void loop() {
  button.read();
  if(openServer)
    server.handleClient();
}

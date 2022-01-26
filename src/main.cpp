#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "custom.h"
ESP8266WebServer server(80);
std::vector<String> addresses = ADDRESSES;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  beginWifi();
  server.begin();
  server.on("/", HTTP_GET, showWebpage);
  server.on("/", HTTP_POST, updateList);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    for (String address : addresses) {
      makeGetRequest(address);
    }
  }

  server.handleClient();
}

void beginWifi() {
  Log("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (!WiFi.isConnected()) {
    delay(500);
    Log(".");
  }
}

int makeGetRequest(String address) {
  HTTPClient http;
  int code = -1;

  Log("Making GET request to: " + address);

  if(http.begin(address)){
    code = http.GET();
    http.end();
  }
  else {
    Log("Request to '" + address + "' failed!");
  }
  return code;
}

void showWebpage() {
  server.send(200, "text/html", index_html);
}

void updateList() {
  String json = server.arg('plain');
  // {"list":["a","b","c", ...]}
  DynamicJsonDocument doc(128);
  DeserializationError dszErr = deserializeJson(doc, json.c_str());

  if(dszErr) {
    Log("Deserialization error!");
    return;
  }


}

//page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      align-items: center;
    }
    li {
      list-style-type: none;
    }
  </style>
  <script>
    function addListItem() {
      let list = document.getElementById('list')
      let entry = document.createElement('li')
      let input = document.createElement('input')
      input.type = 'text'
      entry.appendChild(input)
      list.appendChild(entry)
    }

    function save() {
      let a = document.querySelectorAll('ul li input')
      let out = { list: [] }
      for (let i = 0; i < a.length; i++) {
        out.list[i] = a[i].value
      }

      fetch('127.0.0.1', {
        method: 'POST',
        headers: {
          'content-type': 'application/json'
        },
        body: JSON.stringify(out)
      })
    }
  </script>
  <title>Edit</title>
</head>
<body>
  <h1>
    List of addresses
  </h1>
  <ul id="list">
    <li>
      <input type="text">
    </li>
  </ul>
  <button onclick="addListItem()">Add</button>
  <button onclick="save()">Save</button>
</body>
</html>
)rawliteral";
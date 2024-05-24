/**
 * Code that reads values from two phototransistors and serves a 
 * webpage with the latest values.
 * 
 * 
 * Based on tutorial code for the LAFVIN ESP32 Basic Starter Kit
 * Project 9 ESP32 DHT11 Web Server.
 */
// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include "passwords.hpp"

const int pin10K = 35;

const int pin1K = 35;

int value10K = 0;
int value1K = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readValue10K() {
  value10K = analogRead(pin10K);
  Serial.println(value10K);
  return String(value10K);
}

String readValue1K() {
  value1K = analogRead(pin1K);
  Serial.println(value1K);
  return String(value1K);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Photoresister Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">TENK</span> 
    <span id="tenk">%TENK%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">ONEK</span>
    <span id="onek">%ONEK%</span>
    <sup class="units">&percnt;</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tenk").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/tenk", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("onek").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/onek", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TENK"){
    return readValue10K();
  }
  else if(var == "ONEK"){
    return readValue1K();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/tenk", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readValue10K().c_str());
  });
  server.on("/onek", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readValue1K().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


#define LED 16

WiFiClient client;

const char* ssid     = "HP_NETWORK";
const char* password = "PerroAmarilloGatoNegro";

const int httpPort = 80;
const char* host ="automated-parking.herokuapp.com";

HTTPClient http;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 
 
  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
   Serial.print('.');
  }
 
  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());


  Serial.print("[HTTP] begin...\n");
}


void consulta(String numero){
  String URL;
  if(numero=="999"){
    URL = "http://automated-parking.herokuapp.com/consultarPlaza/todas";
  }
  else{
    URL = "http://automated-parking.herokuapp.com/consultarPlaza/";
    URL = URL + numero;
  }

  Serial.println(URL);
  
  if (http.begin(client, URL)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          //char json[] = payload;

          DynamicJsonDocument doc(1024);
          deserializeJson(doc, payload);
          String estado1 = doc["0"]["estado"];
          
          Serial.print("Estado plaza 1: ");
          Serial.println(estado1);
          
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
}


// the loop function runs over and over again forever
void loop() {
  // read from port 1, send to port 0:
  if (Serial.available()) {
    String numero=Serial.readString();
    unsigned int index = numero.length();
    numero.remove(index-1);
    
    Serial.println(numero);
    consulta(numero);
  }
}

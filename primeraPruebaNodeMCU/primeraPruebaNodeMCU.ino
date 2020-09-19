#include <ESP8266WiFi.h>

WiFiClient client;

const char* ssid     = "Su-Red";
const char* password = "Su-Contraseña";

void setup() {
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

  const int httpPort = 80;
  const char* host ="www.google.com";
  
  if(!client.connect(host,httpPort)){
    Serial.println("Conexion fallida");  
    return;
  }
  else{
    Serial.println("Conexión Exitosa");
  }
}

// the loop function runs over and over again forever
void loop() {

}

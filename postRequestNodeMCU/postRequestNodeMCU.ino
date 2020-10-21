#include <ESP8266WiFi.h>

WiFiClient client;

const char* ssid     = "HP_NETWORK";
const char* password = "PerroAmarilloGatoNegro";

//HOST o URL de su aplicación web sin http
const char* host ="githubdemostracion.herokuapp.com";
const int httpPort = 80; // puerto web por defecto: 80

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
}


unsigned long t_ultimoEnvio = 0;

// the loop function runs over and over again forever
void loop() {

  //if (t_ultimoEnvio + 5000 >= 4294967295) { t_ultimoEnvio = 0; }
  
  if ( (millis() - t_ultimoEnvio) >= 5000){
    if(!client.connect(host,httpPort)){
      Serial.println("Conexion fallida");  
    }
    else{
      Serial.print("Conectado a la aplicación :");
      Serial.println(host);
      String tabla = "consumo";
      String lugar = "sala";
      float corriente = random(0,50)/10;
      float voltaje = random(1100,1250)/10;
      sendConsumo(tabla, corriente, voltaje, lugar);
      t_ultimoEnvio = millis();
    }
  }
  
if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if ( (millis() - t_ultimoEnvio) >= 100000){
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
    }
  }
}

void sendConsumo(String tabla, float corriente, float voltaje, String lugar){
  //Creamos la direccion para luego usarla
  String dato = "tabla=" + String(tabla) + "&corriente=" + String(corriente) + "&voltaje=" + String(voltaje) + "&lugar=" + String(lugar);
 
  // Solicitud de tipo post para enviar al servidor 
  client.println("POST /guardarDato HTTP/1.1");
  client.println("Host: githubdemostracion.herokuapp.com");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(dato.length());
  client.println();
  client.println(dato);
  Serial.println("Respuesta: ");
}

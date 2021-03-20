#include <ESP8266WiFi.h>


WiFiClient client;

#define PERIODO_ENVIO 5000  // Periodo de envío en milisegundos

const char* ssid     = "HP_NETWORK";
const char* password = "PerroAmarilloGatoNegro";

//HOST o URL de su aplicación web sin http
const char* host ="githubdemostracion.herokuapp.com";
const int httpPort = 80; // puerto web por defecto: 80
unsigned long t_ultimoEnvio = 0;

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


// the loop function runs over and over again forever
void loop() {
  if ( (millis() - t_ultimoEnvio) >= PERIODO_ENVIO){
    if(!client.connect(host,httpPort)){
      Serial.println("Conexion fallida");  
    }
    else{
      Serial.print("Conectado a la aplicación :");
      Serial.println(host);
      String tabla = "consumo";
      String lugar = "sala";

      Serial.print("Temperatura: ");
      int val; //Crea una variable entera
      double temperatura;//Variable de temperatura = temp
      val=analogRead(A0);//Lee el valor del pin analogo 0 y lo mantiene como val
      temperatura=Thermister(val);//Realiza la conversión del valor analogo a grados Celsius
      Serial.println(temperatura);//Escribe la temperatura en el monitor serial
      
      //sendTemperatura(tabla, temperatura, lugar);
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

void sendTemperatura(String tabla, float temperatura, String lugar){
  //Creamos la direccion para luego usarla
  String dato = "tabla=" + String(tabla) + "&temperatura=" + String(temperatura) + "&lugar=" + String(lugar);
 
  // Solicitud de tipo post para enviar al servidor 
  client.println("POST /postTemperatura HTTP/1.1");
  client.println("Host: githubdemostracion.herokuapp.com");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(dato.length());
  client.println();
  client.println(dato);
  Serial.println("Respuesta: ");
}

double Thermister(int RawADC) {  
double Temp;
Temp = log(((10240000/RawADC) - 10000));
Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
Temp = Temp - 273.15;// Convierte de Kelvin a Celsius
//Para convertir Celsius-Farenheith escriba en esta linea:Temp=(Temp*9.0)/ 5.0 + 32.0;
return Temp;
}

#include <ESP8266WiFi.h>
#include <ThingsBoard.h>

#define WIFI_AP "YourWifiAP"
#define WIFI_PASSWORD "YourWifiPass"

#define TOKEN "YourTokenDeviceInTB"

#define GPIO0_PIN 0

boolean gpioState[] = {false, false};
  
char thingsboardServer[] = "YourServer";

WiFiClient wifiClient;
ThingsBoard tb(wifiClient);
PubSubClient tbupload(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);
  delay(10);
  InitWiFi();
  pinMode(D0, OUTPUT);
  lastSend = 0;
  tbupload.setServer( thingsboardServer, 1883 );
  tbupload.setCallback(on_message);
}

void loop()
{
  if ( !tb.connected() ) {
    reconnect();
  }
  if ( !tbupload.connected() ) {
    reconnect();
  }


  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  tbupload.loop();
  tb.loop();
  

}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = 80;
  // Read temperature as Celsius (the default)
  float temperature = 30;

  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);
  tb.sendTelemetryFloat("rssi", rssi);
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
  
    while (!tbupload.connected()) {
      Serial.print("Connecting to ThingsBoard node Client ...");
      // Attempt to connect (clientId, username, password)
      if ( tbupload.connect("NodeMCU", TOKEN, NULL) ) {
        Serial.println( "[DONE]" );
        // Subscribing to receive RPC requests
        tbupload.subscribe("v1/devices/me/rpc/request/+");
        // Sending current GPIO status
        Serial.println("Sending current GPIO status ...");
        tbupload.publish("v1/devices/me/attributes", get_gpio_status().c_str());
      } else {
        Serial.print( "[FAILED] [ rc = " );
        Serial.print( tbupload.state() );
        Serial.println( " : retrying in 5 seconds]" );
        // Wait 5 seconds before retrying
        delay( 5000 );
      }
    }
}

void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("Llegó un mensaje del servidor");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonDocument<200> jsonBuffer;
  DeserializationError error = deserializeJson(jsonBuffer, json);

  if (error)
  {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  // Check request method
  String methodName = String((const char*)jsonBuffer["method"]);

  if (methodName.equals("getGpioStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    tbupload.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } else if (methodName.equals("setGpioStatus")) {
    // Update GPIO status and reply
    set_gpio_status(jsonBuffer["params"]["pin"], jsonBuffer["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    tbupload.publish(responseTopic.c_str(), get_gpio_status().c_str());
    tbupload.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
    // Output GPIOs state
    digitalWrite(D0, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[0] = enabled;
  }
} 

String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonDocument<200> jsonBuffer;
  //JsonObject data = jsonBuffer.createObject();
  jsonBuffer[String(GPIO0_PIN)] = gpioState[0] ? true : false;
  char payload[256];
  //jsonBuffer.printTo(payload, sizeof(payload));
  serializeJson(jsonBuffer, Serial);
  String strPayload = String(payload);
  Serial.print("Get gpio status: ");
  Serial.println(strPayload);
  return strPayload;
}

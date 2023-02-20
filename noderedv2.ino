// ข้อแนะนำการใช้งาน dht11
// S ต่อกับ IO32
// - ต่อกับ GND
// ตรงกลาง ต่อกับ 3v3


#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
DHT dht(32, DHT11);

// Update these with values suitable for your network.

const char* ssid = "UMR-LAB_2";
const char* password = "Ubon2565";
const char* mqtt_server = "192.168.1.202";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msgTemp;
  String top = String(topic);  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msgTemp +=(char)payload[i];
  }
  Serial.println();
  Serial.println(topic);
  Serial.println(msgTemp);
  if(top== "/relay"){
     Serial.println("xxx");    
     if(msgTemp == "on"){
      digitalWrite(33, HIGH);       
    } 
     if(msgTemp == "off"){
      digitalWrite(33, LOW);       
    }     
  } 
  else if(top == "/led/green"){
     if(msgTemp == "on"){
      digitalWrite(23, HIGH);       
    } 
     if(msgTemp == "off"){
      digitalWrite(23, LOW);       
    }     
  }   
  else if(top == "/led/blue"){
     if(msgTemp == "on"){
      digitalWrite(19, HIGH);       
    } 
     if(msgTemp == "off"){
      digitalWrite(19, LOW);       
    }     
  }
  else if(top == "/led/red"){
     if(msgTemp == "on"){
      digitalWrite(18, HIGH);       
    } 
     if(msgTemp == "off"){
      digitalWrite(18, LOW);       
    }     
  }  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("/relay");
      client.subscribe("/led/red");
      client.subscribe("/led/green");
      client.subscribe("/led/blue");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(33, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(18, OUTPUT);     
  dht.begin();     
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    float h = dht.readHumidity();    
    float t = dht.readTemperature();
    client.publish("/temp", String(t).c_str());
    client.publish("/hum", String(h).c_str());
  }
}

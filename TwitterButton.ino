/*
//########################################################################
//#  ESP8266 MQTT client, can recieve colors to turn on an LED via MQTT          
//#  Also button press to send out a MQTT message.                            
//#  This can be used along with Node-Red and Twitter                                       
//#  Blog post, videos, and code can  be found at www.electronhacks.com  
//########################################################################
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Timer.h"


//#######Configuration Section#########
//Update these with values suitable for your network and MQTT Broker.
const char* ssid = "ssid";
const char* password = "securepassword";
const char* mqtt_server = "192.168.1.12";
const char* mqtt_topic = "Arduino";
const char* mqtt_Pub_topic = "Arduino";
const char* mqtt_user = "user";
const char* mqtt_password = "password";  //if you are not using authentication for MQTT see below note in the MQTT connect section

//#######General Variables#######
Timer t;
char buf[100];
int lf = 10;
char vInpD5 = 0;
String rx;
String tx;
int  rxLength = 0;
int  txLength = 0;
int Counter = 0;
String payload;
const char* cpayload;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


//#######Void Setup######
void setup() {
  t.every(100, timers);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, INPUT_PULLUP);
  
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.print("Starting...");

}


//#######Setup Wifi######
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//#######Callback for MQTT Messages######
void callback(char* topic, byte* payload, unsigned int length) {\
  //Serial.print("RX: ");
  //Convert and clean up the MQTT payload messsage into a String
  rx = String((char *)payload);                    //Payload is a Byte Array, convert to char to load it into the "String" object 
  rxLength = rx.length();                          //Figure out how long the resulting String object is 
  for (int i = length; i <= rxLength; i++)         //Loop through setting extra characters to null as garbage may be there
  {
    rx.setCharAt(i, ' ');
  }
  rx.trim();                                       //Use the Trim function to finish cleaning up the string   
  Serial.print(rx);                                //Print the recieved message to serial
  Serial.println();

  if (rx == "Red") {
    digitalWrite(D1, HIGH);  
    Serial.println("Turning on Red");
    Counter = 1;
  } 
  if (rx == "Green") {
    digitalWrite(D2, HIGH);  
    Serial.println("Turning on Green");
    Counter = 1;
  } 
  if (rx == "Blue") {
    digitalWrite(D4, HIGH);  
    Serial.println("Turning on Blue");
    Counter = 1;
  }
  if (rx == "Magenta") {
    digitalWrite(D1, HIGH);  
    digitalWrite(D4, HIGH);  
    Serial.println("Turning on Magenta");
    Counter = 1;
  }    
  if (rx == "Cyan") {
    digitalWrite(D2, HIGH);  
    digitalWrite(D4, HIGH);  
    Serial.println("Turning on Cyen");
    Counter = 1;
  } 
  if (rx == "Yellow") {
    digitalWrite(D1, HIGH);  
    digitalWrite(D2, HIGH);  
    Serial.println("Turning on Yellow");
    Counter = 1;
  } 
  if (rx == "White") {
    digitalWrite(D1, HIGH);  
    digitalWrite(D2, HIGH);  
    digitalWrite(D4, HIGH);      
    Serial.println("Turning on White");
    Counter = 1;
  }     
   
}


//########MQTT Connection#####
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect(mqtt_topic)) { //This is how you can connect without a user and a password.
    if (client.connect(mqtt_topic,mqtt_user,mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_topic, "Bridge Connected");
      // ... and resubscribe
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//#######Loop######
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (digitalRead(D5) != vInpD5){
    vInpD5 = digitalRead(D5);
    if (vInpD5 == LOW){    
      client.publish(mqtt_Pub_topic, "ButtonPress");
      client.publish("Arduino/Out", "Tweet from Arduino ESP8266. Hello!");
      Serial.println("ButtonPress");
    }
    else{
      client.publish(mqtt_Pub_topic, "ButtonRelease");
      Serial.println("ButtonRelease");
    }
  }

  client.loop();
  t.update();
}



void timers(void){
  if (Counter > 0){
    Counter = Counter + 1;
  }
  if (Counter > 60){
    digitalWrite(D1, LOW);  
    digitalWrite(D2, LOW);  
    digitalWrite(D4, LOW);  
    Serial.println("Turning LEDs off"); 
    Counter = 0;      
  }
}


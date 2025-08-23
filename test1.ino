#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define Nwindow 16 
#define Swindow 17 
#define Ewindow 18 
#define Wwindow 19 
#define LED4 15 // LIGHT
#define LED3 14 // LIGHT
#define LED2 12 // AC
#define LED1 13 // HEAT

// WiFi
const char *ssid = "ooredoo"; // Enter your Wi-Fi name
const char *password = "ooredooo";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "192.168.137.5";
const char *topic = "";
const char *mqtt_username = "openhabian";
const char *mqtt_password = "0000";
const int mqtt_port = 1883;

Adafruit_BMP280 bmp;
WiFiClient espClient;
PubSubClient client(espClient);

float temp ; 
String msgStr ;
/////////////////////////

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String data = "";
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
        data += (char)payload[i];}          
    Serial.println("-----------------------");
    float x,y,z ;
    
    if (String(topic) == "/lights1") {
       if(data == "ON") 
        {digitalWrite(LED1,HIGH);     }
       else {digitalWrite(LED1,LOW);     } }
       
    else if (String(topic) == "/lights2") {
       if(data == "ON") 
        {digitalWrite(LED2,HIGH);     }
       else {digitalWrite(LED2,LOW);     } }

    else if (String(topic) == "/lights3") {
       if(data == "ON") 
        {digitalWrite(LED3,HIGH);     }
       else {digitalWrite(LED3,LOW);     } }
        
    else if (String(topic) == "/lights4") {
       if(data == "ON") 
        {digitalWrite(LED4,HIGH);     }
       else {digitalWrite(LED4,LOW);     } } 

    else if (String(topic) == "/window/s") {
       if(data == "ON") 
        {digitalWrite(Swindow,LOW);     }
       else {digitalWrite(Swindow,HIGH);     } } 
             
    else if (String(topic) == "/window/n") {
       if(data == "ON") 
        {digitalWrite(Nwindow,LOW);     }
       else {digitalWrite(Nwindow,HIGH);     } }
       
    else if (String(topic) == "/window/e") {
       if(data == "ON") 
        {digitalWrite(Ewindow,LOW);     }
       else {digitalWrite(Ewindow,HIGH);     } } 
             
    else if (String(topic) == "/window/w") {
       if(data == "ON") 
        {digitalWrite(Wwindow,LOW);     }
       else {digitalWrite(Wwindow,HIGH);     } }

    else if (String(topic) == "/weather/windspeed") {
      y=data.toFloat(); }   
    else if (String(topic) == "/weather/winddirection") {
      z=data.toFloat(); }   
        
    else if (String(topic) == "/weather/intemp") {
      x=data.toFloat();   
      if(x < 20){client.publish("/lights1", "ON");
                 client.publish("/window/s", "OFF");
                 client.publish("/window/n", "OFF");
                 client.publish("/window/e", "OFF");
                 client.publish("/window/w", "OFF");   }
      else if(x > 28) {client.publish("/lights2", "ON");
                       client.publish("/window/s", "OFF");
                       client.publish("/window/n", "OFF");
                       client.publish("/window/e", "OFF");
                       client.publish("/window/w", "OFF");   } 
      else {client.publish("/lights1", "ON");
            client.publish("/lights2", "ON");
            delay(1000);
            client.publish("/lights1", "OFF");
            client.publish("/lights2", "OFF");
            if(y < 15){ client.publish("/window/s", "ON");
                        client.publish("/window/n", "ON");
                        client.publish("/window/e", "ON");
                        client.publish("/window/w", "ON");   } 
            else { client.publish("/window/s", "OFF");
                   client.publish("/window/n", "OFF");
                   client.publish("/window/e", "OFF");
                   client.publish("/window/w", "OFF");    
                   if((z < 330)&&(z > 210)) { client.publish("/window/w", "ON");}
                   else if((z < 240)&&(z > 120)) { client.publish("/window/s", "ON");}
                   else if((z < 150)&&(z > 30)) { client.publish("/window/e", "ON");}
                   else if((z < 60)&&(z > 300)) { client.publish("/window/n", "ON");}
            }
      }         
    }  
       
              
}

///////////////////////
void setup() {
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(Nwindow, OUTPUT);
    pinMode(Swindow, OUTPUT);
    pinMode(Ewindow, OUTPUT);
    pinMode(Wwindow, OUTPUT);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);
  digitalWrite(Nwindow,HIGH);
  digitalWrite(Swindow,HIGH);
  digitalWrite(Ewindow,HIGH);
  digitalWrite(Wwindow,HIGH);
  
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.println("Connecting to WiFi..");    }
    Serial.println("Connected to the Wi-Fi network");
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("MQTT broker connected");}
        else { Serial.println("failed with state ");
               Serial.print(client.state());
               delay(2000);  }
    }
///////// Publish and subscribe
     client.subscribe("/window/s");
     client.subscribe("/window/n");
     client.subscribe("/window/e");
     client.subscribe("/window/w");
     client.subscribe("/lights1");
     client.subscribe("/lights2");
     client.subscribe("/lights3");
     client.subscribe("/lights4");
     client.subscribe("/weather/windspeed");
     client.subscribe("/weather/winddirection");
     client.subscribe("/weather/intemp");

 unsigned status;
 status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
 bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
     
}


void loop() {
    client.loop();
    
    temp = bmp.readTemperature();
    msgStr = String(temp) ;
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
    msgStr.toCharArray(msg, arrSize);
    client.publish("/temp", msg);
    msgStr = "";
    
    Serial.print(F("Temperature = "));
    Serial.print(temp);
    Serial.println(" *C");
    delay(1000);


}

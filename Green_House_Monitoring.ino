#include <DHT.h>  
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
 
String apiKey = "R6M9LKQ1Z3T0RIBO";     // Enter Write API key from ThingSpeak
const char *ssid =  "J";     // Insert wifi ssid and wpa2 key
const char *pass =  "isnh3359?";
const char* server = "api.thingspeak.com";
 
#define DHTPIN D5         
#define LED_PIN D6
#define LED_PIN1 D7
#define BUZZER_PIN D8
#define TEMP_MAX 31.0

DHT dht(DHTPIN, DHT11);
WiFiClient client;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() 
{
       //LCD Initialization
       lcd.init();
       lcd.clear();
       lcd.backlight();

       lcd.setCursor(0,0);
       lcd.print("Connecting...");

       Serial.begin(115200);
       dht.begin();

       pinMode(LED_PIN, OUTPUT);
       pinMode(LED_PIN1, OUTPUT);
       pinMode(BUZZER_PIN, OUTPUT);

       // Connect to WI-Fi
       Serial.println("Connecting to Wifi ...");
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.println("Connecting...");
     }
      Serial.println("\nWiFi connected");      
}
 
void loop() {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
              if (isnan(h) || isnan(t)){
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
                  Serial.print("Temperature: ");
                  Serial.print(t);
                  Serial.println("°C");
                  Serial.print("Humidity: ");
                  Serial.print(h);
                  Serial.println("%");

                  //Update LCD
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Temp: ");
                  lcd.print(t);
                  lcd.print((char)223); // Degree symbol
                  lcd.print("C");  

                  lcd.setCursor(0, 1);
                  lcd.print("Humidity: ");
                  lcd.print(h);
                  lcd.print("%");
                  delay(2000);  


                  if (t > TEMP_MAX) {
                    digitalWrite(LED_PIN, LOW);
                    digitalWrite(LED_PIN1, HIGH);
                    digitalWrite(BUZZER_PIN, LOW);
                  }else{
                    digitalWrite(LED_PIN, HIGH);
                    digitalWrite(LED_PIN1, LOW);
                    digitalWrite(BUZZER_PIN, HIGH);
                  }

                         if (client.connect(server,80))   
                      {
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                        }
          client.stop();
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(15000);
}

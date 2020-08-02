#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

// Pin Define of NodeMCU made by AMICA
#define PIN_D1_BRAKE_LED 5
#define PIN_D2_CONNECT_STATE 4
#define PIN_D8_CONNECT_STATE 15
#define PIN_D5_IN1  14          // Motor Driver L298N IN1
#define PIN_D6_IN2  12          // Motor Driver L298N IN2
#define PIN_D7_EN_A 13          // Motor Driver L298N ENABLE PIN

// global variable
const char *ssid = "";          // Your WiFi SSID
const char *password = "";      // Your WiFi PASS
const char *pathRoot = "";      // HTML file Path
uint8_t buf[16384];             // Buffer for reading HTML
ESP8266WebServer server(80);    // Web server port

void handleRoot()
{                    
  if (server.method() == HTTP_POST)
  {
    String s = server.arg("ON");
    if (s == "Accel")
    {
      digitalWrite(PIN_D5_IN1, HIGH);
      digitalWrite(PIN_D6_IN2, LOW); 
      digitalWrite(PIN_D7_EN_A, HIGH); 
      digitalWrite(PIN_D1_BRAKE_LED, LOW);
    }
    else if (s == "Brake")
    {
      digitalWrite(PIN_D5_IN1, LOW);
      digitalWrite(PIN_D6_IN2, LOW);
      digitalWrite(PIN_D7_EN_A, LOW);
      digitalWrite(PIN_D1_BRAKE_LED, HIGH);
    } 
  }
  server.send(200, "text/html", (char *)buf); // Send the HTML file to the client
}

void serverConnectOk()
{
  Serial.println("serverConnectOk");
  for(int i = 0; i < 5; ++i)
  {
      digitalWrite(PIN_D8_CONNECT_STATE, HIGH);
      digitalWrite(PIN_D2_CONNECT_STATE, LOW);
      delay(200);
      digitalWrite(PIN_D8_CONNECT_STATE, LOW);
      digitalWrite(PIN_D2_CONNECT_STATE, HIGH);
      delay(200);
  }
  Serial.println("serverConnectOkEND");
      digitalWrite(PIN_D8_CONNECT_STATE, HIGH);
      digitalWrite(PIN_D2_CONNECT_STATE, HIGH);
}

void setup() 
{
  pinMode(PIN_D5_IN1, OUTPUT);
  pinMode(PIN_D6_IN2, OUTPUT);
  pinMode(PIN_D7_EN_A, OUTPUT);
  pinMode(PIN_D8_CONNECT_STATE, OUTPUT);
  pinMode(PIN_D2_CONNECT_STATE, OUTPUT);
  pinMode(PIN_D1_BRAKE_LED, OUTPUT);
  
  // Pin output init
  digitalWrite(PIN_D1_BRAKE_LED, HIGH);
  digitalWrite(PIN_D8_CONNECT_STATE, HIGH);
  digitalWrite(PIN_D2_CONNECT_STATE, LOW);
  digitalWrite(PIN_D5_IN1, LOW);
  digitalWrite(PIN_D6_IN2, LOW);
  digitalWrite(PIN_D7_EN_A, LOW);
  Serial.begin(115200);
  
  bool res = SPIFFS.begin();
  if (!res) 
  {
    Serial.println("SPIFFS.begin fail");
    return;
  }
  
  File htmlFile = SPIFFS.open(pathRoot, "r"); // Open the HTML file readonly

  if (!htmlFile)
  {
    Serial.println("open error");
  }
  
  size_t size = htmlFile.size();               // Size check the HTML file
  htmlFile.read(buf, size);                    // Read contents of HTML file into buffer
  htmlFile.close();                            // Close the HTML file
  
  WiFi.softAP(ssid, password);                 // Start the WiFi access point with the set SSID and password, IPAddress myIP = WiFi.softAPIP();
  IPAddress ip = WiFi.softAPIP();
  delay(500);
  Serial.println("");
  Serial.println(ssid + String(" starts..")); 
  Serial.print("this AP : "); 
  Serial.println(ip);
  
  server.on("/", handleRoot);                  // Specifies the function to be executed in response to a request from a client
  server.begin();                              // Boot the Web server.
  serverConnectOk();                           // Connect OK
}

void loop() 
{
  server.handleClient();                       // Process requests received from clients.
}
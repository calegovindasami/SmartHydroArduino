#include <SPI.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>

// WiFi network settings
char ssid[] = "SmartHydro";      // newtork SSID (name). 8 or more characters
char password[] = "Password123"; // network password. 8 or more characters

WiFiEspServer server(80);
RingBuffer buf(8);
int ledPin=13;

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  WiFi.init(&Serial1); // Initialize ESP module using Serial1
  
  pinMode(ledPin, OUTPUT);
  // Check for the presence of the ESP module
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detected. Please check wiring and power.");
    while (true); // Don't proceed further
  }
    
  Serial.print("Attempting to start AP ");
  Serial.println(ssid);

  IPAddress localIp(192, 168, 8, 13); // create an IP address
  WiFi.configAP(localIp); // set the IP address of the AP
  
  // start access point
  // channel is the number. Ranges from 1-14, defaults to 1
  // last comma is encryption type
  WiFi.beginAP(ssid, 11, password, ENC_TYPE_WPA2_PSK);

  Serial.println("Access point started");

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiEspClient client = server.available(); // Check if a client has connected
  
  if (client) { // If a client is available
    Serial.println("New client connected");
    buf.init();
    while (client.connected()) { // Loop while the client is connected
      if (client.available()) { // Check if data is available from the client
        char c = client.read();
        //Serial.write(c); // Echo received data to Serial Monitor
        buf.push(c);
        
        // you got two newline characters in a row
        // that's the end of the HTTP request, so send a response
        if (buf.endsWith("\r\n"))
        {
          sendHttpResponse(client);
          break;
        }
        
        else if (buf.endsWith("/switch")) {
          delay(1000);
          int lightStatus = digitalRead(ledPin);
          lightStatus = lightStatus == HIGH ? LOW : HIGH;
          digitalWrite(ledPin, lightStatus);  
          break;
        }
      }
    }
    
    Serial.println("Client disconnected");
    client.stop(); // Close the connection with the client
  }
}


void sendHttpResponse(WiFiEspClient client)
  {
       
    client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
      "\r\n");
    
   
    //delay(10000);
    client.print("[\n {\n  \"PH\": \"1\",\n  \"EC\": \"5\",\n  \"Humidity\": \"356\",\n  \"Temperature\": \"28\"\n }\n]\n\n");
  }
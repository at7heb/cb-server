#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include "secrets.h"

// See the license file
// Howard Bussey
// Some code copied from https://www.arduino.cc/en/Guide/MKRWiFi1010/hosting-a-webserver

char ssid[] = SSID_VALUE;
char pass[] = PASSWORD;

int wifi_status = WL_IDLE_STATUS;

WiFiServer server(PORT);
WiFiClient remote_client;

void setup() {
  // set up Serial port
  Serial.begin(1000000);
  while (!Serial) ;

  // set up WIFI
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWiFiStatus();

  // RGB LED
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);

}

void loop() {
  remote_client = server.available();
  if (remote_client) {
    Serial.println("got client");
    printWEB();
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware Version: "); Serial.println(fv);
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (wifi_status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifi_status = WiFi.begin(ssid, pass);
    Serial.print("WiFi Status="); Serial.print(wifi_status); Serial.print(" and want it to be="); Serial.println(WL_CONNECTED);

    // wait 10 seconds for connection:
    if (wifi_status != WL_CONNECTED) {
      delay(10000);
    }
  }
}

void printWEB() {

  if (remote_client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (remote_client.connected()) {            // loop while the client's connected
      if (remote_client.available()) {             // if there's bytes to read from the client,
        char c = remote_client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            remote_client.println("HTTP/1.0 200 OK");
            remote_client.println("Content-type:text/html");
            remote_client.println();

            //create the links
            remote_client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            remote_client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

            int randomReading = analogRead(A1);
            remote_client.print("Random reading from analog pin: ");
            remote_client.print(randomReading);

            // The HTTP response ends with another blank line:
            remote_client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /IN HTTP/1.1")) {
          Serial.print("Current Line=["); Serial.print(currentLine); Serial.println("]");
          WiFiDrv::analogWrite(26, 50); //GREEN
        }
        if (currentLine.endsWith("GET /OUT HTTP/1.1")) {
          WiFiDrv::analogWrite(26, 0); //GREEN
        }

        // return application/octet-stream data for the files

      }
    }
    // close the connection:
    remote_client.stop();
    Serial.println("client disconnected");
  }
}

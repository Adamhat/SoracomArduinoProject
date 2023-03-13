// Libraries
#include <MKRNB.h>

#include "arduino_secrets.h"

// SIM PIN #
const char PINNUMBER[]    = SECRET_PINNUMBER;

// Initialize Library Instance
NBClient   client;
GPRS       gprs;
NB         nbAccess;

// URL, PATH and PORT Values
//char       server[]       = "harvest.soracom.io";
//char       path[]         = "/";
//int        port           = 80;
char       server[]       = "quechanirrigation.pythonanywhere.com";
char       path[]         = "/data";
int        port           = 80;


// SORACOM APN Settings
char       apn[]          = "soracom.io";
char       user[]         = "sora";
char       password[]     = "sora";

char       type[]         = "application/json";

// Testing Data
float      flowRate       = 123.12;
float      volume         = 456.23;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Beginning Program");

}

void loop() {
  Serial.println("Preparing Connection");

  // connection state
  boolean connected = false;

  // After starting the modem with NB.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER, apn, user, password))) {
      connected = true;
    } else {
      Serial.println("Connection to cell tower failed, attempting reconnection...");
      delay(1000);
    }
  }

  // Connect to Soracom harvest
  Serial.print("Connected to cell and sending POST request to ");
  Serial.println(server);

  Serial.println("Preparing Data:");

  // Prepare post data
  //String PostData = "{\"FlowRate\":"+ String(flowRate) +",\"Volume\":"+ String(volume) +"}";
  String PostData = "{\"FlowRate\" : \""+ String(flowRate) +"\", \"Volume\" : \""+ String(volume) +"\"}";
  Serial.println(PostData);

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    // Make a HTTP request:
    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.print("Content-Type: ");
    client.println(type);
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);

    // HTTP on plotter for debug:
    Serial.print("POST ");
    Serial.print(path);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.print("Content-Type: ");
    Serial.println(type);
    Serial.print("Content-Length: ");
    Serial.println(PostData.length());
    Serial.println();
    Serial.println(PostData);

    delay(1000); // This delay is necessary to send the POST properly.
    Serial.println("Connected to SERVER...");

  } else {
    // if you didn't get a connection to the server:
    Serial.println("Connection to SERVER failed. Reattempting in 5 minutes.");
  }  
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    Serial.print((char)client.read());
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    Serial.println();
    Serial.println("Disconnected from SERVER. Reattempting in 5 minutes.");
    client.stop();
  }  

  // Switching off NB Access
  nbAccess.shutdown();

  // Wait for 5mins until we send the next sensor reading
  Serial.println("Sleeping for 5 mins before next sensor output.");
  delay(300000);
}

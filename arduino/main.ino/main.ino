#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <String.h>

byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x6F, 0x4A
};

EthernetServer server(80);

void setup() {
  Ethernet.init(10);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Arduino Ethernet WebServer");

  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (true) {
      delay(1);
    }
  }
  
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    while (true) {
      delay(1);
    }
  }

  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  EthernetClient client = server.available();
  if (client) {
    String res = client.readString();
    String params[3];
    int index = 0;
    int start = 0;
    for (int i = 0; i < 3; i++) {
      index = res.indexOf(' ', index);
      params[i] = res.substring(start, index);
      start = index;
      index++;
    }
    if (client.available()) {
      if (params[0] != "GET") {
        sendMethodNotAllowed(client);
      } else {
        sendHeaders(client);
      }
    }
    delay(15);
    client.stop();
  }
}

void sendHeaders(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println();
}

void sendMethodNotAllowed(EthernetClient client) {
  client.println("HTTP/1.1 405 Method Not Allowed");
  client.println("Content-Type: text/html");
  client.println("Allow: GET");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println();
  client.println("<!doctype html>");
  client.println("<html lang=\"en\">");
  client.println("<title>Arduino Dashboard</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>405 Method Not Allowed</h1>");
  client.println("<p>the method received in the request-line is known by the origin server but not supported by the target resource.</p>");
  client.println("</body>");
  client.println("</html>");
  client.println();
}

#include <SPI.h>
#include <Ethernet.h>
#include <SdFat.h>
#include <String.h>

byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x6F, 0x4A
};
SdFat SD;
EthernetServer server(80);

void setup() {
  Ethernet.init(10);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Arduino Ethernet WebServer");

  Serial.print("Starting SD...");
  if (!SD.begin(4)) {
    Serial.println("failed");
    while (true) {
      delay(15);
    }
  }
  else Serial.println("ok");

  Serial.println("Initialize Ethernet with DHCP:");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    while (true) {
      delay(15);
    }
  }

  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  EthernetClient client = server.available();
  if (client) {
    SdFile file;
    {
      char* path;
      {
        String res = client.readStringUntil(" HTTP/1.1");
        char char_array[res.length() + 1];
        res.toCharArray(char_array, res.length() + 1);
        strtok(char_array, " ");
        path = strtok(0, " ");
      }
      if (strcmp(path, "/") == 0) {
        path = "/index.html";
      }
      Serial.println(path);
      if (!file.open(path)) {
        sendNotFound(client);
        delay(15);
        client.stop();
      }
    }
    {
      char buf[25];
      sendHeaders(client);
      while (file.available())
      {
        file.read(buf, sizeof(buf));
        client.write(buf, sizeof(buf));
      }
      delay(15);
      client.stop();
    }
  }
}

void sendHeaders(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Cache-Control: max-age=31536000");
  client.println("Connection: close");
  client.println();
}

void sendNotFound(EthernetClient client) {
  client.println("HTTP/1.1 404 Not Found");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println();
  client.println("<!doctype html>");
  client.println("<html lang=\"en\">");
  client.println("<title>Arduino Dashboard</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>404 Not Found</h1>");
  client.println("<p>The requested URL was not found on this server.</p>");
  client.println("</body>");
  client.println("</html>");
  client.println();
}

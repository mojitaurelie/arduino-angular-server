#include <Ethernet.h>
#include <SdFat.h>
#include <OneWire.h>

EthernetServer server(80);
SdFat SD;

void setup() {
  Ethernet.init(10);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Arduino Ethernet WebServer");

  if (!SD.begin(4)) {
    Serial.println("Failed initialize SD Card");
    while (true) {
      delay(15);
    }
  }

  Serial.println("Initialize Ethernet with DHCP:");
  byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x6F, 0x4A };
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
    {
      char* path;
      free(readStringUntil(client, ' '));
      path = readStringUntil(client, ' ');
      if (strcmp(path, "/") == 0) {
        free(path);
        path = "/index.html";
        sendFile(path, client);
      } else if (strcmp(path, "/sensors") == 0) {
        free(path);
        sendHeaders(client);
        sendSensors(client);
      } else {
        sendFile(path, client);
        free(path);
      }
      
    }
  }
}

void sendFile(char* path, EthernetClient client) {
  SdFile file;
  if (!file.open(path)) {
    sendNotFound(client);
    client.stop();
  }
  {
    char buf[25];
    sendHeaders(client);
    while (file.available())
    {
      file.read(buf, sizeof(buf));
      client.write(buf, sizeof(buf));
    }
    file.close();
    client.stop();
  }
}

void sendSensors(EthernetClient client) {
  float temperature;
  if (getTemperature(&temperature, true) != 0) {
    client.print("[{ \"name\": \"Room temperature\", \"value\": ");
    client.print(0);
    client.print("}]");
    client.stop();
    return;
  }
  client.print("[{ \"name\": \"Room temperature\", \"value\": ");
  client.print(temperature, 2);
  client.print("}]");
  client.stop();
}

void sendHeaders(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Cache-Control: max-age=31536000");
  client.println("Connection: close");
  client.println();
}

void sendInternalServerError(EthernetClient client) {
  client.println("HTTP/1.1 500 Internal Server Error");
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

byte getTemperature(float *temperature, byte reset_search) {
  OneWire ds(2);
  byte data[9], addr[8];
  // data[] : Données lues depuis le scratchpad
  // addr[] : Adresse du module 1-Wire détecté

  if (reset_search) {
    ds.reset_search();
  }

  if (!ds.search(addr)) {
    // Pas de capteur
    return 1;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    // Adresse invalide
    return 2;
  }

  if (addr[0] != 0x28) {
    // Mauvais type de capteur
    return 3;
  }

  ds.reset();
  ds.select(addr);

  ds.write(0x44, 1);
  delay(800);

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  for (byte i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  *temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625;

  // Pas d'erreur
  return 0;
}

char* readStringUntil(EthernetClient client, char c) {
  char last = '\0';
  char* result = malloc(sizeof(char) * 10);
  for (byte i = 0; i < 9; i++) {
    result[i] = ' ';
  }
  result[9] = '\0';
  byte i = 0;
  while (last != c && i < 9) {
    last = client.read();
    if (last != c) {
      result[i] = last;
      i++;
    }
  }
  result[i] = '\0';
  return result;
}

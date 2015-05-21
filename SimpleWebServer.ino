#include <EtherCard.h>

static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
static byte myip[] = { 192, 168, 1, 45 };

byte Ethernet::buffer[500];
BufferFiller bfill;

const char HttpNotFound[] PROGMEM =
  "HTTP/1.0 404 Unauthorized\r\n"
  "Content-Type: text/html\r\n\r\n"
  "<h1>404 Page Not Found</h1>";


void setup ()
{
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
  {
    Serial.println("Failed to access Ethernet controller");
  }
  ether.staticSetup(myip);
}

static word HomePage(word useRed)
{
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
                 "HTTP/1.0 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Pragma: no-cache\r\n"
                 "\r\n"
                  "<head>"
                  "<style>"
                  "body {background-color:lightgray}"
                  "h1   {color:$S}"
                  "</style>"
                  "<title>Hello!</title>"
                 "<h1>Hello from Arduino!</h1>"), useRed ? "red" : "blue");
  return bfill.position();
}

void loop ()
{
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos)  // check if valid tcp data is received
  {
    bfill = ether.tcpOffset();
    char* data = (char*) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0)
    {
      bfill.emit_p(HttpNotFound);
    }
    else
    {
      data += 5;
      if (strncmp("?red", data, 4) == 0) 
      {
        HomePage(1);
      }
      else 
      {
        HomePage(0);
      }
    }
    ether.httpServerReply(bfill.position());    // send http response
  }
}

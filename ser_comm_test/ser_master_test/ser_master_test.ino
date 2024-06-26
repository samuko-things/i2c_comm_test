#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX TX

char Mymessage[6]; // Initialized variable to store recieved data

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);
}

void loop()
{
  if (mySerial.available() > 0)
  {
    delay(1);
    while (mySerial.available())
    {
      byte c = mySerial.read();
      Serial.println(c);
    }
    Serial.println();
  }
  delay(10);
}
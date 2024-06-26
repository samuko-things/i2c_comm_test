
byte Mymessage[6] = {10, 104, 78, 20, 90, 12}; // byte data

void setup()
{
  Serial.begin(115200);
}

void loop()
{

  Serial.write(Mymessage, 6); // Write the serial data

  delay(1000);
}
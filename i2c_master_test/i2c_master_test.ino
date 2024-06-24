/*
  I2C Master Demo
  i2c-master-demo.ino
  Demonstrate use of I2C bus
  Master sends character and gets reply from Slave
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/

// Include Arduino Wire library for I2C
#include <Wire.h>

// Define Slave I2C Address
#define SLAVE_ADDR 9

const byte FILTER_GAIN_ADDRESS = 1;
const byte RPY_ADDRESS = 2;

// value per bit(LSB)
float rpy_resolution = 0.000123077; // -4.00 to +4.00 (rad/s)
float filter_gain_resolution = 0.001538462; //-50.0 to 50.0

byte highByte, lowByte;

//------------------------------------------------------//
int combineBytesToInt(byte highByte, byte lowByte)
{
  return (highByte << 8) | lowByte;
}

void splitIntToBytes(int value, byte &high, byte &low)
{
  high = (value >> 8) & 0xFF;
  low = value & 0xFF;
}

int convertActualToIntData(float data, float resolution)
{
  return (int)(data / resolution);
}

float convertIntToActualData(int data, float resolution)
{
  return (float)data * resolution;
}

void splitActualToBytes(float data, float resolution, byte &high, byte &low)
{
  int int_data = convertActualToIntData(data, resolution);
  splitIntToBytes(int_data, high, low);
}

float combineBytesToActual(float resolution, byte high, byte low)
{
  int int_data = combineBytesToInt(high, low);
  return convertIntToActualData(int_data, resolution);
}
//--------------------------------------------------------//

void setup()
{

  // Initialize I2C communications as Master
  Wire.begin();

  // Setup serial monitor
  Serial.begin(115200);
  Serial.println("I2C Master Demonstration");

  delay(4000);

  // Send filter gain value of 0.5
  float gain = 20.05;
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(FILTER_GAIN_ADDRESS);
  splitActualToBytes(gain, filter_gain_resolution, highByte, lowByte);
  Wire.write(highByte);
  Wire.write(lowByte);
  Wire.endTransmission();

  // read the filter gain that was just set.
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(FILTER_GAIN_ADDRESS); // which address you want to read from
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADDR, 2); // request 2 bytes of data
  int data = Wire.read() << 8 | Wire.read();
  float read_gain = convertIntToActualData(data, filter_gain_resolution);

  Serial.print("Received Gain: ");
  Serial.println(read_gain, 3);

  delay(2000);
}

void loop()
{
  delay(100);

  // read the roll pitch and yaw value.
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(RPY_ADDRESS); // which address you want to read from
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADDR, 6); //request 6 bytes of data
  int r_data = Wire.read() << 8 | Wire.read();
  int p_data = Wire.read() << 8 | Wire.read();
  int y_data = Wire.read() << 8 | Wire.read();
  float roll = convertIntToActualData(r_data, rpy_resolution);
  float pitch = convertIntToActualData(p_data, rpy_resolution);
  float yaw = convertIntToActualData(y_data, rpy_resolution);

  String msg = "RPY: [" + String(roll, 4) + "," + String(pitch, 4) + "," + String(yaw, 4) + "]";
  Serial.println(msg);
  Serial.println();
}
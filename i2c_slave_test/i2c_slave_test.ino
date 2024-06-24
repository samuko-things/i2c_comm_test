/*
  I2C Slave Demo
  i2c-slave-demo.ino
  Demonstrate use of I2C bus
  Slave receives character from Master and responds
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/

// Include Arduino Wire library for I2C
#include <Wire.h>

// Define Slave I2C Address
const byte SLAVE_ADDR = 9;

// Access variable data with address
const byte FILTER_GAIN_ADDRESS = 1;
const byte RPY_ADDRESS = 2;

// actual value per integer value
float filter_gain_resolution = 0.001538462;
float rpy_resolution = 0.000123077; // -4.00 to +4.00 (rad/s)

byte address;
byte dataBufferInBytes[6];

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

int convertActualToIntData(float ang, float resolution)
{
  return (int)(ang / resolution);
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

//------------------------------------------------------//
float filterGain = 0.1;

float roll = 0.0;  // rad/sec
float pitch = 0.0; // rad/sec
float yaw = 0.0;   // rad/sec

void myCode()
{
  if (roll < -4.00)
    roll = 0.000;

  if (pitch > 4.00)
    pitch = 0.000;

  if (yaw < -4.00)
    yaw = 0.000;

  roll -= 0.00025;
  pitch += 0.0005;
  yaw -= 0.0025;
}
//------------------------------------------------------//

//-----------------------------------------------------//
void saveData(byte data_address)
{
  switch (data_address)
  {
  case FILTER_GAIN_ADDRESS:
    filterGain = combineBytesToActual(filter_gain_resolution, dataBufferInBytes[0], dataBufferInBytes[1]);
    break;

  default:
    break;
  }
}

void getData(byte data_address)
{
  int int_data;
  switch (data_address)
  {
  case FILTER_GAIN_ADDRESS:
    splitActualToBytes(filterGain, filter_gain_resolution, dataBufferInBytes[0], dataBufferInBytes[1]);
    break;

  case RPY_ADDRESS:
    splitActualToBytes(roll, rpy_resolution, dataBufferInBytes[0], dataBufferInBytes[1]);
    splitActualToBytes(pitch, rpy_resolution, dataBufferInBytes[2], dataBufferInBytes[3]);
    splitActualToBytes(yaw, rpy_resolution, dataBufferInBytes[4], dataBufferInBytes[5]);
    break;

  default:
    break;
  }
}

void clearAllDataBuffer()
{
  dataBufferInBytes[0] = 0;
  dataBufferInBytes[1] = 0;
  dataBufferInBytes[2] = 0;
  dataBufferInBytes[3] = 0;
  dataBufferInBytes[4] = 0;
  dataBufferInBytes[5] = 0;
}

void receiveDataEvent(int dataSizeInBytes)
{
  address = Wire.read();
  float checkVal = (float)address;
  if (dataSizeInBytes == 3)
  {
    dataBufferInBytes[0] = Wire.read(); // highbyte
    dataBufferInBytes[1] = Wire.read(); // lowbyte

    saveData(address);

    checkVal = filterGain;
    address = 0;
    clearAllDataBuffer();
  }

  // Print to Serial Monitor
  Serial.print("Received: ");
  Serial.println(checkVal, 4);
}

void sendDataEvent()
{
  // Setup byte variable in the correct size
  getData(address);

  // Send response back to Master
  // Wire.write(dataBufferInBytes, sizeof(dataBufferInBytes));
  Wire.write(dataBufferInBytes, 6);

  clearAllDataBuffer();
}
//-----------------------------------------------------//

unsigned long serialCommTime, serialCommSampleTime = 10; // ms -> (1000/sampleTime) hz

void setup()
{

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);

  // Function to run when data requested from master
  Wire.onRequest(sendDataEvent);

  // Function to run when data received from master
  Wire.onReceive(receiveDataEvent);

  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println("I2C Slave Demonstration");
}

void loop()
{

  ////////// using the serial communiaction API ////////////////////////
  if ((millis() - serialCommTime) >= serialCommSampleTime)
  {
    myCode();
    serialCommTime = millis();
  }
  /////////////////////////////////////////////////////////////////////
}
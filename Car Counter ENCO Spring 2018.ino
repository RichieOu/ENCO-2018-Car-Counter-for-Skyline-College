//////////////////// Car Counter  ENCO SPRING 2018. Richard Ou and Kelly Wong /////////////////////
#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); //This denotes which pin on the SparkFun Explorer.
int sensorPin1 = A0; //If Sensor 1 is greater than sensor 2 bool=false
int sensorPin2 = A2; //If Sensor 2 is greater than sensor 1 bool = true
int ledPin = 11; //Used to see if sensors are being triggered
int sensorValue1 = 0; //Initialize to 0. This will change in serial monitor.
int sensorValue2 = 0;
int count = 0; //This is an updating count of how many cars passed through the sensor. Subtract the count from parkingMax to display vacancy.
int parkingMax = 5; //This is the parking lot's maximum vacancy. Set this equal to the number of student spaces avaliable. 
int Overflow = 0; //This is a measure of how many cars continue to enter the low when "LOT FULL" message is displayed. Could also be utilized to say there is ___ extra cars in lot.
bool CountUpFlag = false; //This will be used to tell the sensor to only detect the sensor change the INSTANCE the wheel touches the tube.
bool CountDownFlag = false; //We'll use one for both exit and entrance to make life easier.

void setup()
{
  Serial.begin (9600); //Baud rate for serial monitor
  digitalWrite(sensorPin1, HIGH); //Turns the sensor pin on to be read within the loop.
  digitalWrite(sensorPin2, HIGH); //Turns sensor 2 on to be read
  XBee.begin (9600); //Buad rate for the XBee Radios.
}

void loop() {
  sensorValue1 = analogRead(sensorPin1); //Reads the sensor pin value and records it as sensor value.
  sensorValue2 = analogRead(sensorPin2); //Reads the sensor pin value and records it as sensor value.
  Serial.print ("Cars in lot: ");
  Serial.print (count); //Prints out the count of how many cars are in the parking lot.
  Serial.print ("\n"); //Spacing for new line.
  Serial. print ("Overflow Count: ");
  Serial.print (Overflow);
  Serial.print ("\n");

  if (XBee.available()) { //If Radio is open for communication
    byte PairedSystem = XBee.read(); //Reads the paired system's output. Paired system sends byte types.
    if (PairedSystem == 'I') //I for In
    {
      count++; //Car comes in, count up.
      //No need for a maximum. Both devices can set a local max during initialization.
    }
    if (PairedSystem == 'O') //O for Out
    {
      if (count <= parkingMax && count > 0) 
      {
        count--; //Car goes out, count down. But never fall below 0.
      }
      if (count == parkingMax) 
      {
        //Print LOT FULL. We need to communicate a maximum between the systems.
        Serial.print ("LOT FULL");
      }
      if (Overflow>0)
      {
        Overflow--; //If we have an overflow, decrement the overflow count. 
      }
     }
    }
  if (sensorValue1 >= 120 && !CountUpFlag)
  { //Only record if sensorValues are > 120 & if the countdown flag is open for reading.
    delay(1000); //This delay allows both axles to cross over and count the car as 1 reading.
    CountUpFlag = true;
    if (count < parkingMax) {
      count++;
      XBee.write ('I'); //Communicate with other system that a car has entered.
    }
  }
  if (sensorValue1 < 80 && CountUpFlag)
  { //Resetting the Flag
    CountUpFlag = false;
  }
  if (sensorValue2 >= 120 && !CountDownFlag)
  {
    delay (1000); //Delay for both axles crossing and having 1 count.
    CountDownFlag = true;

    if (count <= parkingMax && count > 0 )
    { //Cannot fall below 0.
      count--;
      XBee.write('O'); //Comunicate with the other systems that a car has left.
    }
  }
  if (sensorValue2 < 80 && CountDownFlag)
  { //Resetting the Flag
    CountDownFlag = false;
  }

  if (count == parkingMax)
  {
    //Display full when all parking spots have been taken.
    Serial.print("Lot FULL");
  }
  //Overflow Measures
  if (sensorValue1 >= 120 && !CountUpFlag)
  { 
    //Only record if sensorValues are > 120 & if the countdown flag is open for reading.
    delay(1000); //This delay allows both axles to cross over and count the car as 1 reading.
    CountUpFlag = true;
      if (count >= parkingMax)
      {
        Overflow++;
      }
    }
  }

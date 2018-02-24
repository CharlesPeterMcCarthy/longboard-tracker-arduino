#include <Time.h>
#include <TimeLib.h>

const double WHEEL_CIRCUM = 235.6194;   // Circumference of the longboard wheel in (mm)
const int REED_PIN = 2;
int prevReedVal = 0;                    // Inital value
int curReedVal;
time_t time;
double totalDistance = 0;               // Initial value
//double totalSeconds = 0;

void setup() {
  pinMode(REED_PIN, INPUT);
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  getReedReading();
  delay(1); 
}

void getReedReading() {
  curReedVal = digitalRead(REED_PIN);

  if (curReedVal != prevReedVal && curReedVal == 1) {
    // Magnet has triggered the reed switch
    logRevolution(curReedVal);
    addDistance();
    getSeconds();
  }

  prevReedVal = curReedVal;
}

void logRevolution(int reedVal) {
  Serial.println("Full Revolution");
}

void addDistance() {
  totalDistance += WHEEL_CIRCUM;
  
  Serial.print("Distance: ");
  Serial.print(totalDistance);              // Millimeters
  Serial.println(" mm");

  Serial.print("Distance: ");
  Serial.print(totalDistance / 1000);       // Meters
  Serial.println(" m");

  Serial.print("Distance: ");
  Serial.print(totalDistance / 1000000);    // Kilometers
  Serial.println(" km");

  Serial.println("----------------");
}

void getSeconds() {
  time = now();
  
  Serial.print("Total Seconds: ");
  Serial.println(time);
}

String getDatetime() {
  time = now();
  Serial.print(hour(time));
  Serial.print(":");
  Serial.print(minute(time));
  Serial.print(":");
  Serial.println(second(time));
  delay(1000);
}


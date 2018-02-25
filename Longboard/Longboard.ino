  #include <Time.h>
  #include <TimeLib.h>
  
  const double WHEEL_CIRCUM = 235.6194;   // Circumference of the 75mm (diameter) longboard wheel in (mm)
  const int REED_PIN = 2;
  int prevReedVal = 0;                    // Inital value
  int curReedVal;
  time_t time;
  double totalDistance = 0;               // Initial value
  long curRevTime;
  long lastRevTime = 0;
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
      
      //logRevolution(curReedVal);
      addDistance();
      //getSeconds();

      curRevTime = millis();        // Get program length in milliseconds
      calculateSpeed(curRevTime);
      lastRevTime = curRevTime;

      Serial.println("----------------");
    }
  
    prevReedVal = curReedVal;
  }
  
  void logRevolution(int reedVal) {
    // To be used to save speeds and times to an array
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
    Serial.print((totalDistance / 1000000), 3);    // Kilometers
    Serial.println(" km");
  }
  
  void getSeconds() {
    time = now();
    
    Serial.print("Total Seconds: ");
    Serial.println(time);
  }

  void calculateSpeed(long curRevTime) {
    long revTimeLength = curRevTime - lastRevTime;                      // Wheel Revolution Period
    double distKM = WHEEL_CIRCUM / 1000000;                             // Change mm to km
    double revTimeHours = (double) revTimeLength / 1000 / 60 / 60;      // Change milliseconds to hours
    double KPH = distKM / revTimeHours;                                 // Speed = Distance / Time

    /*Serial.print("Rev Length (milliseconds): ");
    Serial.println(revTimeLength);
    Serial.print("Distance (KM): ");
    Serial.println(distKM, 14);
    Serial.print("Rev Length (hours): ");
    Serial.println(revTimeHours, 14);*/
    Serial.print("Speed: ");
    Serial.print(KPH);
    Serial.println(" KPH");
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


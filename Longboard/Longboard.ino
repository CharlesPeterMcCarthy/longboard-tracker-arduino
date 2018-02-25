  #include <Time.h>
  #include <TimeLib.h>
  #include <rgb_lcd.h>
  #include <Process.h>
  //#include <MemoryFree.h>
  
  const double WHEEL_CIRCUM = 235.6194;   // Circumference of the 75mm (diameter) longboard wheel in (mm)
  const int REED_PIN = 5;
  int lastReedVal = 0;                    // Initial value
  int curReedVal;
  long lastRevTime = 0;                   // Initial value
  long curRevTime;
  double curSpeed;
  double totalDistance = 0;               // Initial value
  time_t time;
  rgb_lcd lcd;
  
  void setup() {    
    pinMode(REED_PIN, INPUT);

    Bridge.begin();                 // Enable communication with Linux chip
    
    Serial.begin(9600);
    delay(1000);
    Serial.println("Starting Up...");
    
    lcd.begin(16,2);                // 2 rows, 16 columns
    lcd.setRGB(24, 167, 219);       // Set blue background
    lcd.print("Starting Up...");
    
    delay(2000);

    clearLCD();
    lcd.print("Begin Skating!");
  }
  
  void loop() {
    getReedReading();
    delay(1); 
  }
  
  void getReedReading() {
    curReedVal = digitalRead(REED_PIN);
  
    if (curReedVal != lastReedVal && curReedVal == 1) {     // Magnet has triggered the reed switch
      clearLCD();      
      addDistance();
      calculateSpeed();
      displaySpeed();
      logSpeed();

      Serial.println("----------------");
    }
  
    lastReedVal = curReedVal;
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

    lcd.setCursor(0, 0);    // Row (line) 1
    lcd.print("Dist: ");
    lcd.print(totalDistance / 1000);
    lcd.print(" m");
  }
  
  long getSecondsElapsed() {
    return now();         // Returns the total amount of seconds that has elapsed during execution
  }

  void calculateSpeed() {      // Calculate speed of longboard in Kilometers Per Hour
    curRevTime = millis();                                              // Get program length in milliseconds
    long revTimeLength = curRevTime - lastRevTime;                      // Wheel Revolution Period
    double distKM = WHEEL_CIRCUM / 1000000;                             // Change mm to km
    double revTimeHours = (double) revTimeLength / 1000 / 60 / 60;      // Change milliseconds to hours
    curSpeed = distKM / revTimeHours;                                   // Speed = Distance / Time
    lastRevTime = curRevTime;
  }

  void displaySpeed() {
    Serial.print("Speed: ");
    Serial.print(curSpeed);
    Serial.println(" KPH");

    lcd.setCursor(0, 1);    // Row (line) 2
    lcd.print("Speed: ");
    lcd.print(curSpeed);
    lcd.print(" KPH");
  }

  void logSpeed() {
    long seconds = getSecondsElapsed();
    // Send speed, distance, seconds to Shell Script on Linux chip
  }

  void clearLCD() {
    lcd.clear();
  }


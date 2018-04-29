#include <Time.h>
#include <TimeLib.h>
#include <FileIO.h>
#include <rgb_lcd.h>
#include <Process.h>

const double WHEEL_CIRCUM = 235.6194;   // Circumference of the 75mm (diameter) longboard wheel in (mm)
const byte HALL_PIN = A0;
const byte BUTTON_PIN = 8;
const byte TIME_INTERVAL = 2;            // Seconds between data log
char* TEXT_FILE_URL = "/mnt/sd/longboard_project/speeds.txt";         // Text file containing speeds logs

bool isSessionActive = false;           // Flag for active skate session
byte curBtnVal = 0;
byte lastReedVal = 0;                    // Initial value
byte curReedVal;
long lastRevTime = 0;                   // Initial value
long curRevTime;
double curSpeed;
double totalDistance = 0;               // Initial value

double curIntervalTotalSpeed = 0;
double curIntervalTotalRevs = 0;

int sensorValue =  0;
int norm = 525;                         // Normal magnetic reading
bool isTriggered = false;

time_t time;
rgb_lcd lcd;
Process linux;

void setup() {
  pinMode(HALL_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  Bridge.begin();                 // Enable communication with Linux chip
  FileSystem.begin();
  Serial.begin(9600);

  delay(1000);
  Serial.println("Starting Up...");
  setupLCD();
  delay(2000);

  displayStartScreen();
}

void setupLCD() {
  lcd.begin(16,2);                // 2 rows, 16 columns
  lcd.setRGB(24, 167, 219);       // Set blue background
  lcd.print("Starting Up...");
}

void displayStartScreen() {
  clearLCD();
  lcd.print("To Start:");
  lcd.setCursor(0, 1);
  lcd.print("Click Button");
}

void loop() {
  checkButtonValue();

  if (isSessionActive) {
    getHallReading();
  }

  delay(1); // Stability
}

    /*  Check the value of the button used to
        control the start and end of a skate session */

void checkButtonValue() {
  int btnVal = digitalRead(BUTTON_PIN);

  if (btnVal == 1) {                        // Button has been pressed
    Serial.println("Button On");
    while (digitalRead(BUTTON_PIN) == 1);   // Wait until button is not being pressed

    if (!isSessionActive) {
      startSkateSession();
    } else {
      endSkateSession();
    }
  }
}

void startSkateSession() {
  isSessionActive = true;

  createLogFile();
}

void endSkateSession() {
  isSessionActive = false;

  logTotalDistance();
  deleteLogFile();
  sendDataToAPI();
  resetValues();
}

void createLogFile() {
  File dataFile = FileSystem.open(TEXT_FILE_URL, FILE_APPEND);
}

void deleteLogFile() {
  linux.begin("rm");
  linux.addParameter(TEXT_FILE_URL);
  linux.run();
}

void getHallReading() {
  sensorValue = analogRead(HALL_PIN);

  if ((sensorValue > (norm + 5) || sensorValue < (norm - 5)) && !isTriggered) {
    isTriggered = true;

    clearLCD();
    addDistance();
    calculateSpeed();
    displayDistance();
    displaySpeed();
    checkInterval();
  }

  if ((sensorValue <= (norm + 1) && sensorValue >= (norm - 1)) && isTriggered) {
    isTriggered = false;
  }
}

void addDistance() {
  totalDistance += WHEEL_CIRCUM;
}

    /*  Calculate speed of longboard in Kilometers Per Hour   */

void calculateSpeed() {
  curRevTime = millis();                                              // Get program length in milliseconds
  long revTimeLength = curRevTime - lastRevTime;                      // Wheel Revolution Period
  double distKM = WHEEL_CIRCUM / 1000000;                             // Change mm to km
  double revTimeHours = (double) revTimeLength / 1000 / 60 / 60;      // Change milliseconds to hours
  curSpeed = distKM / revTimeHours;                                   // Speed = Distance / Time
  lastRevTime = curRevTime;
}

void displayDistance() {
  lcd.setCursor(0, 0);    // Row (line) 1
  lcd.print("Dist: ");
  lcd.print(totalDistance / 1000);
  lcd.print(" m");
}

void displaySpeed() {
  lcd.setCursor(0, 1);    // Row (line) 2
  lcd.print("Speed: ");
  lcd.print(curSpeed);
  lcd.print(" KPH");
}

void clearLCD() {
  lcd.clear();
}

void checkInterval() {
  curIntervalTotalSpeed += curSpeed;
  curIntervalTotalRevs++;

  if (now() % TIME_INTERVAL == 0) {       // Every *interval* seconds
    double avgSpeed = curIntervalTotalSpeed / curIntervalTotalRevs;     // Get average speed

    curIntervalTotalSpeed = 0;    // Reset values for next interval
    curIntervalTotalRevs = 0;

    logSpeed(avgSpeed);
  }
}

void logSpeed(double avgSpeed) {
  File dataFile = FileSystem.open(TEXT_FILE_URL, FILE_APPEND);    // Open data log file

  if (dataFile) {                                               // If the file is available, write to it
    dataFile.println(avgSpeed);                                 // Save average speed
  } else {
    Serial.println("Error Opening Data File!");
  }
}

    // Log total distance in text file after skate session ends
void logTotalDistance() {
  File dataFile = FileSystem.open(TEXT_FILE_URL, FILE_APPEND);

  dataFile.print("d_");                                     // Used so shell script can recognise this is distance
  dataFile.println((totalDistance / 1000000), 3);
}

void sendDataToAPI() {
  // Call shell script
  // runShellCommandAsynchronously() -- Not needed?
}

    // Reset all session values eg. Distance, current speed, etc.
void resetValues() {
  lastRevTime = 0;
  curRevTime = 0;
  curSpeed = 0;
  totalDistance = 0;
}

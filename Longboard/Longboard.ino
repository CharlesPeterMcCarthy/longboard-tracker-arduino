#include <ArduinoJson.h>
#include <Time.h>
#include <TimeLib.h>
#include <FileIO.h>
#include <rgb_lcd.h>
#include <Process.h>

const double WHEEL_CIRCUM = {{WHEEL_CIRCUMFERENCE}};   // Enter your longboard wheel circumference here
const byte HALL_PIN = A1;
const byte BUTTON_PIN = 8;
const byte TIME_INTERVAL = 2;            // Seconds between data log
char* TEXT_FILE_URL = "speeds.txt";

bool isSessionActive = false;           // Flag for active skate session
byte curBtnVal = 0;
byte lastReedVal = 0;                    // Initial value
byte curReedVal;
long lastRevTime = 0;                   // Initial value
long curRevTime;
double curSpeed;
double totalDistance = 0;               // Initial value

long lastLogTime = 0;

double curIntervalTotalSpeed = 0;
double curIntervalTotalRevs = 0;

int sensorValue =  0;
int magneticNorm = {{MAGNET_NORMAL}};         // Set this to the normal magnetic reading
bool isTriggered = false;

time_t time;
rgb_lcd lcd;
Process linux;
StaticJsonBuffer<200> jsonBuffer;

void setup() {
  Serial.begin(9600);
  Bridge.begin();                 // Enable communication with Linux chip
  FileSystem.begin();

  delay(1000);
  pinMode(HALL_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  setupLCD();
  clearLCD();
  displayStartScreen();
}

void setupLCD() {
  lcd.begin(16,2);                // 2 rows, 16 columns
  lcd.setRGB(24, 167, 219);       // Set blue background
  lcd.print("Starting Up...");
  delay(2000);
}

void displayStartScreen() {
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

  emptyLogFile();
}

void endSkateSession() {
  isSessionActive = false;

  logTotalDistance();
  sendDataToAPI();
  resetValues();
}

void emptyLogFile() {
  linux.runShellCommand("echo -n > speeds.txt");
}

void getHallReading() {
  sensorValue = analogRead(HALL_PIN);

  if ((sensorValue > (magneticNorm + 5) || sensorValue < (magneticNorm - 5)) && !isTriggered) {
    isTriggered = true;

    clearLCD();
    addDistance();
    calculateSpeed();
    displayDistance();
    displaySpeed();
    checkInterval();

    Serial.println("----------------");
  }

  if ((sensorValue <= (magneticNorm + 1) && sensorValue >= (magneticNorm - 1)) && isTriggered) {
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

  long curTime = now();

  if (lastLogTime != curTime && curTime % 2 == 0) {       // Every *interval* seconds
    double avgSpeed = curIntervalTotalSpeed / curIntervalTotalRevs;     // Get average speed

    curIntervalTotalSpeed = 0;    // Reset values for next interval
    curIntervalTotalRevs = 0;
    lastLogTime = curTime;

    logSpeed(avgSpeed);
  }
}

void logSpeed(double avgSpeed) {
  File dataFile = FileSystem.open(TEXT_FILE_URL, FILE_APPEND);    // Open data log file

  if (dataFile) {                                               // If the file is available, write to it
    dataFile.println(avgSpeed);                              // Save average speed
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
  String response = "";

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Sending Data");
  lcd.setCursor(0, 1);
  lcd.print("To API...");

  linux.runShellCommand("ash send_speeds.sh");    // Run ash (not bash)

  while (linux.running());          // Wait for script to run
  while (linux.available()) {       // Get response
    response += linux.readString();
  }

  Serial.println(response);
  Serial.flush();

  JsonObject& json = ChangeToJSON(response);
  PrintResponseData(json);
}

JsonObject& ChangeToJSON(String st) {
  Serial.println(st);
  JsonObject& json = jsonBuffer.parseObject(st);

  return json;
}

void PrintResponseData(JsonObject& response) {
  String sessionID = response["sessionID"];
  String sessionLength = response["sessionLength"];
  String avgSpeed = response["averageSpeed"];
  String highSpeed = response["highestSpeed"];

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Session #");
  lcd.print(sessionID);

  delay(3000);

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Total Time:");
  lcd.setCursor(0, 1);
  lcd.print(sessionLength);
  lcd.print(" Seconds");

  delay(3000);

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Average Speed:");
  lcd.setCursor(0, 1);
  lcd.print(avgSpeed);
  lcd.print(" KM/H");

  delay(3000);

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Highest Speed:");
  lcd.setCursor(0, 1);
  lcd.print(highSpeed);
  lcd.print(" KM/H");

  delay(3000);

  clearLCD();
  lcd.setCursor(0, 0);
  lcd.print("Click Button");
  lcd.setCursor(0, 1);
  lcd.print("To Go Again!");
}

    // Reset all session values eg. Distance, current speed, etc.
void resetValues() {
  lastRevTime = 0;
  curRevTime = 0;
  curSpeed = 0;
  totalDistance = 0;
}

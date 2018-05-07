# longboard_arduino
C++ (Arduino) Program to monitor longboard/skateboard stats (distance, speed, etc.)

[Project Demo YouTube Video](https://www.youtube.com/watch?v=q_IGEHvx30A)

![Longboard Project Image 1](images/img1.jpg?raw=true "Longboard Project Image 1")

## What You Will Need
  - Arduino Yún
  - Grove Kit
    - Grove Base Shield
    - Grove LCD RGB Screen
    - Grove Button Module
    - 2x Grove Connector Cables
  - Analog Hall Magnetic Sensor Module
  - 4x Male-to-Female Jumper Cables
  - Power Source (Portable USB Charger)
  - Micro SD Card
  - Micro SD Card Adapter
  - Micro USB Cable
  - A Magnet
  - A Longboard
  - PC / Laptop
  - Remote Web Server

*Note: This can all be done with or without a breadboard - Using a breadboard will give more functionality for any future additions to the project. For now, I will explain it without using the breadboard.*

## How To Set Up (Hardware)
  - Connect the Grove Base Shield to the Arduino
  - Connect the Grove LCD to one of the `I2C` ports on the Base Shield using a Grove Connector Cable
  - Connect the Grove Button Module to pin `8` on the Base Shield using a Grove Connector Cable
  - Connect one Male-to-Female Jumper Cable from the Analog Pin `A1` on the Base Shield to the `A0` connection on the Analog Hall Module
  - Connect one Male-to-Female Jumper Cable from the `GND` (Ground) Pin on the Base Shield to the `G` connection on the Analog Hall Module
  - Connect one Male-to-Female Jumper Cable from the `5V` Pin on the Base Shield to the `+` connection on the Analog Hall Module
  - Connect one Male-to-Female Jumper Cable from Pin `7` on the Base Shield to `D0` connection on the Analog Hall Module
  - Attach the Analog Hall Module to the side of the longboard deck, inside of the wheel
  - Attach the magnet to the inside of the wheel

## How To Set Up (Software)
  - Follow the steps in these repositories first:
    - [Database GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_db)
    - [API GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_api)
    - [Web App GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_web_app)
  - Hold down the WLAN RST button on the Arduino Yún for 5 seconds
  - Connect your PC / Laptop to the Arduino's WiFi network
  - Go `(http://arduino.local/)` (Arduino) or `(http://192.168.240.1)` (Linino) in your browser
  - Enter the Arduino password (The default is `arduino` (Arduino) or `doghunter` (Linino))
  - The Arduino and Linino Dashboards have different layouts depending on the version
  - Find the section that allows you to Configure the WiFi
  - Locate the WiFi network that you want to connect your Arduino and PC / Laptop to
  - Enter your network password
  - Click `Configure`
  - When this process has finished, re-connect your PC / Laptop to the same WiFi network
  - Follow the steps for adding the necessary files to the Arduino here: [Shell Script GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_shell_script)
  - When you have done that, open `/Longboard/Longboard.ino` in the [Arduino IDE](https://www.arduino.cc/en/Main/Software)
  - Change the values listed below under [To Use](https://github.com/CharlesPeterMcCarthy/longboard_arduino#to-use)
  - Upload the sketch from the Arduino IDE onto the Arduino via USB or WiFi
  - Wait for the LCD to light up and say `"Starting up..."`
  - Wait for the LCD to show `"To Start Click Button"`
  - Click the button attached to the Arduino
  - Spin the wheel with the magnet attached to it
  - The Distance and Speed should be displayed on the LCD
  - Click the button again
  - Information should be received back from the API via Shell and will be displayed on the LCD

### To Use
  - Change `{{WHEEL_CIRCUMFERENCE}}` to the Circumference of the longboard wheel
  - Change `{{MAGNET_NORMAL}}` to the normal magnetic reading (*Test out the Analog Hall Module separately first to find out what reading it normally gets - I find mine gets a normal reading of around 524/525 when no magnets are nearby*)

## How It Works
  - A magnet is attached to the inside of the wheel of the longboard, with a magnetic sensor (Analog Hall) attached to the side of the board
  - The user is prompted via the LCD to click the button attached to the Arduino to begin their skate
  - When the button is clicked, the device is monitoring the analog value coming from the magnetic sensor
  - The text file which sits on the Arduino's Linux file system is emptied for the new skate logs to be entered
  - When the value spikes above a certain threshold, this signals that the wheel had made a full rotation
    - The circumference of the wheel (distance travelled from one revolution of the wheel) is added to the total distance
    - The current speed is also calculated by dividing the circumference of the wheel by the time it took for the full revolution
    - The current distance and speed is being updated and displayed to the user via the LCD
  - The total amount of revolutions and the total speed (all revolutions accumulated) are incremented every 2 seconds
  - On the 2 second mark, the average speed is calculated and logged into the text file `speeds.txt`
  - This continues until the user clicks the button again to end their skate
  - When the button is clicked, the total distance is prepended with "d_" and logged into `speeds.txt`
  - A Shell Script which sits on the Linux file system is called by the command `ash send_speeds.sh`
  - This script [Shell Script GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_shell_script) sends data to the API [API GitHub Repo](https://github.com/CharlesPeterMcCarthy/longboard_api)
  - The response is then received back from the shell script containing the following data which was calculated by the API
    - The skate session ID number
    - The total length of the skate in seconds
    - The average speed throughout the session
    - The highest speed from the session
  - This data is displayed to the user via the LCD
  - The user is then prompted to press the button again to begin another skate session

![Longboard Project Image 2](images/img2.jpg?raw=true "Longboard Project Image 2")

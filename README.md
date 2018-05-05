# longboard_arduino
C++ (Arduino) Program to monitor longboard/skateboard stats (distance, speed, etc.)

![Longboard Project Image 1](images/img1.jpg?raw=true "Longboard Project Image 1")

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
  - The response is then received back from the shell script containing the following data which was caluclated by the API
    - The skate session ID number
    - The total length of the skate in seconds
    - The average speed throughout the session
    - The highest speed from the session
  - This data is displayed to the user via the LCD
  - The user is then prompted to press the button again to begin another skate session

![Longboard Project Image 2](images/img2.jpg?raw=true "Longboard Project Image 2")

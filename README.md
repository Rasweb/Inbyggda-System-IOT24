## File structure for my embedded programming course, espressif
- [Overview](#overview)
  - [Built with](#built-with)
  - [Project](#project)
  - [Project image](#project-image)

## Overview
This is my file structure for my embedded programming course. </br>
This is where every exercises and practical examination is stored. </br>

In the folder Projekt/project-IoT my alarm system is stored, to read more about that scroll down to the project headline. </br>

### Built with

- C
- Espressif
- ESP32-C6 Devkit
- Visual Studio Code (vs code)
- Espressif vs code exstension

### Project
In the folder Projekt/project-IoT my alarm system is stored. </br>
That project uses most of the components stored in components folder. </br>
Alarm functionality:
1. Turn the potentiometer to set a threshold value (warning value).
2. Photoresistor reads values.
3. If photoresistor values are close to the threshold.
   * The rgb led on the ESP will shine orange.
   * The display will show this: "Alert: Rising".
4. If the photoresistor values are below the threshold
   * The rgb led on the ESP will shine red.
   * After it reaches below a certain threshold the buzzer will start.
   * The display will show this: "Caution: Alert"
5. The button is an extra addition and not entirely necessary to the project but just fun:
   * If you press it, it will cycle through different alarm melodies.
     1. Imperial March
     2. Pirates of the Caribbean
     3. Game of Thrones
     4. Star Wars main theme
     5. Super Mario Bros
6. The display will at all times display something like this:
   * Status
     1. "Caution: Alert" - When threshold has been reached.
     2. "Alert: Rising" - When its close to the threshold.
     3. "All Clear: Safe" - When all values are okay.
   * Average Values - A calculated average value of nr values that works like a filter.
     1. Sensor value
     2. Potentiometer value

#### Finding the correct folder
* Base in github
1. Display found in: 
   * Projekt/project-IoT/components/display_component
3. Onboard ESP32 rgb led
   * Projekt/project-IoT/components/rgb_component
4. Potentiometer
   * Components/potentiometer_comp
5. Photoresistor
   * Components/potentiometer_comp
   * Uses the same folder as the potentiometer because it reads adc
6. Passive Buzzer
   * Components/analog_led_component
   * Folder is called analog_led_component because it was created for an analog led.
   * Later used for buzzer because of pwm.
7. Button
   *  Components/button_component

#### Hardware components used:
* Photoresistor 
* Passive Buzzer
* Potentiometer
* Button
* ESP32-C6 devkit
* Display - SSD1306
* onboard rgb-led on the ESP32-C6

#### Other
I tried to implement sleep and wakeup from the ULP LP Core Coprocessor using ADC but I couldn't do it before the project deadline. </br>
I did implement some kind of "lazy" sleep where it wakesup after a few seconds to check values and then goes to sleep. </br>
As you may notice my folder names are not that accurate according to their usages. For example anlog_led_component is used for a buzzer because of the pwm. 
  
### Project image
Image of the hardware components used, for more info look under the project headline. 
![alarm-system](https://github.com/user-attachments/assets/255f3322-2f8c-45a7-94fa-862c51834ec0)

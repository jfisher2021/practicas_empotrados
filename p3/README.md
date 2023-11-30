# Coffee Machine Project

This project is a coffee machine simulation implemented using an Arduino board and various sensors and actuators. The coffee machine allows users to select different types of coffee, adjust the price of each coffee, and provides an administration mode for monitoring temperature, humidity, and distance using an LCD display.

## Table of Contents

Components
Installation
Usage
Contributing
License

## Components

The following components are used in this project:

● Arduino UNO

● LCD

● Joystick

● Sensor temperatura/Humedad DHT11

● Sensor Ultrasonido

● Boton

● 2 LEDS Normales (LED1, LED2)

## Installation

To set up the coffee machine project, follow these steps:

Connect the Arduino board to your computer.
Bibliotecas utilizadas:

● LiquidCrystal: https://www.arduino.cc/en/Reference/LiquidCrystal

● ArduinoThread: https://www.arduino.cc/reference/en/libraries/arduinothread/

● Watch Dog: https://create.arduino.cc/projecthub/rafitc/what-is-watchdog-timer-fffe20

● DHT-sensor-library: https://github.com/adafruit/DHT-sensor-library

● TimerOne: https://www.arduino.cc/reference/en/libraries/timerone/

Upload the code provided in this repository to the Arduino board.

## BLOG (USAGE)

At first I started playing with a led and the lcd display to see how they worked. I had to visit the Arduino page to learn how to conect the pins. Once I learned to dispaly the information on the lcd display I started to work with the joystick. I had to learn how to read the values of the joystick and how to use them to move the cursor on the lcd display. I had to use de analog Pins to read the values of the joystick so instead of using the digitalWrite() function I had to use the analogRead() function. The map() function in Arduino is used to re-map a number from one range to another. In my case i use it to convert the 1024 values of the joystick to the 180 degrees of the servo motor.

```c++

    // include the library code:
    #include <LiquidCrystal.h>
    /*

    The circuit:

    - LCD RS pin to digital pin 12
    - LCD Enable pin to digital pin 11
    - LCD D4 pin to digital pin 5
    - LCD D5 pin to digital pin 4
    - LCD D6 pin to digital pin 3
    - LCD D7 pin to digital pin 2
    - LCD R/W pin to ground
    - LCD VSS pin to ground
    - LCD VCC pin to 5V
    - 10K resistor:
    - ends to +5V and ground
    - wiper to LCD VO pin (pin 3)
    */

    #define PIN_VRx A0 // Pin analógico 0 para el eje X del joystick
    #define PIN_VRy A1 // Pin analógico 1 para el eje Y del joystick
    #define PIN_SW 10 // Pin digital 10 para el botón
    #define PIN_LED A3 // Pin analógico 3 para el LED
    const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
    LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
    void setup() {
        pinMode(PIN_SW, INPUT_PULLUP);
    }

    void loop() {
        int x = analogRead(PIN_VRx);
        int y = analogRead(PIN_VRy);
        x_ang = map(x, 0, 1023, 0, 180);
        y_ang = map(y, 0, 1023, 0, 180);
        int sw = digitalRead(PIN_SW);
        lcd.setCursor(0, 0);
        lcd.print("X: ");
        lcd.print(x_ang);
        lcd.setCursor(0, 1);
        lcd.print("Y: ");
        lcd.print(y_ang);
    }

```

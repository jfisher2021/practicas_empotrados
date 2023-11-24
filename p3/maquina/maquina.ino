/*
LiquidCrystal Library - Hello World

Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
library works with all LCD displays that are compatible with the
Hitachi HD44780 driver. There are many of them out there, and you
can usually tell them by the 16-pin interface.

This sketch prints "Hello World!" to the LCD
and shows the time.

The circuit:
* LCD RS pin to digital pin 12
* LCD Enable pin to digital pin 11
* LCD D4 pin to digital pin 5
* LCD D5 pin to digital pin 4
* LCD D6 pin to digital pin 3
* LCD D7 pin to digital pin 2
* LCD R/W pin to ground
* LCD VSS pin to ground
* LCD VCC pin to 5V
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

Library originally added 18 Apr 2008
by David A. Mellis
library modified 5 Jul 2009
by Limor Fried (http://www.ladyada.net)
example added 9 Jul 2009
by Tom Igoe
modified 22 Nov 2010
by Tom Igoe
modified 7 Nov 2016
by Arturo Guadalupi

This example code is in the public domain.

https://docs.arduino.cc/learn/electronics/lcd-displayss

*/

// include the library code:
#include <DHT.h>
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

// states
#define START 1
#define SERVICE 2
#define WAITING 3
#define PRODUCT 4
#define ADMIN 5

#define PIN_VRx A0
#define PIN_VRy A1
#define PIN_SW 10
#define PIN_LED 9
#define PIN_TRIGGER 8  // Pin digital 8 para el PIN_TRIGGER del sensor
#define PIN_ECHO 7     // Pin digital 7 para el PIN_ECHO del sensor
#define DHTPIN 2       // Pin digital 2 para el sensor
#define DHTTYPE DHT11  // DHT 11

DHT dht(DHTPIN, DHTTYPE);

int product = 0;
int state;

const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);  // Iniciamos la comunicación
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, OUTPUT);    // Pin como salida
  pinMode(PIN_ECHO, INPUT);        // Pin como entrada
  digitalWrite(PIN_TRIGGER, LOW);  // Inicializamos el pin con 0
  dht.begin();                     // Inicializamos el sensor DHT11

  lcd.begin(16, 2);
  state = SERVICE;
}

// void sensor_temperatura_humedad() {
//   delay(1000);
//   // Leemos la humedad relativa
//   float h = dht.readHumidity();
//   // Leemos la temperatura en grados centígrados (por defecto)
//   float t = dht.readTemperature();
//   // Leemos la temperatura en grados Fahreheit
//   float f = dht.readTemperature(true);

//   // Comprobamos si ha habido algún error en la lectura
//   if (isnan(h) || isnan(t) || isnan(f)) {
//     Serial.println("Error obteniendo los datos del sensor DHT11");
//     return;
//   }

//   // Calcular el índice de calor en Fahreheit
//   float hif = dht.computeHeatIndex(f, h);
//   // Calcular el índice de calor en grados centígrados
//   float hic = dht.computeHeatIndex(t, h, false);

//   Serial.print("Humedad: ");
//   Serial.print(h);
//   Serial.print(" %\t");
//   Serial.print("Temperatura: ");
//   Serial.print(t);
//   Serial.print(" *C ");
//   Serial.print(f);
//   Serial.print(" *F\t");
//   Serial.print("Índice de calor: ");
//   Serial.print(hic);
//   Serial.print(" *C ");
//   Serial.print(hif);
//   Serial.println(" *F");
// }

void start() {
  lcd.print("CARGANDO ...");
  delay(1000);

  // Blink PIN_LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(1000);
    digitalWrite(PIN_LED, LOW);
    delay(1000);
  }

  lcd.clear();
}

bool sensor_distancia() {
  long t;          // Tiempo que demora en llegar el eco
  long distancia;  // Distancia en centímetros

  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);  // Enviamos un pulso de 10us
  digitalWrite(PIN_TRIGGER, LOW);

  t = pulseIn(PIN_ECHO, HIGH);  // Obtenemos el ancho del pulso
  distancia = t / 59;           // Escalamos el tiempo a una distancia en cm
  if (distancia > 100) {
    lcd.print("Fuera de rango");
    Serial.println("Fuera de rango");
    Serial.println(distancia);
    delay(200);
    lcd.clear();
    return false;
  } else {
    return true;
  }
}

void show_products() {
  static unsigned long previousMillis = 0;
  const long interval = 300;  // Adjust this interval as needed

  unsigned long currentMillis = millis();

  int x = analogRead(PIN_VRx);
  int y = analogRead(PIN_VRy);
  int x_ang = map(x, 0, 1023, 0, 180);
  int y_ang = map(y, 0, 1023, 0, 180);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (x_ang > 100) {
      product++;
      if (product > 4) {
        product = 0;
      }
    } else if (x_ang < 80) {
      product--;
      if (product < 0) {
        product = 4;
      }
    }

    // Imprimir datos al monitor serie
    Serial.print("X_ang:");
    Serial.print(x_ang);
    Serial.print("  ");

    Serial.print("Y_ang:");
    Serial.print(y_ang);
    Serial.println();
    Serial.print(" SW:");
    Serial.print(digitalRead(PIN_SW));
    Serial.print("product");
    Serial.print(product);
    Serial.println();

    lcd.clear();

    switch (product) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Cafe So");
        break;
      case 1:
        lcd.setCursor(0, 1);
        lcd.print("Cafe Cor1.10");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Cafe Do1.25");
        break;
      case 3:
        lcd.setCursor(0, 1);
        lcd.print("Cafe Prem1.50");
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("Choco 2.00");
        break;
      default:
        break;
    }
  }
}

void loop() {
  // switch (state) {
  // case START:
  //     start();
  //     state = SERVICE;
  //     break;
  // case SERVICE:
  //     if (sensor_distancia()) {
  //         show_products();
  //     }
  //     break;
  // case WAITING:
  //     break;
  // default:
  //     break;
  // }
  sensor_temperatura_humedad();
}

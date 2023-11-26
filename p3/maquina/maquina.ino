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
#define PRESIO 4
#define ADMIN 5

#define PIN_VRx A0
#define PIN_VRy A1
#define PIN_SW 10
#define PIN_LED 2
#define PIN_LED2 9
#define PIN_TRIGGER 8  // Pin digital 8 para el PIN_TRIGGER del sensor
#define PIN_ECHO 7     // Pin digital 7 para el PIN_ECHO del sensor
#define BOTON 13       // Pin digital 10 para el botón
// #define DHTPIN       // Pin digital 2 para el sensor
// #define DHTTYPE DHT11  // DHT 11

// DHT dht(DHTPIN, DHTTYPE);

int state;
int valor = 0;
float cafe_solo = 1;
float cafe_cortado = 1.10;
float cafe_doble = 1.25;
float cafe_premium = 1.50;
float chocolate = 2.00;
String menu_admin[] = {"Ver temperatura", "Ver distancia sensor", "Ver contador", "Modificar precio"};
unsigned long previousMillis, previousMillis_precio, time_switch, time_dist, previous_time_coffe;
const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
    Serial.begin(9600);  // Iniciamos la comunicación
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_SW, INPUT_PULLUP);
    pinMode(PIN_TRIGGER, OUTPUT);    // Pin como salida
    pinMode(PIN_ECHO, INPUT);        // Pin como entrada
    digitalWrite(PIN_TRIGGER, LOW);  // Inicializamos el pin con 0
    pinMode(BOTON, INPUT_PULLUP);    // Pin como entrada
    // dht.begin();                     // Inicializamos el sensor DHT11

    lcd.begin(16, 2);
    state = START;
}

/* void sensor_temperatura_humedad() {
    delay(1000);
    // Leemos la humedad relativa
    float h = dht.readHumidity();
    // Leemos la temperatura en grados centígrados (por defecto)
    float t = dht.readTemperature();
    // Leemos la temperatura en grados Fahreheit
    float f = dht.readTemperature(true);

    // Comprobamos si ha habido algún error en la lectura
    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Error obteniendo los datos del sensor DHT11");
        return;
    }

    // Calcular el índice de calor en Fahreheit
    float hif = dht.computeHeatIndex(f, h);
    // Calcular el índice de calor en grados centígrados
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Índice de calor: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
}
 */
void start() {
    lcd.clear();
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

int sensor_distancia() {
    long t;          // Tiempo que demora en llegar el eco
    long distancia;  // Distancia en centímetros

    digitalWrite(PIN_TRIGGER, HIGH);
    delayMicroseconds(10);  // Enviamos un pulso de 10us
    digitalWrite(PIN_TRIGGER, LOW);

    t = pulseIn(PIN_ECHO, HIGH);  // Obtenemos el ancho del pulso
    distancia = t / 59;           // Escalamos el tiempo a una distancia en cm

    return distancia;
}
void productos(int product) {
    switch (product) {
    case 0:
        lcd.setCursor(0, 0);
        lcd.print("cafe solo");
        lcd.setCursor(0, 1);
        lcd.print(cafe_solo);
        lcd.setCursor(5, 1);
        lcd.print("$");
        break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("cafe cortado");
        lcd.setCursor(0, 1);
        lcd.print(cafe_cortado);
        lcd.setCursor(5, 1);
        lcd.print("$");
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print("cafe doble");
        lcd.setCursor(0, 1);
        lcd.print(cafe_doble);
        lcd.setCursor(5, 1);
        lcd.print("$");
        break;
    case 3:
        lcd.setCursor(0, 0);
        lcd.print("cafe premium");
        lcd.setCursor(0, 1);
        lcd.print(cafe_premium);
        lcd.setCursor(5, 1);
        lcd.print("$");
        break;
    case 4:
        lcd.setCursor(0, 0);
        lcd.print("chocolate");
        lcd.setCursor(0, 1);
        lcd.print(chocolate);
        lcd.setCursor(5, 1);
        lcd.print("$");
        break;
    default:
        break;
    }
}

void servicio() {
    static int product = 0;
    static unsigned long buttonPressStartTime = 0;

    const long interval = 250;  // Ajusta este intervalo según sea necesario

    unsigned long currentMillis = millis();

    int x_servicio = analogRead(PIN_VRx);
    int x_ang_servicio = map(x_servicio, 0, 1023, 0, 180);

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (digitalRead(BOTON) == LOW) {
            if (buttonPressStartTime == 0) {
                // Si el botón acaba de ser presionado, registra el tiempo actual
                buttonPressStartTime = millis();
            }
        } else {
            // Reinicia el tiempo de inicio si el botón no está siendo presionado
            buttonPressStartTime = 0;
        }

        if (buttonPressStartTime > 0 && (currentMillis - buttonPressStartTime >= 5000)) {
            // Si el botón ha estado presionado durante más de 5 segundos, cambia al estado ADMIN
            state = ADMIN;
            buttonPressStartTime = 0;  // Reinicia el tiempo de inicio
        }

        if (x_ang_servicio > 100) {
            product++;
            if (product > 4) {
                product = 0;
            }
        } else if (x_ang_servicio < 80) {
            product--;
            if (product < 0) {
                product = 4;
            }
        }

        // Imprimir datos al monitor serie
        Serial.print("X_ang_servicio:");
        Serial.print(x_ang_servicio);
        Serial.print("  ");

        Serial.print(" SW:");
        Serial.print(digitalRead(PIN_SW));
        Serial.print(" product");
        Serial.print(product);
        Serial.print(" Button Press Time: ");
        Serial.println(currentMillis - buttonPressStartTime);

        lcd.clear();
        if (digitalRead(PIN_SW) == LOW) {
            state = WAITING;
        }
        productos(product);
    }
}

void preparando_cafe() {
    lcd.clear();
    unsigned long listo_pa = random(4000, 8000);  // 4 a 8 segundos
    if (millis() - previous_time_coffe >= listo_pa) {
        previous_time_coffe = millis();
        lcd.setCursor(0, 0);
        lcd.print("PREPARANDO");
        lcd.setCursor(0, 1);
        lcd.print("CAFE");
        int analogValue;
        unsigned long startTime_coffe = millis();

        // Bucle while para simular el progreso de la preparación
        while (millis() - startTime_coffe <= listo_pa) {
            int progress = map(millis() - startTime_coffe, 0, listo_pa, 0, 255);
            analogValue = map(progress, 0, 255, 0, 255);
            analogWrite(PIN_LED2, analogValue);
        }

        // Mostrar "RETIRE BEBIDA" durante 3 segundos
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RETIRE BEBIDA");

        // Esperar 3 segundos sin bloquear el bucle principal
        unsigned long removeDrinkTime = millis();
        while (millis() - removeDrinkTime <= 3000) {
        }

        lcd.clear();

        // Volver a la funcionalidad inicial de Servicio
        state = SERVICE;
        analogWrite(PIN_LED2, LOW);
    }
}
void admin(int option_) {
    int option = option_;
    static int distancia_admin = 0;
    int x_admin = analogRead(PIN_VRx);
    int y_admin = analogRead(PIN_VRy);
    int y_ang_admin = map(y_admin, 0, 1023, 0, 180);
    int x_ang_admin = map(x_admin, 0, 1023, 0, 180);
    while (y_ang_admin > 80) {
        y_admin = analogRead(PIN_VRy);
        y_ang_admin = map(y_admin, 0, 1023, 0, 180);
        const long interval = 250;  // Ajusta este intervalo según sea necesario

        if (millis() - previousMillis >= interval) {
            previousMillis = millis();

            // Imprimir datos al monitor serie
            Serial.print("X_ang_admin:");
            Serial.print(x_ang_admin);
            Serial.print("  ");
            Serial.print("Y_ang_admin:");
            Serial.print(y_ang_admin);
            Serial.println();
            Serial.print(" SW_admin:");
            Serial.print(digitalRead(PIN_SW));
            Serial.print(" option");
            Serial.print(option);
            lcd.clear();

            switch (option) {
            case 0:
                lcd.setCursor(0, 0);
                lcd.print("temperatura:XXC");
                lcd.setCursor(0, 1);
                lcd.print("humedad: YY%");
                break;
            case 1:
                lcd.setCursor(0, 0);
                distancia_admin = sensor_distancia();
                lcd.print("DISTANCIA ADMIN:");
                lcd.setCursor(0, 1);
                lcd.print(distancia_admin);
                lcd.setCursor(5, 1);
                lcd.print("cm");
                break;
            case 2:
                lcd.setCursor(0, 0);
                lcd.print("Tiempo ZZ seg");
                lcd.setCursor(0, 1);
                lcd.print("s");
                break;
            case 3:
                cambiar_precio();
                break;
            default:
                break;
            }
        }
    }
    // Imprimir datos al monitor serie
}

void cambiar_precio() {
    int producto_modificar = 0;
    int y_presio = analogRead(PIN_VRy);
    int y_ang_presio = map(y_presio, 0, 1023, 0, 180);
    bool FIRST_TIME = true;
    bool pulsado = false;
    float cambiar_presio = 0;
    int x_presio = analogRead(PIN_VRx);
    int x_ang_presio = map(x_presio, 0, 1023, 0, 180);
    unsigned long interval = 250;
    while (y_ang_presio > 80) {
        // Imprimir datos al monitor serie
        y_presio = analogRead(PIN_VRy);
        y_ang_presio = map(y_presio, 0, 1023, 0, 180);
        x_presio = analogRead(PIN_VRx);
        x_ang_presio = map(x_presio, 0, 1023, 0, 180);

        Serial.print("Y_ang_presio:");
        Serial.print(x_ang_presio);
        Serial.println();
        Serial.print(" SW_presio:");
        Serial.print(digitalRead(PIN_SW));
        Serial.println();

        if (millis() - previousMillis_precio >= interval) {
            if (digitalRead(PIN_SW) == HIGH) {
                FIRST_TIME = false;
            }
            if (FIRST_TIME == false) {
                if (digitalRead(PIN_SW) == LOW) {
                    pulsado = true;
                }
                lcd.clear();
                previousMillis_precio = millis();
                if (pulsado == true) {
                    if (x_ang_presio > 100) {
                        cambiar_presio += 0.05;

                    } else if (x_ang_presio < 80) {
                        cambiar_presio -= 0.05;
                    }
                    switch (valor) {
                    case 0:
                        cafe_solo = cafe_solo + cambiar_presio;
                        break;
                    case 1:
                        cafe_cortado = cafe_cortado + cambiar_presio;
                        break;
                    case 2:
                        cafe_doble = cafe_doble + cambiar_presio;
                        break;
                    case 3:
                        cafe_premium = cafe_premium + cambiar_presio;
                        break;
                    case 4:
                        chocolate = chocolate + cambiar_presio;
                        break;
                    default:
                        break;
                    }
                } else {
                    if (x_ang_presio > 100) {
                        producto_modificar++;
                        if (producto_modificar > 4) {
                            producto_modificar = 0;
                        }
                    } else if (x_ang_presio < 80) {
                        producto_modificar--;
                        if (producto_modificar < 0) {
                            producto_modificar = 4;
                        }
                    }
                    valor = producto_modificar;
                }
            }
            productos(valor);
        }
    }
}
void loop() {
    int distancia_persona = 0;
    static int option_admin = 0;
    int x_menu = analogRead(PIN_VRx);
    int x_ang_menu = map(x_menu, 0, 1023, 0, 180);

    const long interval = 250;  // Ajusta este intervalo según sea necesario
    switch (state) {
    case START:
        start();
        state = SERVICE;
        break;
    case SERVICE:
        distancia_persona = sensor_distancia();
        if (distancia_persona > 100) {
            lcd.print("Fuera de rango");
            delay(200);
            lcd.clear();

        } else {
            servicio();
        }
        break;
    case WAITING:
        preparando_cafe();
        break;
    case PRESIO:
        cambiar_precio();
        break;
    case ADMIN:
        digitalWrite(PIN_LED, HIGH);
        analogWrite(PIN_LED2, 255);
        if ((millis() - previousMillis) > interval) {
            Serial.println("x_ang_menu: " + String(x_ang_menu));
            if (x_ang_menu > 100) {
                option_admin++;
                if (option_admin > 3) {
                    option_admin = 0;
                }
            } else if (x_ang_menu < 80) {
                option_admin--;
                if (option_admin < 0) {
                    option_admin = 3;
                }
            }
            lcd.clear();
            if (option_admin != 1) {
                lcd.setCursor(0, 0);
                lcd.print(menu_admin[option_admin]);
            } else {
                if ((millis() - time_dist) > interval) {
                    lcd.setCursor(0, 0);
                    lcd.write("Ver distancia");
                    lcd.setCursor(0, 1);
                    lcd.write("sensor");
                    time_dist = millis();
                }
            }
            Serial.println("option_admin: " + String(option_admin));
            if (digitalRead(PIN_SW) == 0) {
                admin(option_admin);
            }
            previousMillis = millis();
        }

        break;
    default:
        break;
    }
}

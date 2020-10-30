#define DEBUG 0

#define DIFFICULTY_PIN A0
#define DIFFICULTY_STEPS 8

#define PLACE_ONE PIN2
#define PLACE_TWO PIN3
#define PLACE_THREE PIN4
#define PLACE_FOUR PIN5
#define PIN_RED PIN6

#define BUTTON_ONE 9
#define BUTTON_TWO 10
#define BUTTON_THREE 11
#define BUTTON_FOUR 12

#define START_GAME 0
#define GAME_ON 1
#define WAIT_INPUT 2

#define TMAX 8000

#define EI_ARDUINO_INTERRUPTED_PIN // to enable pin states functionality 
#include <EnableInterrupt.h>


class Fly {
  private:
    int place;
    int minimum;
    int maximum;
  public:
    Fly() {}

    void setUp(int minimum, int maximum, int place = 0) {
      this->minimum = minimum;
      this->maximum = maximum - 1;
      this->place = place;
    }

    void fly() {
      if (random(2)) {
        this->goRight();
      } else {
        this->goLeft();
      }
    }

    void goRight() {
      this->place++;
      if (this->place > this->maximum) {
        this->place = this->minimum;
      }
    }

    void goLeft() {
      this->place--;
      if (this->place < this->minimum) {
        this->place = this->maximum;
      }
    }

    int getPlace() {
      return this->place;
    }
};

class StepAnalogReader {
  private:
    int pin;
    int steps;
    int minimum;
    int maximum;

  public:
    StepAnalogReader(int pin) : StepAnalogReader(pin, 10) {}
    StepAnalogReader(int pin, int steps) : StepAnalogReader(pin, steps, 0, 1024) {}
    StepAnalogReader(int pin, int steps, int minimum, int maximum) {
      this->pin = pin;
      this->steps = steps;
      this->minimum = minimum;
      this->maximum = maximum;
    }

    int getLevel() {
      int read = analogRead(this->pin);
      return abs(read / ((minimum - maximum) / steps)) + 1;
    }
};



int places[] = {PLACE_ONE, PLACE_TWO, PLACE_THREE, PLACE_FOUR};
int placesSize = sizeof(places) / sizeof(places[0]);
int buttons[] = {BUTTON_ONE, BUTTON_TWO, BUTTON_THREE, BUTTON_FOUR};
int buttonsSize = sizeof(buttons) / sizeof(buttons[0]);

int score;
int gameStat = START_GAME;
unsigned long maxTime = 0;
unsigned long flyStops = 0;

int fadeStep = 5;
int brightness = 0;

Fly fly;
StepAnalogReader stepAnalogReader (DIFFICULTY_PIN, DIFFICULTY_STEPS);
volatile uint8_t interruptedPinShared;

int current;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  fly.setUp(0, placesSize, random(0, placesSize));

  initLeds();
  if (DEBUG) {
    testLeds();
  }
  initButtons();
 
  Serial.println("Welcome to the Track to Led Fly Game. Press Key T1 to Start.");
}

void loop() {
  static uint8_t interruptedPin;
  noInterrupts();
  interruptedPin = interruptedPinShared;
  interruptedPinShared = 0;
  interrupts();
  switch (gameStat) {
    case START_GAME:
      analogWrite(PIN_RED, brightness);
      brightness = brightness + fadeStep;
      if (brightness <= 0 || brightness >= 255) {
        fadeStep = -fadeStep;
      }
      delay(30);
      if (interruptedPin == buttons[0]) {
        gameStat = GAME_ON;
        maxTime = TMAX / stepAnalogReader.getLevel();
        score = 0;
        Serial.println("Go!");
        analogWrite(PIN_RED, 0);
      }
      break;
    case GAME_ON:
      fly.fly();
      gameStat = WAIT_INPUT;
      digitalWrite(places[fly.getPlace()], HIGH);
      flyStops = millis();
    case WAIT_INPUT:
      if (millis() - flyStops > maxTime || (interruptedPin != buttons[fly.getPlace()] && interruptedPin != 0)) {
        Serial.println("Game Over - Score: " + String(score));
        gameStat = START_GAME;
        digitalWrite(places[fly.getPlace()], LOW);
      } else if (interruptedPin == buttons[fly.getPlace()]) {
        gameStat = GAME_ON;
        Serial.println("Tracking the fly: pos " + String(fly.getPlace() + 1));
        score++;
        digitalWrite(places[fly.getPlace()], LOW);
        maxTime = maxTime/8*7;
        if (DEBUG) {
          printDebug("maxTime decreasing! Now is: " + String(maxTime));
        }
      }
      break;
  }
  interruptedPin = 0;
}

void initLeds() {
  for (int i = 0; i < placesSize; i++) {
    pinMode(places[i], OUTPUT);
  }
  pinMode(PIN_RED, OUTPUT);
}

void testLeds() {
  printDebug("Testing leds...");
  for (int i = 0; i < placesSize; i++) {
    digitalWrite(places[i], HIGH);
    delay(500);
  }
  digitalWrite(PIN_RED, HIGH);
  delay(500);
  printDebug("Shutting them down...");
  for (int i = 0; i < placesSize; i++) {
    digitalWrite(places[i], LOW);
  }
  digitalWrite(PIN_RED, LOW);
}

void initButtons() {
  for (int i = 0; i < buttonsSize; i++) {
    enableInterrupt(buttons[i], clickEvent, RISING);
  }
}

void printDebug(String text) {
  Serial.println("DEBUG: " + text);
}

void clickEvent() {
  interruptedPinShared = arduinoInterruptedPin;
}

#define DEBUG 0

#define DIFFICULTY_PIN A0
#define DIFFICULTY_STEPS 8

#define PLACE_ONE PIN2
#define PLACE_TWO PIN3
#define PLACE_THREE PIN4
#define PLACE_FOUR PIN5
#define PIN_RED 7

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
      return abs(read / ((minimum - maximum) / steps));
    }
};



int places[] = {PLACE_ONE, PLACE_TWO, PLACE_THREE, PLACE_FOUR};
int placesSize = sizeof(places) / sizeof(places[0]);
int currentPlace;
Fly fly;
StepAnalogReader stepAnalogReader (DIFFICULTY_PIN, DIFFICULTY_STEPS);

int current;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  currentPlace = random(0, placesSize);
  fly.setUp(0, placesSize, currentPlace);

  initLeds();
  if (DEBUG) {
    testLeds();
  }
}

void loop() {
  //  bool goRight = random(2);
  //  if (goRight) {
  //    fly.goRight();
  //  } else {
  //    fly.goLeft();
  //  }
  //  Serial.println("Tracking the fly: pos " + String(fly.getPlace()));
  //  digitalWrite(places[fly.getPlace()], HIGH);
  //  delay(500);
  //  digitalWrite(places[fly.getPlace()], LOW);
  if (stepAnalogReader.getLevel() != current) {
    current = stepAnalogReader.getLevel();
    Serial.println(current);
  }
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

void printDebug(String text) {
  Serial.println("DEBUG: " + text);
}

#define LEFT 1
#define RIGHT 2

int desiredDirection = 0; // 1 for Left or 2 for Right
double horizontalFrequency = 2;
double verticalFrequency = 40;

const int activatePinout = 2;
const int switchPinout = 3;

// Arduino Nano
const int a1a = 10;
const int a1b = 12;
const int b1a = 6;
const int b1b = 8;
// // Arduino Uno
// const int a1a = 11;
// const int a1b = 13;
// const int b1a = 6;
// const int b1b = 8;

int currentDirection = 2;

double horizontalInterval = (1.0 / (horizontalFrequency * 2) * 1000.0);
double verticalInterval = (1.0 / (verticalFrequency * 2) * 1000.0);
double runInterval = 20000;

int l_on = LOW;
int r_on = LOW;

unsigned long previousHorizontalMillis = 0;
unsigned long previousVerticalMillis = 0;
unsigned long previousRunMillis = 0;

int dragSkipCounter = 0;
int maxDragSkipCounter = 3;
int runCounter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(activatePinout, INPUT);
  pinMode(switchPinout, INPUT);

  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);

  // Initialization
  initiatePinouts();

  l_on = LOW;
  r_on = LOW;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  
  if (digitalRead(switchPinout) == LOW) {
    // switch direction
    desiredDirection = LEFT;    
  } else if (digitalRead(switchPinout) == HIGH) {
    desiredDirection = RIGHT;
  }
  

  if (runCounter == 0 || currentMillis >= previousHorizontalMillis + horizontalInterval) {
    previousHorizontalMillis = currentMillis;

    if (currentDirection != desiredDirection || dragSkipCounter > maxDragSkipCounter || runCounter == 0) {
      currentDirection = desiredDirection;
      dragSkipCounter = maxDragSkipCounter;
      runCounter++;
      l_on = digitalRead(switchPinout);
      r_on = 1 - l_on;

      drag();
      delay(horizontalInterval);
    } else if (currentDirection == desiredDirection) {
      dragSkipCounter++;
    }
  }

  if (currentMillis >= previousVerticalMillis + verticalInterval && currentDirection == desiredDirection) {
    //
    r_on = l_on = digitalRead(switchPinout);

    tapOrVibrate();
  }

  if (currentMillis >= previousRunMillis + runInterval) {
    Serial.print("Finish");
    initiatePinouts();
    delay(10000);
  }

}

void initiatePinouts() {
  digitalWrite(a1a, LOW);
  digitalWrite(a1b, LOW);
  digitalWrite(b1a, LOW);
  digitalWrite(b1b, LOW);
  l_on = LOW;
  r_on = LOW;
}

void drag() {
  digitalWrite(a1a, LOW);
  digitalWrite(a1b, l_on);
  digitalWrite(b1a, LOW);
  digitalWrite(b1b, r_on);
}

void tapOrVibrate() {
  digitalWrite(a1a, l_on);
  digitalWrite(a1b, 1 - l_on);
  digitalWrite(b1a, r_on);
  digitalWrite(b1b, 1 - r_on);
}

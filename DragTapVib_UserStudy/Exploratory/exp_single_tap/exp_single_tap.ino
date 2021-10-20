// Tapping
// Customized Variables
double tappingFrequency = 5.0 / 2.5;
//double tappingFrequency  = 5.0 / 5.0;
//double tappingFrequency = 5.0 / 7.5;
//double tappingFrequency = 5.0 / 10.0;
double runInterval = 5000;
int maxRunningTimes = 2;

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

double interval = (1.0/(tappingFrequency * 2))*1000.0;

int l_on = LOW;
int r_on = LOW;

unsigned long previousMillis = 0;

unsigned long previousRun = 0;
int runCounter = 0;
bool runningIsAllowed = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);

  // Initialization
  initiate_pinouts();

  l_on = LOW;
  r_on = LOW;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (runningIsAllowed == true) {
    if (currentMillis >= previousMillis + interval) {
      previousMillis = currentMillis;

      r_on = l_on = l_on == LOW?HIGH:LOW;
      
      tap_or_vib();
    }
  }

  // vibrating for runInterval = 5000 ms
  if (currentMillis >= previousRun + runInterval) {
    previousRun = currentMillis;
    runningIsAllowed = !runningIsAllowed;
    initiate_pinouts();
    runCounter += 1;
  }

  // stop after maxRunningTimes
  if (runCounter > maxRunningTimes) {
    initiate_pinouts();
    runningIsAllowed = false;
    Serial.println("Tapping Finished!");
    Serial.print(tappingFrequency);
    delay(1000000);
  }
}

void initiate_pinouts() {
  digitalWrite(a1a, LOW);
  digitalWrite(a1b, LOW);
  digitalWrite(b1a, LOW);
  digitalWrite(b1b, LOW);
  l_on = LOW;
  r_on = LOW;
}

void slide() {
  digitalWrite(a1a, LOW);
  digitalWrite(a1b, l_on);
  digitalWrite(b1a, LOW);
  digitalWrite(b1b, r_on);
}

void tap_or_vib() {
  digitalWrite(a1a, l_on);
  digitalWrite(a1b, 1 - l_on);
  digitalWrite(b1a, r_on);
  digitalWrite(b1b, 1 - r_on);
}

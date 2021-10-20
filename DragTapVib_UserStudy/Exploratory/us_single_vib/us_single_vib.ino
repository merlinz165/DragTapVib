// Vibration
// Customized Variables
double vibratingFrequency = 40;
double runInterval = 5000;
int maxRunningTimes = 3;
//int maxRunningTimes = 1; // detection

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
// Response LED
const int led = 3;

double interval = (1.0/(vibratingFrequency * 2))*1000.0;

int l_on = LOW;
int r_on = LOW;

unsigned long previousMillis = 0;

unsigned long previousRun = 0;
int runCounter = 0;
bool runningIsAllowed = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(a1a, OUTPUT);
  pinMode(a1b, OUTPUT);
  pinMode(b1a, OUTPUT);
  pinMode(b1b, OUTPUT);
  
  pinMode(led, OUTPUT);

  // Initialization
  initiatePinouts();

  l_on = LOW;
  r_on = LOW;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
    // vibrating
    if (runningIsAllowed == true) {
      if (currentMillis >= previousMillis + interval) {
        previousMillis = currentMillis;

        r_on = l_on = l_on == LOW?HIGH:LOW;
      
        tapOrVibrate();
        digitalWrite(led, HIGH);
      }
    }

    // vibrating for runInterval = 5000 ms
    if (currentMillis >= previousRun + runInterval) {
      previousRun = currentMillis;
      runningIsAllowed = !runningIsAllowed;
      initiatePinouts();
      if (runningIsAllowed == true) {
        runCounter += 1;        
      }
      digitalWrite(led, LOW);          
    }
    // stop after maxRunningTimes
    if (runCounter > maxRunningTimes) {
      initiatePinouts();
      runningIsAllowed = false;
      Serial.print("Vibrating is Finished with maxRunningTimes: ");
      Serial.print(maxRunningTimes);
      Serial.print(", vibratingFrequency: ");
      Serial.print(vibratingFrequency);
      Serial.println(".");
      digitalWrite(led, LOW);
      delay(1000000);
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

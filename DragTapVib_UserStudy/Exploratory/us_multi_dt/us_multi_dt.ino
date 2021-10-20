// Dragging D-T
#define D 1
#define T 2
#define V 3
#define F 4
#define NUM_ENTRIES(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))


// Cuszomized Variables
const int runArray[] = {D, T}; // the order of stimuli
// Customized Variables
double draggingFrequency = 2;
double tappingFrequency = 5.0 / 7.5;
double vibratingFrequency = 40;
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


const size_t len = NUM_ENTRIES(runArray);
double draggingInterval = (1.0/(draggingFrequency * 2)) * 1000.0;
double tappingInterval = (1.0/(tappingFrequency * 2)) * 1000.0;
double vibratingInterval = (1.0/(vibratingFrequency * 2)) * 1000.0;
unsigned long previousMillis = 0;
unsigned long previousRun = 0;
int runCounter = 0;

int l_on = LOW;
int r_on = LOW;

int runState = 0; // D T V or F
int runIndex = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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

  switch (runState) {
    case D:
      if (currentMillis >= previousMillis + draggingInterval) {
        previousMillis = currentMillis;

        l_on = l_on == LOW?HIGH:LOW;
        r_on = 1 - l_on;

        drag();
      }
      break;
    case T:
      if (currentMillis >= previousMillis + tappingInterval) {
        previousMillis = currentMillis;

        r_on = l_on = l_on == LOW?HIGH:LOW;

        tapOrVibrate();
      }
      break;
    case V:
      if (currentMillis >= previousMillis + vibratingInterval) {
        previousMillis = currentMillis;

        r_on = l_on = l_on == LOW?HIGH:LOW;

        tapOrVibrate();
      }
      break;
    case F:
      initiatePinouts();
      break;
    default:
      initiatePinouts();
      break;
  }

  if (currentMillis >= previousRun + runInterval) {
    previousRun = currentMillis;
    initiatePinouts;
    if (runIndex < len) {
      runState = runArray[runIndex];
      runIndex += 1;
    } else {
      runIndex = 0;
      runCounter += 1;
      initiatePinouts();
      Serial.print("Pause: ");
      Serial.println(runCounter);
      delay(5000);
    }
  }

  if (runCounter >= maxRunningTimes) {
    initiatePinouts();
    Serial.print("Finished with Order: ");
    Serial.print(runArray[0]);
    Serial.print(" ");
    Serial.println(runArray[1]);
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

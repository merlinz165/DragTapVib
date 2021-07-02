// T-V-D
#define D 1
#define T 2
#define V 3
#define F 4

#define NUM_ENTRIES(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

const int runArray[] = {T, V, D, F};
const size_t len = NUM_ENTRIES(runArray);

const int a1a = 4;
const int a1b = 2;
const int b1a = 12;
const int b1b = 8;

double slid_freq = 2;
double slid_interval = (1.0 / (slid_freq * 2)) * 1000.0;
double tap_freq = 7;
double tap_interval = (1.0 / (tap_freq * 2)) * 1000.0;
double vib_freq = 40;
double vib_interval = (1.0 / (vib_freq * 2)) * 1000.0;
unsigned long previousMillis = 0;

double switch_interval = 5000;
unsigned long previousSwitch = 0;

int run_counter = 0;

int l_on = LOW;
int r_on = LOW;

int run_state = 0;
int run_index = 0;

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
  run_counter = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  switch (run_state) {
    case D:
      if (currentMillis >= previousMillis + slid_interval) {
        previousMillis = currentMillis;

        l_on = l_on == LOW ? HIGH : LOW;
        r_on = 1 - l_on;

        slide();
      }
      break;
    case T:
      if (currentMillis >= previousMillis + tap_interval) {
        previousMillis = currentMillis;

        r_on = l_on = l_on == LOW ? HIGH : LOW;

        tap_or_vib();
      }
      break;
    case V:
      if (currentMillis >= previousMillis + vib_interval) {
        previousMillis = currentMillis;

        r_on = l_on = l_on == LOW ? HIGH : LOW;

        tap_or_vib();
      }
      break;
    case F:
      initiate_pinouts();
      break;
    default:
      initiate_pinouts();
      break;
  }

  if (currentMillis >= previousSwitch + switch_interval) {
    previousSwitch = currentMillis;
    previousMillis = currentMillis;
    initiate_pinouts;
    if (run_index < len) {
      run_state = runArray[run_index];
      run_index += 1;
    } else {
      run_index = 0;
      run_counter += 1;
    }
  }

  if (run_counter >= 5) {
    initiate_pinouts();
    Serial.println("Finished!!");
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

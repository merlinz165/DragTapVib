// Vibration

const int a1a = 4;
const int a1b = 2;
const int b1a = 12;
const int b1b = 8;

double tap_freq = 40;
double tap_interval = (1.0 / (tap_freq * 2)) * 1000.0;
unsigned long previousTap = 0;

double run_interval = 5000;
unsigned long previousRun = 0;

int run_counter = 0;

int l_on = LOW;
int r_on = LOW;

bool run_state = true;

// unsigned long previousMillis = 0;

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
  if (run_state == true) {
    if (currentMillis >= previousTap + tap_interval) {
      previousTap = currentMillis;

      r_on = l_on = l_on == LOW?HIGH:LOW;
      
      tap_or_vib();
    }
  }
  if (currentMillis >= previousRun + run_interval) {
    previousRun = currentMillis;
    run_state = !run_state;
    initiate_pinouts();
    run_counter += 1;
  }
  
  if (run_counter >= 10) {
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

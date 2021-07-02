// Sliding

const int a1a = 4;
const int a1b = 2;
const int b1a = 12;
const int b1b = 8;

double slid_freq = 2;
double interval = (1.0/(slid_freq * 2))*1000.0;

int l_on = LOW;
int r_on = LOW;

unsigned long previousMillis = 0;

void setup() {
  // put your setup code here, to run once:
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

    if (currentMillis >= previousMillis + interval) {
      previousMillis = currentMillis;
      
      l_on = l_on == LOW?HIGH:LOW;
      r_on = 1 - l_on;
      
      slide();
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

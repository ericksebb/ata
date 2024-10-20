int buttonA = 36; 
int buttonB = 35; 
int buttonC = 25; 
int ledPin = 2;  

void setup() {

  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
  pinMode(buttonC, INPUT);
  
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int A = digitalRead(buttonA);
  int B = digitalRead(buttonB);
  int C = digitalRead(buttonC);

  pinMode(buttonB, INPUT_PULLUP);
  //  
  int F = (A && B && C) || (!A && B) || (!B && C);
  

  if (F) {
    digitalWrite(ledPin, HIGH);
    Serial.println("F = 1");
  } else {
    digitalWrite(ledPin, LOW);  
    Serial.println("F = 0");
  }
  delay(250);
}

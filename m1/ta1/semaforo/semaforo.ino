const int greenPin = 15;
const int yellowPin = 2;
const int redPin = 4;
const int buttonPin = 5;


int currentState = 0; 
unsigned long lastStateChangeTime = 0;
bool buttonPressedInRed = false;


const unsigned long GREEN_TIME = 55000; 
const unsigned long REDUCED_GREEN_TIME = 3000; 
const unsigned long YELLOW_TIME = 2000; 
const unsigned long RED_TIME = 5000; 

void setup() {
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  changeState(0); 
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long stateTime = currentTime - lastStateChangeTime;
  bool buttonPressed = digitalRead(buttonPin) == LOW;

  switch (currentState) {
    case 0: 
      if (buttonPressed || stateTime >= GREEN_TIME) changeState(1);
      break;
    case 1: 
      if (stateTime >= YELLOW_TIME) changeState(2);
      break;
    case 2: 
      if (buttonPressed) buttonPressedInRed = true;
      if (stateTime >= RED_TIME) changeState(0);
      break;
  }
}

void changeState(int newState) {
  digitalWrite(greenPin, newState == 0 ? HIGH : LOW);
  digitalWrite(yellowPin, newState == 1 ? HIGH : LOW);
  digitalWrite(redPin, newState == 2 ? HIGH : LOW);

  currentState = newState;
  lastStateChangeTime = millis();

  if (newState == 0 && buttonPressedInRed) {
    delay(REDUCED_GREEN_TIME);
    changeState(1);
    buttonPressedInRed = false;
  }
}
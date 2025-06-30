#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayScore(int score); //Initalizes the function for later


int b1 = 2;   //Define button input pins
int b2 = 3;
int b3 = 4;
int b4 = 5;

int buttons[4] = { b1, b2, b3, b4 };  //Array of button pins

int L1 = 12;    //Define LED output pins
int L2 = 11;
int L3 = 9;
int L4 = 8;

bool LEDstates[4] = {false, false, false, false}; //Array of LED activation states

bool lastButtonState[4] = {true, true, true, true}; //Last state of each button
int LED[4] = { L1, L2, L3, L4 };  //Array of LED pins
unsigned long event[4] = { 0, 0, 0, 0 }; //Tracks the last change in button state
unsigned long duration[4] = { 0, 0, 0, 0 }; //Tracks the duration for the LED to be on for
unsigned long offDuration[4] = { 0, 0, 0, 0 };  //Tracks the duration for the LED to be off for
unsigned long startTime[4] = { 0, 0, 0, 0 };
int score = 0;  //Variable to track score
unsigned long reactionTime = 0;

int lastScore = -1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(b1, INPUT_PULLUP);    //Set button pins at inputs with a pullup resistor
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);


  pinMode(L1, OUTPUT);    //Set LED pins as outputs
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);


  randomSeed(analogRead(A0)); //Seed for random timing

//Set the LEDs to off and determines the off duration
  for (int i = 0; i < 4; i++) {
    LEDstates[i] = false;
    event[i] = millis();
    offDuration[i] = random(5000, 10001); //Random off time between 5-10 seconds
    digitalWrite(LED[i], LOW);
  }


  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C is your screen's I2C address
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Stop if display initialization failed
  }

  display.clearDisplay();  // Clear the buffer
  display.display();       // Push buffer to the screen
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool currentState = digitalRead(buttons[i]);  //Records current button state

    /*If LED is on and the current time is greater than the on duration and
    the time until last event, turns off LED for a random duration between 2.5-5 seconds*/
    if (LEDstates[i]) {
      if (millis() >= event[i] + duration[i]) {
        digitalWrite(LED[i], LOW);
        LEDstates[i] = false;
        event[i] = millis();
        offDuration[i] = random(2500, 5001);
      }
    } else {
      /* If the LED is off and the current time exceeds the off duration,
      turn the LED on for a random time between 2.5-5 seconds */
      if (millis() >= event[i] + offDuration[i]) {
        digitalWrite(LED[i], HIGH);
        LEDstates[i] = true;
        duration[i] = random(2500, 5001);
        event[i] = millis();
        startTime[i] = millis();
      }
    }


    if (currentState == LOW && lastButtonState[i] == HIGH) { //Detect button press
      //If button press is detected and LED is on, turns off the LED and rewards points based on the speed of the button press
      if (LEDstates[i]) {
        digitalWrite(LED[i], LOW);
        LEDstates[i] = false;
        reactionTime = millis() - startTime[i];
        score += (duration[i] - reactionTime) / 1000;
      } else {  //If the LED is off and the button is pressed, deducts 3 points
        score -= 3;
      }
    }

    lastButtonState[i] = currentState; //Save state for next loop
  }

  // Updates the display only if the score is changed
  if (score != lastScore) {
    lastScore = score;
    displayScore(score);
  }
}


//Function to display score on LCD screen
void displayScore(int score) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);  // Set text color
  display.setCursor(0, 0);              // Set text start position
  display.print("Score: ");             // Print label
  display.print(score);                 // Print score value
  display.display();                    // Push buffer to screen
}

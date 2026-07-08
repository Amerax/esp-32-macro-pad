#include <BleKeyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// for th piuns
const int BTN1_PIN = 18;
const int BTN2_PIN =  4;
const int BTN3_PIN =  5;
const int BTN4_PIN = 19;
const int POT_PIN  = 34;

int buttonPins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
const char* btnLabels[4]  = {"COPY", "PASTE", "UNDO", "SEL ALL"};
const char* btnSubtext[4] = {"Ctrl+C", "Ctrl+V", "Ctrl+Z", "Ctrl+A"};
BleKeyboard bleKeyboard("MacroPad", "DIY", 100);

// deb
const unsigned long DEBOUNCE_MS = 50;
bool lastState[4]            = {HIGH, HIGH, HIGH, HIGH};
bool currentState[4]         = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};

// Potentiomiter values variables
const int POT_STEPS           = 20;
const int POT_DEADZONE        = 30;
const unsigned long POT_INTERVAL = 150;
int  lastPotStep              = -1;
unsigned long lastPotTime     = 0;
int  currentVolume            = 50;
// display val

#define MODE_IDLE    0
#define MODE_BUTTON  1
#define MODE_VOLUME  2

int  displayMode              = MODE_IDLE;
unsigned long modeTimer       = 0;
const unsigned long MSG_HOLD  = 2000;
int  lastButtonPressed        = -1;

bool startupDone = false;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found — check wiring!");
    while (true); // halt
  }

  runStartupAnimation();
  startupDone = true;

  bleKeyboard.begin();
  Serial.println("MacroPad ready — pair with 'MacroPad'");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    handleButtons();
    handlePotentiometer();
  }
  updateDisplay();
}

// button code
void handleButtons() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading != lastState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_MS) {
      if (reading != currentState[i]) {
        currentState[i] = reading;
        if (currentState[i] == LOW) {
          sendMacro(i);
          lastButtonPressed = i;
          displayMode = MODE_BUTTON;
          modeTimer = millis();
        }
      }
    }
    lastState[i] = reading;
  }
}

void sendMacro(int i) {
  switch (i) {
    case 0:
      Serial.println("Copy");
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press('c');
      delay(10); bleKeyboard.releaseAll(); break;
    case 1:
      Serial.println("Paste");
      bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press('v');
      delay(10); bleKeyboard.releaseAll(); break;
    case 2:
      Serial.println("Undo");
      bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press('z');
      delay(10); bleKeyboard.releaseAll(); break;
    case 3:
      Serial.println("Select All");
      bleKeyboard.press(KEY_LEFT_CTRL); bleKeyboard.press('a');
      delay(10); bleKeyboard.releaseAll(); break;
  }
}

void handlePotentiometer() {
  int raw  = analogRead(POT_PIN);
  int step = map(raw, 0, 4095, 0, POT_STEPS - 1);
  if (lastPotStep == -1) { lastPotStep = step; return; }

  int diff = step - lastPotStep;
  if (abs(raw - map(lastPotStep, 0, POT_STEPS - 1, 0, 4095)) < POT_DEADZONE) return;
  if (millis() - lastPotTime < POT_INTERVAL) return;
  if (diff > 0) {
    for (int i = 0; i < abs(diff); i++) bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
    currentVolume = constrain(currentVolume + (abs(diff) * (100 / POT_STEPS)), 0, 100);
    Serial.print("Vol UP: "); Serial.println(currentVolume);
  } else if (diff < 0) {
    for (int i = 0; i < abs(diff); i++) bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    currentVolume = constrain(currentVolume - (abs(diff) * (100 / POT_STEPS)), 0, 100);
    Serial.print("Vol DN: "); Serial.println(currentVolume);
  }

  lastPotStep = step;
  lastPotTime = millis();
  displayMode = MODE_VOLUME;
  modeTimer = millis();
}

void updateDisplay() {
  if ((displayMode == MODE_BUTTON || displayMode == MODE_VOLUME) &&
      (millis() - modeTimer > MSG_HOLD)) {
    displayMode = MODE_IDLE;
  }

  switch (displayMode) {
    case MODE_IDLE:   drawIdle();   break;
    case MODE_BUTTON: drawButton(); break;
    case MODE_VOLUME: drawVolume(); break;
  }
}

void drawIdle() {
  display.clearDisplay();
  // Small label at top
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print("MacroPad");
  // Dotted border
  for (int x = 0; x < SCREEN_WIDTH; x += 4) {
    display.drawPixel(x, 12, SSD1306_WHITE);
    display.drawPixel(x, SCREEN_HEIGHT - 1, SSD1306_WHITE);
  }
  for (int y = 12; y < SCREEN_HEIGHT; y += 4) {
    display.drawPixel(0, y, SSD1306_WHITE);
    display.drawPixel(SCREEN_WIDTH - 1, y, SSD1306_WHITE);
  }
  display.display();
}

// for button press
void drawButton() {
  if (lastButtonPressed < 0) return;
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, 14, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(4, 3);
  display.print("BTN ");
  display.print(lastButtonPressed + 1);
  display.setCursor(60, 3);
  display.print("PRESSED");

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  int labelLen = strlen(btnLabels[lastButtonPressed]) * 12;
  display.setCursor((SCREEN_WIDTH - labelLen) / 2, 22);
  display.print(btnLabels[lastButtonPressed]);

  display.setTextSize(1);
  int subLen = strlen(btnSubtext[lastButtonPressed]) * 6;
  display.setCursor((SCREEN_WIDTH - subLen) / 2, 46);
  display.print(btnSubtext[lastButtonPressed]);

  display.drawLine(10, 56, SCREEN_WIDTH - 10, 56, SSD1306_WHITE);

  display.display();
}

void drawVolume() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(38, 2);
  display.print("VOLUME");
  // Note: ai wrote me this part 
  int cx = 64, cy = 38, r = 22;
  display.drawCircle(cx, cy, r, SSD1306_WHITE);
  display.drawCircle(cx, cy, r - 1, SSD1306_WHITE); 

  float startAngle = -90.0 * PI / 180.0;
  float endAngle   = startAngle + (2.0 * PI * currentVolume / 100.0);
  for (float a = startAngle; a <= endAngle; a += 0.05) {
    int px = cx + (int)((r - 4) * cos(a));
    int py = cy + (int)((r - 4) * sin(a));
    display.fillCircle(px, py, 2, SSD1306_WHITE);
  }

  display.setTextSize(1);
  String pct = String(currentVolume) + "%";
  int pctLen = pct.length() * 6;
  display.setCursor(cx - pctLen / 2, cy - 4);
  display.print(pct);

  display.display();
}

//start up
void runStartupAnimation() {
  for (int x = 0; x < SCREEN_WIDTH; x += 4) {
    display.clearDisplay();
    display.fillRect(0, 0, x, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(8);
  }

  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(80);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(14, 10);
  display.print("MacroPad");
  display.setTextSize(1);
  display.setCursor(28, 34);
  display.print("by ESP32  :)");
  display.setCursor(16, 48);
  display.print("Connecting BLE...");
  display.display();
  delay(1800); 

  display.clearDisplay();
  display.display();
}

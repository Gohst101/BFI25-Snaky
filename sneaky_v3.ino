#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);

#define buttonUp 17
#define buttonDown 14
#define buttonLeft 16
#define buttonRight 15

// Game Variables
int display_min_Y = 0;
int display_max_Y = 62;
int display_min_X = 0;
int display_max_X = 127;

int updateTime = 100;

unsigned long lastUpdate = 0;

int direction_new = 3;
int direction_old = 1;

int snake_lenght = 1;
int snake_X[100];
int snake_Y[100];

int apple_X = 46;
int apple_Y = 11;

static const unsigned char head_dot[] = {0x01};

void setup() {

  Serial.begin(115200);
  u8g2.begin();

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  randomSeed(millis());

  snake_X[0] = 51;
  snake_Y[0] = 31;
}

void loop() {

  // Buttons IMMER prüfen
  readButtons();

  if (millis() - lastUpdate > updateTime) {

    movement();

    if (snake_X[0] == apple_X && snake_Y[0] == apple_Y) {
      snake_lenght++;
      newApple();
    }

    // Wand Collision
    if (snake_X[0] < display_min_X || snake_X[0] > display_max_X - 5 ||
        snake_Y[0] < display_min_Y || snake_Y[0] > display_max_Y - 5) {
      death();
    }

    // Snake Collision
    for (int p = 1; p < snake_lenght; p++) {
      if (snake_X[0] == snake_X[p] && snake_Y[0] == snake_Y[p]) {
        death();
      }
    }

    updateDisplay();

    lastUpdate = millis();
  }
}

void readButtons() {

  if (!digitalRead(buttonUp)) {
    direction_new = 2;
  }
  else if (!digitalRead(buttonDown)) {
    direction_new = 0;
  }
  else if (!digitalRead(buttonLeft)) {
    direction_new = 1;
  }
  else if (!digitalRead(buttonRight)) {
    direction_new = 3;
  }

}

void movement() {

  if (
    !((direction_new == 0 && direction_old == 2) ||
      (direction_new == 2 && direction_old == 0) ||
      (direction_new == 1 && direction_old == 3) ||
      (direction_new == 3 && direction_old == 1))
  ) {
    direction_old = direction_new;
  }

  for (int i = snake_lenght - 1; i > 0; i--) {
    snake_X[i] = snake_X[i - 1];
    snake_Y[i] = snake_Y[i - 1];
  }

  switch (direction_old) {
    case 0: snake_Y[0] += 5; break;
    case 1: snake_X[0] -= 5; break;
    case 2: snake_Y[0] -= 5; break;
    case 3: snake_X[0] += 5; break;
  }
}

void updateDisplay() {

  u8g2.firstPage();
  do {

    u8g2.drawFrame(display_min_X, display_min_Y, display_max_X, display_max_Y);

    u8g2.drawXBM(snake_X[0] + 2, snake_Y[0] + 2, 1, 1, head_dot);
    u8g2.drawFrame(snake_X[0], snake_Y[0], 5, 5);

    for (int p = 1; p < snake_lenght; p++) {
      u8g2.drawFrame(snake_X[p], snake_Y[p], 5, 5);
    }

    u8g2.drawEllipse(apple_X + 2, apple_Y + 2, 2, 2);

  } while (u8g2.nextPage());
}

void newApple() {

  apple_X = random(0, 128 / 5) * 5 + 1;
  apple_Y = random(0, 64 / 5) * 5 + 1;

  for (int p = 1; p < snake_lenght; p++) {
    if (snake_X[p] == apple_X && snake_Y[p] == apple_Y) {
      newApple();
    }
  }
}

void death() {

  delay(500);

  while (1) {

    u8g2.firstPage();
    do {

      u8g2.drawFrame(0, 0, 128, 64);

      u8g2.setFont(u8g2_font_4x6_tr);

      u8g2.drawStr(34, 10, "Snaky by Heinke");

      u8g2.drawStr(47, 21, "Score:");

      char scoreStr[6];
      itoa(snake_lenght - 1, scoreStr, 10);

      int xPos = 47 + u8g2.getStrWidth("Score: ");
      u8g2.drawStr(xPos, 21, scoreStr);

      u8g2.drawStr(56, 43, "Play");
      u8g2.drawStr(36, 55, "[Press Button]");

    } while (u8g2.nextPage());

    if (!digitalRead(buttonUp) || !digitalRead(buttonDown) ||
        !digitalRead(buttonLeft) || !digitalRead(buttonRight)) {

      snake_lenght = 1;
      snake_X[0] = 51;
      snake_Y[0] = 31;

      direction_old = 1;
      direction_new = 3;

      newApple();

      break;
    }
  }
}

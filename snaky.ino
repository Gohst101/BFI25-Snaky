// Code created by Heinke - 16.12.2025


#include <Arduino.h>
#include <U8g2lib.h>



// Hallo Norwin :3
// Dieser Code ist extra für dich Dokumentiert <3

// Gemacht mit:
// Designs: https://lopaka.app
// Code: https://docs.arduino.cc/programming/

// Display: SSD1306 Bibibliothek
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);

// Definiere Button Pins mit beschreibungen wie "buttonUp" damit es im
// späterem Code übersichtlicher ist und einfacher zu bearbeiten
#define buttonUp 17
#define buttonDown 14
#define buttonLeft 16
#define buttonRight 15


// Spiel Variablen

// Bildschirmgröße - Die Variablen sind für das Spiel Snake angepasst da Snake ein 5 Raster Muster nutzt wie
// ein Karo Heft
// 64             
//
// 0              128
int display_min_Y = 0;
int display_max_Y = 62;
int display_min_X = 0;
int display_max_X = 127;

// Wie schnell das Programm einmal durchläuft - Client Sided FPS
int updateTime = 100; // Screen Update in ms
int direction_new = 3; // Aktuelle Richtung für den anfang
int direction_old = 1; // Allte Richtung für den anfang

int snake_lenght = 1; // Aktuelle Länge (Kopf mitgerechnet)
int snake_X[100]; // Maximal 100 Schubladen - Dort kann aber auch nichts eingetragen werden
int snake_Y[100]; // Maximal 100 Schubladen - Dort kann aber auch nichts eingetragen werden

int apple_X;
int apple_Y;

static const unsigned char head_dot[] = {0x01}; // Das ist für den Kopf der Schlange der Punkt in der mitte der Box (Kopf)

// So jetzt geht es endlich mit ein wenig Code los :3
void setup() {
  Serial.begin(115200); // Für den Serial Monitor - Öffnungsbar über den Knopf oben rechts mit den Punkten und der Lupe
  u8g2.begin(); // Display u8g2 initalisieren
  randomSeed(analogRead(11)); // Random Seed

  snake_X[0] = 51; // Setze den Kopf der Schlange X
  snake_Y[0] = 31; // Setze den Kopf der Schlange Y

  apple_X = random(display_min_X - 1, display_max_X - 1 / 5) * 5 + 1;
  apple_Y = random(display_min_Y - 1, display_max_Y - 1 / 5) * 5 + 1;

  drawStartmenu(); // Startmenu anzeigen - Ist eine funktion ganz unten
}

void loop() {
  u8g2.clearBuffer(); // Löscht alles auf dem Bildschirm

  movement(); // Ruft die Funktion movement

  // Überprüfe ob der Kopf der Schlange auf einem Apfel ist.
  if(snake_X[0] == apple_X && snake_Y[0] == apple_Y) {
    // Wenn der Kopf auf dem Apfel ist macht er länge plus eins und ruft die Funktion newApple auf
    snake_lenght = snake_lenght + 1;
    newApple();
  }


  collisionCheck(); // Ruft die funktion collisionCheck auf um zu überprüfen ob die Schlange gegen wand oder eigenen Körper schlängelt :)
  updateDisplay(); // Display aktualisieren
}

// Colisions Check Funktion
void collisionCheck() {
  // WAND
  // Wenn Snake X kleiner als display min X oder snake X größer als display max X oder snake Y kleiner als display min X oder snake Y größer als display max Y
  if (snake_X[0] < display_min_X || snake_X[0] > display_max_X || snake_Y[0] < display_min_Y || snake_Y[0] > display_max_Y) {
    death(); // Wenn wahr funktion death aufrufen
  }

  // SCHLANGE
  // Solange wie p kleiner als schlangenlänge
  for (int p=1; p < snake_lenght; p++) {
    // Wenn snake X gleich snake X p und snake X gleich snake Y p
    // p ist eine Variable welche von eins nach oben zählt und prüft nebenbei ob der kopf der schlange in diesem Körper ist
    if (snake_X[0] == snake_X[p] && snake_Y[0] == snake_Y[p]) {
      death(); // Rufe die funktion death auf
    }
  }
}


// Death Funktion - Aktuell keine weitere Logik
void death() {
  while(1) {
    drawStartmenu();
  }
}

// New Apple
void newApple() {
  // Random Apple
  apple_X = random(display_min_X - 1, display_max_X - 1 / 5) * 5 + 1;
  apple_Y = random(display_min_Y - 1, display_max_Y - 1 / 5) * 5 + 1;

  // Überprüft für jeden Körperteil der Schlange ob der Apfel in diesem Schlangenkörper ist und wenn ja soll
  // er erneut versuchen eine position für den apfel zu erstellen indem er die funktion neustartet
  for (int p=1; p < snake_lenght; p++) {
    if (snake_X[p] == apple_X && snake_Y[p] == apple_Y) {
      newApple();
    }
  }
}

// Display Aktuallisieren - Ich kann nicht mehr das ist so viel Text ich hasse Dokumentationen
void updateDisplay() {
  u8g2.firstPage();
  do {
    // Äußere Ramen für die Spielfeldmakierung
    u8g2.drawFrame(display_min_X, display_min_Y, display_max_X, display_max_Y);

    // Schlangen Kopf
    u8g2.drawXBM(snake_X[0]+2, snake_Y[0]+2, 1, 1, head_dot);
    u8g2.drawFrame(snake_X[0], snake_Y[0], 5, 5);

    // Körper der Schlange für jedes Segment oder so
    for (int p = 1; p < snake_lenght; p++) {
      u8g2.drawFrame(snake_X[p], snake_Y[p], 5, 5);
    }

    // Leckerer Apfel jumy
    u8g2.drawEllipse(apple_X + 2, apple_Y + 2, 2, 2);

  } while(u8g2.nextPage());

  // Client Sided FPS - Du errinerst dich? Wenn nein DANN LIEß DIE DRECKS DOKU DU HU***** <3
  delay(updateTime);
}

// Movement funktion
void movement() {
  // Muster zum Checken der Funktion
  //    2
  // 1     3
  //    0

  // Wenn buttonUp gedrückt setze direction_new auf 2
  if (digitalRead(buttonUp)) {
    direction_new = 2;
  }
  // Wenn buttonDown gedrückt setze direction_new auf 0
  else if (digitalRead(buttonDown)) {
    direction_new = 0;
  }
  // Wenn buttonLeft gedrückt setze direction_new auf 1
  else if (digitalRead(buttonLeft)) {
    direction_new = 1;
  }
  // Wenn buttonRight gedrückt setze direction_new auf 3
  else if (digitalRead(buttonRight)) {
    direction_new = 3;
  }

  // Wenn die richtung das um 180 Grad gedrehte ergebniss ist dann darf die bewegung nicht gemacht werden
  if (
    !((direction_new == 0 && direction_old == 2) ||
      (direction_new == 2 && direction_old == 0) ||
      (direction_new == 1 && direction_old == 3) ||
      (direction_new == 3 && direction_old == 1))
  ) {
    direction_old = direction_new;
  }

  // Liste von Hinten nach Vorne aktuallisieren
  for (int p = snake_lenght - 1; p > 0; p--) {
    snake_X[p] = snake_X[p - 1];
    snake_Y[p] = snake_Y[p - 1];
  }

  // Wenn direction_old == 0 dann snake_Y += 5 ende
  switch (direction_old) {
    case 0: snake_Y[0] += 5; break;
    case 1: snake_X[0] -= 5; break;
    case 2: snake_Y[0] -= 5; break;
    case 3: snake_X[0] += 5; break;
  }
}

// Startmenu durch die Design Website
void drawStartmenu() {
  u8g2.firstPage();
  int TONGUE_X = 81;
  int TONGUE_Y = 18;

  do {
    u8g2.setFontMode(1);

    // ---------- LINKER BEREICH ----------
    u8g2.setFont(u8g2_font_5x8_tr);

    // Play
    u8g2.drawFrame(0, 0, 55, 13);
    u8g2.drawStr(3, 10, "Play");

    // Cursor/Pfeil nach links
    u8g2.drawTriangle(52, 2, 52, 10, 48, 6);

    // Settings
    u8g2.drawFrame(0, 12, 55, 13);
    u8g2.drawStr(3, 22, "Settings");

    // Help
    u8g2.drawFrame(0, 24, 55, 13);
    u8g2.drawStr(3, 34, "Help");

    // Footer
    u8g2.drawFrame(0, 36, 55, 28);
    u8g2.drawStr(3, 46, "By Heinke");
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(3, 57, "V.1.0.0");

    // ---------- RECHTER BEREICH ----------
    u8g2.drawFrame(55, 0, 73, 64);
    u8g2.drawLine(55, 12, 128, 12);

    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(61, 9, "Your Score:");
    Serial.println(snake_lenght);
    u8g2.drawStr(105, 9, String(snake_lenght).c_str());

    // Snake als Blöcke (je 5x5)
    int snakeX[] = {80, 80, 80, 85, 90, 90, 90, 95};
    int snakeY[] = {24, 29, 34, 34, 34, 39, 44, 44};
    for (int i = 0; i < 8; i++) {
      u8g2.drawFrame(snakeX[i], snakeY[i], 5, 5);
    }

    // Schlangenzunge strukturiert (2 „Zungenzungen“)
    u8g2.drawFrame(TONGUE_X, TONGUE_Y, 1, 4);          // linke Zunge
    u8g2.drawFrame(TONGUE_X + 1, TONGUE_Y + 2, 1, 4);  // rechte Zunge leicht versetzt

  } while (u8g2.nextPage());
}


// Für die generelle Code Strucktur wurde keine KI genutzt.
// KI kam nur zum einsatz als es ein Problem mit der alten Display Bib gab wo diese zu viel speicher nutzte
// und ich mir von der KI die alten Display Codes in die neue Lib umschreiben haben lasse.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN_PLAY 32
#define BTN_NEXT 33
#define BTN_PREV 25

struct Song {
  const char* title;
  const char* artist;
  int durationSec;
};

Song playlist[] = {
  { "Blinding Lights",  "The Weeknd",   200 },
  { "As It Was",        "Harry Styles", 167 },
  { "Levitating",       "Dua Lipa",     203 },
  { "Bad Habit",        "Steve Lacy",   232 },
  { "Golden Hour",      "JVKE",         209 },
  { "Flowers",          "Miley Cyrus",  183 }
};

const int TOTAL = sizeof(playlist) / sizeof(playlist[0]);

int  currentSong = 0;
bool isPlaying   = true;
int  progressSec = 0;
unsigned long lastTick = 0;
unsigned long lastBtn  = 0;
#define DEBOUNCE 250

void setup() {
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.print("SPOTIFY");
  display.setTextSize(1);
  display.setCursor(20, 32);
  display.print("ESP32 DISPLAY");
  display.setCursor(16, 46);
  display.print("by Ana Antunez");
  display.display();
  delay(2500);

  lastTick = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastBtn > DEBOUNCE) {
    if (digitalRead(BTN_PLAY) == LOW) {
      isPlaying = !isPlaying;
      lastBtn = now;
    }
    if (digitalRead(BTN_NEXT) == LOW) {
      currentSong = (currentSong + 1) % TOTAL;
      progressSec = 0;
      isPlaying = true;
      lastBtn = now;
    }
    if (digitalRead(BTN_PREV) == LOW) {
      if (progressSec > 3) {
        progressSec = 0;
      } else {
        currentSong = (currentSong - 1 + TOTAL) % TOTAL;
        progressSec = 0;
      }
      isPlaying = true;
      lastBtn = now;
    }
  }

  if (isPlaying && now - lastTick >= 1000) {
    progressSec++;
    if (progressSec >= playlist[currentSong].durationSec) {
      currentSong = (currentSong + 1) % TOTAL;
      progressSec = 0;
    }
    lastTick = now;
  }

  Song& s = playlist[currentSong];
  display.clearDisplay();

  display.fillRect(0, 0, 128, 13, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(3, 3);
  display.print("SPOTIFY");
  display.setCursor(75, 3);
  display.print(isPlaying ? ">> PLAYING" : "|| PAUSED");

  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 17);
  display.print(s.title);
  display.setCursor(2, 28);
  display.print(s.artist);

  display.drawLine(0, 38, 128, 38, SSD1306_WHITE);

  int barW = 120;
  int filled = map(progressSec, 0, s.durationSec, 0, barW);
  display.drawRect(4, 42, barW, 5, SSD1306_WHITE);
  display.fillRect(4, 42, filled, 5, SSD1306_WHITE);
  display.fillCircle(4 + filled, 44, 3, SSD1306_WHITE);

  display.setCursor(2, 53);
  printTime(progressSec);
  display.setCursor(100, 53);
  printTime(s.durationSec);
  display.setCursor(55, 53);
  display.print(currentSong + 1);
  display.print("/");
  display.print(TOTAL);

  display.display();
  delay(50);
}

void printTime(int sec) {
  int m = sec / 60;
  int s = sec % 60;
  if (m < 10) display.print("0");
  display.print(m);
  display.print(":");
  if (s < 10) display.print("0");
  display.print(s);
}

#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include <SPI.h>
#include <SD.h>

#define TFT_DC 9
#define TFT_CS 10


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
const int resistorPin = A0;
#define SD_CS 4
int sensorValue = 0;
int x[4];
int characterX = 120;  // 캐릭터의 초기 위치
int characterY = 30;
int numWalls = 4;      // 생성할 벽의 개수
int wallY[4];          // 벽의 초기 위치 (아래에서 시작)
int wallHeight[4];     // 벽의 높이
int characterSpeed = 8;
bool movingUp = true;
int wallSpacing = 100;  // 벽 사이의 간격 (y축 간격 설정)
int score = 0; 
bool isGameOver = false; 
unsigned long gameOverTime = 0;
unsigned long restartTime = 0;
int countdown = 5;

void setup(void) {
  Serial.begin(9600);

  tft.begin();
  tft.fillScreen(ILI9341_BLUE);
  tft.fillCircle(characterX, characterY, 10, ILI9341_YELLOW);

  // 벽 초기화
   for (int i = 0; i < numWalls; i++) {
    wallY[i] = 320 + i * wallSpacing;  // 각 벽의 초기 위치 설정
    
    wallHeight[i] = random(10, 200);  // 높이 랜덤 설정
  }
}

void loop() {
  if (!isGameOver) {
    // 게임 진행 중인 경우

    int sensorValue = analogRead(A0);
    int mappedValue = map(sensorValue, 0, 1023, 0, 240);
    tft.fillCircle(characterX, characterY, 10, ILI9341_BLUE);
    tft.fillCircle(mappedValue, characterY, 10, ILI9341_YELLOW);
    characterX = mappedValue;

    // 벽 이동
    for (int i = 0; i < numWalls; i++) {
      if (wallY[i] + 30 <= 0) {
        wallY[i] = 420;
        wallHeight[i] = random(10, 200);
        score++;
      }

      tft.fillRect(x[i], wallY[i], 240 - wallHeight[i], 30, ILI9341_BLUE);
      tft.fillRect(0, wallY[i], wallHeight[i], 30, ILI9341_BLUE);
      if (130 < wallHeight[i]) {
        x[i] = 230;
      } else if (60 < wallHeight[i]) {
        x[i] = 160;
      } else if (10 <= wallHeight[i]) {
        x[i] = 90;
      }
      
      if (movingUp) {
        wallY[i] -= characterSpeed;
      }
      tft.fillRect(0, wallY[i], wallHeight[i], 30, ILI9341_GREEN);
      tft.fillRect(x[i], wallY[i], 240 - wallHeight[i], 30, ILI9341_GREEN);
    }

    for (int i = 0; i < numWalls; i++) {
      int wallLeft = x[i];
      int wallRight = x[i] + wallHeight[i];
      int wallTop = wallY[i];
      int wallBottom = wallY[i] + 30;

      int characterLeft = characterX - 10;
      int characterRight = characterX + 10;
      int characterTop = characterY + 10;
      int characterBottom = characterY + 10;

      if (((characterRight >= wallLeft) || (characterLeft <= wallHeight[i])) &&
          (wallTop <= 40) && (wallTop >= 20)) {
        isGameOver = true;
        gameOverTime = millis();
      }
    }

    delay(30);
  } else {
    // 게임 오버 상태에서 카운트다운 및 게임 재시작
    unsigned long currentTime = millis();
    if (gameOverTime == 0) {
      gameOverTime = currentTime;
    }

    tft.setRotation(1);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(90, 90);
    tft.println("Game Over");
    tft.setTextSize(2);
        tft.setCursor(120, 130);
        tft.println("Score: " + String(score));
    int remainingTime = countdown - ((currentTime - gameOverTime) / 1000);
    if (remainingTime >= 0) {
      tft.setTextSize(2);
      tft.setCursor(140, 190);

  // 숫자만 지우기
  tft.fillRect(140, 185, 170, 25, ILI9341_BLUE);

  // 업데이트된 숫자 표시
  tft.println("Restart in " + String(remainingTime) + "s");
} else {
      isGameOver = false;
      gameOverTime = 0;
      score = 0;
      restartGame();
    }
  }
}
void restartGame() {
  // 게임을 초기화하고 재시작하는 함수
  tft.fillScreen(ILI9341_BLUE);
  characterX = 120;
  characterY = 30;
  tft.setRotation(0);

  // Initialize walls
  for (int i = 0; i < numWalls; i++) {
    wallY[i] = 320 + i * wallSpacing;
    wallHeight[i] = random(10, 200);
  }
}

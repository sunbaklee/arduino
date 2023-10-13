#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

int ballX, ballY, ballSpeedX, ballSpeedY;
int paddleX, paddleWidth, paddleHeight;
int paddlePin = A0; // 가변 저항 연결 핀
int analogValue;
int previousPaddleX; // 이전 패들 위치
boolean gameover = false;
unsigned long gameoverTime = 0;
unsigned long restartTime = 0;
int countdown = 5;
int score = 0;
int previousScore = -1; // 이전 점수를 저장할 변수

void setup() {
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  ballX = tft.width() / 2;
  ballY = tft.height() / 2;
  // 가로 방향 속도를 랜덤으로 설정
  ballSpeedX = random(-4, 4);
  ballSpeedY = -4; // 공이 처음에 위로 이동

  paddleWidth = 60;
  paddleHeight = 10;
  paddleX = (tft.width() - paddleWidth) / 2;
  previousPaddleX = paddleX;

  pinMode(paddlePin, INPUT);
  tft.fillRoundRect(paddleX, tft.height() - paddleHeight, paddleWidth, paddleHeight, 5, ILI9341_GREEN);
}

void loop() {
  if (gameover) {
    if (millis() - gameoverTime > restartTime * 1000) {
      startNewGame();
    } else {
      displayGameOver();
    }
  } else {
    analogValue = analogRead(paddlePin);
    paddleX = map(analogValue, 0, 1023, 0, tft.width() - paddleWidth);

    moveBall();
    drawPaddle();
    updateScore();
    delay(10);
  }
}

void moveBall() {
  tft.fillCircle(ballX, ballY, 5, ILI9341_BLACK);

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballX - 5 <= 0 || ballX + 5 >= tft.width()) {
    ballSpeedX = -ballSpeedX;
  }

  if (ballY - 5 <= 0) {
    ballSpeedY = -ballSpeedY;
  }

  if (ballY + 5 >= tft.height() - paddleHeight) {
    if (ballX >= paddleX && ballX <= paddleX + paddleWidth) {
      ballSpeedY = -ballSpeedY;
      // 랜덤 각도로 방향 변경
      ballSpeedX = random(-4, 4);
      // 이전 점수를 저장
      previousScore = score;
      score++;
    } else {
      gameover = true;
      gameoverTime = millis();
      restartTime = countdown;
    }
  }

  tft.fillCircle(ballX, ballY, 5, ILI9341_WHITE);

  tft.fillRect(previousPaddleX, tft.height() - paddleHeight, paddleWidth, paddleHeight, ILI9341_BLACK);

  previousPaddleX = paddleX;
}

void drawPaddle() {
  tft.fillRoundRect(paddleX, tft.height() - paddleHeight, paddleWidth, paddleHeight, 5, ILI9341_GREEN);
}

void displayGameOver() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(40, tft.height() / 2 - 20);
  tft.println("Game Over");
  tft.setCursor(65, tft.height() / 2 + 20);
  tft.print("Score: ");
  tft.println(score);
}

void startNewGame() {
  gameover = false;
  ballX = tft.width() / 2;
  ballY = tft.height() / 2;
  ballSpeedX = random(-4, 4);
  ballSpeedY = -4;
  previousScore = -1; // 이전 점수 초기화
  score = 0;
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRoundRect(paddleX, tft.height() - paddleHeight, paddleWidth, paddleHeight, 5, ILI9341_GREEN);
}

void updateScore() {
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 10);
  tft.print("Score: ");
  
  // 이전 점수를 지우고 새로운 점수 표시
  tft.fillRect(10, 10, 60, 10, ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.print("Score: ");
  tft.println(score);
}

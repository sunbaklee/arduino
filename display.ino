#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST -1
#define SD_CS 4
#define BUTTON_PIN 3   // 버튼 핀
#define ANALOG_PIN A0  // 가변저항 핀

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

int selectedMode = 0;  // 선택한 모드
int analogValue = 0;
int sensorValue = 0;
int x[4];
int characterX = 120;  // 캐릭터의 초기 위치
int characterY = 30;
int numWalls = 4;   // 생성할 벽의 개수
int wallY[4];       // 벽의 초기 위치 (아래에서 시작)
int wallHeight[4];  // 벽의 높이
int characterSpeed = 15;
bool movingUp = true;
int wallSpacing = 100;  // 벽 사이의 간격 (y축 간격 설정)
int score = 0;
bool isGameOver = false;
unsigned long gameOverTime = 0;
unsigned long gameoverTime = 0;
unsigned long restartTime = 0;
int countdown = 5;
int ballX, ballY, ballSpeedX, ballSpeedY;
int paddleX, paddleWidth, paddleHeight;
int paddlePin = A0;
int previousPaddleX;  // 이전 패들 위치
boolean gameover = false;
int previousScore = -1;  // 이전 점수를 저장할 변수

void setup() {
  tft.begin();
  tft.setRotation(1);  // 디스플레이를 가로 모드로 설정
  tft.fillScreen(ILI9341_BLACK);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // 내부 풀업 사용

  Serial.begin(9600);  // 시리얼 통신 시작
  Serial.println("Mode Selection Test");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    selectMode();
    if (selectedMode == 1) {
      tft.setRotation(0);
      startGame();  // 게임을 시작
      while (1) {
        flappybird();
        if (selectedMode != 1) {
          break;
        }
      }
      tft.setRotation(1);
    } else if (selectedMode == 0) {
      tft.setRotation(0);
      setsd();
      while (1) {
        viewer();
      }
    } else if (selectedMode == 2) {
      score = 0;
      pingpongset();
      while (1) {
        pingponggame();
        if(selectedMode != 2){
          break;
        }
      }
    }
    delay(1000);
  }

  analogValue = analogRead(ANALOG_PIN);

  if (analogValue < 200) {
    selectedMode = 0;
  } else if (analogValue < 400) {
    selectedMode = 1;
  } else if (analogValue < 600) {
    selectedMode = 2;
  } else if (analogValue < 800) {
    selectedMode = 3;
  } else {
    selectedMode = 4;
  }
  displaySelectedMode();

  delay(100);
}

void selectMode() {
  Serial.print("Selected Mode: ");
  Serial.println(selectedMode);
  tft.setTextSize(2);
  tft.setCursor(20, tft.width() / 2);  // 가로 모드에서도 가로로 출력
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Selected Mode: ");
  tft.println(selectedMode);
}

void displaySelectedMode() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, tft.width() / 2);  // 가로 모드에서도 가로로 출력
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Selected Mode: ");
  tft.println(selectedMode);
  tft.setCursor(20, tft.width() / 2 - 50);
  if (selectedMode == 0) {
    tft.setCursor(20, tft.width() / 2 - 50);
    tft.print("bmp viewer");
    tft.setTextColor(ILI9341_RED);
    tft.setCursor(20, tft.width() / 2 - 100);
    tft.print("can't out");
    tft.setTextColor(ILI9341_WHITE);
  } else if (selectedMode == 1) {
    tft.print("flappy bird");
  } else if (selectedMode == 2) {
    tft.print("ping-pong");
  } else {
    tft.print("comming soon");
  }
}

void startGame() {

  tft.fillScreen(ILI9341_BLUE);
  tft.fillCircle(characterX, characterY, 10, ILI9341_YELLOW);

  // 벽 초기화
  for (int i = 0; i < numWalls; i++) {
    wallY[i] = 320 + i * wallSpacing;  // 각 벽의 초기 위치 설정

    wallHeight[i] = random(10, 200);  // 높이 랜덤 설정
  }
}

void flappybird() {
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

      if (((characterRight >= wallLeft) || (characterLeft <= wallHeight[i])) && (wallTop <= 40) && (wallTop >= 20)) {
        isGameOver = true;
        gameOverTime = millis();
      }
    }

    delay(80);
  } else {
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW) {
      selectedMode = 100;  // 버튼을 누르면 모드 100으로 변경
      delay(1000);
    }
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
    tft.setCursor(10, 10);
    tft.println("hold button to quit");
    int remainingTime = countdown - ((currentTime - gameOverTime) / 1000);
    if (remainingTime >= 0) {
      tft.setTextSize(2);
      tft.setCursor(140, 190);


      // 숫자만 지우기
      tft.fillRect(140, 185, 170, 25, ILI9341_BLUE);

      // 업데이트된 숫자 표시
      tft.println("Restart in " + String(remainingTime) + "s");
      delay (1000);
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

void setsd() {

  tft.fillScreen(ILI9341_BLUE);

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  }
  Serial.println("OK!");
}

void viewer() {
  bmpDraw("segu1.bmp", 0, 0);
  delay(2000);  //길면 사진 오래봄
  bmpDraw("segu2.bmp", 0, 0);
  delay(2000);
  bmpDraw("segu3.bmp", 0, 0);
  delay(2000);
bmpDraw("segu4.bmp", 0, 0);
  delay(2000);
  bmpDraw("segu5.bmp", 0, 0);
  delay(2000);
#define BUFFPIXEL 20
}

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File bmpFile;
  int bmpWidth, bmpHeight;
  uint8_t bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  uint8_t sdbuffer[3 * BUFFPIXEL];
  uint8_t buffidx = sizeof(sdbuffer);
  boolean goodBmp = false;
  boolean flip = true;
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  if (read16(bmpFile) == 0x4D42) {
    Serial.print(F("File size: "));
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile);
    Serial.print(F("Image Offset: "));
    Serial.println(bmpImageoffset, DEC);
    Serial.print(F("Header size: "));
    Serial.println(read32(bmpFile));
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      Serial.print(F("Bit Depth: "));
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {

        goodBmp = true;
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        rowSize = (bmpWidth * 3 + 3) & ~3;


        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }


        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) {  // For each scanline...


          if (flip)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else  // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) {
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);
          }

          for (col = 0; col < w; col++) {
            if (buffidx >= sizeof(sdbuffer)) {
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;
            }

            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }
        }
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println(F("BMP format not recognized."));
}

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}

void pingpongset() {
  tft.fillScreen(ILI9341_BLACK);

  ballX = tft.width() / 2;
  ballY = tft.height() / 2;
  // 가로 방향 속도를 랜덤으로 설정
  ballSpeedX = random(-4, 4);
  ballSpeedY = -4;  // 공이 처음에 위로 이동

  paddleWidth = 60;
  paddleHeight = 10;
  paddleX = (tft.width() - paddleWidth) / 2;
  previousPaddleX = paddleX;

  pinMode(paddlePin, INPUT);
  tft.fillRoundRect(paddleX, tft.height() - paddleHeight, paddleWidth, paddleHeight, 5, ILI9341_GREEN);
}
void pingponggame() {
  if (gameover) {
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW) {
      selectedMode = 100;  // 버튼을 누르면 모드 100으로 변경
      delay(500);
    }
    if (millis() - gameoverTime > restartTime * 1000) {
      startNewGame();
    } else {
      displayGameOver();
      int remainingTime = restartTime - ((millis() - gameoverTime) / 1000) + 1;
      displayCountdown(remainingTime); // Display the countdown
      delay(700);
    }
  } else {
    analogValue = analogRead(paddlePin);
    paddleX = map(analogValue, 0, 1023, 0, tft.width() - paddleWidth);

    moveBall();
    drawPaddle();
    updateScore();
    delay(5);
  }
}

void displayCountdown(int remainingTime) {
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(140, 200);

  // Clear the previous countdown text by filling it with the background color (ILI9341_BLACK)
  tft.fillRect(10, 50, 200, 20, ILI9341_BLACK);

  tft.print("Restart in ");
  tft.print(remainingTime - 1);
  
  tft.print("s");
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
  tft.setCursor(10, 10);
  tft.println("hold button to quit");
  tft.setCursor(100, tft.height() / 2 - 30);
  tft.println("Game Over");
  tft.setCursor(110, tft.height() / 2 + 10);
  tft.print("Score: ");
  tft.println(score);
}

void startNewGame() {
  gameover = false;
  ballX = tft.width() / 2;
  ballY = tft.height() / 2;
  ballSpeedX = random(-4, 4);
  ballSpeedY = -4;
  previousScore = -1;  // 이전 점수 초기화
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

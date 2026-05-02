#include <Adafruit_NeoPixel.h>
#include <Adafruit_LiquidCrystal.h>

#define PIN 6         
#define NUMPIXELS 64   

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_LiquidCrystal lcd(0); 


const int btnUp = 2;
const int btnDown = 3;
const int btnLeft = 4;
const int btnRight = 5;

int snakeX[64];
int snakeY[64];
int snakeLen = 3;

int dirX = 1;
int dirY = 0;

int foodX;
int foodY;
int score = 0;

bool gameOver = false;

void setup() {
  pixels.begin();
  pixels.setBrightness(50); 
  
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);

  snakeX[0] = 4; snakeY[0] = 4; 
  snakeX[1] = 3; snakeY[1] = 4; 
  snakeX[2] = 2; snakeY[2] = 4; 
  
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("SNAKE GAME");
  delay(1500); 
  lcd.clear();  
  
  lcd.setCursor(0, 0);
  lcd.print("Score: 0");

  spawnFood();
}

void loop() {
  if (gameOver) {    
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER!");
    lcd.setCursor(4, 1);
    lcd.print("Score: ");
    lcd.print(score);

    pixels.fill(pixels.Color(255, 0, 0));
    pixels.show();
    delay(500);
    pixels.clear();
    pixels.show();
    delay(500);
    return;
  }
  
  if (digitalRead(btnUp) == LOW && dirY != 1) { dirX = 0; dirY = -1; }
  else if (digitalRead(btnDown) == LOW && dirY != -1) { dirX = 0; dirY = 1; }
  else if (digitalRead(btnLeft) == LOW && dirX != 1) { dirX = -1; dirY = 0; }
  else if (digitalRead(btnRight) == LOW && dirX != -1) { dirX = 1; dirY = 0; }

  
  for (int i = snakeLen - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  
  snakeX[0] += dirX;
  snakeY[0] += dirY;
 
  if (snakeX[0] < 0 || snakeX[0] > 7 || snakeY[0] < 0 || snakeY[0] > 7) {
    gameOver = true;
  }
  
  for (int i = 1; i < snakeLen; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
    }
  }
  
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    if (snakeLen < 64) snakeLen++;
    score++;    
    
    lcd.setCursor(7, 0); 
    lcd.print(score);
    
    spawnFood();
  }
  
  pixels.clear();
  pixels.setPixelColor(getPixel(foodX, foodY), pixels.Color(255, 0, 0));
  for (int i = 0; i < snakeLen; i++) {
    pixels.setPixelColor(getPixel(snakeX[i], snakeY[i]), pixels.Color(0, 255, 0));
  }
  pixels.show();  
  // Játék sebessége
  delay(800); 
}

void spawnFood() {
  bool valid = false;
  while (!valid) {
    foodX = random(0, 8);
    foodY = random(0, 8);
    valid = true;
    for (int i = 0; i < snakeLen; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        valid = false;
        break;
      }
    }
  }
}

int getPixel(int x, int y) {
  return y * 8 + x;
}

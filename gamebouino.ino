/**************************************************************************
GameBuino by Preston Rooker
 **************************************************************************/
 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address (generally 0x3D or 0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define VRX_PIN A0
#define VRY_PIN A1
#define SW_PIN 2

#define NONE  0
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define SNAKE 5

#define FRAMERATECONST 130
//#define FRAMERATECONST 67

//344  bytes are used just by the libraries and display construction >:(
//1100 bytes are needed for the OLED buffer
//apparently sometimes you need 1200 bytes for OLED buffer, its hard to tell
//okay so I did the math and it takes (or should take) 1136 bytes
//so good luck working with like only 800 bytes


//snake
byte dir = RIGHT;
uint16_t score = 0; //score increases each time a player eats a food
byte headX;
byte headY;
byte tailX;
byte tailY;
byte foodX;
byte foodY;

//board
#define BOARD_WIDTH 32
#define BOARD_HEIGHT 16
byte board[BOARD_WIDTH][BOARD_HEIGHT]; //could potentially try storing everying into two bits, only giving direction up down left or right

//going to try to do byte array of 32 x 16 by using 4x4 blocks

void drawBlock(byte x, byte y, uint16_t color = SSD1306_WHITE); //apparently for some reason if the function has a default parameter, it needs to be put before setup

void setup() 
{
  // put your setup code here, to run once:


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Don't proceed, loop forever
    for(;;){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(300);
      digitalWrite(LED_BUILTIN, LOW);
      delay(2000);  
    }
  }

  for(byte x = 0; x < BOARD_WIDTH; x++){
    for(byte y = 0; y < BOARD_HEIGHT; y++){
      board[x][y] = NONE; //set each cell to NONE
    }
  }
  
  pinMode(VRX_PIN,INPUT);
  pinMode(VRY_PIN,INPUT);
  pinMode(SW_PIN,INPUT_PULLUP);
  
  randomSeed(A2); //A2 has nothing connected to it, which causes noise which seeds the random
  //randomSeed(12);

  // Clear the buffer
  display.clearDisplay();

  generateFood();

  dir = UP;
  headX = 16;
  headY = 8;
  tailX = 16;
  tailY = 10;
  drawBlock(headX,headY);
  board[headX][headY] = SNAKE;
  drawBlock(16,9);
  board[16][9] = UP;
  drawBlock(tailX,tailY);
  board[tailX][tailY] = UP;
  display.display();
  delay(1000);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  long initTime = millis();
  calculateDirection();
  moveSnake();
  display.display();
  delay(FRAMERATECONST -(millis() - initTime));
  //runGameOver();
}

//code food drawing

void generateFood(){
  do{
    foodX = random(0,BOARD_WIDTH-1);
    foodY = random(0,BOARD_HEIGHT-1);
  }while(board[foodX][foodY] != NONE);
  drawFood();
}

void drawFood(){
  display.drawPixel(foodX*4+1,foodY*4,SSD1306_WHITE);
  display.drawPixel(foodX*4+2,foodY*4,SSD1306_WHITE);

  display.drawPixel(foodX*4,foodY*4+1,SSD1306_WHITE);
  display.drawPixel(foodX*4,foodY*4+2,SSD1306_WHITE);

  display.drawPixel(foodX*4+1,foodY*4+3,SSD1306_WHITE);
  display.drawPixel(foodX*4+2,foodY*4+3,SSD1306_WHITE);

  display.drawPixel(foodX*4+3,foodY*4+1,SSD1306_WHITE);
  display.drawPixel(foodX*4+3,foodY*4+2,SSD1306_WHITE);
}

//32 by 16
void drawBlock(byte x, byte y, uint16_t color = SSD1306_WHITE)
{
  x = x*4;
  y = y*4;
  display.fillRect(x,y,4,4,color);
}

void calculateDirection(){
  int jsX = analogRead(VRX_PIN);
  int jsY = analogRead(VRY_PIN);
  if(jsX > 800 && dir != LEFT){
    dir = RIGHT;
  }
  else if(jsX < 200 && dir != RIGHT){
    dir = LEFT;
  }
  else if(jsY < 200 && dir != DOWN){
    dir = UP;
  }
  else if(jsY > 800 && dir != UP){
    dir = DOWN;
  }
}

void moveSnake(){
  switch(dir){
    case NONE:
      break;
    case UP:
      board[headX][headY] = UP;
      if(headY == 0){
          headY = BOARD_HEIGHT - 1;
      }
      else{
        headY--;
      }
      break;
    case DOWN:
      board[headX][headY] = DOWN;
      if(headY == BOARD_HEIGHT - 1){
          headY = 0;
      }
      else{
        headY++;
      }
      break;
    case LEFT:
      board[headX][headY] = LEFT;
      if(headX == 0){
          headX = BOARD_WIDTH - 1;
      }
      else{
        headX--;
      }
      break;
    case RIGHT:
      board[headX][headY] = RIGHT;
      if(headX == BOARD_WIDTH - 1){
          headX = 0;
      }
      else{
        headX++;
      }
      break;
  }
  if(headX != foodX || headY != foodY){
    drawBlock(tailX,tailY,SSD1306_BLACK);
    switch(board[tailX][tailY]){
      case UP:
        board[tailX][tailY] = NONE;
        if(tailY == 0){
            tailY = BOARD_HEIGHT - 1;
        }
        else{
          tailY--;
        }
        break;
      case DOWN:
        board[tailX][tailY] = NONE;
        if(tailY == BOARD_HEIGHT - 1){
            tailY = 0;
        }
        else{
          tailY++;
        }
        break;
      case LEFT:
        board[tailX][tailY] = NONE;
        if(tailX == 0){
            tailX = BOARD_WIDTH - 1;
        }
        else{
          tailX--;
        }
        break;
      case RIGHT:
        board[tailX][tailY] = NONE;
        if(tailX == BOARD_WIDTH - 1){
            tailX = 0;
        }
        else{
          tailX++;
        }
        break;
    }
  }
  else{
    score++;
    generateFood();
  }
  detectCollision();
  board[headX][headY] = SNAKE;
  drawBlock(headX,headY);
}

void detectCollision(){
  if(board[headX][headY] != NONE){
    runGameOver();
  }
}

void runGameOver(){
  bool toggle = false;
  for(byte i = 0; i < 8; i++){
    toggle = !toggle;
    display.invertDisplay(toggle);
    delay(200);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  //text size 1 is 6x8 pixels, other sizes just multiply, so size 2 is (6*2)x(8*2) so 12x16
  
  display.setTextSize(2);
   display.setCursor(10,display.height()/4);
  display.println(F("GAME OVER"));
  
  display.setTextSize(1);    
  display.setCursor(31,45);
  display.print(F("Score: "));

  if(score > 199){
    display.print(F("2"));
    score = score - 200;
  }
  else if(score > 99){
    display.print(F("1"));
    score = score - 100;
  }

  if(score < 10){
    display.print(F("0"));
  }
  else if(score < 20){
    display.print(F("1"));
    score = score - 10;
  }
  else if(score < 30){
    display.print(F("2"));
    score = score - 20;
  }
  else if(score < 40){
    display.print(F("3"));
    score = score - 30;
  }
  else if(score < 50){
    display.print(F("4"));
    score = score - 40;
  }
  else if(score < 60){
    display.print(F("5"));
    score = score - 10;
  }
  else if(score < 70){
    display.print(F("6"));
    score = score - 60;
  }
  else if(score < 80){
    display.print(F("7"));
    score = score - 70;
  }
  else if(score < 90){
    display.print(F("8"));
    score = score - 80;
  }
  else if(score < 100){
    display.print(F("9"));
    score = score - 90;
  }

  if(score < 1){
    display.print(F("0"));
  }
  else if(score < 2){
    display.print(F("1"));
  }
  else if(score < 3){
    display.print(F("2"));
  }
  else if(score < 4){
    display.print(F("3"));
  }
  else if(score < 5){
    display.print(F("4"));
  }
  else if(score < 6){
    display.print(F("5"));
  }
  else if(score < 7){
    display.print(F("6"));
  }
  else if(score < 8){
    display.print(F("7"));
  }
  else if(score < 9){
    display.print(F("8"));
  }
  else if(score < 10){
    display.print(F("9"));
  }
  display.display();
  for(;;); //Don't proceed, loop forever
}

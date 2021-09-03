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

#define FRAMERATECONST 100

//344 bytes are used just by the libraries and display construction >:(

//snake
byte dir = RIGHT;
//uint16_t length = 3;
//uint16_t numSnakeBlocks = 1; //can only use one uint16_t for this by simply putting detection for removing tail pixel in placement pixel and then just not removing a tail pixel when food is eaten
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
  //Serial.begin(9600); //REMOVE ON RELEASE
  pinMode(VRX_PIN,INPUT);
  pinMode(VRY_PIN,INPUT);
  pinMode(SW_PIN,INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed")); //REMOVE ON RELEASE
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    for(;;); // Don't proceed, loop forever
  }

  for(byte x = 0; x < BOARD_WIDTH; x++){
    for(byte y = 0; y < BOARD_HEIGHT; y++){
      board[x][y] = NONE; //set each cell to NONE
    }
  }

  // Clear the buffer
  display.clearDisplay();

  foodX = 0;
  foodY = 0;

  dir = LEFT;
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

  display.display();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  // Serial.print("X: ");
  // Serial.print(analogRead(VRX_PIN));
  // Serial.print("  Y: ");
  // Serial.print(analogRead(VRY_PIN));
  // Serial.print("  SW: ");
  // Serial.println(digitalRead(SW_PIN));
  int jsX = analogRead(VRX_PIN);
  int jsY = analogRead(VRY_PIN);
  long initTime = millis();
  if(jsX > 800){
    dir = RIGHT;
  }
  else if(jsX < 200){
    dir = LEFT;
  }
  else if(jsY < 200){
    dir = UP;
  }
  else if(jsY > 800){
    dir = DOWN;
  }

  // Serial.print("direction: ");
  // switch(dir){
  //   case UP:
  //     Serial.println("UP");
  //     break;
  //   case DOWN:
  //     Serial.println("DOWN");
  //     break;
  //   case LEFT:
  //     Serial.println("LEFT");
  //     break;
  //   case RIGHT:
  //     Serial.println("RIGHT");
  //     break;
  // }
  moveSnake();
  display.display();
  delay(FRAMERATECONST -(millis() - initTime));
}

//32 by 16
void drawBlock(byte x, byte y, uint16_t color = SSD1306_WHITE)
{
  x = x*4;
  y = y*4;
  display.fillRect(x,y,4,4,color);
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
  board[headX][headY] = SNAKE;
  drawBlock(headX,headY);
}

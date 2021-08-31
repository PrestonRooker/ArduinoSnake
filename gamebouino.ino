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

#define NONE  0
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define SNAKE 5

//snake
byte dir = RIGHT;
uint16_t length = 3;
uint16_t numSnakeBlocks = 1;
byte headX;
byte headY;
byte tailX;
byte tailY;

//board
#define BOARD_WIDTH 32
#define BOARD_HEIGHT 16
byte board[BOARD_WIDTH][BOARD_HEIGHT];

//going to try to do byte array of 32 x 16 by using 4x4 blocks

void drawBlock(byte x, byte y, uint16_t color = SSD1306_WHITE); //apparently for some reason if the function has a default parameter, it needs to be put before setup

void setup() 
{
  // put your setup code here, to run once:
  //Serial.begin(9600); //REMOVE ON RELEASE

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed")); //REMOVE ON RELEASE
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    for(;;); // Don't proceed, loop forever
  }

  for(byte x = 0; x < BOARD_WIDTH; x++){
    for(byte y = 0; y < BOARD_HEIGHT; y++){
      board[x][y] = -1; //set each cell to empty
    }
  }

  // Clear the buffer
  display.clearDisplay();

  dir = RIGHT;
  headX = 20;
  headY = 10;
  drawBlock(headX,headY);
  display.display();
  delay(1000);

}

void loop() 
{
  // put your main code here, to run repeatedly:
  moveSnake();
  processBoard();
  display.display();
}

//40 by 20
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
      if(headY == 0){
          headY = BOARD_HEIGHT;
      }
      else{
        headY--;
      }
      break;
    case DOWN:
      if(headY == BOARD_HEIGHT){
          headY = 0;
      }
      else{
        headY++;
      }
      break;
    case LEFT:
      if(headX == 0){
          headX = BOARD_WIDTH;
      }
      else{
        headX--;
      }
      break;
    case RIGHT:
      if(headX == BOARD_WIDTH){
          headX = 0;
      }
      else{
        headX++;
      }
      break;
  }
  board[headX][headY] = length + 1;
  drawBlock(headX,headY);
}

void processBoard(){
  for(byte x = 0; x < BOARD_WIDTH; x++){
    for(byte y = 0; y < BOARD_HEIGHT; y++){
      if(board[x][y] > 0){ //snake in cell
        board[x][y]--;
      }
      else if (board[x][y] == 0){ //snake just left cell
        drawBlock(x,y,SSD1306_BLACK);
        board[x][y]--;
      }
    }
  }
}

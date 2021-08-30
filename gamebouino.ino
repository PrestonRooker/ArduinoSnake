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

enum direction{
  up,
  down,
  left,
  right,
  none
};

//snake
direction dir = none;
int16_t length = 3;
byte snakeX;
byte snakeY;

//board
#define BOARD_WIDTH 40
#define BOARD_HEIGHT 20
int16_t board[BOARD_WIDTH][BOARD_HEIGHT];

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

  dir = up;
  snakeX = 20;
  snakeY = 10;
  drawBlock(snakeX,snakeY);
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
  x = x*3 + 6;
  y = y*3 + 3;
  display.fillRect(x,y,3,3,color);
}

void moveSnake(){
  switch(dir){
    case none:
      break;
    case up:
      snakeY--;
      if(snakeY < 0){
          snakeY = BOARD_HEIGHT;
      }
      break;
    case down:
      snakeY++;
      if(snakeY > BOARD_HEIGHT){
          snakeY = 0;
      }
      break;
    case left:
      snakeX--;
      if(snakeX < 0){
          snakeX = BOARD_WIDTH;
      }
      break;
    case right:
      snakeX++;
      if(snakeX > BOARD_WIDTH){
          snakeX = 0;
      }
      break;
  }
  board[snakeX][snakeY] = length + 1;
  drawBlock(snakeX,snakeY);
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

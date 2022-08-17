#include <RGBmatrixPanel.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define CLK  8  
#define OE   9
#define LAT 10
#define A   A1
#define B   A0
#define C   A2

#define B_MODE  11
#define B_TASK1 12
#define B_TASK2 13

const uint8_t xCount = 32;
const uint8_t yCount = 16;


unsigned long gameboard[yCount];
unsigned long oldGameboard[yCount];

//TODO: Turn this into a 2D array. It is late and I don't want to deal with this.
unsigned long oldGameboard2[yCount];
unsigned long oldGameboard3[yCount];
unsigned long oldGameboard4[yCount];
unsigned long oldGameboard5[yCount];

unsigned int habitBoard[yCount/2];

int bModeState;
int btask1State;
int btask2State;
bool isGameBoard;
int habitIndex = 0;

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// Gets the value at an x and y position.
bool getBoardValue(unsigned long *board, int x, int y){
  unsigned long index = ((unsigned long)1 << x);
  return (board[y] & index) ? 1 : 0;
}

// Sets the value at an x and y position.
void setBoardValue(unsigned long *board, int x, int y, bool value){
  if(value == 0){
    board[y] = (board[y] & (~((unsigned long)1 << x)));
  }else if(value == 1){
    board[y] = (board[y] | ((unsigned long)1 << x));
  }
}


bool getHabitBoardValue(int x, int y){
  unsigned int index = ((unsigned int)1 << x);
  return (habitBoard[y] & index) ? 1 : 0;
}


void setHabitBoardValue(int x, int y, bool value){
  if(value == 0){
    habitBoard[y] = (habitBoard[y] & (~((unsigned long)1 << x)));
  }else if(value == 1){
    habitBoard[y] = (habitBoard[y] | ((unsigned long)1 << x));
  }
}


void printGOLBoard(){
  for(uint8_t y=0; y<16; y++) {
    for(uint8_t x=0; x<32; x++) {
      if(getBoardValue(gameboard, x, y)){
        matrix.drawPixel(x, y, matrix.Color333(155, 155, 155));
      }else{
        matrix.drawPixel(x, y, matrix.Color333(0, 0, 0));
      }
    }
  }
}


uint8_t countNeighbours(uint8_t x, uint8_t y){
  uint8_t numNeigh = 0;

  for(int indx = -1; indx < 2; indx++){
    for(int indy = -1; indy < 2; indy++){

      if(getBoardValue(oldGameboard, ((x + indx)%32), ((y + indy)%16))){
        numNeigh = numNeigh + 1;
      }
    }
  }
  if(getBoardValue(oldGameboard,x,y)){
    numNeigh -= getBoardValue(oldGameboard,x,y);
  }
  
  return numNeigh;
}
void calculateNextBoard(){
  int numNeigh;
  for(uint8_t y=0;y<16;y++){
    oldGameboard[y] = gameboard[y];
    oldGameboard2[y] = oldGameboard[y];
    oldGameboard3[y] = oldGameboard2[y];
    oldGameboard4[y] = oldGameboard3[y];
    oldGameboard5[y] = oldGameboard4[y];
  } 
  
  for(uint8_t y=0; y<16; y++){
    for(uint8_t x=0; x<32;x++){
      numNeigh = countNeighbours(x,y);
      if(getBoardValue(oldGameboard, x, y) && numNeigh < 2){
        setBoardValue(gameboard, x, y, false);
      }else if(getBoardValue(oldGameboard, x, y) && numNeigh >=2 && numNeigh <= 3){
        setBoardValue(gameboard, x, y, true);
      }else if(getBoardValue(oldGameboard, x, y) && numNeigh > 3){
        setBoardValue(gameboard, x, y, false);
      }else if(not getBoardValue(oldGameboard, x, y) && numNeigh == 3){
        setBoardValue(gameboard, x, y, true);
      } 
    }
    //Serial.print("Done Calculating neighbours oldGameboard\n");
  }
  
}

bool checkGOLBoard(){
  bool duplicateBoard;
  unsigned long *oldBoards[5] = {
    oldGameboard,
    oldGameboard2,
    oldGameboard3,
    oldGameboard4,
    oldGameboard5,
  };

  for(uint8_t x=0; x<5;x++){
    unsigned long *oldBoard = oldBoards[x];
    duplicateBoard = true;
    for(uint8_t y=0; y<16;y++){
      if (gameboard[y] != oldBoard[y]){
        duplicateBoard = false;
        break;
      } 
    }
    if(duplicateBoard){
      break;
    } 
  }
  return duplicateBoard;
}

void randomizeGameBoard(){
  randomSeed(analogRead(5));
  for(uint8_t y = 0; y<16; y++){
    for(int x = 0; x < 32; x++){
      if (random(101) <= 40){
        setBoardValue(gameboard,x,y,true);
      }else{
        setBoardValue(gameboard,x,y,false);
      }
    }

    if (y % 2 == 0){
      habitBoard[y/2] = 0;
    }
  }
}


void resetGOL(){
  randomizeGameBoard();
  for(uint8_t y = 0; y<16; y++){
    oldGameboard[y] = 0;
    oldGameboard2[y] = 0;
    oldGameboard3[y] = 0;
    oldGameboard4[y] = 0;
    oldGameboard5[y] = 0; 
  }
}

void printHabitBoard(){
 for(uint8_t y=0; y<8; y++) {
  for(uint8_t x=0; x<16; x++) {
    int trueX = x * 2;
    int trueY = y * 2;
    if(getHabitBoardValue(x, y)){
      matrix.drawPixel(trueX, trueY, matrix.Color333(155, 155, 155));
      matrix.drawPixel(trueX + 1, trueY, matrix.Color333(155, 155, 155)); 
      matrix.drawPixel(trueX, trueY + 1, matrix.Color333(155, 155, 155)); 
      matrix.drawPixel(trueX + 1, trueY + 1, matrix.Color333(155, 155, 155));
    }else{
      matrix.drawPixel(trueX, trueY, matrix.Color333(0, 0, 0));
      matrix.drawPixel(trueX + 1, trueY, matrix.Color333(0, 0, 0)); 
      matrix.drawPixel(trueX, trueY + 1, matrix.Color333(0, 0, 0)); 
      matrix.drawPixel(trueX + 1, trueY + 1, matrix.Color333(0, 0, 0));
    }
  }
 }  
}

void handleButtons(){
  bool modeChange = false;
  bool task1Change = false;
  bool task2Change = false;
  int stateModeButton = digitalRead(B_MODE);
  int stateTask1Button = digitalRead(B_TASK1);
  int stateTask2Button = digitalRead(B_TASK2);

  if (stateModeButton != bModeState){
    bModeState = stateModeButton;
    if(bModeState == LOW){
      isGameBoard = !isGameBoard;
      modeChange = true; 
    }
    delay(50);
    return; // Exit because we don't want to read any other button.
  } 

  if (stateTask1Button != btask1State && !isGameBoard){
    btask1State = stateTask1Button;
    if(btask1State == LOW){
      task1Change = true;
    }
  }

  if (stateTask2Button != btask2State && !isGameBoard){
    btask2State = stateTask2Button;
    if(btask2State == LOW){
      task2Change = true;
    }
  }

  if(task1Change && task2Change){
    // Run backspace command
    int x =1; 
  }else if (task1Change){
     int x =1;   // Run mark day as complete function.
  }else if (task2Change){
     int x =1;// Make day as incomplete.
  } 

  if(task1Change || task2Change){
    delay(50);
  } 
}
//Print boards is not working correctly for some reason
void setup() {
  Serial.begin(9600);
  matrix.begin(); 
  isGameBoard = false;
  pinMode(B_MODE, INPUT_PULLUP);
  pinMode(B_TASK1, INPUT_PULLUP);
  pinMode(B_TASK2, INPUT_PULLUP);

  bModeState = digitalRead(B_MODE);
  btask1State = digitalRead(B_TASK1);
  btask2State = digitalRead(B_TASK2);
}

void loop() {
  //TODO: Add a stanby mode where nothing is displayed.
  // Start in the task mode
  // Use button presses to switch between the modes.
  handleButtons();
  
  if(isGameBoard){
    bool isFinished = checkGOLBoard();
    if(isFinished){
      resetGOL();
    }
    printGOLBoard();
    calculateNextBoard();
    delay(50);
  }else{
    printHabitBoard();
  }
}

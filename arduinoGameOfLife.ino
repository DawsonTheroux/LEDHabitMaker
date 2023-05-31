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
#define B_TASK3 1

#define PR A3       // Photo Resister pin.

const float MAX_BRIGHTNESS = 0.3; // Max brightness percentage;
const float MIN_BRIGHTNESS = 0.1; // Mine brightness percentage;

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
int btask3State;
bool isGameBoard;
int habitIndex;

float boardBrightness;


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
    habitBoard[y] = (habitBoard[y] & (~((unsigned int)1 << x)));
  }else if(value == 1){
    habitBoard[y] = (habitBoard[y] | ((unsigned int)1 << x));
  }
}

void drawBoardPixel(uint8_t x, uint8_t y, String strColour){ 
  uint16_t colour; 
  if(strColour.equals("WHITE")){
    colour = matrix.Color888(130 * boardBrightness, 255 * boardBrightness, 100 * boardBrightness);
  }else if (strColour.equals("RED")){
    colour = matrix.Color888(255,120,50);
  }else{
    colour = matrix.Color888(0,0,0);
  }
  matrix.drawPixel(x,y,colour);

}

void printGOLBoard(){
  for(uint8_t y=0; y<16; y++) {
    for(uint8_t x=0; x<32; x++) {
      if(getBoardValue(gameboard, x, y)){
        drawBoardPixel(x,y,"WHITE");
      }else{
        drawBoardPixel(x,y,"NONE");
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
    
    oldGameboard5[y] = oldGameboard4[y];
    oldGameboard4[y] = oldGameboard3[y];
    oldGameboard3[y] = oldGameboard2[y];
    oldGameboard2[y] = oldGameboard[y];
    oldGameboard[y] = gameboard[y];
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
  bool duplicateBoard = true;
  unsigned long *oldBoards[5] = {
    oldGameboard,
    oldGameboard2,
    oldGameboard3,
    oldGameboard4,
    oldGameboard5,
  };

     
  // Check if the current gameboard looks like any of the past ones.
  for(uint8_t x=0; x<5;x++){
    //unsigned long oldBoard = oldBoards[x];
    duplicateBoard = true;
    for(uint8_t y=0; y<16;y++){
      if (gameboard[y] != oldBoards[x][y]){
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
  int indX = habitIndex % 16;
  int indY = habitIndex / 16;
      
  for(uint8_t y=0; y<yCount; y++){
    for(uint8_t x=0; x<32; x++){
      int trueX = x / 2;
      int trueY = y / 2;
      if((trueX > indX) && (trueY == indY) || (trueY > indY)){
        drawBoardPixel(x, y, "NONE");
      }else if(getHabitBoardValue(trueX, trueY)){
        drawBoardPixel(x, y, "WHITE");
      }else{
        drawBoardPixel(x, y, "RED");
      } 
    } 
 }  
}


void taskComplete(){
  //Set the space at the habitIndex to the completed value.
 // This seems like a job for modulo. 
  habitIndex += 1;
  int habitX = habitIndex % 16;
  int habitY = habitIndex / 16;
  setHabitBoardValue(habitX, habitY, true);
  return; 
}

void taskIncomplete(){
  habitIndex += 1;
  int habitX = habitIndex % 16;
  int habitY = habitIndex / 16; 

  // It is necessary to set the value because undo task could have left completed tasks ahead.
  setHabitBoardValue(habitX, habitY, false);
  return;
}

void undoTask(){
  // Decrement the index counter.
  habitIndex -=1;
  return;

}

void handleButtons(){
  bool modeChange = false;
  int stateModeButton = digitalRead(B_MODE);
  int stateTask1Button = digitalRead(B_TASK1);
  int stateTask2Button = digitalRead(B_TASK2);
  int stateTask3Button = digitalRead(B_TASK3);

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
      modeChange = true;
      taskComplete();
    }
  }

  if (stateTask2Button != btask2State && !isGameBoard){
    btask2State = stateTask2Button;
    if(btask2State == LOW){
      modeChange = true;
      taskIncomplete();
    }
  }

  if (stateTask3Button != btask3State && !isGameBoard){
    btask3State = stateTask3Button;
    if(btask3State == LOW){
      modeChange = true;
      undoTask();
    }
  }

  if(modeChange){
    delay(50);
  } 

  return;
}


void initializeHabitBoards(){
  for(uint8_t y=0; y<8; y++) {
    habitBoard[y] = 0;
  }   
}

void setBoardBrightness(){
  // The photo resister will output a value between 0 and 1000.
  short photoResist = analogRead(PR); // PR max value around 1000, lowest 200.
  float newBoardBrightness = float(photoResist) / 1000;
  if(abs(newBoardBrightness - boardBrightness) > 0.1){
    boardBrightness = newBoardBrightness;
  }
  
  if(boardBrightness > MAX_BRIGHTNESS){
    boardBrightness  = MAX_BRIGHTNESS;
  }else if(boardBrightness < MIN_BRIGHTNESS){
    boardBrightness = MIN_BRIGHTNESS;
  }
  boardBrightness = 0.25;
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
  btask3State = digitalRead(B_TASK3);
  habitIndex = -1;
  initializeHabitBoards();
  boardBrightness = 1;

}


void loop() {
  //TODO: Add a stanby mode where nothing is displayed.
  // Start in the task mode
  // Use button presses to switch between the modes.
  setBoardBrightness();
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
    delay(25);
  }
}

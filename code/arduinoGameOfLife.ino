// testcolors demo for Adafruit RGBmatrixPanel library.
// Renders 512 colors on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420
// Library supports 4096 colors, but there aren't that many pixels!  :)

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <RGBmatrixPanel.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Most of the signal pins are configurable, but the CLK pin has some
// special constraints.  On 8-bit AVR boards it must be on PORTB...
// Pin 8 works on the Arduino Uno & compatibles (e.g. Adafruit Metro),
// Pin 11 works on the Arduino Mega.  On 32-bit SAMD boards it must be
// on the same PORT as the RGB data pins (D2-D7)...
// Pin 8 works on the Adafruit Metro M0 or Arduino Zero,
// Pin A4 works on the Adafruit Metro M4 (if using the Adafruit RGB
// Matrix Shield, cut trace between CLK pads and run a wire to A4).

#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
/*
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
*/
#define OE   9
#define LAT 10
#define A   A1
#define B   A0
#define C   A2

#define BUTTON_ONE   11
#define BUTTON_TWO   12
#define BUTTON_THREE 13

const uint8_t xCount = 32;
const uint8_t yCount = 16;

unsigned long gameboard[yCount];
unsigned long oldGameboard[yCount];
unsigned long oldGameboard2[yCount];
unsigned long oldGameboard3[yCount];


RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);


bool getBoardValue(unsigned long *board, int x, int y){
  unsigned long index = ((unsigned long)1 << x);
  return (board[y] & index) ? 1 : 0;
}

void setBoardValue(unsigned long *board, int x, int y, bool value){
  if(value == 0){
    board[y] = (board[y] & (~((unsigned long)1 << x)));
  }else if(value == 1){
    board[y] = (board[y] | ((unsigned long)1 << x));
  }
}


void printBoard(){
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
  //Serial.print("Calculating next board\n");
  //time_t t;
  //srand((unsigned) time(&t));
  int numNeigh;
  for(uint8_t y=0;y<16;y++){
    oldGameboard[y] = gameboard[y];
    oldGameboard2[y] = gameboard[y];
    oldGameboard3[y] = gameboard[y];
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
      }else{
        //Serial.print("ERROR");
      }
    }
    //Serial.print("Done Calculating neighbours oldGameboard\n");
  }
  
}

//Print boards is not working correctly for some reason
void setup() {
  Serial.begin(9600);
  matrix.begin();
  randomSeed(analogRead(0));
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

void loop() {
  printBoard();
  calculateNextBoard();
  delay(750);
}

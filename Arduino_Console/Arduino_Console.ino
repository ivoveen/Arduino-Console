#include <RGBmatrixPanel.h>

#define CLK  8   
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

//--------------------------
//matrix is the screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);


//---------------------------
//input value variables
//---------------------------
int xValue = 0 ;
int yValue = 0 ; 
int joystickDirection = 0;
int joystickPressed = 0 ;
int actionPressed = 0;
int pausePressed = 0;

//---------------------------
//basic operating variables
//---------------------------
int gameState = 0; 
int waitCounter = 0; //this counter can be re-used by every different game state.
int noSpam = 0;
//---------------------------
//main menu variables
//---------------------------
const int SCROLLSPEED = 1500;
const int GAMESPEED = 800;
const char *GAMES[5] = {"Snake", "Ducks", "Catch", "Shoot"};
int gameSelected = 0;


// -------------------------------------------------------------------------------
// SETUP CODE
// -------------------------------------------------------------------------------

void setup() {
  // Setup ports:
  
  Serial.begin(9600) ;
  pinMode(11,INPUT); 
  digitalWrite(11,HIGH); 
  pinMode(12,INPUT); 
  digitalWrite(12,HIGH); 
  pinMode(13,INPUT); 
  digitalWrite(13,HIGH); 
  
  matrix.begin();

  randomSeed(analogRead(0));

  startupAnimation();  
}


void startupAnimation(){
  int width = 12;

  int rainbow[ 12 ][ 3 ] = { { 255, 0, 0 }, { 255, 127, 0 }, { 255, 255, 0 }, { 127, 255, 0 }, { 0, 255, 0 },{ 0, 255, 127 }, { 0, 255, 255 }, { 0, 127, 255 }, { 0, 0, 255 }, { 127, 0, 255 }, { 255, 0, 255 }, { 255, 0, 127 }};
  
  for(int i=0; i<32 + width; i++){
    for(int j=0; j<16; j++){
      matrix.drawPixel(i, j, matrix.Color888(rainbow[i % 12][1],rainbow[i % 12][2],rainbow[i % 12][3]));
    
      if(i>=width){
        matrix.drawPixel(i - width, j, matrix.Color333(0, 0, 0));
      }    
    }
    delay(25);
  }

  matrix.setCursor(1, 0);  // start at top left, with one pixel of spacing
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.print("Retro");
  matrix.setCursor(8, 8);  // next line
  matrix.print("frog");

  delay(2000);

  for(int i=32; i>=0 - width; i--){
    for(int j=0; j<16; j++){
      matrix.drawPixel(i, j, matrix.Color888(rainbow[i % 12][1],rainbow[i % 12][2],rainbow[i % 12][3]));
    
      if(i<=32 - width){
        matrix.drawPixel(i + width, j, matrix.Color333(0, 0, 0));
      }    
    }
    delay(25);
  }

  //render the main menu  
  mainMenuRender();
}


// -------------------------------------------------------------------------------
// GENERAL FUNCTION CODE
// -------------------------------------------------------------------------------

void readInputValues(){
  //-----------------------------
  //explanation:
  //this function reads all input values and saves them in public variables.
  //for the joystick input value it refers to the joystickInput() function to handle it.
  //-----------------------------
  xValue = analogRead(A4);  
  yValue = analogRead(A5);  
  joystickPressed = digitalRead(11);  
  actionPressed = digitalRead(13);  
  pausePressed = digitalRead(12);  
  joystickDirection = joystickInput();
}


int joystickInput(){
  //-----------------------------
  //explanation:
  //this function returns value 1,2,3 or 4 corresponding to the four possible directions: Up, Right, Down, Left. In that order.
  //if no direction is being held the function will return 0.
  //-----------------------------
  int deltaXvalue;
  int deltaYvalue;
  //look for which axis the joystick is mostly pointing
  deltaXvalue = abs(xValue - 500);
  deltaYvalue = abs(yValue - 511);

  if (deltaXvalue > deltaYvalue && deltaXvalue > 200){
    if (xValue > 500){
      return 2;
      
    }
    if(xValue < 500){
      return 4;
    }
  }
  
  if (deltaYvalue > deltaXvalue && deltaYvalue > 200){
    if (yValue < 500){
      return 1;
      
    }
    if(yValue > 500){
      return 3;
    }
  }
  // if no direction is being held return 0.
  return 0;
}



void debugPrintInputValues(){
  // this function prints all input values to the serial monitor. In general you wont have to use this function if I tested everything well.
  //
  Serial.print(joystickDirection);
  Serial.print(",");
  Serial.print(xValue,DEC);
  Serial.print(",");
  Serial.print(yValue,DEC);
  Serial.print(",");
  Serial.print(!joystickPressed);
  Serial.print(",");
  Serial.print(pausePressed);
  Serial.print(",");
  Serial.print(actionPressed);
  Serial.print("\n");
  
  delay(10); 
}

void clearScreen(){
  //this function clears the screen when called.
  matrix.fillScreen(matrix.Color333(0,0,0));
  matrix.updateDisplay();
}

void renderSprite(byte sprite[4][3], int topLeftX, int topLeftY){
  // ----------------------------------------------------------------------------------------------------------------------
  // explanation:
  // this function can print 2 x 2 sprites for you, for an example how to use it refer to one of the snakerender functions.
  // provide the sprite array together with the x and y coordinate of the top left of where the sprite needs to be.
  //----------------------------------------------------------------------------------------------------------------------
  for(int i=0; i<4; i++){
     if(sprite[i][0] == 254){
      
     }else{
      matrix.drawPixel(topLeftX + (i % 2), topLeftY + (floor(i / 2)), matrix.Color333(sprite[i][0], sprite[i][1], sprite[i][2]));
      
     }
  }
}





// -------------------------------------------------------------------------------
// GAME LOOP CODE
// -------------------------------------------------------------------------------

void loop() {
  // Go to the loop of the current game state, for example: Main menu or, Snake or, Duck game or, etc
   
   readInputValues();
   //debugPrintInputValues();

  if (pausePressed != 1){
    noSpam = 0;    
  }
  if (pausePressed == 1 && noSpam != 1){
    gameState = 0;
    mainMenuRender();
    noSpam = 1;
  }

  switch(gameState){
   case 0:
    mainMenuRun();
    break;
   case 1:
    snakeRun();
    break;
   case 2:
    snakeLose();
    break;
  }
}


// -------------------------------------------------------------------------------
// MAIN MENU CODE
// -------------------------------------------------------------------------------

void mainMenuRun(){
  //check if a game was selected this tick
  if(actionPressed == 1){
    Serial.print("BUTTON IS PRESSED");
    switch (gameSelected){
      case 0:
      gameState = 1;   
      snakeSetup();
      
      break;
      case 1:
    
      break;
      case 2:

      break;
      case 3:

      break;
      case 4:

      break;
      
    }

    
  }

  //if the joystick is moved up or down and the waitcounter has waited long enough:
  if(joystickDirection !=0  && joystickDirection !=2  && joystickDirection !=4  &&  waitCounter >= SCROLLSPEED){ 
    mainMenuRender();
    waitCounter = 0;
  }
  waitCounter++;

}

void mainMenuRender(){
  clearScreen();

  //update the selected game, looping around when going too low or too high
  if(joystickDirection == 1){
    gameSelected--;
    if (gameSelected == -1){
      gameSelected = sizeof(GAMES) / sizeof(GAMES[0]) - 2;
    } 
  } else if(joystickDirection == 3){
    gameSelected++;

    if (gameSelected > sizeof(GAMES)/ sizeof(GAMES[0]) - 2){
      gameSelected = 0;
    }
  }

  //print the text onto the screen
  matrix.setCursor(1, 0);  // start at top left, with one pixel of spacing
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.print(GAMES[gameSelected]);
  
}

// -------------------------------------------------------------------------------
// SNAKE CODE
// -------------------------------------------------------------------------------
const int SNAKEDISPX = 15;
const int SNAKEDISPY = 7;
int snakeDisp[SNAKEDISPX+1][SNAKEDISPY+1];
int growLonger;

struct snake{
  int xPosHead;
  int yPosHead;
  
  int xPosBody;
  int yPosBody;
  int bodyCount;

  int xPosTail;
  int yPosTail;
  int sDirection;
  int score; // the length of the snake

  int foodX;
  int foodY;
};

struct snake snake;





void snakeSetup(){
  
  //setup the snake game
  
  clearScreen();
  waitCounter = 0;
   
  int i, j;
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 8; j++) {
      snakeDisp[i][j] = 0;
    }
  }

  snake.xPosHead = 8;
  snake.yPosHead = 4;

  snake.xPosBody = 8;
  snake.yPosBody = 4;
  snakeDisp[snake.xPosBody][snake.yPosBody] = 2;
  snake.bodyCount = 0;
  
  snake.xPosTail = snake.xPosBody;
  snake.yPosTail = snake.yPosBody;
  snake.sDirection = 2;
  growLonger = 2;
  
  snake.score = 3;


  respawnFood();
}


void snakeRun(){

  //always read the joystick so movement isnt lost
  if (joystickDirection != 0){
      //making sure you cant back into yourself and die that way, because its unsatisfying    
      if(snake.sDirection == 1 && joystickDirection == 3 || snake.sDirection == 3 && joystickDirection == 1 ||snake.sDirection == 2 && joystickDirection == 4 ||snake.sDirection == 4 && joystickDirection == 2){

      }else{
      snake.sDirection = joystickDirection;
      }
    }

   
  //make sure the game doesnt update every tick because thats way too fast.
  if(waitCounter == GAMESPEED){
    waitCounter = 0;

    //use input to update the location of the head of the snake.
    updateSnakeHead();

    //render the snake onto the screen.
    snakeRender();

    //the game should end when touching a space already occupied by the snake.
    if(snakeDisp[snake.xPosHead][snake.yPosHead] != 0){
      waitCounter = -1;
      gameState = 2;
    }

    //if the snake eats the food it should grow 1 longer and spawn new food at random.
    if(snake.xPosHead == snake.foodX && snake.yPosHead == snake.foodY){
      growLonger = 1;
      snake.score++;
      respawnFood();
    }
    //update the next location for the snake body
    updateSnakeBody();

    //update the next location for the snake tail
    updateSnakeTail();
  }
  waitCounter++;
  
}

void updateSnakeHead(){
  snakeDisp[snake.xPosHead][snake.yPosHead] = snake.sDirection;
    
    // read which direction the snake will go and change head location accordingly
    switch (snake.sDirection) {
      case 1:
      snake.yPosHead++; //up
      if (snake.yPosHead > SNAKEDISPY){
        snake.yPosHead = 0;
      }
      break;
      case 2:
        snake.xPosHead++; //right
        if (snake.xPosHead > SNAKEDISPX){
          snake.xPosHead = 0;
        }
      break;
      case 3:
        snake.yPosHead--; // down
        if (snake.yPosHead < 0){
          snake.yPosHead = SNAKEDISPY;
        }
      break;
      case 4:
        snake.xPosHead--; //left 
        if (snake.xPosHead < 0){
         snake.xPosHead = SNAKEDISPX;
        }
      break;
    }
}

void updateSnakeBody(){
  
    //update the tail coordinates, unless it needs to grow then skip once
    
      
      switch (snakeDisp[snake.xPosBody][snake.yPosBody]) {
        case 1:
        snake.yPosBody++; //up
        if (snake.yPosBody > SNAKEDISPY){
          snake.yPosBody = 0;
        }
        break;
        case 2:
          snake.xPosBody++; //right
          if (snake.xPosBody > SNAKEDISPX){
            snake.xPosBody = 0;
          }
        break;
        case 3:
          snake.yPosBody--; // down
          if (snake.yPosBody < 0){
            snake.yPosBody = SNAKEDISPY;
          }
        break;
        case 4:
          snake.xPosBody--; //left 
          if (snake.xPosBody < 0){
          snake.xPosBody = SNAKEDISPX;
          }
        break;
      }
    
}

void updateSnakeTail(){
  
    //update the tail coordinates, unless it needs to grow then skip once
    if (growLonger == 0){
      
      switch (snakeDisp[snake.xPosTail][snake.yPosTail]) {
        case 1:
        snakeDisp[snake.xPosTail][snake.yPosTail] = 0;
        snake.yPosTail++; //up
        if (snake.yPosTail > SNAKEDISPY){
          snake.yPosTail = 0;
        }
        break;
        case 2:
          snakeDisp[snake.xPosTail][snake.yPosTail] = 0;
          snake.xPosTail++; //right
          if (snake.xPosTail > SNAKEDISPX){
            snake.xPosTail = 0;
          }
        break;
        case 3:
          snakeDisp[snake.xPosTail][snake.yPosTail] = 0;
          snake.yPosTail--; // down
          if (snake.yPosTail < 0){
            snake.yPosTail = SNAKEDISPY;
          }
        break;
        case 4:
          snakeDisp[snake.xPosTail][snake.yPosTail] = 0;
          snake.xPosTail--; //left 
          if (snake.xPosTail < 0){
          snake.xPosTail = SNAKEDISPX;
          }
        break;
      }
    } else{
     growLonger--; 
    }
}

void snakeRender(){

byte snakeSprite[ 4 ][ 3 ] = 
   { { 0, 255, 0 }, { 255, 255, 255}, 
   { 0, 255, 0 }, { 0, 255, 0 }};
   
byte emptySnakeSprite[ 4 ][ 3 ] = 
   { { 0, 0, 0 }, { 0, 0, 0}, 
   { 0, 0, 0 }, { 0, 0, 0 }};
  
  
byte snakeBody1Sprite[ 4 ][ 3 ] = 
   { { 0, 255, 0 }, { 0, 255, 0},
   { 0, 255, 0 }, { 0, 0, 0 }};

byte snakeBody2Sprite[ 4 ][ 3 ] = 
    { { 0, 255, 0 }, { 0, 0, 0},
   { 0, 255, 0 }, { 0, 255, 0 }};
   

if(snake.bodyCount == 0){
  renderSprite(snakeBody1Sprite,snake.xPosBody * 2, snake.yPosBody *2);
  snake.bodyCount++;
  } else{
  renderSprite(snakeBody2Sprite,snake.xPosBody * 2, snake.yPosBody *2);
  snake.bodyCount--;
  }

  
  renderSprite(snakeSprite,snake.xPosHead * 2 , snake.yPosHead *2);
    
  renderSprite(emptySnakeSprite,snake.xPosTail * 2, snake.yPosTail *2);
  

  //matrix.drawPixel(snake.xPosHead, snake.yPosHead, matrix.Color333(0, 7, 0))
  //matrix.drawPixel(snake.xPosTail, snake.yPosTail, matrix.Color333(0, 0, 0));
  
}






//-----------------------------
//Respawn the food at a random location that isnt occupied by the snake. Render it too.
//-----------------------------
   
void respawnFood(){
  
  snake.foodX = rand()%SNAKEDISPX;
  snake.foodY = rand()%SNAKEDISPY;
  while(snakeDisp[snake.foodX][snake.foodY] != 0){
    snake.foodX = rand()%SNAKEDISPX;
    snake.foodY = rand()%SNAKEDISPY;
  }
  //render the food
   byte foodSprite[ 4 ][ 3 ] = 
   { { 255, 0, 0 }, { 255, 0, 0}, 
   { 255, 0, 0 }, { 255, 0, 0 }};
   
  
  renderSprite(foodSprite,snake.foodX * 2, snake.foodY *2 );
  
  //matrix.drawPixel(snake.foodX, snake.foodY, matrix.Color333(7, 0, 0));
}


//-----------------------------
//the lose screen of snake
//-----------------------------

void snakeLose(){
if(waitCounter == 0){
  clearScreen();
  matrix.setCursor(1, 0);  // start at top left, with one pixel of spacing
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print("Score");
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 8);
  matrix.print(snake.score);
  waitCounter++;
}

  if(actionPressed == 1){
    gameState = 0;
  }


}

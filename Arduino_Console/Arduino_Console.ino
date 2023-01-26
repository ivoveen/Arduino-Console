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
int noSpam2 = 0;
int noSpam3 = 0;
int waitCounter2 = 0;
//---------------------------
//main menu variables
//---------------------------
const int SCROLLSPEED = 1200;
const int GAMESPEED = 800;
const char *GAMES[6] = {"Snake", "Dino", "Whack", "Catch", "Shoot"};
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
   //render the main menu  
  mainMenuSetup();
  mainMenuRender();
  gameState = 0;
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
    mainMenuSetup();
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
   case 3:
    WhackRun();
    break;
   case 4:
    WhackLose();
    break;
   case 5:
    dinoRunRun();
    break;
   case 6:
    dinoRunLose();
    break;
  }
}


// -------------------------------------------------------------------------------
// MAIN MENU CODE
// -------------------------------------------------------------------------------

void mainMenuSetup(){
  waitCounter = 0;
  noSpam = 0;
}

void mainMenuRun(){
  //check if a game was selected this tick
  if(actionPressed == 1){
    switch (gameSelected){
      case 0:
      gameState = 1;   
      snakeSetup();
      
      break;
      case 1:
      gameState = 5;   
      dinoRunSetup();
      break;
      case 2:
      gameState = 3;   
      WhackSetup();
      break;
      case 3:

      break;
      case 4:

      break;
      
    }

    
  }


  //if the joystick is moved up or down and the waitcounter has waited long enough:
  if(joystickDirection !=0  && joystickDirection !=2  && joystickDirection !=4 && noSpam2 == 0){ 
    noSpam2 = 1;
    mainMenuRender();
  }

  if (noSpam2 == 1){
    waitCounter++;
    if(waitCounter == SCROLLSPEED){
      noSpam2 = 0;
      waitCounter = 0;
    }    
  }
  

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
byte snakeDisp[SNAKEDISPX+1][SNAKEDISPY+1];
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


//--------------------------------------------------------------------
//        DINO RUN GAME
// -------------------------------------------------------------------

const int dinoMaxObstacles = 5;
const int DINOAIRTIME = 7;
const int dinoSpeedUpTime = 20;
const int BIRDHEIGHT = 8;
const int DINORUNSPEEDSTART = 200;

int dinoYpos = 14;
int dinoState;
int dinoRunSpeed;
int jumpBool;
int duckBool;
int dinoScore;
int dinoLives;


struct dinoObstacle{
  int type; // type refers to the sprite that needs to be rendered, the hitbox it has and if it is ground or air
  int xPos;
};

struct dinoObstacle Obstacle;

//set up of dino run
void dinoRunSetup(){
  clearScreen();
  dinoYpos = 14;
  dinoState = 1;
  dinoRunSpeed = DINORUNSPEEDSTART;
  
  jumpBool = 0;
  duckBool = 0;
  
  waitCounter2 = 0;
  noSpam3 = 0;
  noSpam = 1;
  noSpam2 = 0;
  
  dinoScore = 0;
  dinoLives = 3;


  Obstacle.type = 0;
  Obstacle.xPos = 31;

  spawnObstacle(10);

  //draw sand ground
   matrix.fillRect(0, 15, 32, 1, matrix.Color888(153, 229, 80));  

        //draw score
    
        matrix.fillRect(26, 0, 16, 8, matrix.Color888(0, 0, 0)); 
        matrix.setTextColor(matrix.Color333(7,0,0));
        matrix.setCursor(26, 0);
        matrix.print(dinoLives);
}

//-------------------------------------
//the main game loop
//-----------------------------------

void dinoRunRun(){
  
  //handle inputs
  if(actionPressed == 1 || joystickDirection == 3){
    
    //dino is jumping
    if(jumpBool == 0){ //making sure you cant spam jump in the air
    
      jumpBool = 1;      
    
      //and remove dino face if it was ducking previously
      for(int i = 7; i < 9; i++){
          for(int j = 0; j < 6; j++){
            matrix.drawPixel(i + 1, dinoYpos - j, matrix.Color333(0, 0, 0));
          }
        }
    }
    dinoState = 2;
  }
  else if(joystickDirection == 1 && dinoState != 2){
    //dino is ducking
    duckBool = 1;
    dinoState = 1;
  }  else if(dinoState != 2){ 
    //dino is running
    dinoState = 1;
    duckBool = 0;
  }

  if(waitCounter == dinoRunSpeed){
    waitCounter = 0;  
  
    //move the dino
    dinoRender();

    //move Obstacles
    dinoMoveObstacles();


    //redraw the life counter
    dinoUpdateLife();
  }
  waitCounter++;

}

//-------------------------------------
//Dino health
//-----------------------------------

void dinoUpdateLife(){
  matrix.fillRect(26, 0, 16, 8, matrix.Color333(0, 0, 0)); 
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.setCursor(26, 0);
  matrix.print(dinoLives);  
}

void dinoHit(){
  if (noSpam3 == 0){ //
  dinoLives--;
  dinoUpdateLife(); 
  if(dinoLives == 0){
      waitCounter = -1;
      gameState = 6;
      noSpam = 0;
      if(actionPressed == 1){ // make sure the end screen is not skipped
        noSpam = 1;
      }
  }

  }
}

//-------------------------------------
// obstacle code
//-----------------------------------


void spawnObstacle(int extraX){
  Obstacle.xPos = 29 + extraX + rand()%6;
  Obstacle.type = rand()%3 + 1;
}


void dinoMoveObstacles(){
  
    if (Obstacle.type != 0){
      
      /*
      Serial.print( "type: ");
      Serial.print( Obstacles[i].type);
      Serial.print( "i: ");
      Serial.print(i);
      Serial.print( "xpos: ");
      Serial.print( Obstacles[i].xPos);
      */
      
      //this obstacles exists and should be moved
      Obstacle.xPos = Obstacle.xPos - 1;
      
      //check if its offscreen completely, if so spawn a new one
      if(Obstacle.xPos < -9){
        spawnObstacle(0);

        dinoScore++;
        
        if(dinoRunSpeed - dinoSpeedUpTime > 40){
          dinoRunSpeed = dinoRunSpeed - dinoSpeedUpTime;          
        }
      } else {


        renderObstacle(Obstacle.type, Obstacle.xPos);
      }
    }
  
}

void renderObstacle( int type, int xPos){
  int tempVal;
  switch (type) {
    case 1:
      //check if the cactus hits the player

      if (xPos <= 7 && xPos + 6  >= 3){ // is the cactus even close to the players x
        if(dinoYpos > 9){ // did the player jump high enough
          dinoHit();
          noSpam3 = 1;
        }
      }else{
        noSpam3 = 0;
      }


    
      

     matrix.fillRect(xPos+1, 9, 6, 6, matrix.Color333(0, 0, 0));  
     //matrix.fillRect(xPos, 10, 3, 6, matrix.Color888(26, 122, 26));

      matrix.drawPixel(xPos + 2, 14 - 0, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 3, 14 - 0, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos + 2, 14 - 1, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 3, 14 - 1, matrix.Color888(255,255,255));

      matrix.drawPixel(xPos    , 14 - 2, matrix.Color888(255,255,255));
      matrix.drawPixel(xPos + 1, 14 - 2, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 2, 14 - 2, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 3, 14 - 2, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 5, 14 - 3, matrix.Color888(255,255,255));

      matrix.drawPixel(xPos    , 14 - 3, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 2, 14 - 3, matrix.Color888(255,255,255));
      matrix.drawPixel(xPos + 3, 14 - 3, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 4, 14 - 3, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 5, 14 - 3, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos + 2, 14 - 4, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 3, 14 - 4, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 5, 14 - 4, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos + 2, 14 - 5, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 3, 14 - 5, matrix.Color888(255,255,255));

      break;
    case 2:
      if (xPos <= 7 && xPos + 6  >= 3){ // is the cactus even close to the players x
        if(dinoYpos > 10){ // did the player jump high enough
          dinoHit();
          noSpam3 = 1;
        }
      }else{
        noSpam3 = 0;
      }

    
     matrix.fillRect(xPos+1, 9, 6, 6, matrix.Color333(0, 0, 0));  
     //matrix.fillRect(xPos, 10, 3, 6, matrix.Color888(26, 122, 26));

      matrix.drawPixel(xPos + 1, 14 - 0, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 2, 14 - 0, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 3, 14 - 0, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 4, 14 - 0, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos    , 14 - 1, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 1, 14 - 1, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 2, 14 - 1, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 3, 14 - 1, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 4, 14 - 1, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 5, 14 - 1, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos + 1, 14 - 2, matrix.Color888(106, 190, 48));
      matrix.drawPixel(xPos + 2, 14 - 2, matrix.Color888(153, 229, 80));
      matrix.drawPixel(xPos + 3, 14 - 2, matrix.Color888(215, 123, 186));
      matrix.drawPixel(xPos + 4, 14 - 2, matrix.Color888(153, 229, 80));

      matrix.drawPixel(xPos + 2, 14 - 3, matrix.Color888(215, 123, 186));
      matrix.drawPixel(xPos + 3, 14 - 3, matrix.Color888(215, 123, 186));
      matrix.drawPixel(xPos + 4, 14 - 3, matrix.Color888(215, 123, 186));

      matrix.drawPixel(xPos + 3, 14 - 4, matrix.Color888(215, 123, 186));
      break;


    case 3: //flying thing

      if (xPos <= 7 && xPos + 8  >= 3){ // is the flyer even close to the players x
        if(duckBool == 1){
          if(dinoYpos - 5 <= BIRDHEIGHT){ // is the player ducking
            dinoHit();
            noSpam3 = 1;
          }
        }else{
          if(dinoYpos - 6 <= BIRDHEIGHT){ // is the player low enough
            dinoHit();
            noSpam3 = 1;
          }
        }

        
      }else{
        noSpam3 = 0;
      }

      matrix.fillRect(xPos+1, 1, 9, 8, matrix.Color333(0, 0, 0));  
     //matrix.fillRect(xPos, 10, 3, 6, matrix.Color888(26, 122, 26));

      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 0, matrix.Color888(102, 57, 49));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 0, matrix.Color888(102, 57, 49));

      matrix.drawPixel(xPos + 3, BIRDHEIGHT - 1, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 4, BIRDHEIGHT - 1, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 1, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 1, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 1, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 8, BIRDHEIGHT - 1, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos    , BIRDHEIGHT - 2, matrix.Color888(102, 57, 49));
      matrix.drawPixel(xPos + 1, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 2, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 3, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 4, BIRDHEIGHT - 2, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 2, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 8, BIRDHEIGHT - 2, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos    , BIRDHEIGHT - 3, matrix.Color888(102, 57, 49));
      matrix.drawPixel(xPos + 1, BIRDHEIGHT - 3, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 2, BIRDHEIGHT - 3, matrix.Color888(102, 57, 49));
      matrix.drawPixel(xPos + 3, BIRDHEIGHT - 3, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 4, BIRDHEIGHT - 3, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 3, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 3, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos + 1, BIRDHEIGHT - 4, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 2, BIRDHEIGHT - 4, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 4, BIRDHEIGHT - 4, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 4, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 4, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos + 4, BIRDHEIGHT - 5, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 5, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 5, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 5, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos + 5, BIRDHEIGHT - 6, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 6, matrix.Color888(238, 195, 154));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 6, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 8, BIRDHEIGHT - 6, matrix.Color888(143, 86, 59));

      matrix.drawPixel(xPos + 6, BIRDHEIGHT - 6, matrix.Color888(143, 86, 59));
      matrix.drawPixel(xPos + 7, BIRDHEIGHT - 6, matrix.Color888(143, 86, 59));

    break;
 }
}

//-------------------------------------
// dino code
//-----------------------------------



void dinoRender(){
  switch (dinoState){
    case 1:
      

      drawBasicDinosaurBody();

      if (duckBool == 0){
        for(int i = 7; i < 9; i++){
          for(int j = 0; j < 6; j++){
            matrix.drawPixel(i + 1, dinoYpos - j, matrix.Color333(0, 0, 0));
          }
        }
        
        matrix.drawPixel(7, dinoYpos -3, matrix.Color333(0, 0, 0));

        drawBasicDinosaurHead(0, 0);
      } else if  (duckBool == 1){
        //dino is ducking while running
        for(int i = 0; i < 7; i++){
          for(int j = 4; j < 7; j++){
            matrix.drawPixel(i + 1, dinoYpos - j, matrix.Color333(0, 0, 0));
          }
        }

        drawBasicDinosaurHead(2, 1);        
      }
      
     //Dino is running
      if(noSpam2 == 1){
       noSpam2--;
       matrix.drawPixel(3, dinoYpos - 1, matrix.Color888(89, 193, 53));
       matrix.drawPixel(4, dinoYpos - 1, matrix.Color888(89, 193, 53));
       matrix.drawPixel(6, dinoYpos - 1, matrix.Color888(121, 58, 128));
       matrix.drawPixel(7, dinoYpos - 1, matrix.Color888(121, 58, 128));
      } else if(noSpam2 == 0){
        noSpam2++;
        matrix.drawPixel(3, dinoYpos - 1, matrix.Color888(223, 62, 35));
        matrix.drawPixel(4, dinoYpos - 1, matrix.Color888(223, 62, 35));
        matrix.drawPixel(6, dinoYpos - 1, matrix.Color888(89, 193, 53));
        matrix.drawPixel(7, dinoYpos - 1, matrix.Color888(89, 193, 53));
      }
      break;
    case 2:

    //clear the old position
    for(int i = 0; i < 7; i++){
      for(int j = 0; j < 7; j++){
        matrix.drawPixel(i + 1, dinoYpos - j, matrix.Color333(0, 0, 0));
      }
    }
    
    //Dino is jumping
      if (jumpBool == 1){
        //move up
        jumpBool = 1;
        dinoYpos--;
        
        if(dinoYpos == 6){
          
          //when back down jump is over
          jumpBool = 2;
        }
      } else if (jumpBool == 2){
        if(waitCounter2 == DINOAIRTIME){
          jumpBool = 3;
          waitCounter2 = 0;
        }
        waitCounter2++;

      }else if (jumpBool == 3){
        //go down when top is reached
        dinoYpos++;
        jumpBool = 3;
        
        if(dinoYpos == 14){
          //when back down jump is over
          dinoState = 0;
          jumpBool = 0;          
        }
      } 

      drawBasicDinosaurBody();
      drawBasicDinosaurHead(0, 0);
     break;
  }
}

void drawBasicDinosaurBody(){

   //this dinosaur drawing takes 3% storage !!!!!!!!!!!!!!!!!
      matrix.drawPixel(7, dinoYpos, matrix.Color888(121, 58, 128));
      matrix.drawPixel(6, dinoYpos, matrix.Color888(121, 58, 128));
      matrix.drawPixel(4, dinoYpos, matrix.Color888(223, 62, 35));
      matrix.drawPixel(3, dinoYpos, matrix.Color888(223, 62, 35));

      matrix.drawPixel(7, dinoYpos - 1, matrix.Color888(89, 193, 53));
      matrix.drawPixel(6, dinoYpos - 1, matrix.Color888(233, 181, 163));
      matrix.drawPixel(5, dinoYpos - 1, matrix.Color888(233, 181, 163));
      matrix.drawPixel(4, dinoYpos - 1, matrix.Color888(89, 193, 53));
      matrix.drawPixel(3, dinoYpos - 1, matrix.Color888(89, 193, 53));
      matrix.drawPixel(2, dinoYpos - 1, matrix.Color888(89, 193, 53));
      
      matrix.drawPixel(7, dinoYpos - 2, matrix.Color888(89, 193, 53));
      matrix.drawPixel(6, dinoYpos - 2, matrix.Color888(233, 181, 163));
      matrix.drawPixel(5, dinoYpos - 2, matrix.Color888(233, 181, 163));
      matrix.drawPixel(4, dinoYpos - 2, matrix.Color888(89, 193, 53));
      matrix.drawPixel(3, dinoYpos - 2, matrix.Color888(89, 193, 53));
      matrix.drawPixel(2, dinoYpos - 2, matrix.Color888(250, 106, 10));
      matrix.drawPixel(1, dinoYpos - 2, matrix.Color888(89, 193, 53));


}

void drawBasicDinosaurHead(int xOffset, int yOffset){
      matrix.drawPixel(6 +xOffset, dinoYpos - 3 + yOffset, matrix.Color888(254, 243, 192));
      matrix.drawPixel(5 +xOffset, dinoYpos - 3 + yOffset,matrix.Color888(254, 243, 192));
      matrix.drawPixel(4 +xOffset, dinoYpos - 3 + yOffset, matrix.Color888(254, 243, 192));
      matrix.drawPixel(3 +xOffset, dinoYpos - 3 + yOffset, matrix.Color888(250, 106, 10));
     
      matrix.drawPixel(7 + xOffset, dinoYpos - 4 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(6 + xOffset, dinoYpos - 4 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(5 + xOffset, dinoYpos - 4 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(4 + xOffset, dinoYpos - 4 + yOffset,  matrix.Color888(254, 243, 192));
      matrix.drawPixel(3 + xOffset, dinoYpos - 4 + yOffset, matrix.Color888(254, 243, 192));
      matrix.drawPixel(2 + xOffset, dinoYpos - 4 + yOffset,  matrix.Color888(250, 106, 10));

      matrix.drawPixel(7 + xOffset, dinoYpos - 5 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(6 + xOffset, dinoYpos - 5 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(5 + xOffset, dinoYpos - 5 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(4 + xOffset, dinoYpos - 5 + yOffset, matrix.Color888(6, 6, 8));
      matrix.drawPixel(3 + xOffset, dinoYpos - 5 + yOffset, matrix.Color888(156, 219, 67));

      matrix.drawPixel(6 + xOffset, dinoYpos - 6 + yOffset, matrix.Color888(156, 219, 67));
      matrix.drawPixel(5 + xOffset, dinoYpos - 6 + yOffset,matrix.Color888(89, 193, 53));
      matrix.drawPixel(4 + xOffset, dinoYpos - 6 + yOffset,matrix.Color888(156, 219, 67));
      matrix.drawPixel(3 + xOffset, dinoYpos - 6 + yOffset, matrix.Color888(250, 106, 10));
}


//-------------------------------------
// dino lose screen
//-----------------------------------

void dinoRunLose(){
if(waitCounter == 0){
  clearScreen();
  matrix.setCursor(1, 0);  // start at top left, with one pixel of spacing
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color888(143, 86, 59));
  matrix.print("Score");
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 8);
  matrix.print(dinoScore);
  waitCounter++;
}

  if(noSpam == 0){
    if(actionPressed == 1){
      gameState = 0;
    }
  }
  if(actionPressed == 0){
      noSpam = 0;
  }
}

// ------------------------------------------------------------------------------- 

// Whack CODE 

// ------------------------------------------------------------------------------- 

// copied from snake, fitted to WHACK: 

const int WHACKDISPX = 31; 
const int WHACKDISPY = 15; 
int WHACKDisp[WHACKDISPX+1][WHACKDISPY+1]; 
int selectedside = 4; 
int WaitForLaser = 0;
int WhackScore = 0;

struct hole{ 
  int xPos; 
  int yPos; 
  int side; // 1,2,3,4 = up, right, down, left 
  int mode; // if mode is 1 hole goes right, if 0 hole goes left. 
}; 

struct laser{
  int xPos;
  int yPos;
  int side; //1,2,3,4 = up, right, down, left 
  int mode; // if mode is 1 laser comes out, if 0 laser retracts. 
  int foodX;
  int foodY;
  
};
struct laser laser;
struct hole holeLeft; 
struct hole holeRight;
struct hole holeUp; 
struct hole holeDown; 


// ----------------------------------------------------- 

 

 //setup the Whack game 

 
 
// ----------------------------------------------------- 

void WhackSetup(){    

  // initial values 
holeLeft.xPos = 0; 
holeLeft.yPos = 8; 
holeLeft.side = 4; 
holeLeft.mode = 1;
holeRight.xPos = 31; 
holeRight.yPos = 8; 
holeRight.side = 2; 
holeRight.mode = 1; 
holeUp.xPos = 16; 
holeUp.yPos = 0; 
holeUp.side = 1; 
holeUp.mode = 1; 
holeDown.xPos = 16; 
holeDown.yPos = 15; 
holeDown.side = 3; 
holeDown.mode = 1; 
laser.xPos = 0;
laser.yPos = 0;
laser.side = 1;
laser.mode = 0;
laser.foodX = 0;
laser.foodY = 0;
 
  clearScreen(); 

  WhackHoleRender(); 

  respawnTarget();
  //respawnFood();

  waitCounter = 0; 

  int i, j; 

  for (i = 0; i < 32; i++) { 
    for (j = 0; j < 16; j++) { 
      WHACKDisp[i][j] = 0; 
    } 
  } 
//------------------------------------------------------ 


// ----------------------------------------------------- 

// copied from snake, fitted to WHACK: 
} 

void WhackHoleRender(){ 
  
      // Down erase
      matrix.drawPixel( holeDown.xPos+1, holeDown.yPos ,matrix.Color333(0, 0, 0));
      matrix.drawPixel( holeDown.xPos-2, holeDown.yPos ,matrix.Color333(0, 0, 0));      
 
      // Up erase     
      matrix.drawPixel( holeUp.xPos+1, holeUp.yPos ,matrix.Color333(0, 0, 0));
      matrix.drawPixel( holeUp.xPos-2, holeUp.yPos ,matrix.Color333(0, 0, 0));
 
      // Left erase
      matrix.drawPixel( holeLeft.xPos, holeLeft.yPos+1 ,matrix.Color333(0, 0, 0));
      matrix.drawPixel( holeLeft.xPos, holeLeft.yPos-2 ,matrix.Color333(0, 0, 0));
 
      // Right erase
      matrix.drawPixel( holeRight.xPos, holeRight.yPos+1 ,matrix.Color333(0, 0, 0));
      matrix.drawPixel( holeRight.xPos, holeRight.yPos-2 ,matrix.Color333(0, 0, 0));

      // Down print
      matrix.drawPixel( holeDown.xPos, holeDown.yPos ,matrix.Color333(7, 7, 7)); 
      matrix.drawPixel( holeDown.xPos-1, holeDown.yPos ,matrix.Color333(7, 7, 7));
 
      // Up print
      matrix.drawPixel( holeUp.xPos, holeUp.yPos ,matrix.Color333(7, 7, 7)); 
      matrix.drawPixel( holeUp.xPos-1, holeUp.yPos ,matrix.Color333(7, 7, 7)); 


      // Left print
      matrix.drawPixel( holeLeft.xPos, holeLeft.yPos ,matrix.Color333(7, 7, 7)); 
      matrix.drawPixel( holeLeft.xPos, holeLeft.yPos-1 ,matrix.Color333(7, 7, 7)); 

      // Right print
      matrix.drawPixel( holeRight.xPos, holeRight.yPos ,matrix.Color333(7, 7, 7)); 
      matrix.drawPixel( holeRight.xPos, holeRight.yPos-1 ,matrix.Color333(7, 7, 7)); 
}
// ----------------------------------------------------- 



// run the wack game 


 
// ----------------------------------------------------- 

// copied from snake, still need to edit: 

void WhackRun(){
  //read the joystick 
  if (joystickDirection != 0){ 
      if (WaitForLaser == 0){
      selectedside = joystickDirection; 
      }
  } 
    //make sure the game doesnt update every tick because thats way too fast. 
  if(waitCounter == GAMESPEED){ 
    waitCounter = 0; 
  if (WaitForLaser == 0){// so the hole stands still when the laser comes out
    WhackSideSelect(); 
    WhackHoleRender();
     }
    WhackLaser();
  } 
    waitCounter++; 
} 
// ----------------------------------------------------- 
void WhackSideSelect(){ 
  switch(selectedside){ 
   case 1: 
      WhackHorizontalMovement(3); 
      laser.yPos=holeDown.yPos;
    break;
   case 2: 
      WhackVerticalMovement(2); 
      laser.xPos=holeRight.xPos;
    break; 
   case 3: 
      WhackHorizontalMovement(1); 
      laser.yPos=holeUp.yPos;
    break; 
   case 4: 
      WhackVerticalMovement(4); 
      laser.xPos=holeLeft.xPos;
    break; 
  } 
} 
 
// movement of holes (not their sprites), so a 1x1 pixel
void WhackHorizontalMovement(int side){ 
  
    if (side == 1){
// switch direction after reaching last pixel
      if (holeUp.xPos > (WHACKDISPX - 1)){
        holeUp.mode = 1;
      } 
      
 // switch direction after reaching first pixel
      else if (holeUp.xPos < 2){
        holeUp.mode = 0;
      }
      
      // moving the hole
      if (holeUp.mode == 0){
          holeUp.xPos++;
      }

      else if (holeUp.mode == 1){
          holeUp.xPos--;
      }
    } 

    else if (side == 3){ 

 // switch direction after reaching last pixel
      if (holeDown.xPos > (WHACKDISPX - 1)){
        holeDown.mode = 1;
      } 
      
 // switch direction after reaching first pixel
      else if (holeDown.xPos < 2){
        holeDown.mode = 0;
      }

     // moving the hole
      if (holeDown.mode == 0){
          holeDown.xPos++;
      }

      else if (holeDown.mode == 1){

          holeDown.xPos--;

        }
    } 
} 

// movement of holes (not their sprites), so a 1x1 pixel
void WhackVerticalMovement(int side){ 
    if (side == 2){ 

 // switch direction after reaching last pixel
      if (holeRight.yPos > (WHACKDISPY - 1)){
        holeRight.mode = 1;
      } 
      
 // switch direction after reaching first pixel
      else if (holeRight.yPos < 2){
        holeRight.mode = 0;
      }



     // moving the hole
      if (holeRight.mode == 0){
          holeRight.yPos++;
      }

      else if (holeRight.mode == 1){
          holeRight.yPos--;
      }
    } 
    else if (side == 4){ 
 // switch direction after reaching last pixel
      if (holeLeft.yPos > (WHACKDISPY - 1)){
        holeLeft.mode = 1;
      } 
      
 // switch direction after reaching first pixel
      else if (holeLeft.yPos < 2){
        holeLeft.mode = 0;
      }
     // moving the hole
      if (holeLeft.mode == 0){
          holeLeft.yPos++;
      }

      else if (holeLeft.mode == 1){
          holeLeft.yPos--;

      }
    } 
    
// --------------------------------------
// laser buisiness
// --------------------------------------

} 

void WhackLaser(){

if (actionPressed == 1){

  WaitForLaser =1;
}

if (WaitForLaser == 1){


  switch(selectedside){

   case 1: 

    WhackLaserDown();
     
    break; 

   case 2: 

    WhackLaserRight();
      
    break; 

   case 3: 

    WhackLaserUp();
      
    break; 

   case 4: 

    WhackLaserLeft();
    
    break; 
    }
  }
}




void WhackLaserUp(){// the coordinates and rendering of the laser in and out of hole movement

  laser.xPos = holeUp.xPos;// laser will come out of hole

  if (laser.mode == 0){// laser comes out when on screen
  laser.yPos++;
  }

  else if (laser.mode == 1){// laser retracts when border is touched
  laser.yPos--;
  // temporary double speed while retracting laser
  LaserRender(1);
  laser.yPos--;
  }

  LaserRender(1);

  if (laser.yPos > WHACKDISPY-1){
  laser.mode=1;//laser will retract
  }

  else if (laser.yPos < 1){// laser fully retracted
  laser.mode=0;// laser is done: 
  WaitForLaser=0;// hole moves on
  laser.yPos=holeUp.yPos;
  }

}

void WhackLaserDown(){

  laser.xPos = holeDown.xPos;// laser will come out of hole
  
  if (laser.mode == 0){// laser comes out when on screen
  laser.yPos--;
  }

  else if (laser.mode == 1){// laser retracts when border is touched
  laser.yPos++;
  // temporary double speed while retracting laser
  LaserRender(3);
  laser.yPos++;
  }

  LaserRender(3);

  if (laser.yPos == laser.foodY & laser.xPos == laser.foodX ){
  laser.mode=1;//laser will retract
  WhackScore++;
  eraseTarget();
  }

  else if (laser.yPos == laser.foodY & laser.xPos == laser.foodX+1 ){
  laser.mode=1;//laser will retract
  WhackScore++;
  eraseTarget();
  }

  else if (laser.yPos == laser.foodY & laser.xPos == laser.foodX+2 ){
  laser.mode=1;//laser will retract
  WhackScore++;
  eraseTarget();
  }

  else if (laser.yPos > WHACKDISPY-1){// laser fully retracted
  laser.mode=0;// laser is done: 
  WaitForLaser=0;// hole moves on
  laser.yPos=holeDown.yPos;
  }
 
  else if (laser.yPos < 1){
  //laser.yPos=4;
        //WhackSetup();
    waitCounter = -1;
    gameState = 4;
    noSpam = 0;
    if(actionPressed == 1){ // make sure the end screen is not skipped
    noSpam = 1;
    }
  } 
}

void WhackLaserLeft(){

  laser.yPos = holeLeft.yPos;// laser will come out of hole

  if (laser.mode == 0){// laser comes out when on screen
  laser.xPos++;
  }
  else if (laser.mode == 1){// laser retracts when border is touched
  laser.xPos--;
  // temporary double speed while retracting laser
  LaserRender(2);
  laser.xPos--;
  }

  LaserRender(2);

  if (laser.xPos > WHACKDISPX-1){
  laser.mode=1;//laser will retract
  }

  else if (laser.xPos < 1){// laser fully retracted
  laser.mode=0;// laser is done: 
  WaitForLaser=0;// hole moves on
  laser.xPos=holeLeft.xPos;
  }
}

void WhackLaserRight(){

  laser.yPos = holeRight.yPos;// laser will come out of hole
  
  if (laser.mode == 0){// laser comes out when on screen
  laser.xPos--;
  }

  else if (laser.mode == 1){// laser retracts when border is touched
  laser.xPos++;

  // temporary double speed while retracting laser
  LaserRender(4);
  laser.xPos++;
  }
  
  LaserRender(4);

  if (laser.xPos < 1){
  laser.mode=1;//laser will retract
  }

  else if (laser.xPos > WHACKDISPX-1){// laser fully retracted
  laser.mode=0;// laser is done: 
  WaitForLaser=0;// hole moves on
  laser.xPos=holeRight.xPos;
  }
}

void LaserRender(int side){// rendering the laser from the selected position

  switch(side){
   case 1: //up

// draw laser
   matrix.drawPixel( laser.xPos, laser.yPos ,matrix.Color333(7, 7, 7));
   matrix.drawPixel( laser.xPos-1, laser.yPos ,matrix.Color333(7, 7, 7));

// erase laser
   matrix.drawPixel( laser.xPos, laser.yPos+1 ,matrix.Color333(0, 0, 0));
   matrix.drawPixel( laser.xPos-1, laser.yPos+1 ,matrix.Color333(0, 0, 0));
   
    break; 
   case 2: //left

// draw laser
   matrix.drawPixel( laser.xPos, laser.yPos ,matrix.Color333(7, 7, 7));
   matrix.drawPixel( laser.xPos, laser.yPos-1 ,matrix.Color333(7, 7, 7));

// erase laser
   matrix.drawPixel( laser.xPos+1, laser.yPos ,matrix.Color333(0, 0, 0));
   matrix.drawPixel( laser.xPos+1, laser.yPos-1 ,matrix.Color333(0, 0, 0));

    break; 
   case 3: //down
   
// draw laser
   matrix.drawPixel( laser.xPos, laser.yPos ,matrix.Color333(7, 7, 7));
   matrix.drawPixel( laser.xPos-1, laser.yPos ,matrix.Color333(7, 7, 7));

// erase laser
   matrix.drawPixel( laser.xPos, laser.yPos-1 ,matrix.Color333(0, 0, 0));
   matrix.drawPixel( laser.xPos-1, laser.yPos-1 ,matrix.Color333(0, 0, 0));
      
    break; 
   case 4: //right

// draw laser
   matrix.drawPixel( laser.xPos, laser.yPos ,matrix.Color333(7, 7, 7));
   matrix.drawPixel( laser.xPos, laser.yPos-1 ,matrix.Color333(7, 7, 7));

// erase laser
   matrix.drawPixel( laser.xPos-1, laser.yPos ,matrix.Color333(0, 0, 0));
   matrix.drawPixel( laser.xPos-1, laser.yPos-1 ,matrix.Color333(0, 0, 0));
    break; 
   }
}

//-----------------------------
//Respawn the food at a random location. Render it too.
//-----------------------------
   
void respawnTarget(){
  
  laser.foodX = rand()%WHACKDISPX;
  laser.foodY = rand()%WHACKDISPY;
 
  matrix.drawPixel(laser.foodX, laser.foodY, matrix.Color333(7, 0, 0));
  matrix.drawPixel(laser.foodX+1, laser.foodY, matrix.Color333(7, 0, 0));
  matrix.drawPixel(laser.foodX, laser.foodY+1, matrix.Color333(7, 0, 0));
  matrix.drawPixel(laser.foodX+1, laser.foodY+1, matrix.Color333(7, 0, 0));
}

void eraseTarget(){
  matrix.drawPixel(laser.foodX, laser.foodY, matrix.Color333(0, 0, 0));
  matrix.drawPixel(laser.foodX+1, laser.foodY, matrix.Color333(0, 0, 0));
  matrix.drawPixel(laser.foodX, laser.foodY+1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(laser.foodX+1, laser.foodY+1, matrix.Color333(0, 0, 0));
  respawnTarget();
}

void WhackLose(){
if(waitCounter == 0){
  clearScreen();
  matrix.setCursor(1, 0);  // start at top left, with one pixel of spacing
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color888(143, 86, 59));
  matrix.print("Score");
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 8);
  matrix.print(WhackScore);
  waitCounter++;
}

  if(noSpam == 0){
    if(actionPressed == 1){

      gameState = 0;
    }
  }
  if(actionPressed == 0){
      noSpam = 0;
  }



}

/********
   Arduino Pong By Pete Lamonica modified by duboisvb
   updated by James Bruce (http://www.makeuseof.com/tag/author/jbruce
   A simple implementation of Pong on the Arduino using a TV for output.
   To compile, from the library manager (Sketch/Include library/Manage libraries), install: tvout.
   Move the directory, TVoutfonts, to the directory: Documents/Arduino/libraries.
*/
#include <TVout.h>
#include <fontALL.h>

#define WHEEL_ONE_PIN 0 //analog
#define WHEEL_TWO_PIN 1 //analog
#define BUTTON_ONE_PIN 2 //digital to start game
// Not programmed: #define BUTTON_TWO_PIN 3 //digital to reset and go back to main menu

#define PADDLE_HEIGHT 14
#define PADDLE_WIDTH 1

#define RIGHT_PADDLE_X (TV.hres()-4)
#define LEFT_PADDLE_X 2

#define IN_GAMEA 0 //in game state - draw constants of the game box
#define IN_GAMEB 0 //in game state - draw the dynamic part of the game

#define IN_MENU 1 //in menu state
#define GAME_OVER 2 //game over state

#define LEFT_SCORE_X (TV.hres()/2-15)
#define RIGHT_SCORE_X (TV.hres()/2+10)
#define SCORE_Y 4
#define GAME_OVER_X (TV.hres()/2-20)
#define GAME_OVER_Y 12

#define MAX_Y_VELOCITY 6
#define PLAY_TO 6

TVout TV;

int state = IN_MENU;

boolean doDrawMenu = true;
boolean doDrawWin = true;
char volX = 3;
char volY = 3;
unsigned char x, y;

int counter = 0;

int leftPlayerScore = 0;
int rightPlayerScore = 0;

boolean button1Status = false;
int wheelOnePosition = 0;
int wheelTwoPosition = 0;
int rightPaddleY = 0;
int leftPaddleY = 0;
unsigned char ballX = 0;
unsigned char ballY = 0;
char ballVolX = 2;
char ballVolY = 2;

// -----------------------------------------------------------------------
// Start menu

void drawMenu() {
  Serial.println("+ drawMenu");
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.print(6, 5, "Arduino Pong V3");
  TV.select_font(font4x6);
  TV.print(22, 35, "Press Button");
  TV.print(30, 45, "To Start");
}
void drawMenuBall() {
    // TV.delay_frame(3);
    if (x + volX < 1 || x + volX > TV.hres() - 1) volX = -volX;
    if (y + volY < 1 || y + volY > TV.vres() - 1) volY = -volY;
    if (TV.get_pixel(x + volX, y + volY)) {
      TV.set_pixel(x + volX, y + volY, 0);
      if (TV.get_pixel(x + volX, y - volY) == 0) {
        volY = -volY;
      }
      else if (TV.get_pixel(x - volX, y + volY) == 0) {
        volX = -volX;
      }
      else {
        volX = -volX;
        volY = -volY;
      }
    }
    TV.set_pixel(x, y, 0);
    x += volX;
    y += volY;
    TV.set_pixel(x, y, 1);
}

// -----------------------------------------------------------------------
// Game screen

void drawGameBoard() {
  TV.clear_screen();
  for (int i = 1; i < TV.vres() - 4; i += 6) {
    TV.draw_line(TV.hres() / 2, i, TV.hres() / 2, i + 3, 1);
  }
  // had to make box a bit smaller to fit tv
  TV.draw_line(0, 0, 0, 95, 1 ); // left
  TV.draw_line(0, 0, 126, 0, 1 ); // top
  TV.draw_line(126, 0, 126, 95, 1 ); // right
  TV.draw_line(0, 95, 126, 95, 1 ); // bottom
}
void drawPaddles() {
  //draw right paddle
  rightPaddleY = ((wheelOnePosition / 8) * (TV.vres() - PADDLE_HEIGHT)) / 128;
  x = RIGHT_PADDLE_X;
  for (int i = 0; i < PADDLE_WIDTH; i++) {
    TV.draw_line(x + i, rightPaddleY, x + i, rightPaddleY + PADDLE_HEIGHT, 1);
  }
  //draw left paddle
  leftPaddleY = ((wheelTwoPosition / 8) * (TV.vres() - PADDLE_HEIGHT)) / 128;
  x = LEFT_PADDLE_X;
  for (int i = 0; i < PADDLE_WIDTH; i++) {
    TV.draw_line(x + i, leftPaddleY, x + i, leftPaddleY + PADDLE_HEIGHT, 1);
  }
}
void drawScore() {
  TV.select_font(font4x6);
  TV.print_char(LEFT_SCORE_X, SCORE_Y, '0' + leftPlayerScore);
  TV.print_char(RIGHT_SCORE_X, SCORE_Y, '0' + rightPlayerScore);
}

// -----------------------------------------------------------------------
// Game action

void gameBall() {
      ballX += ballVolX;
      ballY += ballVolY;
      // change if hit top or bottom
      if ( ballY <= 1 || ballY >= TV.vres() - 1 ) {
        ballVolY = -ballVolY;
      }
      // test left side for wall hit
      if (ballVolX < 0 && ballX == LEFT_PADDLE_X + PADDLE_WIDTH - 1 && ballY >= leftPaddleY && ballY <= leftPaddleY + PADDLE_HEIGHT) {
        ballVolX = -ballVolX;
        ballVolY += 2 * ((ballY - leftPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
      }
      // test right side for wall hit
      if (ballVolX > 0 && ballX == RIGHT_PADDLE_X && ballY >= rightPaddleY && ballY <= rightPaddleY + PADDLE_HEIGHT) {
        ballVolX = -ballVolX;
        ballVolY += 2 * ((ballY - rightPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
      }
      //limit vertical speed
      if (ballVolY > MAX_Y_VELOCITY) ballVolY = MAX_Y_VELOCITY;
      if (ballVolY < -MAX_Y_VELOCITY) ballVolY = -MAX_Y_VELOCITY;
      // Scoring
      if (ballX <= 1) {
        playerScored(RIGHT);
      }
      if (ballX >= TV.hres() - 1) {
        playerScored(LEFT);
      }
}

void playerScored(byte player) {
  if (player == RIGHT) rightPlayerScore++;
  if (player == LEFT) leftPlayerScore++;
  if (leftPlayerScore == PLAY_TO || rightPlayerScore == PLAY_TO) {
    state = GAME_OVER;
  }
  ballVolX = -ballVolX;
}

// -----------------------------------------------------------------------
void getInputs() {
  wheelOnePosition = analogRead(WHEEL_ONE_PIN);
  wheelTwoPosition = analogRead(WHEEL_TWO_PIN);
  button1Status = (digitalRead(BUTTON_ONE_PIN));
}

void setup()  {
  Serial.begin(9600);
  Serial.println("+++ Setup start.");
  TV.begin(_NTSC);       //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  state = IN_MENU;
  x = 0;
  y = 0;
  ballX = TV.hres() / 2;
  ballY = TV.vres() / 2;
  Serial.println("+ Setup complete.");
}

// -----------------------------------------------------------------------
void loop() {
  getInputs();
  delay(50);
  if (counter == 60) counter = 0; // increment or reset frame counter
  counter++;
  /*
  Serial.print("+ loop counter = ");
  Serial.print(counter);
  Serial.print(": One Two Button: ");
  Serial.print(wheelOnePosition);
  Serial.print(" ");
  Serial.print(wheelTwoPosition);
  Serial.print(" ");
  Serial.println(button1Status);
  */
  if (state == IN_MENU) {
    if (doDrawMenu) {
      drawMenu();
      doDrawMenu = false;
    }
    drawMenuBall();
  }
  if (button1Status) {
    state = IN_GAMEA;
    doDrawWin = true;
  }
  if (state == IN_GAMEA) {
    if (counter % 3 == 0) {
      gameBall(); // // every third loop
    }
    drawGameBoard();
    drawPaddles();
    drawScore();
    TV.set_pixel(ballX, ballY, 2); //draw the ball
    TV.delay_frame(1);
  }
  if (state == GAME_OVER) {
    if (doDrawWin) {
      drawGameBoard();
      drawScore();
      //
      TV.select_font(font8x8);
      TV.print(20, GAME_OVER_Y, "Game over");
      TV.print(20, 35, "Press Button");
      TV.print(20, 45, "For new Game.");
      rightPlayerScore = 0;
      leftPlayerScore = 0;
      //
      doDrawWin = false;
    }
  }
}

// -----------------------------------------------------------------------
// eof

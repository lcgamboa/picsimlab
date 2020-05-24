/*
 * Super Marduino
 * A platform game with Arduino Uno.
 * 
 * Requirements:
 * Nokia 5510 display and three buttons.
 * Additional libraries: Adafruit GFX and Adafruit_PCD8544.
 * 
 * Created by Tobias Beise Ulrich
 * http://tobiasbu.github.io/website 
 * 
 * GitHub:
 * https://github.com/tobiasbu/marduino
 * 
 * 2014-2016.
 * 
 * -------------------------------------
 * LAST UPDATE: 15/06/2016
 * Header information update.
 * 
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>



#include "marduinotypes.h"
#include "tilemap.h"
#include "mariobitmap.h"
#include "levels.h"


Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);
TileMap gameTilemap;

#define DELAY 75 // delay for fixed update interval 
#define P_STILL 0
#define P_MOVE 1
#define P_JUMP 2
#define P_DEAD 3

#define GAME_INTRO 100
#define GAME_TITLE 101
#define GAME_CREDITS 102
#define GAME_LEVEL 103
#define GAME_PLAY 104
#define GAME_GAMEOVER 105

unsigned int gamestate = GAME_INTRO; // game state
long TIMER_PREV = 0; // timer 
unsigned long TIMER_CURRENT = 0;
//mario img
int const pimagew = 16;
int const pimageh = 16;
//marduino settings
int player_state = P_STILL; //player state 
int player_direction = 1;
int last_direction = 1;
int life = 3;
long playertimer = 0;
long playertimer_prev = 0;
//posicao do mario
Vector2f player_position(0,display.height()-pimageh-8);
Vector2f player_position2(0,display.height()-pimageh-8);
Vector2f last_safe_position(0,display.height()-pimageh-8);
//horizontal speed vars
float hspd = 0;
float hspd_speed = 3.0;
//jump vars and gravity control
float grav = 0.6;
float jumpspd = 4.0;
float vspd = 0;
boolean onAir = false;
boolean check_pulo = false;
//animation vars
int frame = 0;
int frameMax = 4;
float frameSpd = 1;
float frameCounter = 0;
boolean animInit = false;
//collision box offset
float pboxoffsetx = 3; 

// camera control
Vector2f camera = {0,0};
Vector2f last_camera = {0,0};
int camera_player_side = 0;


float sign(float x) {

  if (x > 0)
    return 1;
  else if (x < 0)
    return -1;
  
  return 0;
  
};



boolean buttonPressing[] = {false,false,false}; // button is pressing
boolean buttonRelease[] = {false,false,false}; // button is released
boolean buttonPressed[] = {false,false,false}; // button has pressed one time 
boolean buttonPressedCheck[] = {false,false,false};
boolean buttonReleaseCheck[] = {false,false,false};

void inputManager(unsigned int pin0, unsigned int pin1, unsigned int pin2) {

  //input manager in loop || gerenciador de inputs em loop
  
  int p[3];
  p[0] = digitalRead(pin0);//LÃª o pino 5
  p[1] = digitalRead(pin1);//LÃª o pino 6
  p[2] = digitalRead(pin2);//LÃª o pino 7
  
  for (int i = 0; i < 3; i++) {
  
      if (buttonPressed[i]) { // pressed ok
        buttonPressed[i] = false;
        buttonPressedCheck[i] = true;
      }
      
      if (buttonRelease[i]) { // release ok
        buttonRelease[i] = false;
        buttonReleaseCheck[i] = true;
      }
    
    if (p[i] == LOW) { // arduino check
    
      buttonPressing[i] = true; // pressing button
      
      buttonRelease[i] = false; //turn release off
      buttonReleaseCheck[i] = false;     
    
      if (!buttonPressedCheck[i]) 
        buttonPressed[i] = true;
        
    } else {
      //turn off all
      buttonPressing[i] = false;
      buttonPressed[i] = false;
      buttonPressedCheck[i] = false;
      
      if (!buttonReleaseCheck[i]) //released button!
        buttonRelease[i] = true;
    }
   
  }
 
}


boolean intersectionRect(float * rect1, float * rect2) {
  
  if (rect1[0] < rect2[0] + rect2[2] && rect1[0] + rect1[2] > rect2[0]) {
     if (rect1[1] < rect2[1] + rect2[3] && rect1[3] + rect1[1] > rect2[1]) {
      return true;
    }
  }
  return false;

}

void playerCollisionChecker(float hs, float vs) {

  float playerRect[4] = {player_position.x+pboxoffsetx+hs+camera.x, player_position.y+vs,10,16};
  
  for (int i = 0; i < CollisionMap0Size*4; i += 4) {
  
    float rectTest[] = {pgm_read_word_near(&CollisionMap0[i]), pgm_read_word_near(&CollisionMap0[i+1]), pgm_read_word_near(&CollisionMap0[i+2]), pgm_read_word_near(&CollisionMap0[i+3])};
    
        if (intersectionRect(playerRect,rectTest)) {
          if (hs != 0) {
          
              hspd = (int)hspd;
              player_position.x = (int)player_position.x;
              boolean corrector = false;
    
              while (!corrector) {      
                   
                  playerRect[0] = player_position.x+pboxoffsetx+camera.x+sign(hspd);
                  
                  if (!intersectionRect(playerRect,rectTest))  
                    player_position.x += sign(hspd);
                  else 
                    corrector = true;
              }
              
               hspd = 0;
              
              break;
          }
          
          if (vs != 0) {
             
              vspd = (int)vspd;
              player_position.y = (int)player_position.y;
              boolean correctorY = false;
    
              while (!correctorY) {      
                   
                  playerRect[1] = player_position.y+camera.y+sign(vspd);
                  
                  if (!intersectionRect(playerRect,rectTest))  
                    player_position.y += sign(vspd);
                  else 
                    correctorY = true;
              }
              
               vspd = 0;
               check_pulo = false;
              break;
          }
        }
             
  
  }

}

boolean verifyCollision(float * rect) {

    for (int i = 0; i < CollisionMap0Size*4; i += 4) {
  
      float rectTest[4] = {pgm_read_word_near(&CollisionMap0[i]), pgm_read_word_near(&CollisionMap0[i+1]), pgm_read_word_near(&CollisionMap0[i+2]), pgm_read_word_near(&CollisionMap0[i+3])};
    
        if (intersectionRect(rect,rectTest)) {
               return true;
        }
        
    }
    
    return false;
}

void playerLogic(boolean move_esq, boolean move_dir, boolean jump) {

  //HORIZONTAL MOVEMENT

  if (player_state != P_DEAD) {
  
      hspd = 0;
      boolean moving = false;
     
      if (move_esq == true && move_dir == false) {
          player_direction = 1;
          moving = true;
      } else if (move_dir == true && move_esq == false) {
          player_direction = -1;
          moving = true;
      }
      
      if (moving)
          hspd = hspd_speed * (float)player_direction;
    
      playerCollisionChecker(hspd,0);
      
      player_position2.x += hspd;
      
       if (camera_player_side == -1) {
          if (player_position.x > (display.width()/2)-10)
            camera_player_side = 0;
          else
            player_position.x += hspd;
    
          // screen limit <
          if (player_position.x < 0) {
            player_position.x = 0;
            hspd = 0;
          }  
    
       } else if (camera_player_side == 1) {
          if (player_position.x < (display.width()/2)-10)
            camera_player_side = 0;
          else
            player_position.x += hspd;
            
          // screen limit >
          if (player_position.x > display.width()) {
            gamestate = GAME_TITLE;
            /*player_position.x = display.width()-pimagew;
            hspd = 0;*/
          }
        }
        
        if (camera_player_side == 0) {
          //verify for limit side >
          if (camera.x >= 0 && camera.x <= gameTilemap.getMapWidth()*8-display.width()) {
            camera.x += hspd;
          } if (camera.x > gameTilemap.getMapWidth()*8-display.width()) {
            camera_player_side = 1;
            camera.x = gameTilemap.getMapWidth()*8-display.width();
            player_position.x += hspd;
          } if (camera.x < 0) {
            camera_player_side = -1;
            player_position.x += hspd; 
            camera.x = 0;  
          }
        } 
  
      //VERTICAL MOVEMENT
    
      //gravidade (esta no ar)
     float playerRect[4] = { camera.x+player_position.x+pboxoffsetx,camera.y+player_position.y+1, 10, 16 };
      
      if (!verifyCollision(playerRect)) {
        vspd += grav;
        check_pulo = true;
        onAir = true;
      } else {
        onAir = false;
      }
    
      // checa colisao com chao
      if (vspd != 0) {
          playerCollisionChecker(0,vspd);
      }
      
      if (jump == true && check_pulo == false) {
        vspd -= jumpspd;
        check_pulo = true;  
      }
      
      player_position.y += vspd;
      player_position2.y += vspd;
      
      if (vspd == 0 && !onAir) {
          last_direction = player_direction;
          last_camera.x = camera.x;
          last_camera.y = player_position2.y;
          last_safe_position.x = player_position.x;
          last_safe_position.y = player_position.y;
      }
      
      //fall

      if (player_position.y+camera.y > gameTilemap.getMapHeight()*gameTilemap.getTileHeight()) {
          if (player_state != P_DEAD)
              player_state = P_DEAD;
      }
    
    if (player_state != P_DEAD) {
       if (hspd == 0 && vspd == 0) {
        if (player_state != P_STILL) {
          player_state = P_STILL;
          animInit = true;
        }
      } else  if (hspd != 0 && vspd == 0) {
         if (player_state != P_MOVE) {
          player_state = P_MOVE;
          animInit = true;
        }
      } 
      
      if (vspd != 0) {
        if (player_state != P_JUMP) {
          player_state = P_JUMP;
          animInit = true;
         }   
      }
    
      
     }
  } else {
  
   playertimer++; // every DELAY | 1 = 75 millis
    
    if (playertimer > 1000/75) { // 1 sec
    
          playertimer = 0;
          life--;
          
          if (life > 0) {  
            player_position.x = (int)((last_safe_position.x/8)*8);/*+(8*-last_direction);*/
            player_position.y = (int)((last_safe_position.y/8)*8);
            player_position2.x = player_position.x;
            camera.x = (int)((last_camera.x/8)*8);/*+(8*last_direction);*//*-(display.width()/2)-8;*/
            camera.y = 0;
            player_state = P_STILL;
            vspd = 0;
            hspd = 0;
            check_pulo = false;
        } else {
            gamestate = GAME_GAMEOVER;
        }
   }
  }
  
}

void playerDraw() {

  //BMP * frameAtual;
  const unsigned char * frameAtual;
  
  //animation setup
  if (animInit) {
  
    frame = 0;
    frameCounter = 0;   
    
    if (player_state == P_STILL || player_state == P_JUMP) {
      frameSpd = 0.;
      frameMax = 1; 
    } else if (player_state == P_MOVE) {
      frameSpd = 0.8;
      frameMax = 4;
    }
  
     animInit = false;
  }
  
  //controlador de tempo de frames
  if (frameMax > 1) {
    frameCounter += frameSpd*DELAY;
    
    if (frameCounter > DELAY) {
      frame++;
      frameCounter = 0;
      if (frame > frameMax-1) {
        frame = 0;
      }
    }
  }
  
  //frames manager 
  
  if (player_state == P_STILL) {
   if (player_direction == 1) 
    frameAtual = frameMario0;
   else
    frameAtual = frameMario4;
  } else if (player_state == P_JUMP) {
    if (player_direction == 1) 
      frameAtual = frameMario3;
    else
      frameAtual = frameMario7;
  } else if (player_state == P_MOVE) {
  
    if (player_direction == 1) {
      switch (frame) {
        case 0: frameAtual = frameMario1; break;
        case 1: frameAtual = frameMario0; break;
        case 2: frameAtual = frameMario2; break;
        case 3: frameAtual = frameMario0; break;
      }
    } else {
      switch (frame) {
        case 0: frameAtual = frameMario5; break;
        case 1: frameAtual = frameMario4; break;
        case 2: frameAtual = frameMario6; break;
        case 3: frameAtual = frameMario4; break;
      }
    }
  
  }
  
  
  display.drawBitmap(player_position.x, player_position.y, frameAtual, pimagew,pimageh,1);

  // debug collision box
  //display.fillRect(player_position.x+pboxoffsetx, player_position.y, 10, 16, BLACK);

}

void drawGui() {

  String stringint = String(life,DEC);
  String string = String("x" + stringint);
  
  display.setTextSize(1);
  display.setTextColor(BLACK,WHITE);
  display.setCursor(0,0);
  display.print(string);

}

void sceneIntro() {
  
  display.clearDisplay(); //clean screen 
  
  float sizex = 13*6;

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor((display.width()-sizex)/2,display.height()/2-12);
  display.print("Regi and Tobi");
  sizex = 8*6;
  display.setCursor((display.width()-sizex)/2,display.height()/2-4);
  display.print("presents");
  display.display();
  
  TIMER_CURRENT = millis();
    
   if (TIMER_CURRENT - TIMER_PREV > 2500) { 
      
        TIMER_PREV = TIMER_CURRENT;
        display.clearDisplay();
        gamestate = GAME_TITLE;
   }
}

void sceneTitle() {

    // loop title screen
    float sizex = 14*6;

   int select = 0; 
   int selectMax = 3;
   unsigned int triggerSelect = 0;
   unsigned int counter = 0;
   unsigned int counterMax = 0;
  
   
  while (gamestate == GAME_TITLE) {

   TIMER_CURRENT = millis();
    
   if (TIMER_CURRENT - TIMER_PREV > DELAY) { 
      
        TIMER_PREV = TIMER_CURRENT;
    
        inputManager(5,6,7);
        
        if (triggerSelect == 0) {
            if (buttonPressed[2]) {
              select++;
              if (select >= selectMax) 
                select = 0;
                
            } else if (buttonPressed[0]) {
                  select--;
              if (select < 0)
                select = selectMax-1;    
            }
           
           
           if (buttonPressed[1]) {
            triggerSelect = 1;
            counterMax = 3;
          }
         } 
        
        
       String str = "test";
      
        switch (select) {
          case 0: {str = "< START GAME >"; sizex = 14*6; break;};
          case 1: {str = "< CREDITS >"; sizex = 11*6; break;};
          case 2: {str = "< EXIT GAME >"; sizex = 13*6; break;};  
        }
        
        display.clearDisplay();
        display.drawBitmap(0, 0, gameLogo, 84, 24,1);
        display.setTextSize(1);
        
        if (triggerSelect == 1)
          display.setTextColor(WHITE,BLACK);
        else
          display.setTextColor(BLACK);
        
        display.setCursor((display.width()-sizex)/2,24+8);
        display.print(str);
        
        display.display();
       
      
      
       if (triggerSelect != 0) {
        if (counter > counterMax) {
            counter = 0;
            triggerSelect++;
        } else {
            counter++;
        }
      }
      }
            
        if (triggerSelect == 3) {
          
          if (select == 0) {
            gamestate = GAME_LEVEL;
          } else if (select == 1) {
            gamestate = GAME_CREDITS;
          } else {
            triggerSelect = 0;
          }   
        }
      
       
   }
   
  }



void sceneCredit() {

  float sizex = 8*6;
  int page = 0;
  
  while (gamestate == GAME_CREDITS) {
  
    TIMER_CURRENT = millis();
    
      if (TIMER_CURRENT - TIMER_PREV > DELAY) { 
      
        TIMER_PREV = TIMER_CURRENT;
     
          inputManager(5,6,7);
        
        if (buttonPressed[2] || buttonPressed[1] || buttonPressed[0]) {
          page++;
        }
         sizex = 8*6;
      
         display.clearDisplay();
    
          display.setTextSize(1);
          display.setTextColor(BLACK);
          display.setCursor((display.width()-sizex)/2,0);
          if (page < 2) 
            display.print("CREDITS:");
          display.setCursor(0,8);
          if (page == 0) {
            display.println("Created by:");
            display.println("Tobias Ulrich and Reginaldo da Silva.");
          } else if (page == 1) {
             display.println("Written in C  with Arduino  Uno.");
           sizex = 10*6;
           display.setCursor((display.width()-sizex)/2,32);
            display.print("Dec, 2014.");
          } else {
            gamestate = GAME_TITLE;
          } 
          
    
         display.display();
      }
        
  }
}

void sceneLevel() {

  float sizex = 7*6;
  
  while (gamestate == GAME_LEVEL) {
    
         display.clearDisplay();
   
          display.fillRect(0, 0, display.width(), display.height(), BLACK);
          display.setTextSize(1);
          display.setTextColor(WHITE,BLACK);
          display.setCursor((display.width()-sizex)/2,(display.height()/2)-3);
          display.print("LEVEL 1");
                     
          
    
         display.display();
         
    TIMER_CURRENT = millis();
    
      if (TIMER_CURRENT - TIMER_PREV > 2250) { 
      
        TIMER_PREV = TIMER_CURRENT;
        gamestate = GAME_PLAY;
      }
        
  }
}

void sceneGameOver() {

  float sizex = 9*6;
  
  while (gamestate == GAME_GAMEOVER) {
    
         display.clearDisplay();
   
          display.fillRect(0, 0, display.width(), display.height(), BLACK);
          display.setTextSize(1);
          display.setTextColor(WHITE,BLACK);
          display.setCursor((display.width()-sizex)/2,(display.height()/2)-3);
          display.print("GAME OVER");
                     
          
    
         display.display();
         
    TIMER_CURRENT = millis();
    
      if (TIMER_CURRENT - TIMER_PREV > 3000) { 
      
        TIMER_PREV = TIMER_CURRENT;
        gamestate = GAME_TITLE;
      }
        
  }
}

void resetGame() {

  player_state = P_STILL; 
  life = 3;
  player_direction = 1;
  player_position.x = 0;
  player_position.y = display.height()-pimageh-8;
  vspd = 0;
  hspd = 0;


  camera.x = 0;
  camera.y = 0;
  camera_player_side = -1;
  check_pulo = false;

}

void sceneGame() {

  resetGame();
  
  
  while (gamestate == GAME_PLAY) {
  
    TIMER_CURRENT = millis();
    
      if(TIMER_CURRENT - TIMER_PREV > DELAY) { // game loop
      
        TIMER_PREV = TIMER_CURRENT;
      
        inputManager(5,6,7); // 5 = 0
        
        if (buttonPressing[2] && buttonPressing[1] && buttonPressing[0]) 
            gamestate = GAME_TITLE;
        
        
        playerLogic(buttonPressing[2],buttonPressing[0],buttonPressed[1]);
      
        display.clearDisplay(); 
        
        gameTilemap.drawMap(camera.x,camera.y);
        playerDraw();
        drawGui();
        
        display.display();
    }

  }
  
}

void setup()   {

  Serial.begin(9600);

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
  
  //song - DOSENT WORK - DO NOT USE
  //pinMode(3, OUTPUT);//buzzer
  //pinMode(13, OUTPUT);//led indicator when singing a note
  
  // inputs
  pinMode(7, INPUT);//Define o pino 7 como entrada
  digitalWrite(7, HIGH);//Ativa o resistor de pull-up da porta 7
  pinMode(6, INPUT);//Define o pino 7 como entrada
  digitalWrite(6, HIGH);//Ativa o resistor de pull-up da porta 7
  pinMode(5, INPUT);//Define o pino 7 como entrada
  digitalWrite(5, HIGH);//Ativa o resistor de pull-up da porta 7

  gameTilemap.setMapSize(100,6);
  gameTilemap.setDisplayPointer(&display);
  gameTilemap.setTileMap(TileMap0);

  
};


void loop() {
  
   switch (gamestate) { // "STATE MACHINE MANAGER"
  
    case GAME_INTRO: {sceneIntro(); break;}
    case GAME_TITLE: {sceneTitle(); break;}
    case GAME_CREDITS: {sceneCredit(); break;}
    case GAME_LEVEL: {sceneLevel(); break;}
    case GAME_PLAY: {sceneGame(); break;}
    case GAME_GAMEOVER: {sceneGameOver(); break;}
  } 

  
  
}



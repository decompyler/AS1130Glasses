// Graphical pattern functions for glasses

// Global variables
// Messy at the moment
float incrval = 0;
float incr = 0.3;
byte incdir = 1;

float offset  = 0;
float plasIncr = -1;
float plasVector = 0;


// Draw a sine wave on the bit array
// Speeds up and slows down in a cycle
void sines() {
  
  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }

  for (int i = 0; i < 24; i++) {
    GlassesBits[i][0] = 3 << (int)(sin(i/2.0+incrval)*3.5+3.5);
  }
  writeBitFrame(0,0);

  incrval += incr;
  if (incdir == 1 ) {
    incr += 0.001;
  } 
  else {
    incr-= 0.001;
  }

  if (incr > 0.5) incdir = 0;
  if (incr < 0.1) incdir = 1;
  delay(5);

}


// Draw a circular sine plasma
int plasoffset = 0;
void Plasma() {

  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }
  
  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      byte brightness = qsine(sqrt((x-11.5)*(x-11.5) + (y-3.5)*(y-3.5))*60 + plasoffset);

      GlassesPWM[x][y] = pgm_read_byte(&cie[brightness]);
    }
  }

  writePWMFrame(0);
  plasoffset += 15;
  if (plasoffset > 359) plasoffset -= 359;

}

#define fadeIncrement 0.9
void fadeAllPWM() {
  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] *= fadeIncrement;
    }
  }
}



// Initialize / load message string
byte currentCharColumn = 0;
int currentMessageChar = 0;
void initMessage(byte message) {
  currentCharColumn = 0;
  currentMessageChar = 0;
  selectFlashString(message);
  loadCharBuffer(loadStringChar(message, currentMessageChar));
}

// Draw message scrolling across the two arrays
// SCROLL1X is normal scrolling
// SCROLL2X is page-flipping scroll that simulates double horizontal resolution using persistence of vision
#define SCROLL1X 0
#define SCROLL2X 1
void scrollMessage(byte message, byte mode) {
  
  if (!patternInit) {
    switchDrawType(0,0);
    initMessage(message);
    patternInit = true;
  }

  
  if ((currentCharColumn % 2 == 0) || mode != SCROLL2X) {
    scrollBits(1,0);
  } else {
    scrollBits(1,1);
  }
    
  if ((currentCharColumn % 2 == 1) || mode != SCROLL2X) {
    GlassesBits[23][0] = charBuffer[currentCharColumn];
    writeBitFrame(0,0);
  } else {
    GlassesBits[23][1] = charBuffer[currentCharColumn];
    writeBitFrame(0,1);
  }
  
  currentCharColumn++;
  if (currentCharColumn > 7) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) {
      currentMessageChar = 0;
      nextChar = loadStringChar(message, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }
  
  if (mode != SCROLL2X) {
    delay(30);
  } else {
    delay(10);
  }
  
}

#define rainDelay 20
int rainAction = 0;
void sideRain(byte dir) {

  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }
  
  if (rainAction++ > rainDelay) {
    rainAction = 0;
  
    byte tempRain = 0;
    
    tempRain = (1 << random(0,8)) | (1 << random(0,8));
    
    if (dir == 0) {
      scrollBits(0,0);
      GlassesBits[0][0] = tempRain;
    } else {
      scrollBits(1,0);
      GlassesBits[23][0] = tempRain;
    }
    
    writeBitFrame(0,0);
  }

}

void rain() {

  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }
  
  if (rainAction++ > rainDelay) {
    rainAction = 0;
    
    for (int i = 0; i < 24; i++) {
      GlassesBits[i][0] <<= 1;
    }
    
    GlassesBits[random(0,24)][0] |= 1;
    GlassesBits[random(0,24)][0] |= 1;
    
    writeBitFrame(0,0);
  
  }
}

typedef struct Stars {
  float xIncr;
  float yIncr;
  float xPos;
  float yPos;
};

Stars stars[10];

void starField() {
  
  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  
  fadeAllPWM();
  for (int i = 0; i < 10; i++) {
    if (abs(stars[i].xIncr) < 0.02 || abs(stars[i].yIncr) < 0.02) {
      stars[i].xPos = 11.5;
      stars[i].yPos = 3.5;
      stars[i].xIncr = random(0,200)/100.0 - 1.0;
      stars[i].yIncr = random(0,200)/200.0 - 0.5;
    }
    
    stars[i].xPos += stars[i].xIncr;
    stars[i].yPos += stars[i].yIncr;
    
    int xPos = (int)stars[i].xPos;
    int yPos = (int)stars[i].yPos;
    if (xPos < 0 || xPos > 23 || yPos < 0 || yPos > 7) {
      stars[i].xIncr = 0;
      stars[i].yIncr = 0;
    } else {
    GlassesPWM[xPos][yPos] = 255;
    }
  }
  writePWMFrame(0);

}

byte blinkAction = 0;
#define blinkCount 50
void fullOn() {
    
  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  
  if (blinkAction++ > blinkCount) {
  
     blinkAction = 0;
  }
   
  if(blinkAction > blinkCount/2) { 
     for (int x = 0; x < 24; x++) {
       for (int y = 0; y < 8; y++) {
         GlassesPWM[x][y] = 255;
       }
     }
  }
   else {
     for (int x = 0; x < 24; x++) {
       for (int y = 0; y < 8; y++) {
         GlassesPWM[x][y] = 0;
       }
     }  
   }
   
   writePWMFrame(0);
}

int slantPos = 23;
byte slantAction = 0;
#define slantCount 3
void slantBars() {

  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  
  if (slantAction++ > slantCount) {
    slantAction = 0;
  
   for (int x = 0; x < 24; x++) {
     for (int y = 0; y < 8; y++) {
       GlassesPWM[x][y] = pgm_read_byte(&cie[(((x+y+(int)slantPos) % 8)*32)]);
     }
   }
   
   slantPos--;
   if (slantPos < 0) slantPos = 23;
   
   writePWMFrame(0);
  }
}

void sparkles() {

  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  fadeAllPWM();
  for (int i = 0; i < 5; i++) GlassesPWM[random(0,24)][random(0,8)] = 255;
  writePWMFrame(0);
}

int riderCount = 0;
int riderPos = 0;
void rider() {

  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  fadeAllPWM();
  if (riderCount++ > 5) {
    riderCount = 0;
    
    int tpos;
    if (riderPos < 8) {
      tpos = riderPos;
    } else if (riderPos < 12) {
      tpos = -1;
    } else if (riderPos < 20) {
      tpos = 19 - riderPos;
    } else if (riderPos <= 40) {
      tpos = -1;
    } else if (riderPos > 40) {
      riderPos = 0;
    }
    
    for (int x = tpos*3; x < (tpos*3 + 3); x++) {
      for (int y = 0; y < 8; y++) {
        GlassesPWM[x][y] = pgm_read_byte(&cie[255*(tpos != -1)]);
      }
    }
    
    riderPos++;
    writePWMFrame(0);
  }
      
}


// Simply grab a character from the font and put it in the 8x8 section of both sides of the glasses
void displayChar(int character) {

  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }
  
  loadCharBuffer(character);
  
  for (int i = 0; i < 8; i++) {
    GlassesBits[i+1][0] = charBuffer[i];
    GlassesBits[i+15][0] = charBuffer[i];
  }
  
  writeBitFrame(0,0);
  
}

// Draw various emoticon style faces
int emotecounter = 0;
void emote() {
  
  byte currentEmote = 0;
  if (emotecounter < 1) {
    emotecounter = random(200,5000);
  
    currentEmote = random(0,6);

    switch(currentEmote) {
      
      case 0:
      loadCharBuffer('X');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('X');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
      
      case 1:
      loadCharBuffer('?');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('?');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
      
      case 2:
      loadCharBuffer('O');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('o');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
      
      case 3:
      loadCharBuffer('>');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('<');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
      
      case 4:
      loadCharBuffer('o');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('O');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
      
      case 5:
      loadCharBuffer('^');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+2][0] = charBuffer[i];
      }
      
      loadCharBuffer('^');
      for (int i = 0; i < 8; i++) {
        GlassesBits[i+15][0] = charBuffer[i];
      }
      break;
    }
    

  }
    writeBitFrame(0,0);
  emotecounter--;

}




int fireAction = 0;
int fireRandom = 0;
byte lineBuffer[24] = {0};
void fire() {

  if (!patternInit) {
    switchDrawType(0,1);
    patternInit = true;
  }

  
  if (fireAction++ > 2) {
    fireAction = 0;
    int x;
    
    //if (fireRandom++ > 1) {
      //fireRandom = 0;
      for (x = 0; x < 24; x++) {
        GlassesPWM[x][8] = (random(0,4) == 1)*255;
      }
    //}
    
    for (int y = 1; y < 9 ; y++) {
      for (x = 0; x < 24; x++) lineBuffer[x] = GlassesPWM[x][y];
      for (x = 0; x < 24; x++) {
        int tempBright = (lineBuffer[(x-1) % 24] + lineBuffer[(x+1) % 24] + lineBuffer[x] + GlassesPWM[x][(y+1) % 9])/3.7-10;
        if (tempBright < 0) tempBright = 0;
        if (tempBright > 255) tempBright = 255;
        GlassesPWM[x][y-1] = tempBright;
      }
    }
    
    writePWMFrame(0); 
  }
  
}

// Awww!
byte currentHeartFrame = 0;
byte heartLoopCount = 0;
void beatingHearts() {
  
  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }
  
  heartLoopCount++;
  if (heartLoopCount > 50) {
    heartLoopCount = 0;
    
    if (currentHeartFrame < 3) {
      loadGraphicsFrame(currentHeartFrame);
    } else {
      loadGraphicsFrame(5 - currentHeartFrame);
    }
    
    currentHeartFrame++;
    if (currentHeartFrame > 5) currentHeartFrame = 0;
    
    writeBitFrame(0,0);
  }
}

byte currentEyesFrame = 0;
byte eyesLoopCount = 0;
void googlyEyes() {
  
  if (!patternInit) {
    switchDrawType(0,0);
    patternInit = true;
  }
  
  eyesLoopCount++;
  if (eyesLoopCount > 50) {
    eyesLoopCount = 0;
    
    if (currentEyesFrame < 3) {
      loadGraphicsFrame(currentEyesFrame);
    } else {
      loadGraphicsFrame(5 - currentEyesFrame);
    }
    
    currentEyesFrame++;
    if (currentEyesFrame > 5) currentEyesFrame = 0;
    
    writeBitFrame(0,0);
  }
}


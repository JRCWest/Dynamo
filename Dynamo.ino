
//  "HoldOut" - A game by jwest.
//	Inspired by the game "Chicanery", by Anna Anthropy.
//  JJ

#define HEARTBEAT_INTERVAL 1700
Timer heartbeatTimer;

#define DEATH_INTERVAL 300
Timer deathTimer;

enum gameStates {READY, PLAY, DEATH};
int gameState = PLAY;

byte currentHue = 0;

// Added variables by JWEST
byte prevHue = 0;
//byte secondaryHue = 0;
bool skipToReady = false;

void setup() {
  randomize();
  newColor();
  heartbeatTimer.set(HEARTBEAT_INTERVAL);
  skipToReady = true;
}

void loop() {
  switch (gameState) {
    case READY:
      readyLoop();
      readyDisplay();
      break;
    case PLAY:
      playLoop();
      playDisplay();
      break;
    case DEATH:
      deathLoop();
      deathDisplay();
      break;
  }

  //dump button presses
  buttonPressed();
  buttonSingleClicked();
  buttonDoubleClicked();
}

void readyLoop() {
  if (buttonPressed()) {
    heartbeatTimer.set(HEARTBEAT_INTERVAL);
    gameState = PLAY;
  }
}

void playLoop() {
  if (buttonPressed()) {
    heartbeatTimer.set(HEARTBEAT_INTERVAL);
  }

  if (heartbeatTimer.isExpired()) {
    if (skipToReady)
    {
	  gameState = READY;
	  skipToReady = false;
    }
	else
	{
      gameState = DEATH;
      deathTimer.set(DEATH_INTERVAL);
	}
  }
}

void deathLoop() {
  if (buttonDoubleClicked()) {      
    newColor();  
  }
  if (buttonLongPressed()) {
    gameState = READY;
  }
}

#define READY_PULSE_INTERVAL 1000
#define READY_PULSE_FRAMES 10
// #define PULSE_BRIGHTNESS 100
#define PULSE_BRIGHTNESS 75

void readyDisplay() {
  byte currentFrame = (millis() % READY_PULSE_INTERVAL) / (READY_PULSE_INTERVAL / READY_PULSE_FRAMES);
  byte frameProgress = map((millis() % READY_PULSE_INTERVAL) % (READY_PULSE_INTERVAL / READY_PULSE_FRAMES), 0, (READY_PULSE_INTERVAL / READY_PULSE_FRAMES), 0, PULSE_BRIGHTNESS);

  setColor(OFF);
  //setColor(dim(WHITE, frameProgress));
  FOREACH_FACE(f) {
    if (currentFrame == (f - 1) ) {
		setColorOnFace(makeColorHSB(currentHue, 255, frameProgress), f);
      //setColorOnFace(makeColorHSB(currentHue, 110, frameProgress), f);
    } else if (currentFrame == f) {
      setColorOnFace(makeColorHSB(currentHue, 255, PULSE_BRIGHTNESS), f);
	  //setColorOnFace(makeColorHSB(currentHue, 110, PULSE_BRIGHTNESS), f);
    } else if (currentFrame == (f + 1)) {
      setColorOnFace(makeColorHSB(currentHue, 255, PULSE_BRIGHTNESS - frameProgress), f);
	  //setColorOnFace(makeColorHSB(currentHue, 110, PULSE_BRIGHTNESS - frameProgress), f);
    }
  }

  setColorOnFace(makeColorHSB(currentHue, 255, 255), 0);
}

void newColor() {
  currentHue = random(50) * 5;
  
  // Create Analogous color for ready cycle?
  //secondaryHue = map(currentHue, 0, 255, 0, 360) + 30;
  
  // Retry randomization
  //if (currentHue == prevHue) { newColor(); }
  //else { prevHue = currentHue; }
}

void playDisplay() {
  byte heartbeatProgress = map(heartbeatTimer.getRemaining(), 0, HEARTBEAT_INTERVAL, 0, 255);
  //so we need to animate each "group" of faces slightly differently

  //face 0 does its own thing
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, 255), 0);

  //faces 1 and 5 fade the slowest
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, heartbeatProgress), 1);
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, heartbeatProgress), 5);

  //face 3 fades the fastest
  byte quickProgress = max((heartbeatProgress * 2) - 255, 0);
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, quickProgress), 3);

  //faces 2 and 4 fade the next fastest
  byte midProgress = (heartbeatProgress + quickProgress) / 2;
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, midProgress), 2);
  setColorOnFace(makeColorHSB(currentHue, 255 - heartbeatProgress, midProgress), 4);
}

void deathDisplay() {

  FOREACH_FACE(f) {
    byte deathProgress = map(deathTimer.getRemaining(), 0, DEATH_INTERVAL, 0, 255);
    byte currentBrightness = max(deathProgress, random(50));

    if (f > 0) { //regular faces
      setColorOnFace(makeColorHSB(currentHue, 255, currentBrightness), f);
    } else {//face 0
      byte filamentBrightness = max(currentBrightness, 125);
	  //byte filamentBrightness = max(currentBrightness, 100);
	  //byte filamentBrightness = 100;
      setColorOnFace(makeColorHSB(currentHue, 255, filamentBrightness), f);
    }

  }
}

#include <avr/sleep.h>

const int g = 1;
const int r = 4;
const int y = 2;
const int b = 3;


// blue - 329.628
// yellow - 277.183
// red - 220
// green 164.814

uint8_t seq[32];

uint8_t pins[] = {g, r, y, b};
//uint16_t freq[] = {351, 261, 212, 175};   // original
//uint16_t freq[] = {331, 276, 220, 163}; // real

uint16_t freq[] = {139, 186, 234, 278}; // real

uint8_t remap[] = {1, 3, 2, 0};//{0, 2, 3, 1};



enum {
  STATE__START = 0,
  STATE__PLAY,
  STATE__PRESS,
  STATE__RELEASE,
  STATE__FAIL,
  STATE__VICTORY,
  STATE__SLEEP
};



uint8_t btnState[4];
uint8_t lastBtnState[4];
uint32_t lastDebounceTime[4];
uint32_t debounceDelay = 50;

uint8_t btnPress[4];
uint8_t btnRelease[4];

uint8_t test = 0;

uint8_t skill = 0;


void processButtons()
{
  uint8_t i;
  
  for(i=0; i<4; i++) {
    uint8_t r = digitalRead(pins[i]);

    if(r != lastBtnState[i])
      lastDebounceTime[i] = millis();

    if((millis() - lastDebounceTime[i]) > debounceDelay) {

      if(btnState[i] == 1 && r == 0)
        btnPress[i] = 1;
      else if(btnState[i] == 0 && r == 1)
        btnRelease[i] = 1;
        
      btnState[i] = r;
    }

    lastBtnState[i] = r;
  }
}


void playIntro()
{
  uint8_t i;
  for(i=0; i<4; i++) {
    uint8_t x = remap[i];
    uint8_t pin = pins[x];
    digitalWrite(pin, 0);
    pinMode(pin, OUTPUT);
    tone(0, freq[x]);
    delay(50);
    pinMode(pin, INPUT_PULLUP);
    noTone(0);
    digitalWrite(0, 1);
    delay(10);
  }
}

void playVictory()
{
  uint8_t i,j;
  for(j=0; j<8; j++) {
    for(i=0; i<4; i++) {
      uint8_t pin = pins[i];
      digitalWrite(pin, 0);
      pinMode(pin, OUTPUT);
      tone(0, freq[i]);
      delay(50);
      pinMode(pin, INPUT_PULLUP);
      noTone(0);
      digitalWrite(0, 1);
      delay(10);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(0, OUTPUT);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}


void Button_callback(void)
{
  uint8_t i;
  for(i=0; i<4; i++) {
    if(digitalRead(pins[i]) == 0) {
      skill = i;
      break;
    }
  }
}


uint8_t level = 1;



uint8_t state = STATE__START;
uint8_t idx = 0;
uint8_t input = 0;


uint32_t last_press = 0;


void loop() {
  uint8_t i;

  processButtons();

  

  switch(state) {
    case STATE__START: {

      // Wait till start
//      if(btnRelease[0] == 1) {
//        btnRelease[0] = 0;
        
        // Play intro
        delay(100);
        playIntro();
        delay(1000);
  
        state = STATE__PLAY;
        level = 1;
  
        // Generate new sequence
        randomSeed(millis());
        for(i=0; i<32; i++)
          seq[i] = random(256); 
//      }
    } break;

    case STATE__PLAY:
    {
      for(i=0; i<level; i++) {
        uint8_t c = seq[i] & 0x3;

        uint8_t pin = pins[c];
        digitalWrite(pin, 0);
        pinMode(pin, OUTPUT);
        tone(0, freq[c]);
        
        if(level < 6)
          delay(525); // 420ms
        else if(level < 14)
          delay(400); // 320ms
        else
          delay(275); // 220ms
          
        pinMode(pin, INPUT_PULLUP);
        noTone(0);
        digitalWrite(0, 1);
        delay(63);  // 50ms
      }

      state = STATE__PRESS;
      last_press = millis();
      idx = 0;
    } break;

    case STATE__PRESS: {

      if((millis() - last_press) > 10000) {
        state = STATE__FAIL;
      }

      for(i=0; i<4; i++) {
        uint8_t pin = pins[i];
        
        if(btnPress[i]) {
          tone(0, freq[i]);
          input = i;
          state = STATE__RELEASE;

          last_press = millis();
          break;
        }
      }
    } break;

    case STATE__RELEASE: {

      if((millis() - last_press) > 10000) {
        state = STATE__FAIL;
      }
      
      if(btnRelease[input]) {
        noTone(0);
        digitalWrite(0, 1);

        if((seq[idx] & 0x3) == input) {
          idx++;
          
          if(idx == level) {

            if((skill == 0 && level == 8) || (skill == 1 && level == 14) || (skill == 2 && level == 20) || (skill == 3 && level == 31))
              state = STATE__VICTORY;
            else {
              state = STATE__PLAY;
              level++;
              delay(500);
            }
            
            break;
          }

          state = STATE__PRESS;
        } else {
          state = STATE__FAIL;
          break;
        }
      }
    } break;

    case STATE__FAIL: {
      
      uint8_t pin = pins[input];
      digitalWrite(pin, 0);
      pinMode(pin, OUTPUT);
      tone(0, freq[input]);
      delay(3000);
      pinMode(pin, INPUT_PULLUP);
      noTone(0);
      digitalWrite(0, 1);

      state = STATE__SLEEP; 
    } break;

    case STATE__VICTORY: {
      playVictory();
      state = STATE__SLEEP;
    } break;

    case STATE__SLEEP: {
      state = STATE__START;
      
      attachInterrupt(1, Button_callback, CHANGE);
      attachInterrupt(2, Button_callback, CHANGE);
      attachInterrupt(3, Button_callback, CHANGE);
      attachInterrupt(4, Button_callback, CHANGE);
      sei();
      sleep_cpu();
      detachInterrupt(1);
      detachInterrupt(2);
      detachInterrupt(3);
      detachInterrupt(4);
    } break;
  }


  for(i=0; i<4; i++) {
    btnPress[i] = 0;
    btnRelease[i] = 0;
  }

//  if(!digitalRead(g)) {
//    tone(0, 139);
//  } else if(!digitalRead(r)) {
//    tone(0, 186);
//  } else if(!digitalRead(y)) {
//    tone(0, 234);
//  } else if(!digitalRead(b)) {
//    tone(0, 278);
//  } else {
////
//    noTone(0);
//    digitalWrite(0, 1);
//  }

}

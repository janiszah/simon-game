#include <stdint.h>
#include <stdbool.h>

#include "game.h"

#include "SEGGER_RTT.h"


static void _playIntro(void);
static void _playVictory(uint8_t lastInput);

enum {
  STATE__START = 0,
  STATE__PLAY,
  STATE__PRESS,
  STATE__RELEASE,
  STATE__FAIL,
  STATE__VICTORY,
  STATE__SLEEP
};

const uint32_t kFreq[] = {
  RED_FREQ, GREEN_FREQ, YELLOW_FREQ, BLUE_FREQ,
};

static const char* kColor[] = {  
  RTT_CTRL_TEXT_BRIGHT_RED, RTT_CTRL_TEXT_BRIGHT_GREEN, 
  RTT_CTRL_TEXT_BRIGHT_YELLOW, RTT_CTRL_TEXT_BRIGHT_BLUE,
};

// YELLOW - skill 0
// GREEN - skill 1
// BLUE - skill 2
// RED - skill 3
const uint8_t kSkill[] = { 3, 1, 0, 2 };

const uint8_t kSkillLevels[] = { 8, 14, 20, 31 };


static uint8_t s_state;
static uint8_t s_seq[33];
static uint8_t s_idx;

static uint8_t s_level;
static uint8_t s_skill;

static uint32_t s_lastPressTime;
static uint8_t s_input;

static uint32_t s_time;


void Game_init(void) {
  s_lastPressTime = HAL_GetTick();
}


void Game_task(void) {
  uint32_t i;

  switch(s_state) {
    case STATE__START:
      uint32_t tNow = HAL_GetTick();

      if((tNow - s_time) > 250) {
        s_time = HAL_GetTick();
        Led_setState(s_skill, 0);
        s_skill = (s_skill + 1) & 0x3;
        Led_setState(s_skill, 1);
      }

      if((tNow - s_lastPressTime) > 10000) {
        Led_setState(s_skill, 0);
          Buzzer_beep(500);
          HAL_Delay(50);
          Buzzer_stop();
        s_state = STATE__SLEEP;
        break;
      }
    
      // Wait Key press
      for(i=0; i<4; i++) {
        if(Key_isPress(i)) {
          s_skill = kSkill[i];
          s_idx = 0;
          s_level = 1;
          s_seq[0] = HAL_GetTick() & 0x3;
          s_state = STATE__PLAY;
          _playIntro();
          HAL_Delay(SEQ_PLAY_GAP_TIME);
          break;
        }
      }
    break;
    
    // Play sequence
    case STATE__PLAY: {
      for(i=0; i<s_level; i++) {
        uint8_t c = s_seq[i] & 0x3;
        
        // Turn ON LED and beep
        Led_setState(c, 1);
        Buzzer_beep(kFreq[c]);

        // Delay tone time depending on level
        if(s_level < 6)
          HAL_Delay(SEQ1_5_TONE_TIME);
        else if(s_level < 14)
          HAL_Delay(SEQ6_13_TONE_TIME);
        else
          HAL_Delay(SEQ14_31_TONE_TIME);

        // Turn Off
        Led_setState(c, 0);
        Buzzer_stop();

        // Puase
        HAL_Delay(SEQ_PAUSE_TIME);
      }

      Key_flush();
      s_state = STATE__PRESS;
      s_lastPressTime = HAL_GetTick();
      s_idx = 0;
    } break;

    // Repeate sequence
    case STATE__PRESS: {
      if((HAL_GetTick() - s_lastPressTime) > FAIL_TIMEOUT) {
        s_state = STATE__FAIL;
        break;
      }

      for(i=0; i<4; i++) {
        if(Key_isPress(i)) {
          s_lastPressTime = HAL_GetTick();
          s_input = i;
          Led_setState(i, 1);
          Buzzer_beep(kFreq[i]);
        }

        if(Key_isRelease(i)) {
          Buzzer_stop();
          Led_setState(i, 0);

          if(s_input == i) {
            // Check sequence
            if((s_seq[s_idx] & 0x3) == s_input) {
              s_idx++;

              // If full sequence repeated
              if(s_idx == s_level) {
                if(s_level == kSkillLevels[s_skill]) {
                  s_state = STATE__VICTORY;
                } else {
                  s_level++;
                   // Set next seq
                  s_seq[s_level] = s_lastPressTime & 0x3;
                  s_state = STATE__PLAY;
                  HAL_Delay(SEQ_PLAY_GAP_TIME);
                }
                break;
              }

            } else {
              s_state = STATE__FAIL;
              break;
            }
          }
        }
      }
    } break;

    // Fail
    case STATE__FAIL: {
      Led_setState(s_input, 1);
      Buzzer_beep(FAIL_FREQ);
      HAL_Delay(FAIL_BEEP_TIME);
      Led_setState(s_input, 0);
      Buzzer_stop();

      s_state = STATE__SLEEP;
    } break;

    // Victory
    case STATE__VICTORY: {
      _playVictory(s_input);
      s_state = STATE__SLEEP;
    } break;

    // Sleep
    case STATE__SLEEP: {
      s_state = STATE__START;
      // TODO
      Bsp_standby();
    } break;
  } 
}



static void _playIntro(void)
{
  uint8_t i;
  for(i=0; i<4; i++) {
    Led_setState(i, 1);
    Buzzer_beep(kFreq[i]);
    HAL_Delay(50);
    Led_setState(i, 0);
    Buzzer_stop();
    HAL_Delay(10);
  }
}

#ifdef ORIGINAL
static void _playVictory(uint8_t lastInput)
{
  uint8_t i;
  Led_setState(lastInput, 1);
  Buzzer_beep(kFreq[lastInput]);
  HAL_Delay(VICTORY_TONE_TIME_1);
  Led_setState(lastInput, 0);
  Buzzer_stop();
  HAL_Delay(VICTORY_TONE_GAP_TIME);

  for(i=0; i<5; i++) {
    Led_setState(lastInput, 1);
    Buzzer_beep(kFreq[lastInput]);
    HAL_Delay(VICTORY_TONE_TIME_2_6);
    Led_setState(lastInput, 0);
    Buzzer_stop();
    HAL_Delay(VICTORY_TONE_GAP_TIME);
  }
}
#else
static void _playVictory(uint8_t lastInput)
{
  uint8_t i,j;
  for(j=0; j<8; j++) {
    for(i=0; i<4; i++) {
      Led_setState(i, 1);
      Buzzer_beep(kFreq[i]);
      HAL_Delay(50);
      Led_setState(i, 0);
      Buzzer_stop();
      HAL_Delay(10);
    }
  }
}
#endif
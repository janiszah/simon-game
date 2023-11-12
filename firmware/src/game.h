#ifndef _GAME_H
#define _GAME_H

#define BLUE_FREQ     415 
#define YELLOW_FREQ   310
#define RED_FREQ      252
#define GREEN_FREQ    209

#define FAIL_FREQ    42
#define FAIL_BEEP_TIME 1500
#define FAIL_TIMEOUT   3000

#define SEQ_PLAY_GAP_TIME 800

#define SEQ1_5_TONE_TIME    420
#define SEQ6_13_TONE_TIME   320
#define SEQ14_31_TONE_TIME  220
#define SEQ_PAUSE_TIME      50

#define VICTORY_TONE_TIME_1   20
#define VICTORY_TONE_TIME_2_6 70
#define VICTORY_TONE_GAP_TIME 20
#define VICTORY_TIMEOUT       800


void Game_task();

#endif // _GAME_H
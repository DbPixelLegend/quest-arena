/*
This file contains possible stat fields for supported gametypes.

NOTE: GLOBAL and PER USER names:
 * must be unique only within single game mode (ie. WAVE and KIDS can both have "ACCURACY" field; remember though that all gamemodes have BASE included!)
 * may repeat in both GLOBAL and PER USER (ie. both GLOBAL and PER USER can have "ACCURACY" field, but please don't do this)
 */

//------------------------------------BASE

//----------------GLOBAL

//time spent since last GameStart()
#define GAME_TIME "gametime"


//----------------PER USER

#define USER_NAME "username"
#define USER_HID  "hid"

//------------------------------------WAVE

//----------------GLOBAL

#define WAVE_TOTAL_ZOMBIES_SPAWNED "g_spawned_zombies"
#define WAVE_TOTAL_ZOMBIES_KILLED  "g_killed_zombies"
#define WAVE_TOTAL_SHOTS_FIRED     "g_total_shots"
#define WAVE_TOTAL_DAMAGE_DEALT    "g_total_dmg_zombie"
#define WAVE_TOTAL_DAMAGE_RECEIVED "g_total_dmg_player"

//----------------PER USER

#define WAVE_ZOMBIES_KILLED   "p_killcount"
#define WAVE_ZOMBIES_HEADSHOT "p_headcount"
#define WAVE_DAMAGE_DEALT     "p_dmg_zombie"
#define WAVE_DAMAGE_RECEIVED  "p_dmg_player"
#define WAVE_ACCURACY         "p_accuracy"
#define WAVE_SHOTS_FIRED      "p_shotsfired"
#define WAVE_SHOTS_HIT        "p_shotshit"


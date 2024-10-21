#include <c64/joystick.h>
#include <c64/vic.h>
#include <c64/sprites.h>
#include <c64/memmap.h>
#include <c64/rasterirq.h>
#include <gfx/bitmap.h>
#include <gfx/mcbitmap.h>
#include <c64/cia.h>
#include <audio/sidfx.h>
#include <conio.h>

#include <c64/sid.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <c64/easyflash.h>
#include "orbix.h"

#pragma region( zeropage, 0x80, 0xfa, , , {zeropage})
#pragma region(main, 0x0905, 0x8000, , , { code, data, bss, heap, stack } )

// #pragma stacksize(1024)
// #pragma heapsize(600)

#pragma section(bcode1, 0)
#pragma section(bdata1, 0)
#pragma region(bank1, 0x8000, 0xc000, , 1, { bcode1, bdata1 } )

#pragma section(bcode2, 0)
#pragma section(bdata2, 0)
#pragma region(bank2, 0x8000, 0xc000, , 2, { bcode2, bdata2 } )

// Levels Ressouces, Chars
#pragma section(bcode3, 0)
#pragma section(bdata3, 0)
#pragma section(chars, 0)
#pragma region(bank3, 0x8000, 0xbc00, , 3, { bcode3, bdata3 } )
#pragma region(chars, 0xbc00, 0xc000, , 3, { chars } )

// Levels1
#pragma section(bcode4, 0)
#pragma section(bdata4, 0)
#pragma region(bank4, 0x8000, 0xc000, , 4, { bcode4, bdata4 } )

// Levels2
#pragma section(bcode5, 0)
#pragma section(bdata5, 0)
#pragma region(bank5, 0x8000, 0xc000, , 5, { bcode5, bdata5 } )

// Assets
#pragma section(sounds, 0)
#pragma section(sprites, 0)
#pragma region(sounds, 0x8000, 0x8800, , 6, { sounds } )
#pragma region(sprites, 0x8800, 0xc000, , 6, { sprites } )

// Title
#pragma section(imgHires, 0)
#pragma section(scrCol, 0)
#pragma section(imgCol, 0)
#pragma region(imgHires, 0x8000, 0xa000, , 7, { imgHires } )
#pragma region(imgCol, 0xa000, 0xb000, , 7, { imgCol } )

// Sid, Images
#pragma section(music, 0)
#pragma section(musicgame, 0)
#pragma section(musicbg, 0)
#pragma section(logo, 0)
#pragma section(landscape1, 0)
#pragma section(moon, 0)
#pragma section(alien1, 0)
#pragma section(landscape2, 0)
#pragma section(valleyl, 0)
#pragma section(valleyr, 0)
#pragma section(alien2, 0)
#pragma section(gridstar, 0)
#pragma region(music, 0x8000, 0x9000, , 8, { music } )
#pragma region(musicgame, 0x9000, 0x9800, , 8, { musicgame } )
#pragma region(musicbg, 0x9800, 0xA000, , 8, { musicbg } )
#pragma region(logo, 0xA000, 0xA800, , 8, { logo } )
#pragma region(landscape1, 0xA800, 0xAD00, , 8, { landscape1 } )
#pragma region(moon, 0xad00, 0xAEE0, , 8, { moon } )
#pragma region(alien1, 0xAEE0, 0xB0E0, , 8, { alien1 } )
#pragma region(landscape2, 0xB0E0, 0xB5E0, , 8, { landscape2 } )
#pragma region(valleyl, 0xB5E0, 0xB7E0, , 8, { valleyl } )
#pragma region(valleyr, 0xB7E0, 0xB9E0, , 8, { valleyr } )
#pragma region(alien2, 0xB9E0, 0xBD00, , 8, { alien2 } )
#pragma region(gridstar, 0xBE00, 0xBE80, , 8, { gridstar } )

// Sid, Images
#pragma section(landscape3, 0)
#pragma section(ship1, 0)
#pragma section(ship2, 0)
#pragma section(musicbg2, 0)
#pragma section(galaxy, 0)
#pragma region(landscape3, 0x8000, 0x8500, , 9, { landscape3 } )
#pragma region(ship1, 0x8500, 0x8700, , 9, { ship1 } )
#pragma region(ship2, 0x8700, 0x8c40, , 9, { ship2 } )
#pragma region(musicbg2, 0x8c40, 0x9440, , 9, { musicbg2 } )
#pragma region(galaxy, 0x9440, 0x9600, , 9, { galaxy } )

#define PAT_ARROW_UP 0
#define PAT_ARROW_DOWN 1
#define PAT_WALL 2
#define PAT_RAUTE 3
#define PAT_SOLID 4
#define PAT_FALSE 5
#define PAT_TRUE 6
#define PAT_DANGER 7
#define PAT_FLAME 8
#define PAT_PLUS 9
#define PAT_BONUS 10
#define PAT_BONUS_LINE 11
#define PAT_CHESS 12
#define PAT_BOMB 13
#define PAT_BOMBWALL 14

#define SUB_ALL_CLEAR 0
#define SUB_SHOOT 1
#define SUB_LEVEL_START 2
#define SUB_GAMEOVER 3
#define SUB_LEVEL_BUILD 4
#define SUB_BALL_TRAY 5
#define SUB_BALL_SAVED 6
#define SUB_BALL_LOST 7
#define SUB_LEVEL_FINISHED 8

#define LARGE_TEXT_TIMER 100

#define SND_SWITCH_ON 0x01
#define SND_GRAVITY 0x03
#define SND_BUMP 0x04
#define SND_EXPLODE 0x02
#define SND_BALL_ACTIVATED 0x07
#define SND_ORBZ_DELETE 0x00
#define SND_BONUS 0x0b
#define SND_CATCH 0x05
#define SND_MULTIPLIER 0x06
#define SND_SHOOT 0x08

#define FACE_NORM 7
#define FACE_ANGRY 8
#define FACE_SUNNY 9
#define FACE_RIGHT 10
#define FACE_LEFT 11
#define FACE_THINK 12

#define IMG_LANDSCAPE1 0
#define IMG_LANDSCAPE2 1
#define IMG_MOON 2
#define IMG_ALIEN1 3
#define IMG_VALLEYL 4
#define IMG_VALLEYR 5
#define IMG_GRIDSTAR 6
#define IMG_ALIEN2 7
#define IMG_LANDSCAPE3 8
#define IMG_SHIP1 9
#define IMG_GALAXY 10

// const sbyte sintab[256] = {
// 	0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116, 117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 122, 122, 121, 120, 118, 117, 116, 115, 113, 112, 111, 109, 107, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 85, 83, 81, 78, 76, 73, 71, 68, 65, 63, 60, 57, 54, 51, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 12, 9, 6, 3, 0, -3, -6, -9, -12, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46, -49, -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85, -88, -90, -92, -94, -96, -98, -100, -102, -104, -106, -107, -109, -111, -112, -113, -115, -116, -117, -118, -120, -121, -122, -122, -123, -124, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -124, -123, -122, -122, -121, -120, -118, -117, -116, -115, -113, -112, -111, -109, -107, -106, -104, -102, -100, -98, -96, -94, -92, -90, -88, -85, -83, -81, -78, -76, -73, -71, -68, -65, -63, -60, -57, -54, -51, -49, -46, -43, -40, -37, -34, -31, -28, -25, -22, -19, -16, -12, -9, -6, -3};

const sbyte sintab[128] = {
	0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116, 117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 122, 122, 121, 120, 118, 117, 116, 115, 113, 112, 111, 109, 107, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 85, 83, 81, 78, 76, 73, 71, 68, 65, 63, 60, 57, 54, 51, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 12, 9, 6, 3 };

#pragma data(sprites)

__export const byte ballsprites[] = {
#embed "../resources/balls.spr"
};

#pragma data(sounds)

// __export char Sounds[] = {
// #embed 2048 0x7e "../resources/orbix_ingame_c0.sid"
// };

__export char Sounds[] = {
#embed 1024 2 "../resources/sounds.prg"
};

#pragma data(music)

__export char Music[] = {
#embed 4096 0x7e "../resources/orbix_title_c0.sid"
};

#pragma data(musicgame)

__export char MusicInGame[] = {
#embed 2048 0x7e "../resources/orbix_ingame_c0.sid"
};

#pragma data(musicbg)

__export char MusicBackground1[] = {
#embed 2048 0x7e "../resources/orbix_background1_c0.sid"
};

#pragma data(musicbg2)

__export char MusicBackground2[] = {
#embed 2048 0x7e "../resources/orbix_background2_c0.sid"
};


#pragma data(chars)

// Charset assets
__export char TextChars[] = {
#embed "../resources/chars.bin"
};

#pragma data(imgHires)
// Charset assets
__export char ImgHires[] = {
#embed 8000 0 "../resources/title_data.bin"
};

#pragma data(scrCol)

#pragma data(imgCol)

__export char ImgCol[] = {
#embed 1000 2 "../resources/title_color.bin"
};

#pragma data(logo)
__export char ImgLogo[] = {
#embed 960 0 "../resources/logo.bin"
};

#pragma data(landscape1)
__export char ImgLandscape1[] = {
#embed 0x500 0 "../resources/landscape1.bin"
};

#pragma data(landscape2)
__export char ImgLandscape2[] = {
#embed 0x500 0 "../resources/landscape2.bin"
};

#pragma data(moon)
__export char ImgMoon[] = {
#embed 0x1E0 0 "../resources/moon.bin"
};

#pragma data(alien1)
__export char ImgAlien1[] = {
#embed 0x200 0 "../resources/alien1.bin"
};

#pragma data(valleyl)
__export char ImgValleyL[] = {
#embed 0x200 0 "../resources/valleyL.bin"
};

#pragma data(valleyr)
__export char ImgValleyR[] = {
#embed 0x200 0 "../resources/valleyR.bin"
};

#pragma data(gridstar)
__export char ImgGridstar[] = {
#embed 0x080 0 "../resources/gridstar.bin"
};

#pragma data(alien2)
__export char ImgAlien2[] = {
#embed 0x2a0 0 "../resources/alien2.bin"
};

#pragma data(landscape3)
__export char ImgLandscape3[] = {
#embed 0x500 0 "../resources/landscape3.bin"
};

#pragma data(ship1)
__export char ImgShip1[] = {
#embed 0x200 0 "../resources/ship1.bin"
};

#pragma data(ship2)
__export char ImgShip2[] = {
#embed 0x540 0 "../resources/ship2.bin"
};

#pragma data(galaxy)
__export char ImgGalaxy[] = {
#embed 0x1C0 0 "../resources/galaxy.bin"
};


#pragma data(data)

#define GCOL_BLACK 0x00
#define GCOL_WHITE 0x10
#define GCOL_RED   0x20
#define GCOL_CYAN  0x30
#define GCOL_PURPLE 0x40
#define GCOL_GREEN 0x50
#define GCOL_BLUE 0x60
#define GCOL_YELLOW 0x70
#define GCOL_ORANGE 0x80
#define GCOL_BROWN 0x90
#define GCOL_LT_RED 0xa0
#define GCOL_DARK_GREY 0xb0
#define GCOL_MED_GREY 0xc0
#define GCOL_LT_GREEN 0xd0
#define GCOL_LT_BLUE 0xe0
#define GCOL_LT_GREY 0xf0

#define Color1 ((char *)0xd800)
#define Color2 ((char *)0xc800)
#define Hires ((char *)0xe000)
#define HiresLogo ((char *)0xe500)
#define Sprites ((char *)0xd000)
#define Sfx ((char *)0xcc00)
#define Charset ((char *)0xc800)
#define Buffer ((char *)0xc000)

Bitmap Screen;

ClipRect scr = {0, 8, 320, 200};

RIRQCode bottom, top, rmenu;
#define clipX 0
#define clipY 1
#define clipW 40
#define clipH 24

#pragma align(top, 32)
#pragma align(bottom, 32)

// Screen and color ram address

#define ScreenWidth 320
#define ScreenHeight 200

#define Component0_Render 0b00000001
#define Component0_Physics 0b00000010
#define Component0_CollideAble 0b00000100
#define Component0_CircleCollider 0b00001000
#define Component0_CapsuleCollider 0b00010000
#define Component0_DrawCircle 0b00100000
#define Component0_DrawEdge 0b01000000
#define Component0_DrawCannon 0b10000000
#define Component1_MainBall 0b00000001
#define Component1_TimedDelete 0b00000010
#define Component1_CrackDelete 0b00000100
#define Component1_DrawBall 0b00001000
#define Component1_SwitchOrientation 0b00010000
#define Component1_Catch 0b00100000
#define Component1_Valueable 0b01000000
#define Component1_Switch 0b10000000
#define Component2_Laser 0b00000001
#define Component2_Kill 0b00000010
#define Component2_BombWall 0b00000100
#define Component2_BallActivate 0b00001000
#define Component2_LargeText 0b00010000
#define Component2_BonusScore 0b00100000
#define Component2_Destroyed 0b01000000
#define Component2_DrawStar 0b10000000
#define Component3_DrawLetter 0b00000001
#define Component3_CollisionTimerAble 0b00000010
#define Component3_PortalActive 0b00000100
#define Component3_Bomb 0b00001000
#define Component3_ImmediateDelete 0b00010000
#define Component3_DrawImage 0b00100000
#define Component3_NoBonusImage 0b01000000
#define Component3_Exploded 0b10000000

#define EventInitLevel 0
#define EventSwitchOn 1
#define EventSwitchOff 2
#define EventWaypointEnter 3
#define EventWaypointLeave 4
#define EventCollide 4
#define EventBallLost 5
#define EventInterval8 6
#define EventInterval16 7
#define EventInterval32 8
#define EventResetBall 9
#define EventCatchBall 10
#define EventPortal 11
#define EventShootBall 12
#define EventKillBall 13

#define ActionPrintText 0
#define ActionEnableCollision 1
#define ActionDisableCollision 2
#define ActionEnableLaser 3
#define ActionDisableLaser 4
#define ActionHideObject 5
#define ActionShowObject 6
#define ActionTimedRollDeactivateCollideable 7
#define ActionTimedRollActivateCollideable 8
#define ActionRandomActivateCollideable 9
#define ActionRandomDeactivateCollideable 10
#define ActionActivateBall 11
#define ActionDeactivateBall 12
#define ActionKillBall 13

#define TimeElapsingNorm 0.022
#define TimeElapsingFast 0.030
#define Drag 0.999
#define Gravity 300
#define Stable 1
#define CannonAngleStepSlow 0.034905
#define CannonAngleStep 0.06981
#define CannonAngleStepFast 0.13962
#define CannonPower 25
#define TimeTimedDelay 16
#define TimeBombDelay 4
#define TimeTimedScore 8
#define BombDistance 60

#define ORIENTATION_UP -1
#define ORIENTATION_DOWN 1
#define ORIENTATION_LEFT 2
#define ORIENTATION_RIGHT 3

#define SIM_CHANGE 0
#define SIM_FIRE 1
#define SIM_END 2
#define SIM_DONE 3
#define SIM_RUN 4

#define MaxObjects 64
#define MaxLevels 20
#define MaxScoreDisplay 16
#define MaxBombDisplay 8
#define MaxActors 16
#define MaxCollidingPairs 4
#define MaxFakes 4
#define MaxDepObjects 4
#define MaxColTimers 3
#define MaxCntMusic 1000
#define MaxSimTime 30
#define MaxSimEndTime 20


struct GameObject gameObjects[MaxObjects];
struct GameObject fakeBalls[MaxFakes];
struct CollidingPairs colPairs[MaxCollidingPairs];
struct Actor actorList[MaxActors];
struct DepGameObject depObjects[MaxDepObjects];
struct CollidingTimer collidingTimers[MaxColTimers];
float	px[10], py[10];


ScoreBoard scoreBoard[7] =
	{
		{1, 1, 0},
		{0.90, 1, 0},
		{0.75, 2, 0},
		{0.60, 3, 0},
		{0.45, 4, 0},
		{0.30, 5, 0},
		{0.10, 6, 1},
};

const byte lumFromBlack[32] = { 0x00,0x06,0x09,0x0b,0x02,0x04,0x08,0x0c,0x0e,0x0a,0x05,0x0f,0x03,0x07,0x0d,0x01,0x0d,0x07,0x03,0x0f,0x05,0x0a,0x0e,0x0c,0x08,0x04,0x02,0x0b,0x09,0x06,0x00,0x00 };
const byte blues8[8] = { 0x10,0xf0,0xc0, 0xb0, 0xb0, 0xc0,0xf0,0x10 };

const char texts[6][12] = {"LEVEL START", "BALL LOST", "BALL SAVED", "ALL CLEAR", "LEVEL DONE", "GAME OVER"};

const char bonusTxt[5] = {'B','O','N','U','S'};

char hiscore[8] = "00000000";

byte callBank;
sbyte globalOrientation;
byte posYTray, tray;
byte timer;
byte loopc;
byte loopo;
int posXTray;
byte numObjects;
byte numDepObjects;
byte numActors;
byte totalValueableObjects;
byte startValueableObjects;
byte valueableObjects;
byte numColPairs;
byte numFakes;
float timeElapsing;
int remainTimer;
GameState state;
byte currentCannon, mainCannon;
float currentAngleStep = 0;
byte mainBall;
byte collideQueue[MaxObjects];
sbyte collideQueueFront, collideQueueRear;
ScoreDisplay scoreQueue[MaxScoreDisplay];
sbyte scoreQueueFront, scoreQueueRear;
BombDisplay bombQueue[MaxBombDisplay];
sbyte bombQueueFront, bombQueueRear;
byte bumptimer;
bool playTunes;
byte stuckCnt;
unsigned long points;
byte multiplier;
byte spriteCount;
const char *sp;
int largeTextTimer;
byte largeTextId;
bool breakMusic;
byte balls;
byte level;
signed int orbzScore;
signed int roundScore;
signed int levelScore;
byte voice;
Highlight highlight;
byte traySprite;
bool extraBalls;
bool completed;
byte faceState;
signed char ballSide;
byte faceTimer;
byte titlePhase;
byte menuItem;
// Storage for up to 64 particles
Particle	particles[65];
signed int cntMusic;
byte bombDistance;
byte simEndCnt;
bool hasSimHit, simDone;
float saveSimPx, saveSimPy;
bool simLeftRight;
byte simSteps;
signed char tx;
int gravity;
bool isGravCannon;
signed int moveCannonCount;
bool isMusicFf;

// Heads of used and free list
Particle	*	pfirst, * pfree;

// Lookup table for hires row buffer
static char * Hirows[25];

// Pixel masks
static const char setmask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
//static const char clrmask[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

__noinline void proxy32_particle_init()
{
	eflash.bank = 2;
	particle_init_2();
	eflash.bank = 3;
}

__noinline void proxy32_particle_remove(bool isConfetti)
{
	eflash.bank = 2;
	particle_remove_2(isConfetti);
	eflash.bank = 3;

}


__noinline void proxy12_dispatchEvent(byte event, byte senderId, byte p1, byte p2)
{
	eflash.bank = 2;
	dispatchEvent_2(event, senderId, p1, p2);
	eflash.bank = 1;
}

__noinline void proxy32_dispatchEvent(byte event, byte senderId, byte p1, byte p2)
{
	eflash.bank = 2;
	dispatchEvent_2(event, senderId, p1, p2);
	eflash.bank = 3;
}

__noinline void proxy12_addFakeBalls(float px, float py, float vx, float vy, float radius, float efficiency)
{
	eflash.bank = 2;
	addFakeBalls_2(px, py, vx, vy, radius, efficiency);
	eflash.bank = 1;
}

__noinline void proxy32_addFakeBalls(float px, float py, float vx, float vy, float radius, float efficiency)
{
	eflash.bank = 2;
	addFakeBalls_2(px, py, vx, vy, radius, efficiency);
	eflash.bank = 3;
}

__noinline void proxy12_addCollidingPairs(byte go1, byte go2, byte gofake)
{
	eflash.bank = 2;
	addCollidingPairs_2(go1, go2, gofake);
	eflash.bank = 1;
}

__noinline void proxy32_addCollidingPairs(byte go1, byte go2, byte gofake)
{
	eflash.bank = 2;
	addCollidingPairs_2(go1, go2, gofake);
	eflash.bank = 3;
}

__noinline void proxy12_handleScore(byte id)
{
	eflash.bank = 2;
	handleScore_2(id);
	eflash.bank = 1;
}

__noinline void proxy13_setGameObjectCannon(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon)
{
	eflash.bank = 3;
	setGameObjectCannon_3(id, px, py, r, orientation, color, useGlobalOrientation, isMainCannon);
	eflash.bank = 1;
}

__noinline void proxy23_setGameObjectCannon(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon)
{
	eflash.bank = 3;
	setGameObjectCannon_3(id, px, py, r, orientation, color, useGlobalOrientation, isMainCannon);
	eflash.bank = 2;
}


__noinline void proxy21_updateRender(byte id, bool init)
{
	eflash.bank = 1;
	updateRender_1(id, init);
	eflash.bank = 2;
}

__noinline void proxy31_updateRender(byte id, bool init)
{
	eflash.bank = 1;
	updateRender_1(id, init);
	eflash.bank = 3;
}

__noinline void proxy12_colorBoundingBox(byte id)
{
	eflash.bank = 2;
	colorBoundingBox_2(id);
	eflash.bank = 1;
}

__noinline void proxy12_string_write(char cx, char cy, const char *s, char color)
{
	eflash.bank = 2;
	string_write_2(cx,cy,s,color);
	eflash.bank = 1;
}

__noinline void proxy32_string_write(char cx, char cy, const char *s, char color)
{
	eflash.bank = 2;
	string_write_2(cx,cy,s,color);
	eflash.bank = 3;
}


__noinline void proxy0_drawFrame()
{
//	rirq_wait();
	mmap_set(MMAP_NO_ROM);
	drawFrame();
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_cleanScreen(bool withScore)
{
//	rirq_wait();
	mmap_set(MMAP_RAM);
	cleanScreen(withScore);
	mmap_set(MMAP_ROM);
}


__noinline void proxy0_bm_circle_fill(const Bitmap *bm, const ClipRect *clip, int x, int y, char r, const char *pattern)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_circle_fill(bm, clip, x, y, r, pattern);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_bm_line(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, char pattern, LineOp op)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_line(bm, clip, x0, y0, x1, y1, pattern, op);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_bm_triangle_fill(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, int x2, int y2, const char *pat)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_triangle_fill(bm, clip, x0, y0, x1, y1, x2, y2, pat);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_colorChar(unsigned char x, unsigned char y, byte backc, char* col, bool onlyBack)
{
	mmap_set(MMAP_RAM);
	colorChar(x, y, backc, col, onlyBack);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_bm_quad_fill(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const char *pat)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_quad_fill(bm, clip, x0, y0, x1, y1, x2, y2, x3, y3, pat);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_bm_rect_clear(const Bitmap *dbm, const ClipRect *clip, int dx, int dy, int w, int h)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_rect_clear(dbm, clip, dx, dy, w, h);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_bm_put(const Bitmap *bm, int x, int y, bool c)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_put(bm, x, y, c);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_spr_image(byte id, byte image)
{
	mmap_set(MMAP_RAM);
	spr_image(id, image);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_spr_set(char sp, bool show, int xpos, int ypos, char image, char color, bool multi, bool xexpand, bool yexpand)
{
	char *  barSprite = (char*)0xCBF8;

	spr_set(sp, show, xpos, ypos, image, color, multi, xexpand, yexpand);
	mmap_set(MMAP_RAM);
	spr_image(sp, image);
	barSprite[sp] = image;
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_put_score(int x, int y, signed int score)
{
	//rirq_wait();
	mmap_set(MMAP_RAM);
	char buffer[10];
	itoa(score, buffer, 10);

	bm_put_string(&Screen, &scr, x - strlen(buffer) * 6, y - 12, buffer, BLTOP_XOR);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_put_text(int x, int y, byte textId)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_put_string(&Screen, &scr, x - strlen(texts[textId]) * 3, y - 12, texts[textId], BLTOP_XOR);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_put_string(const Bitmap * bm, const ClipRect * clip, int x, int y, const char * str, BlitOp op)
{
	//rirq_wait();
	mmap_set(MMAP_RAM);
	bm_put_string(bm, clip, x, y, str, op);
	mmap_set(MMAP_ROM);
}

__noinline void proxy12_score_reset(void)
{
	eflash.bank = 2;
	score_reset_2();
	eflash.bank = 1;
}

__noinline void proxy12_score_inc(char digit, unsigned val)
{
	eflash.bank = 2;
	score_inc_2(digit, val);
	eflash.bank = 1;
}

__noinline void proxy14_initScene()
{
	eflash.bank = 4;
	initScene_4();
	eflash.bank = 1;
}

__noinline void proxy0_bm_rect_fill(const Bitmap * dbm, const ClipRect * clip, int dx, int dy, int w, int h)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	bm_rect_fill(dbm, clip, dx, dy, w, h);
	mmap_set(MMAP_ROM);
}

__noinline void proxy0_pix_set(unsigned px, unsigned py, bool isConfetti)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	pix_set(px, py, isConfetti);
	mmap_set(MMAP_ROM);
}

__noinline void proxy12_collideQueue_enqueue(byte insertItem)
{
	eflash.bank = 2;
	collideQueue_enqueue_2(insertItem);
	eflash.bank = 1;
}

__noinline sbyte proxy12_collideQueue_dequeue()
{
	eflash.bank = 2;
	sbyte r = collideQueue_dequeue_2();
	eflash.bank = 1;
	return r;
}

__noinline bool proxy12_collideQueue_isQueued(byte item)
{
	eflash.bank = 2;
	bool r = collideQueue_isQueued_2(item);
	eflash.bank = 1;
	return r;
}

__noinline void proxy12_scoreQueue_enqueue(ScoreDisplay insertItem)
{
	eflash.bank = 2;
	scoreQueue_enqueue_2(insertItem);
	eflash.bank = 1;
}

__noinline struct ScoreDisplay proxy12_scoreQueue_dequeue()
{
	eflash.bank = 2;
	struct ScoreDisplay r = scoreQueue_dequeue_2();
	eflash.bank = 1;
	return r;
}

__noinline ScoreDisplay proxy12_scoreQueue_get(byte item)
{
	eflash.bank = 2;
	struct ScoreDisplay r = scoreQueue_get_2(item);
	eflash.bank = 1;
	return r;
}

__noinline void proxy12_showBalls()
{
	eflash.bank = 2;
	showBalls_2();
	eflash.bank = 1;
}

__noinline void proxy12_showMultiplier(byte multiplier)
{
	eflash.bank = 2;
	showMultiplier_2(multiplier);
	eflash.bank = 1;
}

__noinline void proxy12_startHighlight(byte x, byte y, byte w, byte h, bool isHires, byte restoreColor)
{
	eflash.bank = 2;
	startHighlight_2(x, y, w, h, isHires, restoreColor);
	eflash.bank = 1;
}

__noinline void proxy12_doFirework(byte type)
{
	eflash.bank = 2;
	doFirework_2(type);
	eflash.bank = 1;
}

__noinline bool proxy12_setGlobalOrientation(sbyte orientation, bool isInit)
{
	eflash.bank = 2;
	bool r = setGlobalOrientation_2(orientation, isInit);
	eflash.bank = 1;
	return r;
}

__noinline bool proxy32_setGlobalOrientation(sbyte orientation, bool isInit)
{
	eflash.bank = 2;
	bool r = setGlobalOrientation_2(orientation, isInit);
	eflash.bank = 3;
	return r;
}

__noinline bool proxy42_setGlobalOrientation(sbyte orientation, bool isInit)
{
	eflash.bank = 2;
	bool r = setGlobalOrientation_2(orientation, isInit);
	eflash.bank = 4;
	return r;
}


__noinline bool proxy52_setGlobalOrientation(sbyte orientation, bool isInit)
{
	eflash.bank = 2;
	bool r = setGlobalOrientation_2(orientation, isInit);
	eflash.bank = 5;
	return r;
}


__noinline void proxy13_lchar_put(int cx, char cy, char c, char spray)
{
	eflash.bank = 3;
	lchar_put_3(cx, cy, c, spray);
	eflash.bank = 1;
}

__noinline void proxy23_lchar_put(int cx, char cy, char c, char spray)
{
	eflash.bank = 3;
	lchar_put_3(cx, cy, c, spray);
	eflash.bank = 2;
}

__noinline void proxy12_executeLargeTextTimer()
{
	eflash.bank = 2;
	executeLargeTextTimer_2();
	eflash.bank = 1;
}

__noinline void proxy12_updateCannon(byte id)
{
	eflash.bank = 2;
	updateCannon_2(id);
	eflash.bank = 1;
}

__noinline void proxy21_deleteRender(byte id, bool withUpdate)
{
	eflash.bank = 1;
	deleteRender_1(id, withUpdate);
	eflash.bank = 2;
}

__noinline void proxy13_initBonusLine()
{
	eflash.bank = 3;
	initBonusLine_3();
	eflash.bank = 1;
}

__noinline void proxy12_updateFace()
{
	eflash.bank = 2;
	updateFace_2();
	eflash.bank = 1;
}

__noinline void proxy13_preInitScene()
{
	eflash.bank = 3;
	preInitScene_3();	
	eflash.bank = 1;

}

__noinline void proxy13_postInitScene()
{
	eflash.bank = 3;
	postInitScene_3();	
	eflash.bank = 1;

}

__noinline void proxy12_portalSwitch(byte id, byte targetId)
{
	eflash.bank = 2;
	portalSwitch_2(id,targetId);
	eflash.bank = 1;
}

__noinline void proxy12_explodeBomb(byte id)
{
	eflash.bank = 2;
	explodeBomb_2(id);
	eflash.bank = 1;
}


__noinline void proxy12_execCollisionTimer(byte i)
{
	eflash.bank = 2;
	execCollisionTimer_2(i);
	eflash.bank = 1;
}

__noinline void proxy3_addActor(byte event1, byte event2, byte senderId, byte action, byte receiverStart, byte receiverEnd, byte param0, byte param1, byte var0)
{
    eflash.bank = 3;
    addActor_3(event1, event2, senderId, action, receiverStart, receiverEnd, param0, param1, var0);
    eflash.bank = callBank;
}

__noinline void proxy3_addActor(byte fromBank, byte event1, byte event2, byte senderId, byte action, byte receiverStart, byte receiverEnd, byte param0, byte param1, byte var0)
{
    eflash.bank = 3;
    addActor_3(event1, event2, senderId, action, receiverStart, receiverEnd, param0, param1, var0);
    eflash.bank = fromBank;
}

__noinline void proxy3_setGameObjectSwitchCircle(byte id, float px, float py, float r, float efficiency, byte color, bool isOn)
{
    eflash.bank = 3;
    setGameObjectSwitchCircle_3(id, px, py, r, efficiency, color, isOn);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectTimedDelayedCircle(byte id, float px, float py, float r, float efficiency, byte color, byte pattern, bool collideAble, bool valueable, signed int baseScore)
{
    eflash.bank = 3;
    setGameObjectTimedDelayedCircle_3(id, px, py, r, efficiency, color, pattern, collideAble, valueable, baseScore);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectTimedValuableCircle(byte id, float px, float py, float r, bool isCollideAble)
{
    eflash.bank = 3;
    setGameObjectTimedValuableCircle_3(id, px, py, r, isCollideAble);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectBombWallCircle(byte id, float px, float py, float r, bool isCollideAble)
{
    eflash.bank = 3;
    setGameObjectBombWallCircle_3(id, px, py, r, isCollideAble);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectTimedCircle(byte id, float px, float py, float r, bool isCollideAble)
{
    eflash.bank = 3;
    setGameObjectTimedCircle_3(id, px, py, r, isCollideAble);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectWallCircle(byte id, float px, float py, float r, float efficiency, byte color)
{
    eflash.bank = 3;
    setGameObjectWallCircle_3(id, px, py, r, efficiency, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectKillCircle(byte id, float px, float py, float r, byte color)
{
    eflash.bank = 3;
    setGameObjectKillCircle_3(id, px, py, r, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectValuableCrackCircle(byte id, float px, float py, float r)
{
    eflash.bank = 3;
    setGameObjectValuableCrackCircle_3(id, px, py, r);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectCrackCircle(byte id, float px, float py, float r, float efficiency,int color, byte crackLevel, bool isValueable)
{
    eflash.bank = 3;
    setGameObjectCrackCircle_3(id, px, py, r, efficiency, color, crackLevel, isValueable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectCircleOrientation(byte id, float px, float py, float r, byte orientation, byte color, byte pattern, byte comp1)
{
    eflash.bank = 3;
    setGameObjectCircleOrientation_3(id, px, py, r, orientation, color, pattern, comp1);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectCannon(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon)
{
    eflash.bank = 3;
    setGameObjectCannon_3(id, px, py, r, orientation, color, useGlobalOrientation, isMainCannon);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectValueableTimedEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable)
{
    eflash.bank = 3;
    setGameObjectValueableTimedEdge_3(id, px, py, ox, oy, radius, isCollideable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectBombWallEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable)
{
    eflash.bank = 3;
    setGameObjectBombWallEdge_3(id, px, py, ox, oy, radius, isCollideable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectTimedEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable)
{
    eflash.bank = 3;
    setGameObjectTimedEdge_3(id, px, py, ox, oy, radius, isCollideable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectLaser(byte id, float px, float py, float ox, float oy, byte color, bool isCollideable)
{
    eflash.bank = 3;
    setGameObjectLaser_3(id, px, py, ox, oy, color, isCollideable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectWallEdge(byte id, float px, float py, float ox, float oy, float radius, float efficiency, byte color, bool isCollideable)
{
    eflash.bank = 3;
    setGameObjectWallEdge_3(id, px, py, ox, oy, radius, efficiency, color, isCollideable);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectCrackEdge(byte id, float px, float py, float ox, float oy, float radius, float efficiency, byte color, byte crackLevel, bool isValueable, signed int baseScore)
{
    eflash.bank = 3;
    setGameObjectCrackEdge_3(id, px, py, ox, oy, radius, efficiency, color, crackLevel, isValueable, baseScore);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectValueableCrackEdge(byte id, float px, float py, float ox, float oy, float radius)
{
    eflash.bank = 3;
    setGameObjectValueableCrackEdge_3(id, px, py, ox, oy, radius);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectActivateBallCircle(byte id, float px, float py, float r, float efficiency, byte color)
{
    eflash.bank = 3;
    setGameObjectActivateBallCircle_3(id, px, py, r, efficiency, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectPortal(byte id, float px, float py, float radius, bool isActive)
{
    eflash.bank = 3;
    setGameObjectPortal_3(id, px, py, radius, isActive);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectBall(byte id, float px, float py, float vx, float vy, float radius, byte spritenum, bool isMainBall, bool isOn, byte color)
{
    eflash.bank = 3;
    setGameObjectBall_3(id, px, py, vx, vy, radius, spritenum, isMainBall, isOn, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectLargeText(byte id, char py, bool isOn, byte orientation, byte textId, byte color)
{
    eflash.bank = 3;
    setGameObjectLargeText_3(id, py, isOn, orientation, textId, color);
    eflash.bank = callBank;
}

__noinline void proxy3_addDepObject(byte sourceId, byte targetId)
{
    eflash.bank = 3;
    addDepObject_3(sourceId, targetId);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectTimedBonusScoreCircle(byte id, float px, float py, float r, bool isCollideAble)
{
    eflash.bank = 3;
    setGameObjectTimedBonusScoreCircle_3(id, px, py, r, isCollideAble);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectBumpCircle(byte id, float px, float py, float r, float efficiency, byte color)
{
    eflash.bank = 3;
    setGameObjectBumpCircle_3(id, px, py, r, efficiency, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectBombCircle(byte id, float px, float py, float r, float efficiency, byte color)
{
    eflash.bank = 3;
    setGameObjectBombCircle_3(id, px, py, r, efficiency, color);
    eflash.bank = callBank;
}

__noinline void proxy3_setGameObjectImage(byte id, byte imageId, char x, char y, char w, char h, bool isOn, byte color, bool deleteBonus)
{
    eflash.bank = 3;
    setGameObjectImage_3(id, imageId, x, y, w, h, isOn, color, deleteBonus);
    eflash.bank = callBank;
}

__noinline void proxy34_initScene_4()
{
    eflash.bank = 4;
    initScene_4();
    eflash.bank = 3;
}

__noinline void proxy45_initScene10()
{
    eflash.bank = 5;
    initScene10_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene11()
{
    eflash.bank = 5;
    initScene11_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene12()
{
    eflash.bank = 5;
    initScene12_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene13()
{
    eflash.bank = 5;
    initScene13_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene14()
{
    eflash.bank = 5;
    initScene14_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene15()
{
    eflash.bank = 5;
    initScene15_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene16()
{
    eflash.bank = 5;
    initScene16_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene17()
{
    eflash.bank = 5;
    initScene17_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene18()
{
    eflash.bank = 5;
    initScene18_5();
    eflash.bank = 4;
}

__noinline void proxy45_initScene19()
{
    eflash.bank = 5;
    initScene19_5();
    eflash.bank = 4;
}


__noinline void proxy17_copyTitle()
{
    eflash.bank = 7;
	copyTitle();
    eflash.bank = 1;
}

__noinline void proxy13_copyCharset()
{
	eflash.bank = 3;
	copyCharset();
	eflash.bank = 1;
}

__noinline void proxy14_drawBackgroundImage(byte id)
{
	eflash.bank = 4;
	drawBackgroundImage_4(id);
	eflash.bank = 1;
}

__noinline struct BombDisplay proxy12_bombQueue_dequeue()
{
	eflash.bank = 2;
	struct BombDisplay bombDisplay = bombQueue_dequeue_2();
	eflash.bank = 1;
	return bombDisplay;
}

__noinline void proxy0_colorArea(byte x, byte y, byte w, byte h, byte fcol, byte bcol)
{
	//rirq_wait();
	mmap_set(MMAP_NO_ROM);
	colorArea(x, y, w, h, fcol, bcol);
	mmap_set(MMAP_ROM);
}

__noinline void proxy12_updateDestroyed(byte id)
{
    eflash.bank = 2;
	updateDestroyed_2(id);
    eflash.bank = 1;
}

__noinline void proxy21_activateSecondaryBall(byte id, byte sourceId)
{
    eflash.bank = 1;
	activateSecondaryBall_1(sourceId, id);
    eflash.bank = 2;
}


__noinline void proxy21_deactivateSecondaryBall(byte id)
{
    eflash.bank = 1;
	deactivateSecondaryBall_1(id);
    eflash.bank = 2;
}

__noinline void proxy21_resetBall(byte id)
{
    eflash.bank = 1;
	resetBall_1(id);
    eflash.bank = 2;

}

__noinline void proxy12_catchBall(byte id, int target)
{
    eflash.bank = 2;
	catchBall_2(id, target);
    eflash.bank = 1;
}


__noinline void proxy12_bombQueue_enqueue(BombDisplay insertItem)
{
    eflash.bank = 2;
	bombQueue_enqueue_2(insertItem);
    eflash.bank = 1;
}


__noinline bool proxy21_doCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2)
{
    eflash.bank = 1;
	bool r = doCirclesOverlap_1(x1, y1, r1, x2, y2, r2);
    eflash.bank = 2;
	return r;

}

__noinline bool proxy31_doCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2)
{
    eflash.bank = 1;
	bool r = doCirclesOverlap_1(x1, y1, r1, x2, y2, r2);
    eflash.bank = 3;
	return r;

}

__noinline void proxy12_incLevel(bool round)
{
	eflash.bank = 2;
	incLevel_2(round);
	eflash.bank = 1;
}

__noinline void proxy12_setTunes(bool isOn)
{
	eflash.bank = 2;
	setTunes_2(isOn);
	eflash.bank = 1;
}

__noinline  void proxy12_decLevel()
{
	eflash.bank = 2;
	decLevel_2();
	eflash.bank = 1;
}

__noinline void  proxy12_printLargeString(char cy, const char *s, bool withRoundScore, bool withLevelScore, bool withSubText)
{
	eflash.bank = 2;
	printLargeString_2(cy, s, withRoundScore, withLevelScore, withSubText);
	eflash.bank = 1;
}

__noinline void proxy21_handleDelete(byte id)
{
	eflash.bank = 1;
	handleDelete_1(id);
	eflash.bank = 2;
}

__noinline void proxy_memcpy(byte destBank, byte fromBank, void * dst, const void * src, int size)
{
	eflash.bank = destBank;
	memcpy(dst, src, size);
	eflash.bank = fromBank;
}

__noinline void proxy_drawImageUnclipped(byte destBank, byte fromBank, byte* byte_stream, byte x, byte y, byte width, byte height)
{
	eflash.bank = destBank;
	drawImageUnclipped(byte_stream, x, y, width, height);
	eflash.bank = fromBank;
}

__noinline void proxy12_colorAreaCircle(unsigned char x0, unsigned char y0, unsigned char radius, byte backc, bool onlyBack)
{
	eflash.bank = 2;
	colorAreaCircle_2(x0, y0, radius, backc, onlyBack);
	eflash.bank = 1;
}

__noinline struct Box proxy13_getBoundingBoxCircle(float x1, float y1, float radius, int radius2)
{
	eflash.bank = 3;
	struct Box r = getBoundingBoxCircle_3(x1, y1, radius, radius2);
	eflash.bank = 1;
	return r;
}

__noinline struct Box proxy13_getBoundingBoxChar(char x1, char y1, char w, char h)
{
	eflash.bank = 3;
	struct Box r = getBoundingBoxChar_3(x1, y1, w, h);
	eflash.bank = 1;
	return r;
}

__noinline void proxy12_hideGameObject(byte id)
{
	eflash.bank = 2;
	hideGameObject_2(id);
	eflash.bank = 1;
}

__noinline void proxy12_killBall(byte id)
{
	eflash.bank = 2;
	killBall_2(id);
	eflash.bank = 1;
}

__noinline void proxy12_integer_write(char cx, char cy, int i)
{
	eflash.bank = 2;
	integer_write_2(cx, cy, i);
	eflash.bank = 1;

}

__noinline void proxy12_titleFadeOut()
{
	eflash.bank = 2;
	titleFadeOut_2();
	eflash.bank = 1;
}

__noinline void proxy12_intro()
{
	eflash.bank = 2;
	intro_2();
	eflash.bank = 1;
}

__noinline float proxy13_qsqrt(float number)
{
	eflash.bank = 3;
	float ret = qsqrt_3(number);
	eflash.bank = 1;
	return ret;
}

__noinline float proxy23_qsqrt(float number)
{
	eflash.bank = 3;
	float ret = qsqrt_3(number);
	eflash.bank = 2;
	return ret;
}

__noinline bool proxy13_simulatePhysics(byte id)
{
	eflash.bank = 3;
	byte ret = simulatePhysics_3(id);
	eflash.bank = 1;
	return ret;
}

__noinline void proxy23_saveSimulationStart(byte id)
{
	eflash.bank = 3;
	saveSimulationStart_3(id);
	eflash.bank = 2;
}

__noinline void proxy13_restoreSimulationFinish(byte id)
{
	eflash.bank = 3;
	restoreSimulationFinish_3(id);
	eflash.bank = 1;
}

__noinline void proxy12_particle_add(int px, int py, int vx, int vy)
{
	eflash.bank = 2;
	particle_add_2(px, py, vx, vy);
	eflash.bank = 1;
}



inline void registerCallBank(byte fromBank)
{
	callBank = fromBank;
}

void playSfx(byte num)
{
	if (playTunes || (num == SND_BUMP && bumptimer > 0))
	{
		return;
	}

	// if(voice > 1)
	// {
	// 	voice = 0;
	// }
	// else
	// {
	// 	voice++;
	// }

	if(num == SND_BUMP)
	{
		bumptimer = 10;
	}
	
	__asm
	{
        lda num
        ldy #$02
        jsr $cc4a	
    };
}

void playSubtune(byte num)
{
	if(!playTunes)
	{
		return;
	}
	__asm
	{
		lda num
		jsr		$c000
	}
}

void cleanScreen(bool withScore)
{
	memset(Hires, 0, 8000);	
	mmap_set(MMAP_RAM);
	memset(Color1, GCOL_WHITE, 1000);
	mmap_set(MMAP_ROM);
	memset(Color1, VCOL_WHITE, 1000);
	if(withScore)
	{
		memset(Color2, 0x20, 80);
	}
}

void music_play(void)
{
	if(isMusicFf)
	{
		return;
	}

	__asm
	{
		jsr		$c003
	}
	if(cntMusic < MaxCntMusic)
	{
		cntMusic++;
	}
	else
	{
		cntMusic = 0;
	}
}

void music_ff(void)
{
	__asm
	{
		lda	#$0
		jsr $c006
	}
	
	spr_show(0, false);
	isMusicFf = true;
	for(signed int i=0;i < MaxCntMusic;i++)
	{
		__asm
		{
			jsr		$c003
		}
	}
	spr_show(0, true);
	isMusicFf = false;
	__asm
	{
		lda	#$0f
		jsr $c006
	}
}

__native inline bool IsBitSet(const byte comp, const byte test)
{
	return 0 != (comp & test);
}

void copyTitle()
{
	memcpy(Hires, ImgHires, 8000);
	memcpy(Color2, ImgCol, 0x03f8);
	mmap_set(MMAP_RAM);
	memcpy(Color1, Color2, 0x03f8);	
	mmap_set(MMAP_ROM);
}

void drawImageUnclipped(byte* byte_stream, byte x, byte y, byte width, byte height) 
{
	// __assume(x < 40);	
	// __assume(y < 25);	
	// __assume(width < 40);	
	// __assume(height < 25);	
    // Calculate the starting address of the area to be filled
    unsigned char *screen = (unsigned char *)Hires;
	int pos = 0;
    
    // Iterate through each row in the specified height
	unsigned char *row_ptr = screen;
    for (int row = 0; row < height; ++row) {
        // Calculate the offset for the current row
        row_ptr = screen + (y * 8 * 40) + row * 8 * 40 + x * 8;

        // Fill the specified width in the current row with the pattern
        for (int col = 0; col < width * 8; ++col) {
            row_ptr[col] = byte_stream[pos + col];
        }
		pos += width * 8;
    }
}

void copyCharset()
{
	mmap_set(MMAP_ROM);
	memcpy(Buffer, TextChars, 0x300);
	mmap_set(MMAP_RAM);
	memcpy(Charset, Buffer, 0x300);
	mmap_set(MMAP_ROM);
}

void drawFrame()
{
	// bmu_rect_pattern(&Screen, 0, 8, 6, 232, Patterns[PAT_DANGER]);
	// bmu_rect_pattern(&Screen, 314, 8, 6, 232, Patterns[PAT_DANGER]);
}

void colorChar(unsigned char x, unsigned char y, byte backc, char* col, bool onlyBack)
{
	byte t = 1;
	if(onlyBack == false)
	{
		t = 0;
	}
	if(x >= 0 && x <= 39 && y >= t && y <= 24)
	{
		byte b = col[40 * y + x];
		if(onlyBack)
		{
			col[40 * y + x] = change_low_nibble(b, backc);
		}
		else
		{
			col[40 * y + x] = backc;
		}
	}
}

__native inline bool checkAabbCollision(const struct Box *b1, const struct Box *b2)
{
    // Cache the values to avoid redundant memory accesses
    int b1x = b1->x, b1w = b1->width; 
	byte b1y = b1->y, b1h = b1->height; 
    int b2x = b2->x, b2w = b2->width; 
	byte b2y = b2->y, b2h = b2->height; 

    // Perform the checks with the cached values
    return b1x < b2x + b2w &&
           b1x + b1w > b2x &&
           b1y < b2y + b2h &&
           b1y + b1h > b2y;
}

void colorArea(byte x, byte y, byte w, byte h, byte fcol, byte bcol)
{
	// Calculate the intersection of the bounding box and the clip rectangle
	byte startX = (x < clipX) ? clipX : x;
	byte startY = (y < clipY) ? clipY : y;
	byte endX = ((x + w) > (clipX + clipW)) ? (clipX + clipW) : (x + w);
	byte endY = ((y + h) > (clipY + clipH)) ? (clipY + clipH) : (y + h);
	byte c;

	mmap_set(MMAP_RAM);
	// Loop through each 8x8 block within the pixelized bounding box
	if(bcol != 0xff)
	{
		for (byte i = startY; i < endY; i++)
		{
			for (byte j = startX; j < endX; j++)
			{
				c = Color1[40 * i + j];
				Color1[40 * i + j] = change_low_nibble(c, bcol); 
			}
		}
	}
	if(fcol != 0xff)
	{
		for (byte i = startY; i < endY; i++)
		{
			for (byte j = startX; j < endX; j++)
			{
				c = Color1[40 * i + j];
				Color1[40 * i + j] = change_high_nibble(c, fcol); 
			}
		}
	}
	mmap_set(MMAP_ROM);
}

// Function to change the higher nibble of a byte
byte change_high_nibble(byte b, byte c) {
	b &= 0x0f;
	b = (byte)(c | b);
	return b;
}

// Function to change the lower nibble of a byte
byte change_low_nibble(byte b, byte c) {
	b &= 0xf0;
	b = (byte)((byte)(c >> 4) | (byte)b);
	return b;
}


void colorAreaUnclipped(byte x, byte y, byte w, byte h, byte c, bool isHires)
{
	// Calculate the intersection of the bounding box and the clip rectangle
	int endX = x + w;
	int endY = y + h;

	if(isHires)
	{
		mmap_set(MMAP_RAM);
	}
	else
	{
		mmap_set(MMAP_ROM);
	}

	// Loop through each 8x8 block within the pixelized bounding box
	for (int i = y; i < endY; i++)
	{
		for (int j = x; j < endX; j++)
		{
			Color1[40 * i + j] = c;
		}
	}
	mmap_set(MMAP_ROM);
}

__interrupt void menu_interrupt()
{
	joy_poll(0);

	music_play();
	timer++;

	if(highlight.timer == 0xff)
	{
		return;
	}

	if(highlight.timer > 0 && state != GS_TITLE_FADE_OUT && state != GS_INTRO)
	{
		colorAreaUnclipped(highlight.x,highlight.y,highlight.w,highlight.h,lumFromBlack[highlight.timer % 32], highlight.isHires);

		highlight.timer--;
	}
	else
	{
		highlight.timer = 64;
	}
}

__interrupt void joy_interrupt()
{
	// Poll joystick
	joy_poll(0);
	timer++;
	signed char r = 1;

	if(breakMusic)
	{
		if(playTunes)
		{
			music_play();
		}
		else
		{
		    music_play();
			__asm
			{
				jsr $cea9
			};
		}
	}

	if(highlight.timer > 0)
	{
		colorAreaUnclipped(highlight.x,highlight.y,highlight.w,highlight.h,lumFromBlack[highlight.timer % 32], highlight.isHires);
		highlight.timer--;
		if(highlight.timer == 0)
		{
			colorAreaUnclipped(highlight.x,highlight.y,highlight.w,highlight.h,highlight.restoreColor, highlight.isHires);
		}
	}

	if(tray > 127)
	{
		tx = -sintab[tray - 128];
	}
	else
	{
		tx = sintab[tray];
	}

	byte posY;
	if(state != GS_RUN_COMPLETED && state != GS_FINALIZE)
	{
		posY = posYTray;
		if (timer % 2 == 0)
		{
			tray++;
		}
	}
	else
	{
		posY = 255;
	}
	spr_set(6, true, 160 - 23 + tx, posY, 64 + traySprite, VCOL_WHITE, false, true, false);
	spr_set(7, true, 160 + 23 + tx, posY, 64 + traySprite + 1, VCOL_WHITE, false, true, false);
}

// Set a pixel at the given coordinate
void pix_set(unsigned px, unsigned py, bool isConfetti)
{
	// Give the compiler a hand
	__assume(px < 320);
	__assume(py < 200);

	// Calculate base position in hires
	char * dp = Hirows[py >> 3] + (px & ~7);

	// Set the pixel
	dp[py & 7] ^= setmask[px & 7];
	if(isConfetti)
	{
		dp[(py + 1) & 7] ^= setmask[px & 7];
		dp[py & 7] ^= setmask[(px + 1) & 7];
		dp[(py + 1) & 7] ^= setmask[(px + 1) & 7];
	}
}

#pragma code(bcode1)
#pragma data(bdata1)

// --------------- BANK 1 Logic



inline float max_1(float x1, float x2)
{
	if (x1 > x2)
		return x1;
	else
		return x2;
}

inline float min_1(float x1, float x2)
{
	if (x1 < x2)
		return x1;
	else
		return x2;
}

void delayedActions_1()
{
	if(bumptimer > 0)
	{
		bumptimer--;
	}
	

	if (timer % TimeTimedScore == 0)
	{
		ScoreDisplay scoreDisplay = proxy12_scoreQueue_dequeue();
		if (scoreDisplay.score != 0)
		{
			proxy0_put_score(scoreDisplay.x, scoreDisplay.y, scoreDisplay.score);
		}
		else if (scoreDisplay.textId != 0xff)
		{
			proxy0_put_text(scoreDisplay.x, scoreDisplay.y, scoreDisplay.textId);
		}
	}
	if(timer % TimeBombDelay == 0)
	{
		BombDisplay bombDisplay = proxy12_bombQueue_dequeue();

		if(bombDisplay.radius != 0)
		{
			proxy12_colorAreaCircle(bombDisplay.x, bombDisplay.y, bombDisplay.origRadius, GCOL_BLACK, true);
			vic.color_border = VCOL_BLACK;
			if(bombDisplay.radius > 3)
			{
				BombDisplay newBombDisplay ;

				newBombDisplay.x = bombDisplay.x;
				newBombDisplay.y = bombDisplay.y;
				newBombDisplay.origRadius = bombDisplay.radius;
				newBombDisplay.radius = bombDisplay.radius - 2;
				newBombDisplay.color = GCOL_DARK_GREY;

				proxy12_bombQueue_enqueue(newBombDisplay);
				proxy12_colorAreaCircle(newBombDisplay.x, newBombDisplay.y, newBombDisplay.radius, blues8[newBombDisplay.radius % 8], true);
			}
		}
	}

	if (timer % TimeTimedDelay == 0)
	{
		byte nextCollide = proxy12_collideQueue_dequeue();
		if (nextCollide >= 0 && nextCollide != 0xff)
		{
			proxy12_hideGameObject(nextCollide);
		}
	}

	if(state == GS_FIRE || state == GS_SIMULATE)
	{
		if (timer % 8 == 0)
		{
			proxy12_dispatchEvent(EventInterval8, 0xff, 0xff, 0xff);
		}
		if (timer % 16 == 0)
		{
			proxy12_dispatchEvent(EventInterval16, 0xff, 0xff, 0xff);
		}
		if (timer % 32 == 0)
		{
			proxy12_dispatchEvent(EventInterval32, 0xff, 0xff, 0xff);
		}
	}
}

void gameLoopLevelInit()
{
	proxy12_setTunes(true);
	playSubtune(SUB_LEVEL_BUILD);	
	proxy13_preInitScene();
	proxy14_initScene();
	proxy13_postInitScene();
	playSubtune(SUB_LEVEL_START);	


	extraBalls = false;
	completed = false;
	proxy12_showBalls();
	proxy12_showMultiplier(1);
	state = GS_LEVEL;
	spr_show(1, false);
	spr_show(2, false);

	//updateRender_1(largeTextId, true);
//	globalOrientation = ORIENTATION_DOWN;
//	gravity = Gravity;
}

void paintLevMenu_1()
{
	char buffer [3];
	itoa (level + 1,buffer,10);	
	proxy0_bm_rect_clear(&Screen, &scr, 160 + (70 / 2), 104, 16, 8);
	proxy0_put_string(&Screen, &scr, 160 + (70 / 2), 104, buffer, BLTOP_COPY);
}

void frameLoop_1()
{
	char sub[] = "Press FIRE to start";
	char startTxt[] = "Start Game";
	char levelTxt[] = "Starting Level: ";
	char backTxt[] = "Back";
	char authorTxt[] = "code: andy daumann";
	char author2Txt[] = "music: picrard";
	// char scoreTxt[] = "Score:";
	// char hiscoreTxt[] = "Hiscore:";

	char titleTxt[] = "ORBIX";

	if(state == GS_TITLE_INIT)
	{
		highlight.timer = 0xff;
		timer = 0;
		rirq_stop();
		proxy17_copyTitle();
		proxy_memcpy(8, 1, Buffer, Music, 0xF00);
		playSubtune(0);
		rasterMenuInit(true);

		// Switch to hires mode
		vic_setmode(VICM_HIRES, Color1, Hires);
		vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | VIC_CTRL1_RSEL | 3;
		spr_init(Color1);

		bm_init(&Screen, Hires, 40, 25);
		for (byte i = 0; i < 8; i++)
		{
			spr_show(i, false);
		}

		titlePhase = 0;
		state = GS_TITLE;
	}
	else if(state == GS_TITLE)
	{
		if(timer == 128 && titlePhase == 0)
		{
			proxy_drawImageUnclipped(8,1, ImgLogo, 8,4,24, 5);
			colorAreaUnclipped(8,4,24,5, 0x10, true);
			titlePhase = 1;
		}
		if(timer == 255 && titlePhase == 1)
		{
			char subH[20];

			// strcpy(subH, hiscoreTxt);
			// strcat(subH, hiscore);
			//proxy0_put_string(&Screen, &scr, 118, 144, scoreTxt, BLTOP_COPY);
			// proxy0_bm_rect_fill(&Screen, &scr, 96, 64, 128, 24);
			// proxy0_bm_rect_clear(&Screen, &scr, 97, 65, 126, 23);
			// colorAreaUnclipped(12,8,16,8,0x10, true);
			// proxy0_put_string(&Screen, &scr, 96, 168, subH, BLTOP_COPY);
			//colorAreaUnclipped(12,22,12,1,0xf1, true);
			proxy0_put_string(&Screen, &scr, 118, 176, sub, BLTOP_COPY);
			colorAreaUnclipped(14,22,12,1,0xf1, true);
			titlePhase = 2;
		}

		if(joyb[0])
		{
			state = GS_MENU_INIT;
		}
	}
	if(state == GS_MENU_INIT)
	{
		rirq_stop();
		proxy17_copyTitle();
		proxy_memcpy(8, 1, Buffer, Music, 0xF00);
		rasterMenuInit(true);
		proxy0_bm_rect_fill(&Screen, &scr, 96, 64, 128, 64);
		proxy0_bm_rect_clear(&Screen, &scr, 97, 65, 126, 62);
		colorAreaUnclipped(12,8,16,8,0x10, true);
		proxy0_put_string(&Screen, &scr, 1, 192, authorTxt, BLTOP_COPY);
		proxy0_put_string(&Screen, &scr, 258, 192, author2Txt, BLTOP_COPY);
		colorAreaUnclipped(0,24,40,1,0xe1, true);
		proxy0_put_string(&Screen, &scr, 160 - (30 / 2), 72, titleTxt, BLTOP_COPY);
		proxy0_put_string(&Screen, &scr, 160 - (50 / 2), 96, startTxt, BLTOP_COPY);
		proxy0_put_string(&Screen, &scr, 160 - (70 / 2), 104, levelTxt, BLTOP_COPY);
		proxy0_put_string(&Screen, &scr, 160 - (20 / 2), 112, backTxt, BLTOP_COPY);
		menuItem = 0;
		proxy12_startHighlight(13,12, 14, 1, true, GCOL_WHITE);
		paintLevMenu_1();

		state = GS_MENU;
	}
	else if(state == GS_MENU)
	{
		if(joyb[0] && menuItem <= 1)
		{
			state = GS_GAME_INIT;
			// TODO
			//state = GS_TITLE_FADE_OUT;
		}
		if(joyb[0] && menuItem == 2)
		{
			state = GS_TITLE_INIT;
		}
		if(joyy[0] > 0 && menuItem <= 1 && timer % 8 == 0)
		{
			colorAreaUnclipped(12,8,16,8,0x10, true);
			menuItem++;
			proxy12_startHighlight(13,12 + menuItem, 14, 1, true, GCOL_WHITE);
		}
		if(joyy[0] < 0 && menuItem >= 1 && timer % 8 == 0)
		{
			colorAreaUnclipped(12,8,16,8,0x10,true);
			menuItem--;
			proxy12_startHighlight(13,12 + menuItem, 14, 1, true, GCOL_WHITE);
		}

		if(joyx[0] < 0 && menuItem == 1 && timer % 8 == 0)
		{
			proxy12_decLevel();
			paintLevMenu_1();
	
		}
		if(joyx[0] > 0 && menuItem == 1 && timer % 8 == 0)
		{
			proxy12_incLevel(false);
			paintLevMenu_1();
		}
		
	}
	else if(state == GS_TITLE_FADE_OUT)
	{
		proxy12_titleFadeOut();
		state = GS_INTRO;
	}
	else if(state == GS_INTRO)
	{
		proxy0_cleanScreen(false);
		proxy12_intro();
		state = GS_GAME_INIT;
	}
	else if(state == GS_GAME_INIT)
	{
		mmap_set(MMAP_ROM);	
		highlight.timer = 0;
		rirq_stop();
		proxy13_copyCharset();		
		//proxy_memcpy(8, 1, Buffer, MusicInGame, 0x800);
		proxy_memcpy(6, 1, Sfx, Sounds, 0x400);
		proxy12_setTunes(true);
		playSubtune(SUB_LEVEL_START);	
		rasterInit(true);
		proxy0_cleanScreen(true);
		// globalOrientation = ORIENTATION_DOWN;
		// gravity = Gravity;
		const char topLine[40] = s"SC:00000000   X1 BALLS:--    HI:00000000";
		proxy12_string_write(0, 0, topLine, VCOL_WHITE);
		proxy12_score_reset();
	
		state = GS_LEVEL_INIT;	}
	else if(state == GS_LEVEL_INIT)
	{
		proxy0_cleanScreen(false);

		proxy0_drawFrame();
		while(state != GS_TITLE_INIT)
		{
			gameLoop_1();
			rirq_wait();
		}
	}
}

void gameLoop_1()
{
	numColPairs = 0;
	numFakes = 0;
	byte currentValueableObjects = 0;

	struct GameObject *go;

	if (state == GS_LEVEL_INIT)
	{
		gameLoopLevelInit();
		currentValueableObjects = 0xff;
	}
	else if (state == GS_LEVEL)
	{
		proxy12_executeLargeTextTimer();
		if (largeTextTimer == 0)
		{
			proxy12_setTunes(false);
			state = GS_FIRE;
		}
		currentValueableObjects = 0xff;
	}
	else if (state == GS_FIRE)
	{
		stuckCnt = 0;
		proxy12_updateFace();
		for (byte i = 0; i < numObjects; i++)
		{
			go = &gameObjects[i];
			if (IsBitSet(go->comp0, Component0_DrawCannon))
			{
				if (currentCannon == i)
				{
					proxy12_updateCannon(i);
					// updateRender_1(i, false);
				}
			}
			if (IsBitSet(go->comp0, Component0_Render))
			{
				updateRender_1(i, false);
			}
			if (IsBitSet(go->comp0, Component0_Physics) && i != mainBall)
			{
				updatePhysics_1(i);
			}
			if (IsBitSet(go->comp2, Component2_Destroyed))
			{
				proxy12_updateDestroyed(i);
			}
			if (IsBitSet(go->comp1, Component1_Valueable))
			{
				currentValueableObjects++;
			}
			if (IsBitSet(go->comp3, Component3_CollisionTimerAble))
			{
				proxy12_execCollisionTimer(i);
			}
		}
		startValueableObjects = currentValueableObjects;
		orbzScore = 0;
		roundScore = 0;
		proxy12_executeLargeTextTimer();
		delayedActions_1();
		updateCollisions_1();
	}
	else if (state == GS_SIMULATE)
	{
		currentValueableObjects = 0xff;
//		updateRender_1(mainBall, false);
		byte endSim = proxy13_simulatePhysics(mainBall);
		updateCollisions_1();
		simSteps++;
		if(endSim == SIM_CHANGE)
		{
			state = GS_SIMULATE_FINISH;
		}
		if(endSim != SIM_DONE)
		{
			updateRender_1(mainBall, false);
		}
		if(endSim == SIM_END || simSteps > 63)
		{
			simDone = true;
		}
		delayedActions_1();
	}
	else if( state == GS_SIMULATE_FINISH)
	{
		currentValueableObjects = 0xff;
		proxy13_restoreSimulationFinish(mainBall);
		state = GS_FIRE;
	}
	else if (state == GS_RUN)
	{
		proxy12_updateFace();
		// if (joyy[0] > 0 && joyb[0] != 0)
		// {
		// 	proxy12_killBall(mainBall);
		// }


		for (loopo = 0; loopo < numObjects; loopo++)
		{
			go = &gameObjects[loopo];
			if (IsBitSet(go->comp0, Component0_Render))
			{
				updateRender_1(loopo, false);
			}
			if (IsBitSet(go->comp0, Component0_Physics))
			{
				updatePhysics_1(loopo);
			}
			if (IsBitSet(go->comp2, Component2_Destroyed))
			{
				proxy12_updateDestroyed(loopo);
			}

			if (IsBitSet(go->comp1, Component1_Valueable))
			{
				currentValueableObjects++;
			}
			if (IsBitSet(go->comp3, Component3_CollisionTimerAble))
			{
				proxy12_execCollisionTimer(loopo);
			}
		}
		proxy12_executeLargeTextTimer();
		delayedActions_1();
		updateCollisions_1();
	}
	else if (state == GS_WAITCLEAN)
	{
		proxy12_updateFace();
		byte remainBalls = 0;
		remainTimer--;
		for (byte i = 0; i < numObjects; i++)
		{
			go = &gameObjects[i];
			if (IsBitSet(go->comp0, Component0_Render))
			{
				updateRender_1(i, false);
			}
			if (IsBitSet(go->comp0, Component0_Physics))
			{
				updatePhysics_1(i);
			}
			if (IsBitSet(go->comp2, Component2_Destroyed))
			{
				proxy12_updateDestroyed(i);
			}
			if (IsBitSet(go->comp1, Component1_Valueable))
			{
				currentValueableObjects++;
			}
			if (IsBitSet(go->comp1, Component1_DrawBall) && !IsBitSet(go->comp1, Component1_MainBall) && !IsBitSet(go->comp2, Component2_Destroyed) && go->isOn)
			{
				remainBalls++;
			}
			if (IsBitSet(go->comp3, Component3_CollisionTimerAble))
			{
				proxy12_execCollisionTimer(i);
			}
		}
		if (remainBalls == 0 || remainTimer == 0)
		{
			go = &gameObjects[mainBall];

			go->comp0 |= Component0_Physics;			
			resetBall_1(mainBall);
			if (balls > 0)
			{
				if (!extraBalls)
				{
					balls--;
				}
				proxy12_showBalls();
				state = GS_RUN_FINISH_INIT;
			}
			else
			{
				state = GS_RUN_FINISH_INIT;
			}
		}
		proxy12_executeLargeTextTimer();
		delayedActions_1();
		updateCollisions_1();
	}
	else if (state == GS_RUN_FINISH_INIT)
	{
		roundScore = orbzScore * (startValueableObjects - valueableObjects);
		proxy12_setTunes(true);
		if (!completed)
		{
			gameObjects[largeTextId].isOn = true;
			if (extraBalls)
			{
				gameObjects[largeTextId].pattern = 2;
				playSubtune(SUB_BALL_TRAY);	
				proxy12_doFirework(1);
				playSubtune(SUB_BALL_SAVED);	
			}
			else
			{
				playSubtune(SUB_BALL_LOST);	
				gameObjects[largeTextId].pattern = 1;
			}
			gameObjects[largeTextId].orientation = ORIENTATION_DOWN;
			gameObjects[largeTextId].boundingBox = proxy13_getBoundingBoxChar(0, (byte)gameObjects[largeTextId].py, 40, 3);
			largeTextTimer = LARGE_TEXT_TIMER;
			proxy12_score_inc(0, roundScore);
			updateRender_1(largeTextId, true);
			state = GS_RUN_FINISH;
			currentValueableObjects = 0xff;
		}
		else
		{
			levelScore = balls * roundScore;
			playSubtune(SUB_LEVEL_FINISHED);	
			gameObjects[largeTextId].isOn = true;
			gameObjects[largeTextId].pattern = 4;
			gameObjects[largeTextId].orientation = ORIENTATION_UP;
			gameObjects[largeTextId].boundingBox = proxy13_getBoundingBoxChar(0, (byte)gameObjects[largeTextId].py, 40, 3);
			largeTextTimer = LARGE_TEXT_TIMER * 3;
			proxy12_score_inc(0, levelScore);
			updateRender_1(largeTextId, true);

			state = GS_RUN_FINISH;
		}
	}
	else if (state == GS_RUN_FINISH)
	{
		proxy12_executeLargeTextTimer();
		if (largeTextTimer == 0)
		{
			proxy12_setTunes(false);	

			if (completed)
			{
				state = GS_LEVEL_INIT;
				proxy12_incLevel(true);
			}
			else
			{
				if(balls == 0)
				{
					state = GS_GAMEOVER_INIT;
				}
				else
				{
					state = GS_FIRE;
				}
			}
		}
		extraBalls = false;
		currentValueableObjects = 0xff;
	}
	else if (state == GS_RUN_COMPLETED_INIT)
	{
		gameObjects[largeTextId].isOn = true;
		gameObjects[largeTextId].pattern = 3;
		gameObjects[largeTextId].orientation = 0x7f;
		gameObjects[largeTextId].boundingBox = proxy13_getBoundingBoxChar(0, (byte)gameObjects[largeTextId].py, 40, 2);
		largeTextTimer = LARGE_TEXT_TIMER;

		proxy12_updateFace();
		updateRender_1(largeTextId, true);
		proxy12_setTunes(true);
		playSubtune(SUB_ALL_CLEAR);
		proxy12_doFirework(0);
		for (byte i = 0; i < numObjects; i++)
		{
			struct GameObject *target;
			target = &gameObjects[i];
			if (IsBitSet(target->comp3, Component3_NoBonusImage))
			{
				proxy0_bm_rect_clear(&Screen, &scr, (byte)(target->px) * 8, (byte)target->py * 8, (byte)target->ax * 8, (byte)target->ay * 8);
				target->isOn = false;
			}
		}
		proxy13_initBonusLine();
		updateRender_1(mainCannon, true);
		state = GS_RUN_COMPLETED;
		currentValueableObjects = 0xff;
	}
	else if (state == GS_RUN_COMPLETED)
	{
		proxy12_updateFace();
		proxy12_executeLargeTextTimer();
		if (largeTextTimer == 0)
		{
			remainTimer = 800;
			state = GS_FINALIZE;
		}
		currentValueableObjects = 0xff;
	}
	else if (state == GS_GAMEOVER_INIT)
	{
		proxy12_setTunes(true);
		playSubtune(SUB_GAMEOVER);
		gameObjects[largeTextId].isOn = true;
		gameObjects[largeTextId].pattern = 5;
		gameObjects[largeTextId].orientation = 0x7f;
		gameObjects[largeTextId].boundingBox = proxy13_getBoundingBoxChar(0, (byte)gameObjects[largeTextId].py, 40, 3);
		largeTextTimer = LARGE_TEXT_TIMER;
		for (char i = 0; i < 8; i++)
		{
			hiscore[i] = Color2[i+32];
		}

		updateRender_1(largeTextId, true);
		state = GS_GAMEOVER;
		currentValueableObjects = 0xff;
	}
	else if (state == GS_GAMEOVER)
	{
		proxy12_updateFace();
		proxy12_executeLargeTextTimer();
		if (largeTextTimer == 0)
		{
			proxy12_score_reset();
			state = GS_TITLE_INIT;
		}
		remainTimer = 500;
		currentValueableObjects = 0xff;
	}
	else if (state == GS_FINALIZE)
	{
		proxy12_updateFace();
		remainTimer--;
		for (byte i = 0; i < numObjects; i++)
		{
			go = &gameObjects[i];
			if (IsBitSet(go->comp0, Component0_Render))
			{
				updateRender_1(i, false);
			}
			if (IsBitSet(go->comp0, Component0_Physics))
			{
				updatePhysics_1(i);
			}
			if (IsBitSet(go->comp2, Component2_Destroyed))
			{
				proxy12_updateDestroyed(i);
			}
			if (IsBitSet(go->comp3, Component3_CollisionTimerAble))
			{
				proxy12_execCollisionTimer(i);
			}
		}
		delayedActions_1();
		updateCollisions_1();
		if(remainTimer == 0)
		{
			state = GS_RUN_FINISH_INIT;
		}
	}
	if(state != GS_FINALIZE)
	{
		valueableObjects = currentValueableObjects;
		if(valueableObjects == 0 && !completed)
		{
			completed = true;
			faceState = FA_SUNNY;
			faceTimer = 64;

			state = GS_RUN_COMPLETED_INIT;
		}
	}
}

void quad_fill_1(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const char *pat)
{
	proxy0_bm_triangle_fill(bm, clip, x0, y0, x1, y1, x2, y2, pat);
	proxy0_bm_triangle_fill(bm, clip, x1, y1, x2, y2, x3, y3, pat);
	proxy0_bm_triangle_fill(bm, clip, x3, y3, x0, y0, x2, y2, pat);
}

void updateRender_1(byte id, bool init)
{
	struct GameObject *go;
	go = &gameObjects[id];

	if (IsBitSet(go->comp1, Component1_DrawBall))
	{
		if(!(state == GS_FIRE && id == mainBall))
		{
			if(state == GS_SIMULATE || state == GS_SIMULATE_FINISH)
			{
				int px = (int)go->px * 64;
				int py = (int)go->py * 64;

				proxy12_particle_add(px, py, 0, 0);
				proxy0_pix_set(px >> 6, py >> 6, false);
			}
			else
			{
				proxy0_spr_set(go->spriteNum, true, (int)go->px + 12, (int)go->py + 40, go->pattern, go->color, false, false, false);
			}	
		}
	}
	if(init && IsBitSet(go->comp3, Component3_DrawImage))
	{
		if (go->isOn)
		{
			proxy14_drawBackgroundImage(id);
		}
	}
	if (init && IsBitSet(go->comp2, Component2_LargeText))
	{
		if (go->isOn)
		{
			proxy12_colorBoundingBox(id);
			if (go->orientation == ORIENTATION_DOWN)
			{
				proxy12_printLargeString((char)(go->py), texts[go->pattern], true, false, false);
			}
			else if (go->orientation == ORIENTATION_UP)
			{
				proxy12_printLargeString((char)(go->py), texts[go->pattern], false, true, false);
			}
			else if(go->orientation == ORIENTATION_LEFT)
			{
				proxy12_printLargeString((char)(go->py), texts[go->pattern], false, false, true);
			}
			else
			{
				proxy12_printLargeString((char)(go->py), texts[go->pattern], false, false, false);
			}
		}
	}
	if (IsBitSet(go->comp0, Component0_DrawCircle))
	{
		proxy12_colorBoundingBox(id);
		if (IsBitSet(go->comp1, Component1_CrackDelete))
		{
			//proxy0_bm_circle_fill(&Screen, &scr, (int)go->px + 3, (int)go->py + 3, (int)go->radius, NineShadesOfGrey[2]);
			if (IsBitSet(go->comp0, Component0_CollideAble))
			{
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[8]);
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 1, NineShadesOfGrey[3 + go->crackLevel]);
			}
			else
			{
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[5]);
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 1, NineShadesOfGrey[0]);
			}
		}
		else if (IsBitSet(go->comp2, Component2_DrawStar))
		{
			//proxy0_bm_circle_fill(&Screen, &scr, (int)go->px + 3, (int)go->py + 3, (int)go->radius, NineShadesOfGrey[2]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[8]);
			int rpx[10], rpy[10];
			float	r = go->radius-1;

			for(int j=0; j<10; j++)
			{			
				float	fx = px[j], fy = py[j];
				rpx[j] = (int)(go->px + fx * r);
				rpy[j] = (int)(go->py + fy * r);
			}
			//bm_polygon_nc_fill(&Screen, &scr, rpx, rpy, 10, NineShadesOfGrey[7]);
			for(int j=0; j<10; j++)
			{
				int k = (j + 1) % 10;
				proxy0_bm_line(&Screen, &scr, rpx[j], rpy[j], rpx[k], rpy[k], 0xff, LINOP_XOR);
			}

		}
		else if (IsBitSet(go->comp1, Component1_Switch))
		{
			byte pattern = PAT_TRUE;
			if (!go->isOn)
			{
				pattern = PAT_FALSE;
			}
			//proxy0_bm_circle_fill(&Screen, &scr, (int)go->px + 3, (int)go->py + 3, (int)go->radius, NineShadesOfGrey[2]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[8]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 1, Patterns[pattern]);
		}
		else if (IsBitSet(go->comp3, Component3_DrawLetter))
		{
			//proxy0_bm_circle_fill(&Screen, &scr, (int)go->px + 3, (int)go->py + 3, (int)go->radius, NineShadesOfGrey[2]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, Patterns[PAT_SOLID]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px , (int)go->py, (int)go->radius - 2, Patterns[PAT_BONUS_LINE]);
			proxy13_lchar_put((int)go->px - 8, (int)go->py - 8, go->pattern, 1);
		}
		else
		{
			if (IsBitSet(go->comp2, Component2_Kill))
			{
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py - 4, (int)go->radius, Patterns[PAT_FLAME]);
			}
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px + 3, (int)go->py + 3, (int)go->radius, NineShadesOfGrey[2]);
			if (IsBitSet(go->comp0, Component0_CollideAble))
			{
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[8]);
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 1, Patterns[go->pattern]);
			}
			else
			{
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[5]);
				proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 1, NineShadesOfGrey[0]);
			}
		}
	}
	if (IsBitSet(go->comp2, Component2_Laser))
	{
		float nx = -(go->ay - go->py);
		float ny = (go->ax - go->px);

		float qx = (go->ax - go->px);
		float qy = (go->ay - go->py);

		float dq = proxy13_qsqrt(qx * qx + qy * qy);
		qx /= dq;
		qy /= dq;

		float d = proxy13_qsqrt(nx * nx + ny * ny);
		nx /= d;
		ny /= d;
		proxy12_colorBoundingBox(id);
		const char *pat;
		byte linePattern = 0xff;
		int e0 = (int)(go->px + nx * go->radius);
		int e1 = (int)(go->py + ny * go->radius);
		int e2 = (int)(go->px + qx * go->radius * 2);
		int e3 = (int)(go->py + qy * go->radius * 2);
		int e4 = (int)(go->px - nx * go->radius);
		int e5 = (int)(go->py - ny * go->radius);

		if (IsBitSet(go->comp0, Component0_CollideAble))
		{
			pat = Patterns[go->pattern];
			proxy0_bm_line(&Screen, &scr, e2, e3, (int)go->ax, (int)go->ay, 0xff, LINOP_SET);
		}
		else
		{
			pat = NineShadesOfGrey[0];
			proxy0_bm_line(&Screen, &scr, e2, e3, (int)go->ax, (int)go->ay, 0x01, LINOP_SET);
		}

		proxy0_bm_triangle_fill(&Screen, &scr, e0, e1, e2, e3, e4, e5, pat);
		proxy0_bm_line(&Screen, &scr, e0, e1, e2, e3, 0xff, LINOP_SET);
		proxy0_bm_line(&Screen, &scr, e2, e3, e4, e5, 0xff, LINOP_SET);
		proxy0_bm_line(&Screen, &scr, e4, e5, e0, e1, 0xff, LINOP_SET);

		// bm_line(&Screen, &scr, e2 + 3, e3 + 3, go->ax + 3, go->ay + 3, 0x55, LINOP_SET);
		// bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, Patterns[PAT_SOLID]);
		// bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius-1, Patterns[PAT_DANGER]);
	}
	if (IsBitSet(go->comp0, Component0_DrawEdge))
	{
		// bmmc_circle_fill(&Screen, &scr, go->ax, go->ay, go->radius, NineShadesOfGrey[2]);

		float nx = -(go->ay - go->py);
		float ny = (go->ax - go->px);
		float d = proxy13_qsqrt(nx * nx + ny * ny);
		nx /= d;
		ny /= d;
		byte grey = 8;
		proxy12_colorBoundingBox(id);
		const char *pat;
		byte linePattern = 0xff;

		if (IsBitSet(go->comp1, Component1_CrackDelete))
		{
			grey = 3 + go->crackLevel;
			pat = NineShadesOfGrey[grey];
		}
		else
		{
			if (IsBitSet(go->comp0, Component0_CollideAble))
			{
				pat = Patterns[go->pattern];
			}
			else
			{
				linePattern = 0x55;
				pat = NineShadesOfGrey[0];
			}
		}
		float nxr = nx * go->radius;
		float nyr = ny * go->radius;

		int e0 = (int)(go->px + nxr);
		int e1 = (int)(go->py + nyr);
		int e2 = (int)(go->ax + nxr);
		int e3 = (int)(go->ay + nyr);
		int e4 = (int)(go->px - nxr);
		int e5 = (int)(go->py - nyr);
		int e6 = (int)(go->ax - nxr);
		int e7 = (int)(go->ay - nyr);
		//proxy0_bm_quad_fill(&Screen, &scr, e0 + 3, e1 + 3, e2 + 3, e3 + 3, e4 + 3, e5 + 3, e6 + 3, e7 + 3, NineShadesOfGrey[2]);

		quad_fill_1(&Screen, &scr, e0, e1, e2, e3, e4, e5, e6, e7, pat);

		proxy0_bm_line(&Screen, &scr, e0, e1, e2, e3, linePattern, LINOP_SET);
		proxy0_bm_line(&Screen, &scr, e4, e5, e6, e7, linePattern, LINOP_SET);

		proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius, NineShadesOfGrey[6]);
		proxy0_bm_circle_fill(&Screen, &scr, (int)go->ax, (int)go->ay, (int)go->radius, NineShadesOfGrey[6]);
	}
	if (IsBitSet(go->comp0, Component0_DrawCannon))
	{
		if(init)
		{
			proxy12_colorBoundingBox(id);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius , NineShadesOfGrey[8]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 2, NineShadesOfGrey[1]);
			proxy0_bm_circle_fill(&Screen, &scr, (int)go->px, (int)go->py, (int)go->radius - 4, NineShadesOfGrey[8]);
			if(id == mainCannon)
			{
				
				proxy0_spr_set(3, true, (int)go->px + 12 , (int)go->py + 40, 64 + 12, VCOL_BLACK, false,   false, false);
				proxy0_spr_set(4, true, (int)go->px + 12, (int)go->py + 40, 64 + 13, VCOL_MED_GREY, false,  false, false);
				proxy0_spr_set(5, true, (int)go->px + 12 , (int)go->py + 40, 64 + 14, VCOL_YELLOW , false, false, false);
				proxy12_updateFace();
			}
			proxy0_bm_line(&Screen, &scr, (int)go->px, (int)go->py, (int)(go->px + go->cannonVector.v[0]), (int)(go->py + go->cannonVector.v[1]), 0xff, LINOP_XOR);
		}
		else
		{
			struct GameObject *mainGo;
			mainGo = &gameObjects[mainBall];
			proxy0_spr_set(mainGo->spriteNum, true, (int)(go->px + go->cannonVector.v[0]) + 12, (int)(go->py + go->cannonVector.v[1]) + 40, mainGo->pattern, mainGo->color, false, false, false);
			proxy0_bm_line(&Screen, &scr, (int)go->px, (int)go->py, (int)(go->px + go->cannonVector.v[0]), (int)(go->py + go->cannonVector.v[1]), 0xff, LINOP_XOR);
		}
	}
}

void deleteRender_1(byte id, bool withUpdate)
{
	struct GameObject *go;
	struct GameObject *target;

	go = &gameObjects[id];
	if (IsBitSet(go->comp1, Component1_DrawBall))
	{
		spr_show(go->spriteNum, false);
	}
	else
	{
		proxy0_bm_rect_clear(&Screen, &scr, go->boundingBox.x, go->boundingBox.y, go->boundingBox.width, go->boundingBox.height);
		if(withUpdate)
		{
			for (byte i = 0; i < numObjects; i++)
			{
				if (i != id)
				{
					target = &gameObjects[i];

					if (checkAabbCollision(&(go->boundingBox), &(target->boundingBox)))
					{
						updateRender_1(i, true);
					}
				}
			}
		}
	}
}

__native inline bool doCirclesOverlap_1(float x1, float y1, float r1, float x2, float y2, float r2)
{
    // Cache the differences
    float dx = x1 - x2;
    float dy = y1 - y2;

    // Compute the squared distance between the two centers
    float distanceSquared = dx * dx + dy * dy;

    // Compute the squared sum of the radii
    float radiiSum = r1 + r2;
    float radiiSumSquared = radiiSum * radiiSum;

    // Check if the squared distance is less than or equal to the squared sum of radii
    return distanceSquared <= radiiSumSquared;
}

// __native bool doCirclesOverlap_1(float x1, float y1, float r1, float x2, float y2, float r2)
// {
// 	return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= ((r1 + r2) * (r1 + r2));
// };

inline bool isPointInCircle_1(float x1, float y1, float r1, float px, float py)
{
	return fabs((x1 - px) * (x1 - px) + (y1 - py) * (y1 - py)) < (r1 * r1);
};


void handleDelete_1(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	if (IsBitSet(go->comp3, Component3_ImmediateDelete))
	{
		if(valueableObjects <= 1 && IsBitSet(go->comp1, Component1_Valueable))
		{
			completed = true;
		}
		go->comp1 = go->comp1 & ~(Component1_Valueable);
		//proxy12_handleScore(id);
		proxy12_hideGameObject(id);
	}

	if (IsBitSet(go->comp1, Component1_TimedDelete))
	{
		// if (!proxy12_collideQueue_isQueued(id))
		// {
			if(valueableObjects == 1 && IsBitSet(go->comp1, Component1_Valueable))
			{
				completed = true;
			}
			if(!IsBitSet(go->comp3, Component3_Exploded))
			{
				proxy12_handleScore(id);
				proxy12_collideQueue_enqueue(id);
				go->comp1 = go->comp1 & ~(Component1_Valueable);
				go->color = GCOL_MED_GREY;
				go->comp3 |= Component3_Exploded;
				proxy12_colorBoundingBox(id);
			}
//		}
	}

	if (IsBitSet(go->comp1, Component1_CrackDelete))
	{
		if (go->crackLevel > 0)
		{
			go->crackLevel--;
			updateRender_1(id, false);
		}
		if(go->crackLevel == 1)
		{
			if(valueableObjects == 1 && IsBitSet(go->comp1, Component1_Valueable))
			{
				completed = true;
			}
		}
		if (go->crackLevel == 0)
		{
			if(valueableObjects == 1 && IsBitSet(go->comp1, Component1_Valueable))
			{
				completed = true;
			}
			go->comp1 = go->comp1 & ~(Component1_Valueable);
			proxy12_handleScore(id);
			proxy12_hideGameObject(id);
		}

	}
	if (completed && state != GS_FINALIZE)
	{
		state = GS_RUN_COMPLETED_INIT;
	}
}

void deactivateSecondaryBall_1(byte targetId)
{
	struct GameObject *target;
	target = &gameObjects[targetId];
	struct GameObject *depGo;

	byte sourceId = 0xff;
	for (byte j = 0; j < MaxDepObjects; j++)
	{
		if (depObjects[j].targetId == targetId)
		{
			sourceId = depObjects[j].sourceId;
			break;
		}
	}
	if (sourceId != 0xff)
	{
		depGo = &gameObjects[sourceId];
		target->isOn = false;
		target->px = depGo->px;
		target->py = depGo->py;
		target->ax = 0;
		target->ay = 0;
		target->vx = 0;
		target->vy = 0;
		target->pattern = 64;
		target->comp2 &= ~Component2_Destroyed;
		target->color = VCOL_LT_GREEN;

		updateRender_1(targetId, true);
	}
	else
	{
		proxy12_hideGameObject(targetId);
	}
}

void activateSecondaryBall_1(byte id, byte sourceId)
{
	struct GameObject *go;
	go = &gameObjects[id];
	struct GameObject *source;
	source = &gameObjects[sourceId];
	struct GameObject *depGo;


	byte targetId = 0xff;
	for (byte j = 0; j < MaxDepObjects; j++)
	{
		if (depObjects[j].sourceId == sourceId)
		{
			targetId = depObjects[j].targetId;
			break;
		}
	}
	if (targetId != 0xff)
	{
		depGo = &gameObjects[targetId];
		if (!depGo->isOn)
		{
			float angle = rand() & 2 * PI;

			float cos_angle = cos(angle);
			float sin_angle = sin(angle);
			float x;
			float y;

			if(id == 0xff)
			{
				x = 200;
				y = 200;
			}
			else
			{
				x = go->vx;
				y = go->vy;
			}


			depGo->isOn = true;
			depGo->px = source->px;
			depGo->py = source->py;
			depGo->ax = go->ax;
			depGo->ay = go->ay;
			depGo->vx = x * cos_angle - y * sin_angle;;
			depGo->vy = x * sin_angle + y * cos_angle;

			struct CollidingTimer *colTimer;
			for (byte i = 0; i < MaxColTimers; i++)
			{
				colTimer = &collidingTimers[i];
				if(colTimer->timer == 0)
				{
					colTimer->id = sourceId;
					colTimer->timer = 16;
					break;
				}
			}
			source->comp0 &= ~Component0_CollideAble;
			updateRender_1(sourceId, true);
			updateRender_1(targetId, true);
			playSfx(SND_BALL_ACTIVATED);
		}
	}

}


void updatePhysics_1(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	if (!go->isOn)
	{
		return;
	}

	go->ax = -go->vx; 
	go->ay = -go->vy + gravity;
	go->vx += go->ax * timeElapsing; //* go->fSimTimeRemaining;	// Update Velocity
	go->vy += go->ay * timeElapsing; //* go->fSimTimeRemaining;

	go->px += go->vx * timeElapsing; //* go->fSimTimeRemaining;	// Update position
	go->py += go->vy * timeElapsing; //* go->fSimTimeRemaining;

	go->boundingBox = proxy13_getBoundingBoxCircle(go->px, go->py, 4,8);

	if(id == mainBall && stuckCnt > 0)
	{
		if(stuckCnt > 10)
		{
			stuckCnt = 0;
			proxy12_killBall(id);
		}
		else
		{
			if (abs((int)go->vx) <= 2 && abs((int)go->vy) <= 4)
			{
				stuckCnt++;
			}
			else
			{
				stuckCnt = 0;
			}
		}
	}
	if(id == mainBall && timer % 32 == 0)
	{
		// memset(Color2, 0x20, 80);
		// proxy12_integer_write(9,0,go->vx);
		// proxy12_integer_write(13,0,go->vy);
		if (abs((int)go->vx) <= 2 && abs((int)go->vy) <= 4 && !IsBitSet(go->comp2, Component2_Destroyed))
		{
			stuckCnt = 1;
			playSfx(SND_CATCH);
			faceState = FA_ANGRY;
			faceTimer = 64;
		}
	}

	// Crudely wrap go->s to screen - note this cause issues when collisions occur on screen boundaries
	if (go->px < 6 || go->px >= ScreenWidth - 6)
	{
		playSfx(SND_BUMP);
		go->px = go->px < 6 ? 6 : ScreenWidth - 6;
		go->vx = -go->vx;
	}
	if ((globalOrientation == ORIENTATION_DOWN && go->py < 8) )
	{
		playSfx(SND_BUMP);
		go->py = 8;
		go->vy = -go->vy;
	}
	if(id == mainBall)
	{
		if(go->px < 100)
		{
			ballSide = -1;
		}
		else if(go->px > 220)
		{
			ballSide = 1;
		}
		else
		{
			ballSide = 0;
		}
	}

	if (globalOrientation == ORIENTATION_UP && go->py > ScreenHeight)
	{
		playSfx(SND_BUMP);
		go->py = ScreenHeight;
		go->vy = -go->vy;

	}
	if ((globalOrientation == ORIENTATION_DOWN && go->py > ScreenHeight) || (globalOrientation == ORIENTATION_UP && go->py < 16))
	{
		if (IsBitSet(go->comp1, Component1_MainBall))
		{
			if(state != GS_FINALIZE)
			{
				faceState = FA_ANGRY;
				faceTimer = 64;
				go->comp0 &= ~Component0_Physics; 
				resetBall_1(mainBall);
				state = GS_WAITCLEAN;
				remainTimer = 200;
				proxy12_dispatchEvent(EventKillBall,id, 0xff, 0xff);
			}
			else
			{
				proxy12_dispatchEvent(EventKillBall,id, 0xff, 0xff);
				state = GS_RUN_FINISH_INIT;
			}
		}
		else
		{
			deactivateSecondaryBall_1(id);
		}

		return;
	}

	if ((globalOrientation == ORIENTATION_DOWN && go->py < ScreenHeight - 16) || (globalOrientation == ORIENTATION_UP && go->py > 32))
	{
		go->orientation = ORIENTATION_UP;
	}

	if (((globalOrientation == ORIENTATION_DOWN && go->py >= ScreenHeight - 16) 
		|| (globalOrientation == ORIENTATION_UP && go->py <= 32)) 
		&& go->orientation == ORIENTATION_UP && state != GS_FINALIZE)
	{
		go->orientation = ORIENTATION_DOWN;
		posXTray = tx + 160;
		if ((go->px < posXTray - 30 && go->px >= posXTray - 48) || (go->px > posXTray + 30 && go->px <= posXTray + 48))
		{
			if (go->px < posXTray && tx < 0 || go->px > posXTray && tx > 0)
			{
				go->ax += tx;
			}
			playSfx(SND_BUMP);
			go->vy = -2 * go->vy;
		}
		else if (go->px > posXTray - 30 && go->px < posXTray + 30)
		{
			if (IsBitSet(go->comp1, Component1_MainBall))
			{
				extraBalls = true;
				faceState = FA_SUNNY;
				faceTimer = 64;
			}
			else
			{
				struct ScoreDisplay scoreDisplay;
				faceState = FA_SUNNY;
				faceTimer = 64;
				scoreDisplay.x = (int)go->px;
				scoreDisplay.y = (int)go->py - 8;
				scoreDisplay.sourceId = id;
				scoreDisplay.score = 0;
				scoreDisplay.textId = 2;
				proxy12_scoreQueue_enqueue(scoreDisplay);
				proxy0_put_text((int)go->px, (int)go->py - 8, 2);
				balls++;
				playSfx(SND_BONUS);
				proxy12_showBalls();
			}
		}
		return;
	}

	for (loopc = 0; loopc < numObjects; loopc++)
	{
		struct GameObject *target;
		target = &gameObjects[loopc];
		if (loopc != id)
		{
			if (IsBitSet(target->comp0, Component0_CircleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
			{
				if (checkAabbCollision(&(go->boundingBox), &(target->boundingBox)))
				{
					if (doCirclesOverlap_1(go->px, go->py, go->radius, target->px, target->py, target->radius))
					{
						bool swapped = true;

						playSfx(SND_BUMP);
						if (IsBitSet(target->comp3, Component3_PortalActive) && !IsBitSet(go->comp2, Component2_Destroyed))
						{
							proxy12_dispatchEvent(EventPortal, 0xff, 0xff, 0xff);
							proxy12_portalSwitch(id, loopc);

							break;
						}

						if (IsBitSet(target->comp3, Component3_Bomb))
						{
							proxy12_explodeBomb(loopc);
							playSfx(SND_EXPLODE);
						}

						proxy12_dispatchEvent(EventCollide, loopc, 0xff, 0xff);
						if (IsBitSet(target->comp2, Component2_Kill) && !IsBitSet(go->comp2, Component2_Destroyed))
						{
							proxy12_killBall(id);
						}
						if (IsBitSet(target->comp1, Component1_SwitchOrientation))
						{
							swapped = proxy12_setGlobalOrientation(target->orientation, false);
						}
						// Collision has occured
						proxy12_addCollidingPairs(id, loopc, 0xff);

						if (swapped)
						{
							handleDelete_1(loopc);
						}
						if (IsBitSet(target->comp1, Component1_Switch))
						{
							faceState = FA_THINK;
							faceTimer = 64;
							target->isOn = !target->isOn;
							updateRender_1(loopc, false);
							playSfx(SND_SWITCH_ON);
							if (target->isOn)
							{
								proxy12_dispatchEvent(EventSwitchOn, loopc, 0xff, 0xff);
							}
							else
							{
								proxy12_dispatchEvent(EventSwitchOff, loopc, 0xff, 0xff);
							}
						}

						if (IsBitSet(target->comp1, Component1_Catch) && 
							!IsBitSet(go->comp2, Component2_Destroyed) && IsBitSet(go->comp1, Component1_MainBall) && state == GS_RUN )
						{
							playSfx(SND_CATCH);
							proxy12_catchBall(id, loopc);
						}
						if (IsBitSet(target->comp2, Component2_BallActivate))
						{
							playSfx(SND_BALL_ACTIVATED);
							activateSecondaryBall_1(id, loopc);
						}

						// Distance between ball centers
						float fDistance = proxy13_qsqrt((go->px - target->px) * (go->px - target->px) + (go->py - target->py) * (go->py - target->py));

						// Displace Current Ball away from collision

						// Calculate displacement required
						float fOverlap = 0.5 * (fDistance - (float)go->radius - (float)target->radius);
						go->px -= fOverlap * (go->px - target->px) / fDistance;
						go->py -= fOverlap * (go->py - target->py) / fDistance;

						// Displace Target Ball away from collision - Note, this should affect the timing of the target ball
						// and it does, but this is absorbed by the target ball calculating its own time delta later on
						if (IsBitSet(target->comp0, Component0_Physics))
						{
							target->px += fOverlap * (go->px - target->px) / fDistance;
							target->py += fOverlap * (go->py - target->py) / fDistance;
						}
					}
				}
			}

			if (IsBitSet(target->comp0, Component0_CapsuleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
			{
				if (checkAabbCollision(&(go->boundingBox), &(target->boundingBox)))
				{
					// Check that line formed by velocity vector, intersects with line segment
					float fLineX1 = target->ax - target->px;
					float fLineY1 = target->ay - target->py;

					float fLineX2 = go->px - target->px;
					float fLineY2 = go->py - target->py;

					float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

					// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
					// how much of the segment is in the "shadow" of the object vector. The min and max clamp
					// this to lie between 0 and the line segment length, which is then normalised. We can
					// use this to calculate the closest point on the line segment
					float t = max_1(0.0, min_1(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

					// Which we do here
					float fClosestPointX = target->px + t * fLineX1;
					float fClosestPointY = target->py + t * fLineY1;

					// And once we know the closest point, we can check if the ball has collided with the segment in the
					// same way we check if two balls have collided
					float dx = go->px - fClosestPointX;
					float dy = go->py - fClosestPointY;
					float fDistanceSq = dx * dx + dy * dy;

					// Compare squared distance with squared sum of radii
					float radiusSum = go->radius + target->radius;
					if (fDistanceSq <= radiusSum * radiusSum) 
					{
						float fDistance = proxy13_qsqrt(fDistanceSq);
						playSfx(SND_BUMP);

						if (IsBitSet(target->comp2, Component2_Kill) && !IsBitSet(go->comp2, Component2_Destroyed))
						{
							proxy12_killBall(id);
						}
						// Collision has occurred - treat collision point as a ball that cannot move. To make this
						// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
						// so it behaves like a solid object when the momentum calculations are performed
						proxy12_addFakeBalls(fClosestPointX, fClosestPointY, -go->vx, -go->vy, target->radius, target->efficiency);
//						proxy12_addFakeBalls(fClosestPointX, fClosestPointY, 0, 0, target->radius, target->efficiency);
						//proxy12_addFakeBalls(fClosestPointX, fClosestPointY, go->vx * 0.5, go->vy * 0.5, target->radius, go->efficiency);

						proxy12_dispatchEvent(EventCollide, loopc, 0xff, 0xff);

						handleDelete_1(loopc);

						// Add collision to vector of collisions for dynamic resolution
						proxy12_addCollidingPairs(id, 0xff, numFakes - 1);

						// Calculate displacement required
						float fOverlap = 1.0 * (fDistance - go->radius - target->radius);


						// Displace Current Ball away from collision
						go->px -= fOverlap * (go->px - fClosestPointX) / fDistance;
						go->py -= fOverlap * (go->py - fClosestPointY) / fDistance;
					}
				}
			}
		}
	}
}

void resetBall_1(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	go->px = 160;
	go->py = 22;
	if (globalOrientation == ORIENTATION_UP)
	{
		go->py = ScreenHeight - 22;
	}
	go->vx = 0;
	go->vy = 0;
	go->ax = 0;
	go->ay = 0;
	go->comp2 &= ~Component2_Destroyed;
	go->pattern = 64;

	currentCannon = mainCannon;
	updateRender_1(id, false);
	//playSfx(SND_RESET);
	updateRender_1(mainCannon, false);
	proxy12_dispatchEvent(EventResetBall, 0xff, 0xff, 0xff);
}

void updateCollisions_1()
{
	struct GameObject *b1;
	struct GameObject *b2 = nullptr;
	byte z=0;

	for (byte i = 0; i < numColPairs; i++)
	{
		byte go1index = colPairs[i].go1;
		byte go2index = colPairs[i].go2;
		byte fakeIndex = colPairs[i].gofake;

		b1 = &gameObjects[go1index];

		if (go2index != 0xff)
		{
			b2 = &gameObjects[go2index];
		}
		if (fakeIndex != 0xff)
		{
			b2 = &fakeBalls[fakeIndex];
		}

		// Distance between balls
		float fDistance = proxy13_qsqrt((b1->px - b2->px) * (b1->px - b2->px) + (b1->py - b2->py) * (b1->py - b2->py));

		// Normal
		float nx = (b2->px - b1->px) / fDistance;
		float ny = (b2->py - b1->py) / fDistance;

		// Tangent
		float tx = -ny;
		float ty = nx;

		// Dot Product Tangent
		float dpTan1 = b1->vx * tx + b1->vy * ty;
		float dpTan2 = b2->vx * tx + b2->vy * ty;

		// Dot Product Normal
		float dpNorm1 = b1->vx * nx + b1->vy * ny;
		float dpNorm2 = b2->vx * nx + b2->vy * ny;

		// Simplified mass		
		float mass1 = 500;
		float mass2;
		if (IsBitSet(b2->comp0, Component0_Physics))
		{
			mass2 = 500;
		}
		else
		{
			mass2 = 4096;
		}

		float m1 = b2->efficiency * (dpNorm1 * (mass1 - mass2) + 2.0 * mass2 * dpNorm2) / (mass1 + mass2);
		float m2 = b2->efficiency * (dpNorm2 * (mass2 - mass1) + 2.0 * mass1 * dpNorm1) / (mass1 + mass2);

		// Update ball velocities
		b1->vx = (tx * dpTan1 + nx * m1);
		b1->vy = (ty * dpTan1 + ny * m1);
		if(IsBitSet(b2->comp0, Component0_Physics))
		{
			b2->vx = (tx * dpTan2 + nx * m2);
			b2->vy = (ty * dpTan2 + ny * m2);
		}
	}
	numColPairs = 0;
	numFakes = 0;
}

#pragma code(bcode2)
#pragma data(bdata2)

// ------------ BANK 2 Queue, Score, Actors

void killBall_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	faceState = FA_ANGRY;
	faceTimer = 64;
	playSfx(SND_EXPLODE);
	go->comp2 |= Component2_Destroyed;
	go->vx *= 0.25;
	go->vy *= 0.25;
	go->crackLevel = 16;
	proxy12_dispatchEvent(EventKillBall,id, 0xff, 0xff);
}

void hideGameObject_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	struct ScoreDisplay disp;

	if(IsBitSet(go->comp1, Component1_Valueable))
	{
		playSfx(SND_ORBZ_DELETE);
	}
	go->comp0 = 0;
	go->comp1 = 0;
	go->comp2 = 0;
	go->comp3 = 0;

	if (disp.score != 0)
	{
		proxy0_put_score(disp.x, disp.y, disp.score);
	}
	else if (disp.textId != 0xff)
	{
		proxy0_put_text(disp.x, disp.y, disp.textId);
	}

	proxy21_deleteRender(id, true);
}


void catchBall_2(byte id, int target)
{
	struct GameObject *go;
	go = &gameObjects[id];
	struct GameObject *catcher;
	catcher = &gameObjects[target];

	currentCannon = target;
	go->px = catcher->px;
	go->py = catcher->py;

	go->vx = 0;
	go->vy = 0;
	go->ax = 0;
	go->ay = 0;
	currentAngleStep = 0;
	dispatchEvent_2(EventCatchBall, 0xff, 0xff, 0xff);
	state = GS_FIRE;
//	updateRender_1(id, false);
	proxy21_updateRender(target, true);
}

bool setGlobalOrientation_2(sbyte orientation, bool isInit)
{
	if(state == GS_FINALIZE)
	{
		return false;
	}
	if (orientation != globalOrientation || isInit)
	{
		globalOrientation = orientation;
		if(isGravCannon)
		{
			proxy21_deleteRender(mainCannon, true);
			if (orientation == ORIENTATION_DOWN)
			{
				posYTray = 229;
				traySprite = 5;
				gravity = Gravity;
			}
			else
			{
				posYTray = 58;
				traySprite = 15;
				gravity = -Gravity;
			}
			proxy0_spr_image(6, 64 + traySprite);			
			proxy0_spr_image(7, 64 + traySprite + 1);			
			proxy23_setGameObjectCannon(mainCannon, 160, 18, 20, ORIENTATION_DOWN, GCOL_MED_GREY, true, true);
			proxy21_updateRender(mainCannon, true);
			playSfx(SND_GRAVITY);
		}
		else
		{
			if (orientation == ORIENTATION_DOWN)
			{
				gravity = Gravity;
			}
			else
			{
				gravity = -Gravity;
			}
		}

		return true;
	}
	return false;
}

// Write a zero terminated string on screen
void printLargeString_2(char cy, const char *s, bool withRoundScore, bool withLevelScore, bool withSubText)
{
	byte len = strlen(s);
	cy = 8 * cy;
	int cx = (40 - len * 2) / 2;
	cx = 8 * cx;

	proxy0_bm_rect_fill(&Screen, &scr, 7, cy, 314 - 8, 16);
	proxy0_bm_rect_clear(&Screen, &scr, 8, cy + 1, 314 - 10, 14);

	// bm_rect_fill(&Screen, &scr, cx - 1 , cy -1  , (len * 2) * 8 +2  ,16 +2);
	// bm_rect_clear(&Screen, &scr, cx, cy , (len * 2) * 8 ,16);

	// Loop over all characters
	while (*s)
	{
		proxy23_lchar_put(cx, cy, *s, 2);
		s++;
		cx += 16;
	}
	if (withSubText)
	{
		char sub[20];
		char buf1[3];
		itoa(level + 1, buf1, 10);
		strcpy(sub, "Level: ");
		strcat(sub, buf1);

		proxy0_bm_rect_clear(&Screen, &scr, 8, cy + 16, 314 - 10, 8);
		proxy0_put_string(&Screen, &scr, 160 - strlen(sub) * 3, cy + 16, sub, BLTOP_COPY);
	}

	if (withRoundScore)
	{
		char sub[40];
		char buf1[3];
		itoa(startValueableObjects - valueableObjects, buf1, 10);
		char buf2[6];
		itoa(orbzScore, buf2, 10);
		char buf3[6];
		itoa(roundScore, buf3, 10);
		strcpy(sub, buf1);
		strcat(sub, " Orbz X ");
		strcat(sub, buf2);
		strcat(sub, " = ");
		strcat(sub, buf3);

		proxy0_bm_rect_clear(&Screen, &scr, 8, cy + 16, 314 - 10, 8);
		proxy0_put_string(&Screen, &scr, 160 - strlen(sub) * 3, cy + 16, sub, BLTOP_COPY);
	}
	if (withLevelScore)
	{
		char sub[40];
		char buf1[3];
		itoa(balls, buf1, 10);
		char buf2[6];
		itoa(roundScore, buf2, 10);
		char buf3[6];
		itoa(levelScore, buf3, 10);
		strcpy(sub, buf1);
		strcat(sub, " Balls X ");
		strcat(sub, buf2);
		strcat(sub, " = ");
		strcat(sub, buf3);

		proxy0_bm_rect_clear(&Screen, &scr, 8, cy + 16, 314 - 10, 8);
		proxy0_put_string(&Screen, &scr, 160 - strlen(sub) * 3, cy + 16, sub, BLTOP_COPY);
	}
}


void incLevel_2(bool round)
{
	if(level >= MaxLevels - 1)
	{
		if(round)
		{
			level = 0;
			return;
		}
		level = MaxLevels - 1;
		return;
	}
	else
	{
		level++;
	}
}

void decLevel_2()
{
	if(level <= 0)
	{
		level = 0;
		return;
	}
	else
	{
		level--;
	}
}


void updateDestroyed_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	go->crackLevel--;
	if(go->crackLevel == 15)
	{
		go->pattern = 64 + 2;
	}
	if(go->crackLevel == 13)
	{
		go->pattern = 64 + 1;
	}
	if(go->crackLevel == 11)
	{
		go->pattern = 64 + 2;
	}
	if(go->crackLevel == 7)
	{
		go->pattern = 64 + 3;
	}
	if(go->crackLevel == 3)
	{
		go->pattern = 64 + 4;
	}
	if(go->crackLevel <= 0)
	{
		if (IsBitSet(go->comp1, Component1_MainBall))
		{
			go->comp0 &= ~Component0_Physics; 
			proxy21_resetBall(mainBall);
			state = GS_WAITCLEAN;
			remainTimer = 200;
		}
		else
		{
			proxy21_deactivateSecondaryBall(id);
		}
	}
}

void colorAreaCircle_2(unsigned char x0, unsigned char y0, unsigned char radius, byte backc, bool onlyBack)
{
    char* col = Color1;
    int radius_sqr = radius * radius;  
    for (int x = 0; x <= radius; x++) // Loop only from 0 to radius
    {
        byte hh = (byte)proxy23_qsqrt(radius_sqr - x * x);
        byte rx1 = x0 + x;  // Right side x
        byte rx2 = x0 - x;  // Left side x (mirrored)

        // Only loop from 0 to hh and mirror the top and bottom
        for (int y = 0; y <= hh; y++)
        {
            byte py1 = y0 + y;  // Upper half
            byte py2 = y0 - y;  // Lower half (mirrored)

            // Draw upper and lower parts for both left and right sides
            proxy0_colorChar(rx1, py1, backc, col, onlyBack); // Right upper
            proxy0_colorChar(rx1, py2, backc, col, onlyBack); // Right lower
            proxy0_colorChar(rx2, py1, backc, col, onlyBack); // Left upper
            proxy0_colorChar(rx2, py2, backc, col, onlyBack); // Left lower
        }
    }
}


void setTunes_2(bool isOn)
{
	playTunes = isOn;
	breakMusic = false;

	if(playTunes)
	{
		proxy_memcpy(8, 2, Buffer, MusicInGame, 0x800);
		breakMusic = true;
	}
	else
	{
		if(level % 2 == 0)
		{
			proxy_memcpy(8, 2, Buffer, MusicBackground1, 0x800);
		}
		else
		{
			proxy_memcpy(9, 2, Buffer, MusicBackground2, 0x800);
		}
		breakMusic = true;
		playSubtune(0);
		music_ff();
	}
}

void vec2_rotate_2(Vect2 *vec, float angle, float targetRadius)
{
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);
	float x = vec->v[0];
	float y = vec->v[1];
	vec->v[0] = x * cos_angle - y * sin_angle;
	vec->v[1] = x * sin_angle + y * cos_angle;
}

void updateFace_2()
{
	byte sprImage = FACE_NORM;

	struct GameObject *go;
	go = &gameObjects[mainCannon];

	if (faceState == FA_WATCH)
	{
		if(ballSide == -1)
		{
			sprImage = FACE_LEFT;
		}
		else if(ballSide == 1)
		{
			sprImage = FACE_RIGHT;
		}
	}
	else if (faceState == FA_ANGRY)
	{
		sprImage = FACE_ANGRY;
	}
	else if (faceState == FA_SUNNY)
	{
		sprImage = FACE_SUNNY;
	}
	else if (faceState == FA_THINK)
	{
		sprImage = FACE_THINK;
	}
	if(faceTimer-- <= 0)
	{
		faceState = FA_WATCH;
	}
	proxy0_spr_set(3, true, (int)go->px + 12 , (int)go->py + 40, 64 + sprImage, VCOL_BLACK, false,  false, false);
}

void updateCannon_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	currentAngleStep = 0;

	float cannonAngleStep;
	if(abs(moveCannonCount) > 16)
	{
		cannonAngleStep = CannonAngleStepFast;		
	}
	else if(abs(moveCannonCount) > 8)
	{
		cannonAngleStep = CannonAngleStep;
	}
	else
	{
		cannonAngleStep = CannonAngleStepSlow;
	}

	switch (go->orientation)
	{
	case ORIENTATION_DOWN:
		if (go->cannonVector.v[1] >= 0)
		{
			if (joyx[0] < 0)
			{
				if(moveCannonCount <= 0)
				{
					moveCannonCount--;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = cannonAngleStep;
			}
			if (joyx[0] > 0)
			{
				if(moveCannonCount >= 0)
				{
					moveCannonCount++;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = -cannonAngleStep;
			}
		}
		else
		{
			go->cannonVector.v[1] = 0;
			currentAngleStep = 0;
		}
		break;
	case ORIENTATION_UP:
		if (go->cannonVector.v[1] <= 0)
		{
			if (joyx[0] > 0)
			{
				if(moveCannonCount >= 0)
				{
					moveCannonCount++;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = cannonAngleStep;
			}
			if (joyx[0] < 0)
			{
				if(moveCannonCount <= 0)
				{
					moveCannonCount--;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = -cannonAngleStep;
			}
		}
		else
		{
			go->cannonVector.v[1] = 0;
			currentAngleStep = 0;
		}
		break;
	case ORIENTATION_LEFT:
		if (go->cannonVector.v[0] <= 0)
		{
			if (joyy[0] < 0)
			{
				if(moveCannonCount <= 0)
				{
					moveCannonCount--;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = cannonAngleStep;
			}
			if (joyy[0] > 0)
			{
				if(moveCannonCount >= 0)
				{
					moveCannonCount++;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = -cannonAngleStep;
			}
		}
		else
		{
			go->cannonVector.v[0] = 0;
			currentAngleStep = 0;
		}
		break;
	case ORIENTATION_RIGHT:
		if (go->cannonVector.v[0] >= 0)
		{
			if (joyy[0] > 0)
			{
				if(moveCannonCount >= 0)
				{
					moveCannonCount++;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = cannonAngleStep;
			}
			if (joyy[0] < 0)
			{
				if(moveCannonCount <= 0)
				{
					moveCannonCount--;
				}
				else
				{
					moveCannonCount = 0;
				}
				currentAngleStep = -cannonAngleStep;
			}
		}
		else
		{
			go->cannonVector.v[0] = 0;
			currentAngleStep = 0;
		}
		break;
	}
	proxy21_updateRender(id, false);
	vec2_rotate_2(&(go->cannonVector), currentAngleStep, go->radius);
	if(currentAngleStep == 0)
	{
		moveCannonCount = 0;
	}
	bool fire = joyb[0];
	if(!fire)
	{
		proxy23_saveSimulationStart(mainBall);			
		struct GameObject *goBall;
		goBall = &gameObjects[mainBall];
		goBall->px = go->px + go->cannonVector.v[0] * 1.8;
		goBall->py = go->py + go->cannonVector.v[1] * 1.8;
		goBall->vx = go->cannonVector.v[0] * CannonPower;
		goBall->vy = go->cannonVector.v[1] * CannonPower;
		state = GS_SIMULATE;
	}
	else
	{
		struct GameObject *goBall;
		goBall = &gameObjects[mainBall];
		playSfx(SND_SHOOT);
		dispatchEvent_2(EventShootBall, mainBall, 0xff, 0xff);

		goBall->px = go->px + go->cannonVector.v[0] * 1.8;
		goBall->py = go->py + go->cannonVector.v[1] * 1.8;
		goBall->vx = go->cannonVector.v[0] * CannonPower;
		goBall->vy = go->cannonVector.v[1] * CannonPower;
		state = GS_RUN;
	}
	proxy21_updateRender(id, false);

	if(id == mainCannon)
	{
		if(go->cannonVector.v[0] > 5)
		{
			ballSide = 1;
		}
		else if(go->cannonVector.v[0] < -5)
		{
			ballSide = -1;
		}
		else
		{
			ballSide = 0;
		}
	}
}

void executeLargeTextTimer_2()
{
	if (largeTextTimer == 1)
	{
		gameObjects[largeTextId].isOn = false;
		proxy21_deleteRender(largeTextId, true);
	}
	if (largeTextTimer > 0)
	{
		largeTextTimer--;
	}
}

void addCollidingPairs_2(byte go1, byte go2, byte gofake)
{
	if (numColPairs < MaxCollidingPairs)
	{
		colPairs[numColPairs].go1 = go1;
		colPairs[numColPairs].go2 = go2;
		colPairs[numColPairs].gofake = gofake;
		numColPairs++;
	}
}

void explodeBomb_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

    struct BombDisplay bombDisplay;

	byte x = go->boundingBox.x / 8;
	byte y = go->boundingBox.y / 8;
	byte w = (byte)ceil((float)go->boundingBox.width / 8);
	byte h = (byte)ceil((float)go->boundingBox.height / 8);

	bombDisplay.x = x + (w / 2);
	bombDisplay.y = y + (h / 2);
	bombDisplay.radius = bombDistance / 8;
	bombDisplay.origRadius = bombDistance / 8;
	bombDisplay.color = GCOL_WHITE;

	go->comp3 |= Component3_Exploded;
	bombQueue_enqueue_2(bombDisplay);

	colorAreaCircle_2(bombDisplay.x, bombDisplay.y, bombDisplay.radius, bombDisplay.color, true);
	vic.color_border = VCOL_RED;

	for (byte i = 0; i < numObjects; i++)
	{
		struct GameObject *target;
		target = &gameObjects[i];
		if (i == id)
		{
			continue;
		}

		if (IsBitSet(target->comp0, Component0_CircleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
		{
			if(proxy21_doCirclesOverlap(go->px, go->py, bombDistance, target->px, target->py, target->radius))
			{	
				if (IsBitSet(target->comp3, Component3_Bomb) && !IsBitSet(target->comp3, Component3_Exploded))
				{
					explodeBomb_2(i);
				}
				if (IsBitSet(target->comp2, Component2_BombWall))
				{
					target->comp1 |= Component1_TimedDelete;
				}
				proxy21_handleDelete(i);
			}
		}
		if (IsBitSet(target->comp0, Component0_CapsuleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
		{
			if(proxy21_doCirclesOverlap(go->px, go->py, bombDistance, target->px, target->py, target->radius) || 
				proxy21_doCirclesOverlap(go->px, go->py, bombDistance, target->ax, target->ay, target->radius))
			{	
				if (IsBitSet(target->comp2, Component2_BombWall))
				{
					target->comp1 |= Component1_TimedDelete;
				}
				proxy21_handleDelete(i);
			}
		}
	}
}

void portalSwitch_2(byte id, byte targetId)
{
	byte otherPortalId = 0xff;
	struct GameObject *go;
	go = &gameObjects[id];
	struct GameObject *target;
	target = &gameObjects[targetId];
	struct GameObject *otherPortal = nullptr;

	for (byte i = 0; i < numObjects; i++)
	{
		otherPortal = &gameObjects[i];
		if(IsBitSet(otherPortal->comp3, Component3_PortalActive) && IsBitSet(otherPortal->comp0, Component0_CollideAble) && i != targetId)
		{
			otherPortalId = i;
			break;
		}
	}
	if(otherPortalId == 0xff)
	{
		return;
	}

	go->px = otherPortal->px;
	go->py = otherPortal->py;

	otherPortal->comp0 &= ~Component0_CollideAble;
	proxy21_updateRender(otherPortalId, false);

	struct CollidingTimer *colTimer;
	for (byte i = 0; i < MaxColTimers; i++)
	{
		colTimer = &collidingTimers[i];
		if(colTimer->timer == 0)
		{
			colTimer->id = otherPortalId;
			colTimer->timer = 64;
			break;
		}
	}
	
	
}

void execCollisionTimer_2(byte id)
{
	struct GameObject *go;

	struct CollidingTimer *colTimer;
	for (byte i = 0; i < MaxColTimers; i++)
	{
		colTimer = &collidingTimers[i];
		if(colTimer->timer > 0)
		{
			colTimer->timer--;
			if(colTimer->timer == 0)
			{
				go = &gameObjects[colTimer->id];
	
				go->comp0 |= Component0_CollideAble;
				proxy21_updateRender(colTimer->id, false);
			}
		}
	}
}


void addFakeBalls_2(float px, float py, float vx, float vy, float radius, float efficiency)
{
	if (numFakes < MaxFakes)
	{
		fakeBalls[numFakes].spriteNum = 0xff;
		fakeBalls[numFakes].comp0 = Component0_CircleCollider | Component0_CollideAble;
		fakeBalls[numFakes].comp1 = 0;
		fakeBalls[numFakes].comp2 = 0;
		fakeBalls[numFakes].comp3 = 0;
		fakeBalls[numFakes].px = px;
		fakeBalls[numFakes].py = py;
		fakeBalls[numFakes].ax = 0;
		fakeBalls[numFakes].ay = 0;
		fakeBalls[numFakes].vx = vx;
		fakeBalls[numFakes].vy = vy;
		fakeBalls[numFakes].efficiency = efficiency;
		fakeBalls[numFakes].radius = radius;
		numFakes++;
	}
}

void handleScore_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	if (go->baseScore == 0)
	{
		return;
	}
	struct ScoreDisplay scoreDisplay;
	int i;
	int selectedMulti = -1; // Initialize with an invalid value

	for (i = 6; i >= 0; i--)
	{
		// Check if the value is less than or equal to the threshold
		if (valueableObjects <= scoreBoard[i].pegThreshold)
		{
			if(multiplier < scoreBoard[i].multiplier)
			{
				faceState = FA_THINK;
				faceTimer = 64;

				playSfx(SND_MULTIPLIER);
				multiplier = scoreBoard[i].multiplier;
				showMultiplier_2(multiplier);
				break;
			}
		}
	}

	// Return the selected multiplier
	int score = multiplier * go->baseScore;
	if (!IsBitSet(go->comp2, Component2_BonusScore))
	{
		orbzScore += score;
	}
	scoreDisplay.x = (int)go->px;
	scoreDisplay.y = (int)(go->py - go->radius);
	scoreDisplay.sourceId = id;
	scoreDisplay.score = score;
	score_inc_2(0, score);
	scoreDisplay.textId = 0xff;
	scoreQueue_enqueue_2(scoreDisplay);
	proxy0_put_score((int)go->px, (int)go->py - (int)go->radius, score);
}

void colorBoundingBox_2(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	byte x = go->boundingBox.x / 8;
	byte y = go->boundingBox.y / 8;
	byte w = (byte)ceil((float)go->boundingBox.width / 8);
	byte h = (byte)ceil((float)go->boundingBox.height / 8);

	proxy0_colorArea(x,y,w,h,go->color, 0xff);
}

inline void char_put_2(byte x, byte y, char ch, char color)
{
	__assume(y < 25);

	Color2[40 * y + x] = ch;
	Color1[40 * y + x] = color;
}

// Write a zero terminated string on screen
void string_write_2(char cx, char cy, const char *s, char color)
{
	// Loop over all characters
	while (*s)
	{
		char_put_2(cx, cy, *s, color);
		s++;
		cx++;
	}
}

void integer_write_2(char cx, char cy, int i)
{
	char s[10];
	itoa(i, s, 10);

	string_write_2(cx, cy, s, VCOL_WHITE);
}


void startHighlight_2(byte x, byte y, byte w, byte h, bool isHires, byte restoreColor)
{
	if(highlight.timer > 0)
	{
		colorAreaUnclipped(highlight.x, highlight.y, highlight.w, highlight.h, highlight.restoreColor, highlight.isHires);
	}

	highlight.x = x;
	highlight.y = y;
	highlight.w = w;
	highlight.h = h;
	highlight.restoreColor = restoreColor;
	highlight.isHires = isHires;
	highlight.timer = 64;
}

void collideQueue_enqueue_2(byte insertItem)
{
	if ((collideQueueFront == 0 && collideQueueRear == MaxObjects - 1) ||
		((collideQueueRear + 1) % MaxObjects == collideQueueFront))
	{
		return;
	}

	else if (collideQueueFront == -1) /* Insert First Element */
	{
		collideQueueFront = 0;
		collideQueueRear = 0;
		collideQueue[collideQueueRear] = insertItem;
	}

	else if (collideQueueRear == MaxObjects - 1 && collideQueueFront != 0)
	{
		collideQueueRear = 0;
		collideQueue[collideQueueRear] = insertItem;
	}

	else
	{
		collideQueueRear++;
		collideQueue[collideQueueRear] = insertItem;
	}
}

sbyte collideQueue_dequeue_2()
{
	byte ret;

	if (collideQueueFront == -1)
	{
		return -1;
	}

	ret = collideQueue[collideQueueFront];
	collideQueue[collideQueueFront] = 0xff;
	if (collideQueueFront == collideQueueRear)
	{
		collideQueueFront = -1;
		collideQueueRear = -1;
	}
	else if (collideQueueFront == MaxObjects - 1)
		collideQueueFront = 0;
	else
		collideQueueFront++;

	return ret;
}

bool collideQueue_isQueued_2(byte item)
{

	if (collideQueueFront == -1)
	{
		return false;
	}
	if (collideQueueRear >= collideQueueFront)
	{
		for (int i = collideQueueFront; i <= collideQueueRear; i++)
		{
			if (collideQueue[i] == item)
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = collideQueueFront; i < MaxObjects; i++)
		{
			if (collideQueue[i] == item)
			{
				return true;
			}
		}

		for (int i = 0; i <= collideQueueRear; i++)
		{
			if (collideQueue[i] == item)
			{
				return true;
			}
		}
	}
	return false;
}

void scoreQueue_enqueue_2(ScoreDisplay insertItem)
{
	if ((scoreQueueFront == 0 && scoreQueueRear == MaxScoreDisplay - 1) ||
		((scoreQueueRear + 1) % MaxScoreDisplay == scoreQueueFront))
	{
		return;
	}

	else if (scoreQueueFront == -1) /* Insert First Element */
	{
		scoreQueueFront = 0;
		scoreQueueRear = 0;
		scoreQueue[scoreQueueRear] = insertItem;
	}

	else if (scoreQueueRear == MaxScoreDisplay - 1 && scoreQueueFront != 0)
	{
		scoreQueueRear = 0;
		scoreQueue[scoreQueueRear] = insertItem;
	}
	else
	{
		scoreQueueRear++;
		scoreQueue[scoreQueueRear] = insertItem;
	}
}

struct ScoreDisplay scoreQueue_dequeue_2()
{
	ScoreDisplay ret;

	if (scoreQueueFront == -1)
	{
		ret.score = 0;
		ret.textId = 0xff;
		return ret;
	}

	ret = scoreQueue[scoreQueueFront];
	ScoreDisplay n;
	n.score = 0;
	n.textId = 0xff;
	scoreQueue[scoreQueueFront] = n;
	if (scoreQueueFront == scoreQueueRear)
	{
		scoreQueueFront = -1;
		scoreQueueRear = -1;
	}
	else if (scoreQueueFront == MaxScoreDisplay - 1)
		scoreQueueFront = 0;
	else
		scoreQueueFront++;

	return ret;
}

ScoreDisplay scoreQueue_get_2(byte item)
{
	struct ScoreDisplay ret;

	if (scoreQueueFront == -1)
	{
		return false;
	}
	if (scoreQueueRear >= scoreQueueFront)
	{
		for (int i = scoreQueueFront; i <= scoreQueueRear; i++)
		{
			if (scoreQueue[i].sourceId == item)
			{
				return scoreQueue[i];
			}
		}
	}
	else
	{
		for (int i = scoreQueueFront; i < MaxScoreDisplay; i++)
		{
			if (scoreQueue[i].sourceId == item)
			{
				return scoreQueue[i];
			}
		}

		for (int i = 0; i <= scoreQueueRear; i++)
		{
			if (scoreQueue[i].sourceId == item)
			{
				return scoreQueue[i];
			}
		}
	}
	ret.score = 0;
	return ret;
}


void bombQueue_enqueue_2(BombDisplay insertItem)
{
	if ((bombQueueFront == 0 && bombQueueRear == MaxBombDisplay - 1) ||
		((bombQueueRear + 1) % MaxBombDisplay == bombQueueFront))
	{
		return;
	}

	else if (bombQueueFront == -1) /* Insert First Element */
	{
		bombQueueFront = 0;
		bombQueueRear = 0;
		bombQueue[bombQueueRear] = insertItem;
	}

	else if (bombQueueRear == MaxBombDisplay - 1 && bombQueueFront != 0)
	{
		bombQueueRear = 0;
		bombQueue[bombQueueRear] = insertItem;
	}
	else
	{
		bombQueueRear++;
		bombQueue[bombQueueRear] = insertItem;
	}
}

struct BombDisplay bombQueue_dequeue_2()
{
	BombDisplay ret;

	if (bombQueueFront == -1)
	{
		ret.radius = 0;
		return ret;
	}

	ret = bombQueue[bombQueueFront];
	BombDisplay n;
	n.radius = 0;
	bombQueue[bombQueueFront] = n;
	if (bombQueueFront == bombQueueRear)
	{
		bombQueueFront = -1;
		bombQueueRear = -1;
	}
	else if (bombQueueFront == MaxBombDisplay - 1)
		bombQueueFront = 0;
	else
		bombQueueFront++;

	return ret;
}

// BombDisplay bombQueue_get_2(byte item)
// {
// 	struct BombDisplay ret;

// 	if (bombQueueFront == -1)
// 	{
// 		return false;
// 	}
// 	if (bombQueueRear >= bombQueueFront)
// 	{
// 		for (int i = bombQueueFront; i <= bombQueueRear; i++)
// 		{
// 			if (bombQueue[i].sourceId == item)
// 			{
// 				return bombQueue[i];
// 			}
// 		}
// 	}
// 	else
// 	{
// 		for (int i = bombQueueFront; i < MaxBombDisplay; i++)
// 		{
// 			if (bombQueue[i].sourceId == item)
// 			{
// 				return bombQueue[i];
// 			}
// 		}

// 		for (int i = 0; i <= bombQueueRear; i++)
// 		{
// 			if (bombQueue[i].sourceId == item)
// 			{
// 				return bombQueue[i];
// 			}
// 		}
// 	}
// 	ret.score = 0;
// 	return ret;
// }


void showBalls_2()
{
	char buff[2];
	itoa(balls, buff, 10);
	string_write_2(23, 0, "00", VCOL_WHITE);

	byte l = strlen(buff);
	string_write_2(25-l, 0, buff, VCOL_WHITE);
	startHighlight_2(17,0,8, 1, false, VCOL_WHITE);
}

void showMultiplier_2(byte multiplier)
{
	char buff[1];
	itoa(multiplier, buff, 10);
	string_write_2(15, 0, buff, VCOL_WHITE);
	startHighlight_2(14,0,3, 1, false, VCOL_WHITE);
}

// Increment the score from a given digit on
void score_inc_2(char digit, unsigned val)
{
	// Lowest digit to increment
	char at = 7 - digit;

	// Loop while there is still score to account for
	while (val)
	{
		// Increment one character
		char ch = Color2[at + 3] + val % 10;

		// Remove low digit from number
		val /= 10;

		// Check overflow
		if (ch > '9')
		{
			ch -= 10;
			val++;
		}

		Color2[at + 3] = ch;

		// Next higher character
		at--;
	}

	char i = 0;

	while (i < 8 && Color2[i + 3] == Color2[i + 32])
		i++;

	// Check if new score is higher
	if (i < 8 && Color2[i + 3] > Color2[i + 32])
	{
		// If so, copy to highscore
		while (i < 8)
		{
			Color2[i + 32] = Color2[i + 3];
			i++;
		}
	}

	
}

// Reset score and update high score
void score_reset_2(void)
{
	// Clear score
	for (char i = 0; i < 8; i++)
	{
		Color2[i+3] = '0';
		Color2[i+32] = hiscore[i];
	}
}

void execActionEnableCollision_2(byte start, byte end)
{
	struct CollidingTimer *colTimer;

	for (int i = start; i <= end; i++)
	{
		// for (byte j = 0; j < MaxColTimers; j++)
		// {
		// 	colTimer = &collidingTimers[i];
		// 	if(colTimer->id == i)
		// 	{
		// 		colTimer->id = 0xff;
		// 	}
		// }
		struct GameObject *go;
		go = &gameObjects[i];
		go->comp0 |= Component0_CollideAble;
		proxy21_updateRender(i, false);
	}
}

void execActionDisableCollision_2(byte start, byte end)
{
	struct CollidingTimer *colTimer;

	for (int i = start; i <= end; i++)
	{
		// for (byte j = 0; j < MaxColTimers; j++)
		// {
		// 	colTimer = &collidingTimers[i];
		// 	if(colTimer->id == i)
		// 	{
		// 		colTimer->id = 0xff;
		// 	}
		// }
		struct GameObject *go;
		go = &gameObjects[i];
		go->comp0 &= ~Component0_CollideAble;
		proxy21_updateRender(i, false);
	}
}

void execActionShow_2(byte start, byte end)
{
	for (int i = start; i <= end; i++)
	{
		struct GameObject *go;
		go = &gameObjects[i];
		go->comp0 |= Component0_CollideAble;
		proxy21_updateRender(i, false);
	}
}

void execActionHide_2(byte start, byte end)
{
	for (int i = start; i <= end; i++)
	{
		struct GameObject *go;
		go = &gameObjects[i];
		go->comp0 &= ~Component0_CollideAble;
		proxy21_deleteRender(i, false);
	}
}


void execActionLaser_2(byte start, byte end, bool val)
{
	for (int i = start; i <= end; i++)
	{
		struct GameObject *go;
		go = &gameObjects[i];
		go->isOn = val;
		if (val == true)
		{
			go->comp0 |= Component0_CollideAble;
		}
		else
		{
			go->comp0 &= ~Component0_CollideAble;
		}
		proxy21_updateRender(i, false);
	}
}

void execActionRoll_2(byte actorId, bool isCollideable)
{
	struct Actor *act;
	struct GameObject *go;
	byte pre;

	act = &actorList[actorId];

	if(act->var0 >= act->receiverEnd)
	{
		act->var0 = act->receiverStart;
		pre = act->receiverEnd;
	}
	else
	{
		pre = act->var0;
		act->var0 = act->var0 + 1;
	}

	go = &gameObjects[act->var0];
	if (isCollideable)
	{
		go->comp0 |= Component0_CollideAble;
	}
	else
	{
		go->comp0 &= ~Component0_CollideAble;
	}
	go = &gameObjects[pre];
	
	if (isCollideable)
	{
		go->comp0 &= ~Component0_CollideAble;
	}
	else
	{
		go->comp0 |= Component0_CollideAble;
	}
	proxy21_updateRender(act->var0, false);
	proxy21_updateRender(pre, false);
}

void execActionRandom_2(byte actorId, bool isCollideable)
{
	struct Actor *act;
	struct GameObject *go;

	act = &actorList[actorId];

	for(byte i=act->receiverStart;i<=act->receiverEnd;i++)
	{
		go = &gameObjects[i];
		if(isCollideable)
		{
			if(IsBitSet(go->comp0, Component0_CollideAble))
			{
				go->comp0 &= ~Component0_CollideAble;
				proxy21_updateRender(i, false);
			}
		}
		else
		{
			if(!IsBitSet(go->comp0, Component0_CollideAble))
			{
				go->comp0 |= Component0_CollideAble;
				proxy21_updateRender(i, false);
			}
		}
	}
	byte count = act->param0;
	if(count == 0xff)
	{
		count = 1;
	}
	byte r = 0xff;
	for(byte i=0;i<count;i++)
	{
		bool found = false;
		do
		{
			r = rand() % (act->receiverEnd - act->receiverStart + 1) + act->receiverStart; 

			go = &gameObjects[r];
			if(!isCollideable)
			{
				if(IsBitSet(go->comp0, Component0_CollideAble))
				{
					go->comp0 &= ~Component0_CollideAble;
					proxy21_updateRender(r, false);
					found =true;
				}
			}
			else
			{
				if(!IsBitSet(go->comp0, Component0_CollideAble))
				{
					go->comp0 |= Component0_CollideAble;
					proxy21_updateRender(r, false);
					found =true;
				}
			}
		} while(!found);
	}
}

void execActionActivateBall_2(byte sourceId, byte mainBallId)
{
	proxy21_activateSecondaryBall(sourceId, mainBallId);
}
void execActionDeactivateBall_2(byte targetId)
{
	proxy21_deactivateSecondaryBall(targetId);
}

void execActionKillBall_2(byte targetId)
{
	struct GameObject *go;
	go = &gameObjects[targetId];
	if (!IsBitSet(go->comp2, Component2_Destroyed))
	{
		playSfx(SND_EXPLODE);
		go->comp2 |= Component2_Destroyed;
		go->vx *= 0.25;
		go->vy *= 0.25;
		go->crackLevel = 16;
	}
}



void dispatchEvent_2(byte event, byte senderId, byte p1, byte p2)
{
	for (int i = 0; i < numActors; i++)
	{
		if ((actorList[i].event1 == event || actorList[i].event2 == event) && actorList[i].senderId == senderId)
		{
			switch (actorList[i].action)
			{
				case ActionEnableCollision:
					execActionEnableCollision_2(actorList[i].receiverStart, actorList[i].receiverEnd);
					break;
				case ActionDisableCollision:
					execActionDisableCollision_2(actorList[i].receiverStart, actorList[i].receiverEnd);
					break;
				case ActionEnableLaser:
					execActionLaser_2(actorList[i].receiverStart, actorList[i].receiverEnd, true);
					break;
				case ActionDisableLaser:
					execActionLaser_2(actorList[i].receiverStart, actorList[i].receiverEnd, false);
					break;
				case ActionHideObject:
					execActionHide_2(actorList[i].receiverStart, actorList[i].receiverEnd);
					break;
				case ActionShowObject:
					execActionShow_2(actorList[i].receiverStart, actorList[i].receiverEnd);
					break;
				case ActionTimedRollActivateCollideable:
					execActionRoll_2(i, true);
					break;
				case ActionTimedRollDeactivateCollideable:
					execActionRoll_2(i, false);
					break;
				case ActionRandomActivateCollideable:
					execActionRandom_2(i, true);
					break;
				case ActionRandomDeactivateCollideable:
					execActionRandom_2(i, false);
					break;
				case ActionActivateBall:
					execActionActivateBall_2(actorList[i].receiverStart, actorList[i].param0);
					break;
				case ActionDeactivateBall:
					execActionDeactivateBall_2(actorList[i].receiverStart);
					break;
				case ActionKillBall:
					execActionKillBall_2(actorList[i].receiverStart);
				default:
					break;
			}
		}
	}
}


// Clear a pixel at the given coordinate
// void pix_clr_2(unsigned px, unsigned py)
// {
// 	__assume(px < 320);
// 	__assume(py < 200);	

// 	// Calculate base position in hires
// 	char * dp = Hirows[py >> 3] + (px & ~7);

// 	// Clear the pixel
// 	dp[py & 7] &= clrmask[px & 7];
// }

// Init free list of particles
void particle_init_2(void)
{
	// Init address table for hires
	for(int i=0; i<25; i++)
		Hirows[i] = Hires + 320 * i;

	// Init list heads
	pfirst = nullptr;
	pfree = particles;

	// Link all particles in free list
	for(int i=0; i<63; i++)
		particles[i].next = particles + i + 1;
}

// Add a particle to the list
void particle_add_2(int px, int py, int vx, int vy)
{
	// Check if we have a particle left
	if (pfree)
	{
		// Remove from free list
		Particle	*	p = pfree;
		pfree = pfree->next;

		// Add to used list
		p->next = pfirst;
		pfirst = p;

		// Init particle data
		p->px = px;
		p->py = py;
		p->vx = vx;
		p->vy = vy;
	}
}


// Normalized random function
int rnorm_2(byte wide)
{
	int l0 = (rand() & 0xfff) - 0x800;
	int l1 = (rand() & 0xfff) - 0x800;
	int l2 = (rand() & 0xfff) - 0x800;
	int l3 = (rand() & 0xfff) - 0x800;

	return  wide * (l0 + l1 + l2 + l3);
}

void particle_remove_2(bool isConfetti)
{
	// Start with first particle, remember previous
	// particle for list removal
	Particle	*	p = pfirst, * pp = nullptr;

	// Loop over all particles in used list
	byte i=0;
	while (p)
	{
		// Clear previous particle image, using 10.6 fixed point
		proxy0_pix_set(p->px >> 6, p->py >> 6, isConfetti);
		p = p->next;
		if(i++ == 63)
		{
			p = nullptr;
		}
		// Particle	*	pn = p->next;

		// Remove from used list
		// if (pp)
		// 	pp->next = pn;
		// else
		// 	pfirst = pn;

		// Attach to free list
		// p->next = pfree;
		// pfree = p;

		// Advance to next particle
		// p = pn;
	}

	pfirst = nullptr;
	pfree = particles;

	// Link all particles in free list
	for(int i=0; i<63; i++)
		particles[i].next = particles + i + 1;

}

// Move particles in used list
void particle_move_2(byte grav, bool isConfetti)
{
	// Start with first particle, remember previous
	// particle for list removal
	Particle	*	p = pfirst, * pp = nullptr;

	// Loop over all particles in used list
	while (p)
	{
		// Clear previous particle image, using 10.6 fixed point
		proxy0_pix_set(p->px >> 6, p->py >> 6, isConfetti);
		//pix_clr_2(p->px >> 6, p->py >> 6);

		// Advance position by velocity
		p->px += p->vx;
		p->py += p->vy;

		// Apply gravity
		p->vy += grav;

		// Check if particle is still on screen
		if (p->px < 0 || p->px >= 320 * 64 || p->py < 8 * 64 || p->py >= 200 * 64)
		{
			// Particle is offscreen, so we remove it from the used list

			// Remember next particle in used list
			Particle	*	pn = p->next;

			// Remove from used list
			if (pp)
				pp->next = pn;
			else
				pfirst = pn;

			// Attach to free list
			p->next = pfree;
			pfree = p;

			// Advance to next particle
			p = pn;
		}
		else
		{
			// Set image at new position
			proxy0_pix_set(p->px >> 6, p->py >> 6, isConfetti);

			// Advance to next particle
			pp = p;
			p = p->next;
		}
	}
}

void intro_2()
{
	particle_init_2();
	colorAreaUnclipped(13, 1, 14, 12, GCOL_BLACK, true);	
	proxy_drawImageUnclipped(9, 2, ImgShip2,  13, 1, 14, 12);
	byte i=0;
	int t=0;

	const char intro1[] = "TRANSMISSION: You are about to approach our mission target";
	const char intro2[] = "Solar System Orbix 1";
	const char intro3[] = "Priority is to eliminate all orange orbz...";
	const char intro4[] = "...to absorb the mystic energy of element 115";
	const char intro5[] = "Prepare for Landing... good luck";

	while(!joyb[0])
	{
		switch (t)
		{
			case 100:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_DARK_GREY, true);
				break;
			case 180:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_MED_GREY, true);
				break;
			case 260:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_LT_GREY, true);
				break;
			case 200:
				proxy0_put_string(&Screen, &scr, 15, 110, intro1, BLTOP_COPY);
				break;
			case 250:
				proxy0_put_string(&Screen, &scr, 110, 118, intro2, BLTOP_COPY);
				break;
			case 300:
				proxy0_put_string(&Screen, &scr, 57, 126, intro3, BLTOP_COPY);
				break;
			case 350:
				proxy0_put_string(&Screen, &scr, 48, 134, intro4, BLTOP_COPY);
				break;
			case 400:
				proxy0_put_string(&Screen, &scr, 80, 142, intro5, BLTOP_COPY);
				break;
			default:
				break;
		}

		if(t < 402)
		{
			t++;
		}



		// Advance particles

		if (i < 63)
		{
			// Add a particle from the left for the first third

			particle_add_2(160 * 64, 120 * 64, 0 + (rnorm_2(10) >> 6), (192 + (rnorm_2(10) >> 6)));
			i++;
		}
		else
		{
			i=0;
		}
		particle_move_2(0, false);
	}
	t=0;
	while(t<6000)
	{
		switch (t)
		{
			case 50:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_MED_GREY, true);
				break;
			case 100:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_DARK_GREY, true);
				break;
			case 200:
				colorAreaUnclipped(13, 1, 14, 12, GCOL_BLACK, true);
				break;
		}
		if (t < 63)
		{
			// Add a particle from the left for the first third

			particle_add_2(160 * 64, 120 * 64, 0 + (rnorm_2(10) >> 6), (192 + (rnorm_2(10) >> 6)));
		}
		t++;
		particle_move_2(2, false);
	}
}

void doFirework_2(byte type)
{
	// Init particle system
	particle_init_2();

	if(type == 0)
	{
		for(int i=0; i<170; i++)
		{
			// Advance particles
			particle_move_2(8, true);

			if (i < 20)
			{
				// Add a particle from the left for the first third
				particle_add_2(4 * 64, 196 * 64, 256 + (rnorm_2(1) >> 6), -(384 + (rnorm_2(1) >> 6)));
			}
			else if (i < 40)
			{
				// Add a particle from the right for the second third
				particle_add_2(316 * 64, 196 * 64, - (256 + (rnorm_2(1) >> 6)), -(384 + (rnorm_2(1) >> 6)));
			}
			else if (i < 60)
			{
				// Add a particle from the middle for the final third
				particle_add_2(160 * 64, 196 * 64, rnorm_2(1) >> 6, -(384 + (rnorm_2(1) >> 6)));
			}
		}
	}
	else
	{
		byte otray;
		int startx;

		if(globalOrientation == ORIENTATION_DOWN)
		{
			startx = 196;
		}
		else
		{
			startx = 12;
		}
		for(int i=0; i<128; i++)
		{
			// Advance particles

			if (i < 64)
			{
				otray = tray;
				particle_add_2((160 + tx) * 64, startx * 64, rnorm_2(1) >> 6, globalOrientation * -1 * (120 + (rnorm_2(1) >> 6)));
				proxy0_pix_set((160 + tx), startx, true);
			}
			particle_move_2(8, true);
		}

	}
}

void initStarPolygon_2()
{
	for(byte i=0; i<10; i++)
	{
		float	w = i * PI / 5, c = sin(w), s = cos(w), r = (i & 1) ? 1.0 : 0.4;
		px[i] = r * c; py[i] = r * s;

	}
}

void titleFadeOut_2()
{
	float radius = 1;
	float inc = 1;
	byte x = 20;

	while(radius < 28)
	{
		rirq_wait();
		colorAreaCircle_2(20, 12, (byte)radius, GCOL_BLACK, false);
		radius += inc;
		inc += 0.02 * radius;
	}
}

#pragma code(bcode3)
#pragma data(bdata3)

// ---------------- BANK 3 Scene

void lchar_put_3(int cx, char cy, char c, byte spray)
{
	// Get pointer to glyph data
	sp = Charset + 8 * c;
	// Loop over all pixel
	for (char y = 0; y < 8; y++)
	{
		char cl = sp[y];
		for (char x = 0; x < 8; x++)
		{
			// Draw two pixel if bit is set
			if (cl & 128)
			{
				proxy0_bm_put(&Screen, cx + 2 * x + 0, cy + 2 * y + 0, true);
				proxy0_bm_put(&Screen, cx + 2 * x + 0, cy + 2 * y + spray, true);
				proxy0_bm_put(&Screen, cx + 2 * x + spray, cy + 2 * y + 0, true);
				proxy0_bm_put(&Screen, cx + 2 * x + spray, cy + 2 * y + spray, true);
			}

			// Next bit
			cl <<= 1;
		}
	}
}


void saveSimulationStart_3(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	saveSimPx = go->px;
	saveSimPy = go->py;
	hasSimHit = false;
	simEndCnt = 0;
	simDone = false;
	simSteps = 0;
	proxy32_particle_init();
}

void restoreSimulationFinish_3(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];
	go->px = saveSimPx;
	go->py = saveSimPy;
	go->ax = 0;
	go->ay = 0;
	go->vx = 0;
	go->vy = 0;
	proxy32_particle_remove(false);	
}


byte simulatePhysics_3(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	if(joyx[0] != 0 || joyy[0] != 0 || joyb[0] != 0) 	
	{
		return SIM_CHANGE;
	}
	if(simDone)
	{
		return SIM_DONE;
	}

	if(hasSimHit == true)
	{
		simEndCnt++;
		if(simEndCnt > MaxSimEndTime)
		{
			return SIM_END;
		}
	} 	

	go->ax = -go->vx;
	go->ay = -go->vy + (float)gravity;

	go->vx += go->ax * timeElapsing ; //* go->fSimTimeRemaining;	// Update Velocity
	go->vy += go->ay * timeElapsing ; //* go->fSimTimeRemaining;

	go->px += go->vx * timeElapsing ; //* go->fSimTimeRemaining;	// Update position
	go->py += go->vy * timeElapsing ; //* go->fSimTimeRemaining;

	go->boundingBox = getBoundingBoxCircle_3(go->px, go->py, 4,8);

	if (go->px < 6 || go->px >= ScreenWidth - 6)
	{
		hasSimHit = true;
		go->px = go->px < 6 ? 6 : ScreenWidth - 6;
		go->vx = -go->vx;
	}
	if ((globalOrientation == ORIENTATION_DOWN && go->py < 8) )
	{
		hasSimHit = true;
		go->py = 8;
		go->vy = -go->vy;
	}

	if (globalOrientation == ORIENTATION_UP && go->py > ScreenHeight)
	{
		hasSimHit = true;
		go->py = ScreenHeight;
		go->vy = -go->vy;
	}
	if ((globalOrientation == ORIENTATION_DOWN && go->py > ScreenHeight - 8) || (globalOrientation == ORIENTATION_UP && go->py < 16))
	{
		return SIM_END;
	}

	for (loopc = 0; loopc < numObjects; loopc++)
	{
		struct GameObject *target;
		target = &gameObjects[loopc];
		if (loopc != id)
		{
			if (IsBitSet(target->comp0, Component0_CircleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
			{
				if (checkAabbCollision(&(go->boundingBox), &(target->boundingBox)))
				{
					hasSimHit = true;
					if (proxy31_doCirclesOverlap(go->px, go->py, go->radius, target->px, target->py, target->radius))
					{
						if (IsBitSet(target->comp3, Component3_PortalActive) 
						|| IsBitSet(target->comp2, Component2_Kill))
						{
							return SIM_END;
						}

						// Collision has occured
						proxy32_addCollidingPairs(id, loopc, 0xff);

						if (IsBitSet(target->comp1, Component1_Catch) && loopc != mainCannon)
						{
							return SIM_END;
						}
						// Distance between ball centers
						float fDistance = qsqrt_3((go->px - target->px) * (go->px - target->px) + (go->py - target->py) * (go->py - target->py));

						// Displace Current Ball away from collision

						// Calculate displacement required
						float fOverlap = 0.5 * (fDistance - (float)go->radius - (float)target->radius);
						go->px -= fOverlap * (go->px - target->px) / fDistance;
						go->py -= fOverlap * (go->py - target->py) / fDistance;
					}
				}
			}

			if (IsBitSet(target->comp0, Component0_CapsuleCollider) && IsBitSet(target->comp0, Component0_CollideAble))
			{
				if (checkAabbCollision(&(go->boundingBox), &(target->boundingBox)))
				{
					hasSimHit = true;

					// Check that line formed by velocity vector, intersects with line segment
					float fLineX1 = target->ax - target->px;
					float fLineY1 = target->ay - target->py;

					float fLineX2 = go->px - target->px;
					float fLineY2 = go->py - target->py;

					float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

					// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
					// how much of the segment is in the "shadow" of the object vector. The min and max clamp
					// this to lie between 0 and the line segment length, which is then normalised. We can
					// use this to calculate the closest point on the line segment
					float t = max_1(0.0, min_1(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

					// Which we do here
					float fClosestPointX = target->px + t * fLineX1;
					float fClosestPointY = target->py + t * fLineY1;

					// And once we know the closest point, we can check if the ball has collided with the segment in the
					// same way we check if two balls have collided
					float dx = go->px - fClosestPointX;
					float dy = go->py - fClosestPointY;
					float fDistanceSq = dx * dx + dy * dy;

					// Compare squared distance with squared sum of radii
					float radiusSum = go->radius + target->radius;
					if (fDistanceSq <= radiusSum * radiusSum) 
					{
						if (IsBitSet(target->comp2, Component2_Kill))
						{
							return SIM_END;
						}

						float fDistance = qsqrt_3(fDistanceSq);
						// Collision has occurred - treat collision point as a ball that cannot move. To make this
						// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
						// so it behaves like a solid object when the momentum calculations are performed
						proxy32_addFakeBalls(fClosestPointX, fClosestPointY, -go->vx, -go->vy, target->radius, target->efficiency);

						// Add collision to vector of collisions for dynamic resolution
						proxy32_addCollidingPairs(id, 0xff, numFakes - 1);

						// Calculate displacement required
						float fOverlap = 1.0 * (fDistance - go->radius - target->radius);

						// Displace Current Ball away from collision
						go->px -= fOverlap * (go->px - fClosestPointX) / fDistance;
						go->py -= fOverlap * (go->py - fClosestPointY) / fDistance;
					}
				}
			}
		}
	}
	return SIM_RUN;
}

float qsqrt_3(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5;

  x2 = number * 0.5;
  y  = number;
  i  = * ( long * ) &y;
  i  = 0x5f3759df - ( i >> 1 );
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );

  return 1/y;
}

struct Box getBoundingBoxLine_3(float x1, float y1, float x2, float y2, float radius)
{
	struct Box boundingBox;
	boundingBox.x = (x1 < x2) ? (int)x1 : (int)x2;
	boundingBox.y = (y1 < y2) ? (int)y1 : (int)y2;
	boundingBox.width = ((x1 > x2) ? (int)x1 - boundingBox.x : (int)x2 - boundingBox.x);
	boundingBox.height = ((y1 > y2) ? (int)y1 - boundingBox.y : (int)y2 - boundingBox.y);

	boundingBox.x -= (int)radius;
	boundingBox.y -= (int)radius;
	boundingBox.width += (int)radius * 2 + 4;
	boundingBox.height += (int)radius * 2 + 4;

	return boundingBox;
}

__native struct Box getBoundingBoxChar_3(char x1, char y1, char w, char h)
{
	struct Box boundingBox;
	boundingBox.x = x1 * 8;
	boundingBox.y = y1 * 8;
	boundingBox.width = w * 8;
	boundingBox.height = h * 8;
	return boundingBox;
}

__native struct Box getBoundingBoxCircle_3(float x1, float y1, float radius, int radius2)
{
	struct Box boundingBox;
	boundingBox.x = (int)x1 - (int)radius;
	boundingBox.y = (int)y1 - (int)radius;
	boundingBox.width = (int)radius2 + 5;
	boundingBox.height = (int)radius2 + 5;
	return boundingBox;
}

void initBonusLine_3()
{
	struct GameObject *go;
	byte posy;
	if(globalOrientation == ORIENTATION_DOWN)
	{
		posy = 200 - 15;
	}
	else
	{
		posy = 8 + 15;
	}
	for(byte i =0; i<5; i++)
	{
		byte foundObj = 0xff;
		for (byte j = 0; j < numObjects; j++)
		{
			go = &gameObjects[j];
			if(go->comp0 == 0 && go->comp1 == 0 && go->comp2 == 0 && go->comp3 == 0)
			{
				foundObj = j;
				break;
			}
		}
		if(foundObj == 0xff)
		{
			foundObj = numObjects++;
		}
		setGameLetterCircle_3(foundObj, (int)(40 + i * 60), posy, 19, GCOL_PURPLE, bonusTxt[i]);
		proxy31_updateRender(foundObj, true);
	}
}



void addActor_3(byte event1, byte event2, byte senderId, byte action, byte receiverStart, byte receiverEnd, byte param0, byte param1, byte var0)
{
	if (numActors < MaxActors)
	{
		actorList[numActors].event1 = event1;
		actorList[numActors].event2 = event2;
		actorList[numActors].senderId = senderId;
		actorList[numActors].action = action;
		actorList[numActors].receiverStart = receiverStart;
		actorList[numActors].receiverEnd = receiverEnd;
		actorList[numActors].param0 = param0;
		actorList[numActors].param1 = param1;
		actorList[numActors].var0 = var0;
		numActors++;
	}
}

void addDepObject_3(byte sourceId, byte targetId)
{
	if (numDepObjects < MaxDepObjects)
	{
		depObjects[numDepObjects].sourceId = sourceId;
		depObjects[numDepObjects].targetId = targetId;
		numDepObjects++;
	}
}

void setGameObjectBall_3(byte id, float px, float py, float vx, float vy, float radius, byte spritenum, bool isMainBall, bool isOn, byte color)
{
	gameObjects[id].spriteNum = spritenum;
	gameObjects[id].comp0 = Component0_Render | Component0_Physics | Component0_CircleCollider | Component0_CollideAble;
	gameObjects[id].comp1 = Component1_DrawBall;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = vx;
	gameObjects[id].vy = vy;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = color;
	gameObjects[id].isOn = isOn;
	gameObjects[id].pattern = 64;
	if (isMainBall)
	{
		mainBall = id;
		gameObjects[id].comp1 |= Component1_MainBall;
	}
	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, radius, (int)radius*2);
}

void setGameObjectActivateBallCircle_3(byte id, float px, float py, float r, float efficiency, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle | Component0_CollideAble;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_BallActivate;
	gameObjects[id].comp3 = Component3_CollisionTimerAble;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].isOn = true;
	gameObjects[id].pattern = PAT_PLUS;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}

void setGameObjectSwitchCircle_3(byte id, float px, float py, float r, float efficiency, byte color, bool isOn)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle | Component0_CollideAble;
	gameObjects[id].comp1 = Component1_Switch;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].isOn = isOn;

	if (isOn)
	{
		gameObjects[id].pattern = PAT_TRUE;
	}
	else
	{
		gameObjects[id].pattern = PAT_FALSE;
	}

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}

void setGameObjectTimedDelayedCircle_3(byte id, float px, float py, float r, float efficiency, byte color, byte pattern, bool collideAble, bool valueable, signed int baseScore)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if (collideAble)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete;
	if (valueable)
	{
		gameObjects[id].comp1 = Component1_TimedDelete | Component1_Valueable;
		totalValueableObjects++;
	}
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].pattern = pattern;
	gameObjects[id].baseScore = baseScore;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}

void setGameObjectTimedValuableCircle_3(byte id, float px, float py, float r, bool isCollideAble)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if (isCollideAble)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete | Component1_Valueable;
	totalValueableObjects++;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = GCOL_ORANGE;
	gameObjects[id].pattern = PAT_RAUTE;
	gameObjects[id].baseScore = 100;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}

void setGameObjectTimedCircle_3(byte id, float px, float py, float r, bool isCollideAble)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if (isCollideAble)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = GCOL_BLUE;
	gameObjects[id].pattern = PAT_CHESS;
	gameObjects[id].baseScore = 10;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}


void setGameObjectBombWallCircle_3(byte id, float px, float py, float r, bool isCollideAble)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if (isCollideAble)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_BombWall;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = GCOL_BROWN;
	gameObjects[id].pattern = PAT_BOMBWALL;
	gameObjects[id].baseScore = 20;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r, (int)r*2);
}

void setGameObjectTimedBonusScoreCircle_3(byte id, float px, float py, float r, bool isCollideAble)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if (isCollideAble)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete;
	gameObjects[id].comp2 = Component2_BonusScore;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = GCOL_GREEN;
	gameObjects[id].pattern = PAT_BONUS;
	gameObjects[id].baseScore = 500;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectBumpCircle_3(byte id, float px, float py, float r, float efficiency, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle | Component0_CollideAble;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_DrawStar;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].pattern = PAT_SOLID;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectBombCircle_3(byte id, float px, float py, float r, float efficiency, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle | Component0_CollideAble;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = Component3_Bomb | Component3_ImmediateDelete;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].pattern = PAT_BOMB;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectKillCircle_3(byte id, float px, float py, float r, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle | Component0_CollideAble;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_Kill;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = color;
	gameObjects[id].pattern = PAT_DANGER;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectValuableCrackCircle_3(byte id, float px, float py, float r)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_CollideAble | Component0_DrawCircle;
	gameObjects[id].comp1 = Component1_CrackDelete | Component1_Valueable;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	totalValueableObjects++;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = GCOL_ORANGE;
	gameObjects[id].crackLevel = 3;
	gameObjects[id].baseScore = 100;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectCrackCircle_3(byte id, float px, float py, float r, float efficiency, int color, byte crackLevel, bool isValueable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_CollideAble | Component0_DrawCircle;
	gameObjects[id].comp1 = Component1_CrackDelete;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].baseScore = 0;
	if (isValueable)
	{
		gameObjects[id].comp1 = Component1_CrackDelete | Component1_Valueable;
		gameObjects[id].baseScore = 100;
		totalValueableObjects++;
	}
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].crackLevel = crackLevel;

	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameLetterCircle_3(byte id, float px, float py, float r, byte color, byte pattern)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_CollideAble | Component0_DrawCircle;
	gameObjects[id].comp1 = Component1_TimedDelete;
	gameObjects[id].comp2 = Component2_BonusScore;
	gameObjects[id].comp3 = Component3_DrawLetter;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 2;
	gameObjects[id].color = color;
	gameObjects[id].crackLevel = 2;
	gameObjects[id].baseScore = 500;
	gameObjects[id].pattern = pattern;
	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}


void setGameObjectCircleOrientation_3(byte id, float px, float py, float r, byte orientation, byte color, byte pattern, byte comp1)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_CollideAble | Component0_DrawCircle;
	gameObjects[id].comp1 = Component1_SwitchOrientation | comp1;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = color;
	gameObjects[id].orientation = orientation;
	gameObjects[id].crackLevel = 3;
	gameObjects[id].pattern = pattern;
	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectWallCircle_3(byte id, float px, float py, float r, float efficiency, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_CollideAble | Component0_DrawCircle;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].orientation = ORIENTATION_DOWN;
	gameObjects[id].pattern = PAT_WALL;
	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}


void setGameObjectCannon_3(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon)
{
	struct Box boundingBox;
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_DrawCannon | Component0_CircleCollider | Component0_CollideAble;
	gameObjects[id].comp1 = Component1_Catch;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].orientation = orientation;
	if (useGlobalOrientation)
	{
		if (globalOrientation == ORIENTATION_UP)
		{
			gameObjects[id].py = ScreenHeight - py;
		}
		gameObjects[id].orientation = globalOrientation;
	}
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = color;
	boundingBox = getBoundingBoxCircle_3(px, gameObjects[id].py, r,(int)r*2);
	gameObjects[id].boundingBox = boundingBox;
	if (isMainCannon == true)
	{
		mainCannon = id;
		currentCannon = id;
	}
	switch (gameObjects[id].orientation)
	{
	case ORIENTATION_DOWN:
		gameObjects[id].cannonVector.v[0] = 0;
		gameObjects[id].cannonVector.v[1] = gameObjects[id].radius;
		break;
	case ORIENTATION_UP:
		gameObjects[id].cannonVector.v[0] = 0;
		gameObjects[id].cannonVector.v[1] = -(gameObjects[id].radius);
		break;
	case ORIENTATION_LEFT:
		gameObjects[id].cannonVector.v[0] = -(gameObjects[id].radius);
		gameObjects[id].cannonVector.v[1] = 0;
		break;
	case ORIENTATION_RIGHT:
		gameObjects[id].cannonVector.v[0] = gameObjects[id].radius;
		gameObjects[id].cannonVector.v[1] = 0;
		break;
	}
	currentAngleStep = 0;
}

void setGameObjectValueableTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_DrawEdge;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete | Component1_Valueable;
	totalValueableObjects++;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;

	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.3;
	gameObjects[id].color = GCOL_ORANGE;
	gameObjects[id].baseScore = 100;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
	gameObjects[id].pattern = PAT_RAUTE;
}

void setGameObjectBombWallEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_DrawEdge;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_BombWall;
	gameObjects[id].comp3 = 0;

	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.3;
	gameObjects[id].color = GCOL_BROWN;
	gameObjects[id].baseScore = 20;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
	gameObjects[id].pattern = PAT_BOMBWALL;
}

void setGameObjectBonusScoreTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_DrawEdge;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_BonusScore;
	gameObjects[id].comp3 = 0;

	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.3;
	gameObjects[id].color = GCOL_PURPLE;
	gameObjects[id].baseScore = 2000;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
	gameObjects[id].pattern = PAT_BONUS;
}

void setGameObjectTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_DrawEdge;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = Component1_TimedDelete;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;

	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.3;
	gameObjects[id].color = GCOL_BLUE;
	gameObjects[id].baseScore = 10;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
	gameObjects[id].pattern = PAT_CHESS;
}

void setGameObjectLaser_3(byte id, float px, float py, float ax, float ay, byte color, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_Laser | Component2_Kill;
	gameObjects[id].comp3 = 0;

	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;	
	gameObjects[id].ay = ay;
	gameObjects[id].radius = 3;
	gameObjects[id].efficiency = 0.6;
	gameObjects[id].color = color;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, 4);
	gameObjects[id].pattern = PAT_SOLID;
}

void setGameObjectWallEdge_3(byte id, float px, float py, float ax, float ay, float radius, float efficiency, byte color, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_DrawEdge;
	if (isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}

	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
	gameObjects[id].pattern = PAT_WALL;
}

void setGameObjectCrackEdge_3(byte id, float px, float py, float ax, float ay, float radius, float efficiency, byte color, byte crackLevel, bool isValueable, signed int baseScore)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_CollideAble | Component0_DrawEdge;
	gameObjects[id].comp1 = Component1_CrackDelete;
	if (isValueable)
	{
		gameObjects[id].comp1 = Component1_CrackDelete | Component1_Valueable;
		totalValueableObjects++;
	}
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = efficiency;
	gameObjects[id].color = color;
	gameObjects[id].crackLevel = crackLevel;
	gameObjects[id].baseScore = baseScore;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
}

void setGameObjectValueableCrackEdge_3(byte id, float px, float py, float ax, float ay, float radius)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CapsuleCollider | Component0_CollideAble | Component0_DrawEdge;
	gameObjects[id].comp1 = Component1_CrackDelete | Component1_Valueable;
	totalValueableObjects++;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].ax = ax;
	gameObjects[id].ay = ay;
	gameObjects[id].radius = radius;
	gameObjects[id].efficiency = 0.3;
	gameObjects[id].color = GCOL_ORANGE;
	gameObjects[id].crackLevel = 3;
	gameObjects[id].baseScore = 100;
	gameObjects[id].boundingBox = getBoundingBoxLine_3(px, py, ax, ay, radius);
}

void setGameObjectPortal_3(byte id, float px, float py, float r, bool isCollideable)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = Component0_CircleCollider | Component0_DrawCircle;
	if(isCollideable)
	{
		gameObjects[id].comp0 |= Component0_CollideAble;
	}
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = Component3_CollisionTimerAble | Component3_PortalActive;
	gameObjects[id].px = px;
	gameObjects[id].py = py;
	gameObjects[id].ax = 0;
	gameObjects[id].ay = 0;
	gameObjects[id].vx = 0;
	gameObjects[id].vy = 0;
	gameObjects[id].radius = r;
	gameObjects[id].efficiency = 0.8;
	gameObjects[id].color = GCOL_WHITE;
	gameObjects[id].crackLevel = 2;
	gameObjects[id].baseScore = 20;
	gameObjects[id].pattern = PAT_SOLID;
	gameObjects[id].boundingBox = getBoundingBoxCircle_3(px, py, r,(int)r*2);
}

void setGameObjectLargeText_3(byte id, char py, bool isOn, byte orientation, byte textId, byte color)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = 0;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = Component2_LargeText;
	gameObjects[id].comp3 = 0;
	gameObjects[id].px = 0;
	gameObjects[id].py = py;
	gameObjects[id].ax = 40;
	gameObjects[id].ay = 2;
	gameObjects[id].color = color;
	gameObjects[id].pattern = textId;
	gameObjects[id].isOn = isOn;
	gameObjects[id].orientation = orientation;
	char z = 2;
	if (orientation != 0x7f)
	{
		z = 3;
	}

	gameObjects[id].boundingBox = getBoundingBoxChar_3(0, py, 40, z);
}

void setGameObjectImage_3(byte id, byte imageId, char x, char y, char w, char h, bool isOn, byte color, bool disableBonus)
{
	gameObjects[id].spriteNum = 0xff;
	gameObjects[id].comp0 = 0;
	gameObjects[id].comp1 = 0;
	gameObjects[id].comp2 = 0;
	gameObjects[id].comp3 = Component3_DrawImage;
	if(disableBonus)
	{
		gameObjects[id].comp3 |= Component3_NoBonusImage;
	}	
	gameObjects[id].px = x;
	gameObjects[id].py = y;
	gameObjects[id].ax = w;
	gameObjects[id].ay = h;
	gameObjects[id].color = color;
	gameObjects[id].pattern = imageId;
	gameObjects[id].isOn = isOn;
	gameObjects[id].orientation = 0x7f;
	gameObjects[id].boundingBox = getBoundingBoxChar_3(x, y, w, h);
}

void setupScoreBoard_3()
{
	for (byte i = 0; i < 5; i++)
	{
		scoreBoard[i].pegThreshold = (byte)(scoreBoard[i].threshold * totalValueableObjects);
	}
}

void preInitScene_3()
{
	numObjects = 0; // Initialize the item count
	numActors = 0;
	totalValueableObjects = 0;
	spriteCount = 0;
	multiplier = 1;
	extraBalls = false;
	collideQueueFront = -1;
	collideQueueRear = -1;
	scoreQueueFront = -1;
	scoreQueueRear = -1;
	cntMusic = 0;
	timeElapsing = TimeElapsingNorm;
	bombDistance = 60;
	isGravCannon = true;
	proxy32_setGlobalOrientation(ORIENTATION_DOWN, true);	

	struct CollidingTimer *ct;

	for (byte i = 0; i < MaxColTimers; i++)
	{
		ct = &collidingTimers[i];
		ct->id = 0xff;
		ct->timer = 0;
	}
	proxy0_bm_rect_clear(&Screen, &scr, 0, 8, ScreenWidth, ScreenHeight - 8);
	//proxy32_string_write(0, 0, scoreTxt, VCOL_WHITE);
}

void postInitScene_3()
{
	setupScoreBoard_3();
	if(numObjects >= MaxObjects)
	{
		vic.color_border = VCOL_CYAN;
	}

	for (int i = 0; i < numObjects; i++)
	{
		proxy31_updateRender(i, true);
	}
	proxy32_dispatchEvent(EventInitLevel, 0xff, 0xff, 0xff);
}

#pragma code(bcode4)
#pragma data(bdata4)

void initScene_4(void)
{
	switch (level)
	{
	case 0:
		initScene0_4();
		break;
	case 1:
		initScene1_4();
		break;
	case 2:
		initScene2_4();
		break;
	case 3:
		initScene3_4();
		break;
	case 4:
		initScene4_4();
		break;
	case 5:
		initScene5_4();
		break;
	case 6:
		initScene6_4();
		break;
	case 7:
		initScene7_4();
		break;
	case 8:
		initScene8_4();
		break;
	case 9:
		initScene9_4();
		break;
	case 10:
		proxy45_initScene10();
		break;
	case 11:
		proxy45_initScene11();
		break;
	case 12:
		proxy45_initScene12();
		break;
	case 13:
		proxy45_initScene13();
		break;
	case 14:
		proxy45_initScene14();
		break;
	case 15:
		proxy45_initScene15();
		break;
	case 16:
		proxy45_initScene16();
		break;
	case 17:
		proxy45_initScene17();
		break;
	case 18:
		proxy45_initScene18();
		break;
	case 19:
		proxy45_initScene19();
		break;
	default:
		break;
	}
	return;
}

void drawBackgroundImage_4(byte id)
{
	struct GameObject *go;
	go = &gameObjects[id];

	byte* byte_stream;
	byte fromBank;

	switch (go->pattern)
	{
		case IMG_LANDSCAPE1:
			fromBank=8;
			byte_stream = ImgLandscape1;
			break;
		case IMG_LANDSCAPE2:
			fromBank=8;
			byte_stream = ImgLandscape2;
			break;
		case IMG_MOON:
			fromBank=8;
			byte_stream = ImgMoon;
			break;
		case IMG_ALIEN1:
			fromBank=8;
			byte_stream = ImgAlien1;
			break;
		case IMG_VALLEYL:
			fromBank=8;
			byte_stream = ImgValleyL;
			break;
		case IMG_VALLEYR:
			fromBank=8;
			byte_stream = ImgValleyR;
			break;
		case IMG_GRIDSTAR:
			fromBank=8;
			byte_stream = ImgGridstar;
			break;
		case IMG_ALIEN2:
			fromBank=8;
			byte_stream = ImgAlien2;
			break;
		case IMG_LANDSCAPE3:
			fromBank=9;
			byte_stream = ImgLandscape3;
			break;
		case IMG_SHIP1:
			fromBank=9;
			byte_stream = ImgShip1;
			break;
		case IMG_GALAXY:
			fromBank=9;
			byte_stream = ImgGalaxy;
			break;
		default:
			return;
	}
	proxy_drawImageUnclipped(fromBank, 4, byte_stream,  (byte)go->px, (byte)go->py, (byte)go->ax, (byte)go->ay);
	colorAreaUnclipped((byte)go->px, (byte)go->py, (byte)go->ax, (byte)go->ay, (byte)go->color, true);
}

void initScene0_4()
{
	registerCallBank(4);
    balls = 10;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    float step1 = 0;
    float step2 = 0;

    float x1e = 0, y1e = 25, x2e = 320, y2e = 25;

	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE1, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_ALIEN1, 16, 5, 8, 8, true, GCOL_MED_GREY, false);
	//proxy3_setGameObjectWallEdge(numObjects++, 130, 180, 180, 180, 4, 0.2, GCOL_RED, true);
	// proxy3_setGameObjectValueableTimedEdge(numObjects++, 180, 150, 180, 170, 4, true);
	// proxy3_setGameObjectValueableTimedEdge(numObjects++, 200, 150, 200, 170, 4, true);
	// proxy3_setGameObjectValueableTimedEdge(numObjects++, 220, 150, 220, 170, 4, true);

    byte t1 = numObjects;
    proxy3_setGameObjectTimedDelayedCircle(numObjects++, 70, 30, 6, 0.6, GCOL_YELLOW, PAT_BONUS, true, false, 100);

    byte t2 = numObjects;
    proxy3_setGameObjectTimedDelayedCircle(numObjects++, 250, 30, 6, 0.6, GCOL_YELLOW, PAT_BONUS, true, false, 100);
    for(byte i = 0; i < 6; i++)
    {
        float xa1e = x1e + 19;
        float ya1e = y1e + 8 + i;
        float xa2e = x2e - 19;
        float ya2e = y2e + 8 + i;

        if((i + 3) % 4 == 0)
        {
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }
        else
        {
            proxy3_setGameObjectTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }

        x1e = xa1e;
        y1e = ya1e;
        x2e = xa2e;
        y2e = ya2e;
    }

	byte start1 = numObjects;

	for (byte i = 4; i < 20; i++)
	{
		step1 += 2 * PI / 12;

		float x1 = 15 * (i * 0.25) * sin(step1) + 80;
		float y1 = 10 * (i * 0.25) * cos(step1) + 130;
		if((i + 3) % 4 == 0)
		{
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x1, y1, 6, true);
		}
		else
		{
			proxy3_setGameObjectTimedCircle(numObjects++, x1, y1, 6, true);
		}
	}
	proxy3_setGameObjectBombCircle(numObjects++, 80,130,10, 0.8, GCOL_LT_RED);
	byte end1 = numObjects - 1;
	byte start2 = numObjects;

	for (byte i = 4; i < 20; i++)
	{
		step2 -= 2 * PI / 12;

		float x2 = 15 * (i * 0.25) * sin(step2) + 240;
		float y2 = 10 * (i * 0.25) * cos(step2) + 130;
		if((i + 3) % 4 == 0)
		{
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x2, y2, 6, true);
		}
		else
		{
			proxy3_setGameObjectTimedCircle(numObjects++, x2, y2, 6, true);
		}
	}
	proxy3_setGameObjectBombCircle(numObjects++, 240,130,10, 0.8, GCOL_LT_RED);

	byte end2 = numObjects - 1;

	proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionHideObject, start1, end1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionHideObject, start2, end2, 0xff, 0xff, 0xff);
	proxy3_addActor(EventCollide, 0xff, t1, ActionShowObject, start1, end1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventCollide, 0xff, t2, ActionShowObject, start2, end2, 0xff, 0xff, 0xff);	

	largeTextId = numObjects;
	largeTextTimer = LARGE_TEXT_TIMER;
	proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene1_4()
{
	registerCallBank(4);
    balls = 14;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 4, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE1, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_MOON, 14, 17, 12, 5, true, GCOL_MED_GREY, false);
    float step1 = 0;
    float y = 0, xa = 0, ya = 140;
    byte i = 0;
    for (float x = 8; x <= 320; x += 16)
    {
        y = 40 * cos((x / 320) * 5 * PI) + 100;

        if (i == 1 || i == 19)
        {
            proxy3_setGameObjectTimedEdge(numObjects++, x, y, xa, ya, 4, false);
        }
        else if (i % 4 == 0)
        {
            proxy3_setGameObjectTimedEdge(numObjects++, x, y, xa, ya, 4, true);
        }
        else
        {
            proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 4, 0.3, GCOL_RED, true);
        }
        xa = x;
        ya = y;
        i++;
    }

    proxy3_setGameObjectLaser(numObjects++, 90, 75, 165, 75, GCOL_YELLOW, true);
    byte d1 = numObjects;
    proxy3_setGameObjectLaser(numObjects++, 215, 75, 290, 75, GCOL_YELLOW, true);
    byte d2 = numObjects;
    proxy3_setGameObjectLaser(numObjects++, 25, 125, 100, 125, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 155, 125, 230, 125, GCOL_YELLOW, true);
    byte t1 = numObjects;
    proxy3_setGameObjectCrackCircle(numObjects++, 128, 114, 8, 0.4, GCOL_ORANGE, 2, true);
    proxy3_setGameObjectCrackCircle(numObjects++, 253, 114, 8, 0.4, GCOL_ORANGE, 3, true);
    proxy3_setGameObjectCrackCircle(numObjects++, 63, 86, 8, 0.4, GCOL_ORANGE, 3, true);
    byte t2 = numObjects;
    proxy3_setGameObjectCrackCircle(numObjects++, 193, 86, 8, 0.4, GCOL_ORANGE, 2, true);

    proxy3_setGameObjectCircleOrientation(numObjects++, 22, 60, 9, ORIENTATION_UP, GCOL_GREEN, PAT_ARROW_UP, 0);
    proxy3_setGameObjectCircleOrientation(numObjects++, 298, 140, 9, ORIENTATION_DOWN, GCOL_GREEN, PAT_ARROW_DOWN, 0);

    proxy3_addActor(EventCollide, 0xff, t1, ActionDisableCollision, d1, d1, 0xff, 0xff, 0xff);
    proxy3_addActor(EventCollide, 0xff, t2, ActionDisableCollision, d2, d2, 0xff, 0xff, 0xff);


    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene2_4()
{
	registerCallBank(4);
    balls = 8;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE2, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);

    float stepO = 270 * PI / 180;

    for (byte j = 0; j < 5; j++)
    {
        stepO += 30 * PI / 180;
        float xz = 140 * sin(stepO) + 160;
        float yz = 70 * cos(stepO) + 30;
        float step = 0;
        byte start = numObjects;
        byte actId = 0;

        step = 60 * PI / 180;

        for (byte i = 0; i < 5; i++)
        {
            step += 2 * PI / 5;
            float x = 28 * sin(step) + xz;
            float y = 28 * cos(step) + yz;
            bool col = false;
            if (i == j)
            {
                actId = numObjects;
                col = true;
            }
            proxy3_setGameObjectLaser(numObjects++, x, y, xz, yz, GCOL_YELLOW, col);
        }
        byte end = numObjects - 1;
        proxy3_addActor(EventResetBall, 0xff, 0xff, ActionTimedRollActivateCollideable, start, end, 0xff, 0xff, actId);
    }

    for (byte i = 0; i < 2; i++)
    {
        for (byte j = 0; j < 6; j++)
        {
            int x = 40 + j * 48;
            int y = 140 + i * 25;
            proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
        }
    }

    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene3_4()
{
	registerCallBank(4);
    balls = 8;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYL, 0, 20, 11, 5, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYR, 29, 20, 11, 5, true, GCOL_DARK_GREY, true);

    int xz = 0, yz = 0, o = 0, e = 0;

    for (byte j = 0; j < 3; j++)
    {
        if (j == 0)
        {
            o = 4;
            e = 0;
            xz = 70;
            yz = 80;
        }
        if (j == 1)
        {
            o = 6;
            e = 2;
            xz = 250;
            yz = 80;
        }
        if (j == 2)
        {
            o = 5;
            e = 0xff;
            xz = 160;
            yz = 140;
        }

        float step = 15 * PI / 180;

        float xa = 45 * sin(step) + xz;
        float ya = 45 * cos(step) + yz;
        for (byte i = 0; i < 12; i++)
        {
            step += 30 * PI / 180;

            float x = 45 * sin(step) + xz;
            float y = 45 * cos(step) + yz;

            if (i != o && i != 11)
            {
                proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 4, 0.3, GCOL_RED, true);
            }

            xa = x;
            ya = y;
        }
        step = 0;
        for (byte i = 0; i < 4; i++)
        {
            step += 90 * PI / 180;

            float x = 25 * sin(step) + xz;
            float y = 25 * cos(step) + yz;
            if (i == e)
            {
                proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x, y, 6, true);
            }
            else
            {
                proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
            }
        }
    }

    byte trigger1 = numObjects;
    proxy3_setGameObjectBumpCircle(numObjects++, 70 + 4, 80 + 4, 12, 0.8, GCOL_MED_GREY);
    byte trigger2 = numObjects;
    proxy3_setGameObjectBumpCircle(numObjects++, 250 + 4, 80 + 4, 12, 0.8, GCOL_MED_GREY);
    byte trigger3 = numObjects;
    proxy3_setGameObjectBumpCircle(numObjects++, 160 + 4, 140 + 4, 12, 0.8, GCOL_MED_GREY);

    byte startg1 = numObjects;
    proxy3_setGameObjectPortal(numObjects++, 92, 40, 8, false);    
    proxy3_setGameObjectPortal(numObjects++, 228, 40, 8, false);
    proxy3_setGameObjectPortal(numObjects++, 160, 185, 8, false);
    proxy3_setGameObjectPortal(numObjects++, 70, 125, 8, false);
    proxy3_setGameObjectPortal(numObjects++, 160, 95, 8, false);
    proxy3_setGameObjectPortal(numObjects++, 250, 125, 8, false);
    byte endg1 = numObjects - 1;

    proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionRandomActivateCollideable, startg1, endg1, 2, 0xff, 0xff);
    proxy3_addActor(EventCollide, 0xff, trigger1, ActionRandomActivateCollideable, startg1, endg1, 2, 0xff, 0xff);
    proxy3_addActor(EventCollide, 0xff, trigger2, ActionRandomActivateCollideable, startg1, endg1, 2, 0xff, 0xff);
    proxy3_addActor(EventCollide, 0xff, trigger3, ActionRandomActivateCollideable, startg1, endg1, 2, 0xff, 0xff);

    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene4_4()
{
	registerCallBank(4);
    balls = 6;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE2, 0, 22, 40, 3, true, 0xab, true);
    byte targetId = numObjects;
    proxy3_setGameObjectBall(numObjects++, 160, 108, 0, 0, 6, 1, false, false, VCOL_LT_GREEN);

    float radius = 40;
    float stepO = 0;
    for (byte j = 0; j < 3; j++)
    {
        stepO += 15 * PI / 180;
        float step = stepO;
        for (int i = 0; i < 12; i++)
        {
            step += 30 * PI / 180;
            float x = radius * cos(step) + 160;
            float y = radius * 0.6 * sin(step) + 108;
            if (i % 3 == 0)
            {
                proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
            }
            else
            {
                proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
            }
        }
        radius += 30;
    }    

    byte sourceId = numObjects;
    proxy3_setGameObjectActivateBallCircle(numObjects++, 160, 108, 7, 0.6, GCOL_WHITE);

    proxy3_addDepObject(sourceId, targetId);

    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene5_4()
{
	registerCallBank(4);
    balls = 15;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE1, 0, 23, 40, 2, true, 0xab, true);
	proxy3_setGameObjectImage(numObjects++, IMG_GRIDSTAR, 8, 5, 4, 4, true, GCOL_BROWN, true);
	proxy3_setGameObjectImage(numObjects++, IMG_GRIDSTAR, 28, 5, 4, 4, true, GCOL_BROWN, true);

    float x1e = 6;
    float y1e = 8;
    float x2e = 314;
    float y2e = 8;

    for (byte i = 0; i < 12; i++)
    {
        float xa1e = x1e + 5.5;
        float ya1e = y1e + 8 + i;
        float xa2e = x2e - 5.5;
        float ya2e = y2e + 8 + i;

        if (i == 7)
        {
            proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x1e + 135 - i, y1e, 6, true);
            proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x2e - (135 - i), y2e, 6, true);
        }
        if (i > 7)
        {
            proxy3_setGameObjectTimedValuableCircle(numObjects++, x1e + (135 - i), y1e, 6, true);
            proxy3_setGameObjectTimedValuableCircle(numObjects++, x2e - (135 - i), y2e, 6, true);
        }

        if (i > 0 && i % 3 == 0)
        {
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
            if (i > 4)
            {
                proxy3_setGameObjectValueableTimedEdge(numObjects++, x1e + 150, y1e, xa1e + 150, ya1e, 4, true);
                proxy3_setGameObjectValueableTimedEdge(numObjects++, x2e - 150, y2e, xa2e - 150, ya2e, 4, true);
            }
        }
        else
        {
            proxy3_setGameObjectTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
            if (i > 4)
            {
                proxy3_setGameObjectTimedEdge(numObjects++, x1e + 150, y1e, xa1e + 150, ya1e, 4, true);
                proxy3_setGameObjectTimedEdge(numObjects++, x2e - 150, y2e, xa2e - 150, ya2e, 4, true);
            }
        }

        x1e = xa1e;
        y1e = ya1e;
        x2e = xa2e;
        y2e = ya2e;
    }

    proxy3_setGameObjectLaser(numObjects++, 6, 172, 65, 172, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 320 - 6, 172, 320 - 65, 172, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 160 - 4 * 6, 48, 160, 60, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 320 - (160 - 4 * 6), 48, 160, 60, GCOL_YELLOW, true);

    proxy3_setGameObjectCannon(numObjects++, 20, 120, 12, ORIENTATION_RIGHT, GCOL_MED_GREY, false, false);
    proxy3_setGameObjectCannon(numObjects++, 300, 120, 12, ORIENTATION_LEFT, GCOL_MED_GREY, false, false);

    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene6_4()
{
	registerCallBank(4);
    balls = 8;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE3, 0, 22, 40, 3, true, GCOL_BLUE, true);
	proxy3_setGameObjectImage(numObjects++, IMG_ALIEN2, 33, 10, 7, 12, true, GCOL_LT_RED, true);

    byte arraySwitches[9];

    // Adding bump circles in a grid pattern
    for (byte row = 0; row < 3; row++)
    {
        for (byte col = 0; col < 3; col++)
        {
            byte x = 80 + col * 80;
            byte y = 50 + row * 50;
            byte i = (col * 4) + row;

            arraySwitches[i] = numObjects;
            proxy3_setGameObjectSwitchCircle(numObjects++, x, y, 9, 0.8, GCOL_YELLOW, false);
            if (col < 2 && row < 2)
            {
                proxy3_setGameObjectTimedValuableCircle(numObjects++, x + 40, y + 25, 5, true);
            }
        }
    }
    byte i = 0;
    for (byte row = 0; row < 3; row++)
    {
        for (byte col = 0; col < 3; col++)
        {
            byte x = 80 + col * 80;
            byte y = 50 + row * 50;

            if (col < 2 && i % 2 == 0)
            {
                proxy3_setGameObjectLaser(numObjects++, x, y, x + 80, y, GCOL_YELLOW, true);
            }
            if (row < 2 && i % 2 == 1)
            {
                proxy3_setGameObjectLaser(numObjects++, x, y, x, y + 60, GCOL_YELLOW, true);
            }
            proxy3_addActor(EventSwitchOn, 0xff, arraySwitches[i], ActionDisableCollision, numObjects - 2, numObjects - 1, 0xff, 0xff, 0xff);
            proxy3_addActor(EventSwitchOff, 0xff, arraySwitches[i], ActionEnableCollision, numObjects - 2, numObjects - 1, 0xff, 0xff, 0xff);
            i++;
        }
    }

    proxy3_setGameObjectCircleOrientation(numObjects++, 35, 170, 8, ORIENTATION_DOWN, GCOL_RED, PAT_ARROW_DOWN, 0);
    proxy3_setGameObjectCircleOrientation(numObjects++, 320 - 35, 30, 8, ORIENTATION_UP, GCOL_RED, PAT_ARROW_UP, 0);

    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene7_4()
{
	registerCallBank(4);
    balls = 12;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYL, 0, 20, 11, 5, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYR, 29, 20, 11, 5, true, GCOL_DARK_GREY, true);

	proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, 160,125,6,true);
    proxy3_setGameObjectLaser(numObjects++, 140, 140, 160, 100, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 180, 140, 160, 100, GCOL_YELLOW, true);

    int x = 6;
    float y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 60;
    int xa = x;
    float ya = y; 

    byte start1 = numObjects;
    for (byte i = 0; i < 6; i++)
    {
        x += 320 / 12;

        y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 60;

        proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 4, 0.3, GCOL_RED, true);

        xa = x;
        ya = y;
    }
    x = 314;
    y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 60;
    xa = x;
    ya = y; 
    byte end1 = numObjects - 1;

    byte start2 = numObjects;

    for (byte i = 12; i > 6; i--)
    {
        x -= 320 / 12;

        y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 60;

        proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 4, 0.3, GCOL_RED, true);

        xa = x;
        ya = y;
    }
    byte end2 = numObjects - 1;

    float step1 = 270 * PI / 180;
    for (byte i = 0; i < 9; i++)
    {
        x = 100 * sin(step1) + 160;
        y = 25 * cos(step1) + 50;        

        if (i % 2 == 0)
        {        
            proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
        }
        else
        {
            proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
        }
        step1 += 22.5 * PI / 180;
    }

    step1 = 270 * PI / 180;
    for (byte i = 0; i < 8; i++)
    {
        float x2 = 130 * sin(step1) + 160;
        float y2 = 60 * cos(step1) + 115;
		if(i == 3 || i == 4)
		{
			proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x2,y2,6,true);
		}

        else
		{ 
			if (i % 2 == 0)
			{        
				proxy3_setGameObjectTimedValuableCircle(numObjects++, x2, y2, 6, true);
			}
			else
			{
				proxy3_setGameObjectTimedCircle(numObjects++, x2, y2, 6, true);
			}
		}
        step1 += 25.714 * PI / 180;
    }



    // proxy3_setGameObjectCannon(numObjects++, 20, 95, 12, ORIENTATION_RIGHT, GCOL_MED_GREY, false, false);
    // proxy3_setGameObjectCannon(numObjects++, 300, 95, 12, ORIENTATION_LEFT, GCOL_MED_GREY, false, false);
	byte startp = numObjects;
	proxy3_setGameObjectPortal(numObjects++, 20, 95, 10, true);
	proxy3_setGameObjectPortal(numObjects++, 300, 95, 10, true);
	proxy3_setGameObjectPortal(numObjects++, 20, 25, 10, false);
	proxy3_setGameObjectPortal(numObjects++, 300, 25, 10, false);
	byte endp = numObjects - 1;

    proxy3_addActor(EventInterval8, 0xff, 0xff, ActionTimedRollDeactivateCollideable, start1, end1, 0xff, 0xff, start1);
    proxy3_addActor(EventInterval8, 0xff, 0xff, ActionTimedRollDeactivateCollideable, start2, end2, 0xff, 0xff, start2);
    proxy3_addActor(EventPortal, startp, 0xff, ActionRandomActivateCollideable, startp, endp, 2, 0xff, 0xff);
    proxy3_addActor(EventPortal, startp + 1, 0xff, ActionRandomActivateCollideable, startp, endp, 2, 0xff, 0xff);
    proxy3_addActor(EventPortal, startp + 2, 0xff, ActionRandomActivateCollideable, startp, endp, 2, 0xff, 0xff);
    proxy3_addActor(EventPortal, startp + 3, 0xff, ActionRandomActivateCollideable, startp, endp, 2, 0xff, 0xff);

    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene8_4()
{
	registerCallBank(4);
    balls = 12;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYL, 0, 20, 11, 5, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYR, 29, 20, 11, 5, true, GCOL_DARK_GREY, true);


    float step1 = 247.5 * PI / 180;
    float step2 = 292.5 * PI / 180;
	float xa = 40  * sin(step1) + 100 ;
	float ya = 22.5 * cos(step2) + 100;        
    for (byte i = 0; i < 18; i++)
    {
        step1 += (22.5 ) * PI / 180;
        step2 += (22.5) * PI / 180;
        float x1 = (40 + i) * sin(step1) + 100;
        float y1 = (22.5 + i * 6) * cos(step2) + 100 + i; 

		float xam = 320 - xa;
		float x1m = 320 - x1;

        // float x2 = 100 * sin(step1) + 160;
        // float y2 = 25 * cos(step2) + 60;
        // float x3 = 110 * sin(step1) + 160;
        // float y3 = 27.5 * cos(step2) + 70;        
        // float x4 = 120 * sin(step1) + 160;
        // float y4 = 30 * cos(step2) + 80;
        // float x5 = 130 * sin(step1) + 160;
        // float y5 = 32.5 * cos(step2) + 90;        
        // float x6 = 140 * sin(step1) + 160;
        // float y6 = 35 * cos(step2) + 100;
        // float x7 = 160 * sin(step1) + 200;
        // float y7 = 45 * cos(step2) + 120;

		proxy3_setGameObjectValueableTimedEdge(numObjects++, x1, y1, xa, ya, 3, true);     
		proxy3_setGameObjectValueableTimedEdge(numObjects++, x1m, y1, xam, ya, 3, true);     
		//proxy3_setGameObjectValueableTimedEdge(numObjects++, x2, y2, x3, y3, 3, true);     
		// proxy3_setGameObjectTimedEdge(numObjects++, x2, y2, x5, y5, 3, true);     
		// //proxy3_setGameObjectValueableTimedEdge(numObjects++, x4, y4, x5, y5, 3, true);     
		// proxy3_setGameObjectValueableTimedEdge(numObjects++, x4, y4, x6, y6, 3, true);


		xa = x1;
		ya = y1;

        step1 += 11.25 * PI / 180;
        step2 += 11.25 * PI / 180;
    }

	proxy3_setGameObjectPortal(numObjects++, 210,130,10, true); 
	proxy3_setGameObjectPortal(numObjects++, 110,130,10, true); 

    step1 = 270 * PI / 180;
    for (byte i = 0; i < 9; i++)
    {
        float x1 = 90 * sin(step1) + 160;
        float y1 = 22.5 * cos(step1) + 50;        
        float x2 = 100 * sin(step1) + 160;
        float y2 = 25 * cos(step1) + 80;

		if (i != 4)
		{
			proxy3_setGameObjectTimedCircle(numObjects++, x1, y1, 6, true);
			proxy3_setGameObjectTimedCircle(numObjects++, x2, y2, 6, true);
		}
        step1 += 22.5 * PI / 180;
    }

	proxy3_setGameObjectCannon(numObjects++, 160, 180, 12, ORIENTATION_UP, GCOL_MED_GREY, false, false);
	
    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene9_4()
{
	registerCallBank(4);
    balls = 12;
	timeElapsing = TimeElapsingFast;
	byte ballId = numObjects;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    byte targetId = numObjects;
    proxy3_setGameObjectBall(numObjects++, 80, 100, 0, 0, 6, 1, false, false, VCOL_LT_GREEN);


	float ya = 56;
	float y = 56;
	float xa = 20*cos((y - 100) * 0.05)+30;
	float x1, x2, y1, y2;
	x2 = 160 - xa;
	y2 = y;

    for (float y = 72; y <= 152; y += 16)
    {
		float x = 20*cos((y - 100) * 0.05)+40;
        
		if(y == 72)
		{
			x1 = x;
			y1 = y;
			proxy3_setGameObjectWallEdge(numObjects++, x1, y1, x2, y2, 4, 0.3, GCOL_RED, true);
			proxy3_setGameObjectWallEdge(numObjects++, 320 - x1 , y1, 320 - x2, y2, 4, 0.3, GCOL_RED, true);
		}
		if(y == 136)
		{
			x2 = 160 - x;
			y2 = y;
		}
		if(y == 152)
		{
			x1 = x;
			y1 = y;
			proxy3_setGameObjectWallEdge(numObjects++, x1, y1, x2, y2, 4, 0.3, GCOL_RED, true);
			proxy3_setGameObjectWallEdge(numObjects++, 320 - x1, y1, 320 - x2, y2, 4, 0.3, GCOL_RED, true);
		}

		if(y != 72)
		{
			proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 4, 0.3, GCOL_RED, true);
			proxy3_setGameObjectWallEdge(numObjects++, 320 - x, y, 320 - xa, ya, 4, 0.3, GCOL_RED, true);
			if(y != 152 && y != 152  - 16)
			{
				proxy3_setGameObjectValuableCrackCircle(numObjects++, x + 11, y, 4);		
				proxy3_setGameObjectValuableCrackCircle(numObjects++, 320 - (x + 11), y, 4);
			}		
		}
		if( y != 152)
		{
			proxy3_setGameObjectWallEdge(numObjects++, 160 - x, y, 160 - xa, ya, 4, 0.3, GCOL_RED, true);
			proxy3_setGameObjectWallEdge(numObjects++, 320 - (160 - x), y, 320 - (160 - xa), ya, 4, 0.3, GCOL_RED, true);
			if(y != 72 && y != 152 - 16)
			{
				proxy3_setGameObjectValuableCrackCircle(numObjects++, (160 - x)  - 11, y, 4);
				proxy3_setGameObjectValuableCrackCircle(numObjects++, 320 - (160 - x - 11), y, 4);
			}		
		}
        xa = x;
        ya = y;
	}

    byte sourceId = numObjects;
    proxy3_setGameObjectActivateBallCircle(numObjects++, 80, 100, 7, 0.6, GCOL_WHITE);
    proxy3_addDepObject(sourceId, targetId);

	proxy3_setGameObjectTimedValuableCircle(numObjects++, 160, 90, 6, true);
	proxy3_setGameObjectTimedValuableCircle(numObjects++, 160, 100, 6, true);
	proxy3_setGameObjectTimedValuableCircle(numObjects++, 160, 110, 6, true);

	proxy3_setGameObjectPortal(numObjects++, 55, 137, 8, true);
	proxy3_setGameObjectPortal(numObjects++, 223, 73, 8, true);
	proxy3_setGameObjectBumpCircle(numObjects++, 320-80, 105, 8, 1.5, GCOL_LT_BLUE);


    proxy3_setGameObjectCircleOrientation(numObjects++, 35, 100, 8, ORIENTATION_DOWN, GCOL_RED, PAT_ARROW_DOWN, 0);
    proxy3_setGameObjectCircleOrientation(numObjects++, 320 - 35, 100, 8, ORIENTATION_UP, GCOL_RED, PAT_ARROW_UP, 0);
    proxy3_setGameObjectCircleOrientation(numObjects++, 130, 100, 8, ORIENTATION_UP, GCOL_RED, PAT_ARROW_UP, 0);
    proxy3_setGameObjectCircleOrientation(numObjects++, 320 - 130, 100, 8, ORIENTATION_DOWN, GCOL_RED, PAT_ARROW_DOWN, 0);


    proxy3_addActor(EventShootBall, 0xff, ballId, ActionActivateBall, sourceId, 0xff, 0xff, 0xff, 0xff);
    proxy3_addActor(EventKillBall, 0xff, ballId, ActionKillBall, targetId, 0xff, 0xff, 0xff, 0xff);

    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

#pragma code(bcode5)
#pragma data(bdata5)

void initScene10_5()
{
	registerCallBank(5);
    balls = 12;
	byte ballId = numObjects;
	isGravCannon = false;
	proxy52_setGlobalOrientation(ORIENTATION_DOWN, true);	
	proxy3_setGameObjectImage(numObjects++, IMG_GALAXY, 16, 4, 8, 7, true, GCOL_MED_GREY, false);

    proxy3_setGameObjectBall(numObjects++, 160, 180, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 180, 18, ORIENTATION_UP, GCOL_DARK_GREY, false, true);
	proxy3_setGameObjectTimedValuableCircle(numObjects++, 160, 20, 6, true);

	byte l1 = numObjects;
    proxy3_setGameObjectLaser(numObjects++, 0, 120, 80, 140, GCOL_YELLOW, true);
	byte l2 = numObjects;
    proxy3_setGameObjectLaser(numObjects++, 320, 120, 320-80, 140, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 80, 140, 90, 190, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 320-80, 140, 320-90, 190, GCOL_YELLOW, true);

	byte t1 = numObjects;
	proxy3_setGameObjectSwitchCircle(numObjects++, 45, 150, 12, 0.8, GCOL_YELLOW, true);
	byte t2 = numObjects;
	proxy3_setGameObjectSwitchCircle(numObjects++, 320-45, 150, 12, 0.8, GCOL_YELLOW, true);
	byte t3 = numObjects;
	proxy3_setGameObjectSwitchCircle(numObjects++, 24, 87, 9, 0.8, GCOL_YELLOW, true);
	byte t4 = numObjects;
	proxy3_setGameObjectSwitchCircle(numObjects++, 320-24, 87, 9, 0.8, GCOL_YELLOW, true);

	byte b1 = numObjects;
	proxy3_setGameObjectBombCircle(numObjects++, 114, 100, 8, 0.8, GCOL_RED);
	byte b2 = numObjects;
	proxy3_setGameObjectBombCircle(numObjects++, 320-114, 100, 8, 0.8, GCOL_RED);

    int xz = 0, yz = 0;

    for (byte j = 0; j < 3; j++)
    {
        if (j == 0)
        {
            xz = 70;
            yz = 80;
        }
        if (j == 1)
        {
            xz = 250;
            yz = 80;
        }
        if (j == 2)
        {
            xz = 160;
            yz = 108;
        }

        float step = 22.5 * PI / 180;

        float xa = 30 * sin(step) + xz;
        float ya = 20 * cos(step) + yz;
		proxy3_setGameObjectBombCircle(numObjects++, xz, yz, 8, 0.8, GCOL_RED);
        for (byte i = 0; i < 8; i++)
        {
            step += 45 * PI / 180;

            float x = 30 * sin(step) + xz;
            float y = 20 * cos(step) + yz;
            float xb = 15 * sin(step) + xz;
            float yb = 10 * cos(step) + yz;

			proxy3_setGameObjectTimedValuableCircle(numObjects++, xb, yb, 3, true);

            if (i != 3 && i != 7)
            {
                proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 2, 0.3, GCOL_RED, true);
            }
			if(i == 7)
			{
                proxy3_setGameObjectBombWallEdge(numObjects++, x, y, xa, ya, 2, true);

			}

            xa = x;
            ya = y;
        }
	}

	proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionDisableCollision, b1, b1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionDisableCollision, b2, b2, 0xff, 0xff, 0xff);

	proxy3_addActor(EventSwitchOn, 0xff, t1, ActionDisableCollision, b1, b1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t1, ActionEnableCollision, b1, b1, 0xff, 0xff, 0xff);

	proxy3_addActor(EventSwitchOn, 0xff, t2, ActionDisableCollision, b2, b2, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t2, ActionEnableCollision, b2, b2, 0xff, 0xff, 0xff);

	proxy3_addActor(EventSwitchOn, 0xff, t3, ActionEnableCollision, l1, l1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t3, ActionDisableCollision, l1, l1, 0xff, 0xff, 0xff);

	proxy3_addActor(EventSwitchOn, 0xff, t4, ActionEnableCollision, l2, l2, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t4, ActionDisableCollision, l2, l2, 0xff, 0xff, 0xff);


    // Adding large text
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;
    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene11_5()
{
    registerCallBank(5);
    balls = 8;
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_GRIDSTAR, 1, 5, 4, 4, true, GCOL_BROWN, true);
	proxy3_setGameObjectImage(numObjects++, IMG_GRIDSTAR, 35, 5, 4, 4, true, GCOL_BROWN, true);

    // Central Circle - Large feature
    proxy3_setGameObjectKillCircle(numObjects++, 160, 120, 20, GCOL_LT_RED); // Large central circle

    // Surrounding Circles - Spaced properly to avoid overlap
    proxy3_setGameObjectTimedCircle(numObjects++, 110, 120, 15, true); // Left circle, larger spacing
    proxy3_setGameObjectTimedCircle(numObjects++, 210, 120, 15, true); // Right circle, larger spacing

    // Smaller Circles forming a ring around the central circle with adequate spacing
    proxy3_setGameObjectTimedCircle(numObjects++, 130, 80, 8, true);  // Top left, no overlap
    proxy3_setGameObjectTimedCircle(numObjects++, 190, 80, 8, true);  // Top right, no overlap
    proxy3_setGameObjectTimedCircle(numObjects++, 130, 160, 8, true); // Bottom left, no overlap
    proxy3_setGameObjectTimedCircle(numObjects++, 190, 160, 8, true); // Bottom right, no overlap

    // Tiny Circles - Positioned carefully to ensure no overlap with larger circles
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 100, 60, 5, true);   // Top tiny left, distant
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 220, 60, 5, true);   // Top tiny right, distant
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 100, 180, 5, true);  // Bottom tiny left, distant
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 220, 180, 5, true);  // Bottom tiny right, distant

    // Concentric Circles with enough space between them
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 160, 180, 4, true);  // Inner circle
//    proxy3_setGameObjectCrackCircle(numObjects++, 160, 180, 10, 2.0, GCOL_DARK_GREY, 5, true); // Outer circle, no overlap

    // Decorative arcs made from smaller circles with appropriate spacing
    for (float angle = 0; angle < 360; angle += 45) // More spaced arc pattern
    {
        float radius = 50;
        float x = 160 + 120 * cos(angle * PI / 180);
        float y = 120 + radius * sin(angle * PI / 180);
        proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true); // Arc circles with sufficient space
    }

    // Additional small circles positioned randomly but without overlap
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 80, 100, 4, true);   // Left side, no overlap
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 240, 140, 4, true);  // Right side, no overlap
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 120, 170, 4, true);  // Bottom left, spaced
    proxy3_setGameObjectTimedValuableCircle(numObjects++, 200, 170, 4, true);  // Bottom right, spaced

    // Ground element (base)
    //proxy3_setGameObjectCrackCircle(numObjects++, 160, 210, 14, 2.0, GCOL_BROWN, 5, true); // Ground circle

    // Large text for artistic signature
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene12_5()
{
    registerCallBank(5);
    balls = 12; // Number of balls for this level
	bombDistance = 20;
    // Cannon setup
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE1, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);

	proxy3_setGameObjectCannon(numObjects++, 200, 150, 12, ORIENTATION_RIGHT, GCOL_MED_GREY, false, false);
	proxy3_setGameObjectCannon(numObjects++, 280, 150, 12, ORIENTATION_LEFT, GCOL_MED_GREY, false, false);
	proxy3_setGameObjectCannon(numObjects++, 200, 50, 12, ORIENTATION_RIGHT, GCOL_MED_GREY, false, false);
	proxy3_setGameObjectCannon(numObjects++, 280, 50, 12, ORIENTATION_LEFT, GCOL_MED_GREY, false, false);

	const int size=12;
    float a = 13.0; // Amplitude8
    float k = 4.0; // Anzahl der Blütenblätter
    float theta;
    float x,y,r,xa,ya;
	byte p1=0;
	byte p2=0; 

   	proxy3_setGameObjectPortal(numObjects++, 240, 100, 8, true);
	for(byte j = 0; j< 3; j++)
	{
		if(j == 0)
		{
			k=6.0;
		}
		else if(j == 1)
		{
			k=7;
		}
		else if(j == 2)
		{
			k=6.5;
		}
		theta = 0 * (2 * PI / size);
		r = a * (0.4 * cos((4 * theta)) + PI + 3 * sin(k));
		xa = 90 + r * cos(theta);
		ya = 100 + r * sin(theta);
		byte l=0;
 
		for (byte i = 0; i < size + 1; i++)
		{
			theta = i * (2 * PI / size);
			r = a * (0.4 * cos((4 * theta)) + PI + 3 * sin(k));
			x = 90 + r * cos(theta);
			y = 100 + r * sin(theta);
			//proxy3_setGameObjectTimedValuableCircle(numObjects++, 160 + y, 100 + x, 4, true); 
			if(j==0)
			{
				proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
			}
			else if(j == 1)
			{
				proxy3_setGameObjectBombWallEdge(numObjects++, x, y, xa, ya, 5, true);
			}
			else if(j == 2)
			{
				if(i % 6 == 0)
				{
					if(l == 0)
					{
						p1=numObjects;
						proxy3_setGameObjectPortal(numObjects++, x, y, 8, false);
					}
					else if(l == 1)
					{
						p2=numObjects;
						proxy3_setGameObjectPortal(numObjects++, x, y, 8, true);
					}
					l++;
				}
				else if(i % 3 == 0)
				{
					proxy3_setGameObjectBombCircle(numObjects++, x,y,7,0.8, GCOL_LT_RED);
				}
				else
				{
				    proxy3_setGameObjectTimedValuableCircle(numObjects++, x , y, 4, true);   
				}
			}
			xa = x;
			ya = y;
		}

		byte p3 = numObjects;
		proxy3_setGameObjectSwitchCircle(numObjects++, 90, 100, 12, 0.8, GCOL_YELLOW, false);
	
	    proxy3_addActor(EventSwitchOn, 0xff, p3, ActionEnableCollision, p1, p1, 0xff, 0xff, 0xff);
	    proxy3_addActor(EventSwitchOn, 0xff, p3, ActionDisableCollision, p2, p2, 0xff, 0xff, 0xff);
	    proxy3_addActor(EventSwitchOff, 0xff, p3, ActionDisableCollision, p1, p1, 0xff, 0xff, 0xff);
	    proxy3_addActor(EventSwitchOff, 0xff, p3, ActionEnableCollision, p2, p2, 0xff, 0xff, 0xff);
	}

	// 254/146
	// 93/748
	// 241/304

    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);

}

void initScene13_5()
{
    registerCallBank(5);
    balls = 9; // Number of balls for this level
    // Cannon setup
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE1, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_SHIP1, 16, 5, 8, 8, true, GCOL_MED_GREY, false);

//    float x1e = 0, y1e = 25, x2e = 320, y2e = 60;
    float x1e = 0, y1e = 35, x2e = 320, y2e = 70;

    for(byte i = 0; i < 9; i++)
    {
        float xa1e = x1e + 19;
        float ya1e = y1e + 9 + i * 0.6;
        float xa2e = x2e - 19;
        float ya2e = y2e + 9 + i * 0.6;

        if((i + 3) % 4 == 0)
        {
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }
        else
        {
                proxy3_setGameObjectWallEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, 0.3, GCOL_RED, true);
                proxy3_setGameObjectWallEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, 0.3, GCOL_RED, true);
            // proxy3_setGameObjectTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            // proxy3_setGameObjectTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }

        x1e = xa1e;
        y1e = ya1e;
        x2e = xa2e;
        y2e = ya2e;
    }

    x1e = 0; 
	y1e = 75; 
	x2e = 320; 
	y2e = 30;

    for(byte i = 0; i < 6; i++)
    {
        float xa1e = x1e + 19;
        float ya1e = y1e + 9 + i * 0.9;
        float xa2e = x2e - 19;
        float ya2e = y2e + 9 + i * 0.9;

        if((i + 3) % 4 == 0)
        {
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            proxy3_setGameObjectValueableTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }
        else
        {
                proxy3_setGameObjectWallEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, 0.3, GCOL_RED, true);
                proxy3_setGameObjectWallEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, 0.3, GCOL_RED, true);

            // proxy3_setGameObjectTimedEdge(numObjects++, x1e, y1e, xa1e, ya1e, 4, true);
            // proxy3_setGameObjectTimedEdge(numObjects++, x2e, y2e, xa2e, ya2e, 4, true);
        }

        x1e = xa1e;
        y1e = ya1e;
        x2e = xa2e;
        y2e = ya2e;
    }
	//186/87
	//162/110
	//112/132
	//133/149

    proxy3_setGameObjectLaser(numObjects++, 196, 101, 172, 124, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 118, 142, 142, 165, GCOL_YELLOW, true);
    proxy3_setGameObjectCircleOrientation(numObjects++, 142, 142, 8, ORIENTATION_UP, GCOL_RED, PAT_ARROW_UP, 0);


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);

}

void initScene14_5()
{
    registerCallBank(5);
    balls = 15; // Number of balls for this level
	isGravCannon = false;

	#define w 24

	proxy52_setGlobalOrientation(ORIENTATION_DOWN, true);	

    proxy3_setGameObjectCannon(numObjects++, 40, 32, 16, ORIENTATION_RIGHT, GCOL_DARK_GREY, false, true);
    // Cannon setup
    proxy3_setGameObjectBall(numObjects++, 40, 40, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE2, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);

    proxy3_setGameObjectLaser(numObjects++, 0, 64, 270, 64, GCOL_YELLOW, true);
    proxy3_setGameObjectLaser(numObjects++, 320, 128, 50, 128, GCOL_YELLOW, true);
	byte l1 = numObjects;
    proxy3_setGameObjectLaser(numObjects++, 50, 64, 50, 128, GCOL_YELLOW, true);

    proxy3_setGameObjectCannon(numObjects++, 12, 128, 12, ORIENTATION_RIGHT, GCOL_MED_GREY, false, false);	
    proxy3_setGameObjectCannon(numObjects++, 296, 64, 12, ORIENTATION_LEFT, GCOL_MED_GREY, false, false);

    byte t1 = numObjects;
    proxy3_setGameObjectTimedDelayedCircle(numObjects++, 10, 8 + 6, 6, 0.6, GCOL_YELLOW, PAT_BONUS, true, false, 100);
    byte t2 = numObjects;
	proxy3_setGameObjectSwitchCircle(numObjects++, 163, 64 - w, 10, 1.0, GCOL_YELLOW, true);
    byte t3 = numObjects;
    proxy3_setGameObjectTimedDelayedCircle(numObjects++, 288, 64 + w, 6, 0.6, GCOL_YELLOW, PAT_BONUS, true, false, 100);

	byte p1 = numObjects;
    proxy3_setGameObjectPortal(numObjects++, 288, 64 - w, 8, true);    
	byte p2 = numObjects;
    proxy3_setGameObjectPortal(numObjects++, 20, 128 + w , 8, true);    
	byte p3 = numObjects;
    proxy3_setGameObjectPortal(numObjects++, 10, 64 - w , 8, false);    

	byte y;
	byte start1 = numObjects;
    for (float x = 80; x <= 262; x += 32)
    {
        y = 10 * cos((x / 320) * 5 * PI) + 32;
		if(rand() & 2 > 1)
		{
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
		}
		else
		{
		    proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
		}
		
	}
    for (float x = 12; x <= 304; x += 32)
    {
        y = 10 * sin((x / 320) * 5 * PI) + 96;
		if(rand() & 2 > 1)
		{
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
		}
		else
		{
		    proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
		}
	}
    // for (float x = 60; x <= 262; x += 32)
	// {
    //     y = 10 * cos((x / 320) * 5 * PI) + 160;
	// 	if(rand() & 2 > 1)
	// 	{
	// 		proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
	// 	}
	// 	else
	// 	{
	// 	    proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
	// 	}
	// }
	byte end1 = numObjects - 1;


	proxy3_addActor(EventInitLevel, 0xff, 0xff, ActionHideObject, start1, end1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventCollide, 0xff, t1, ActionDisableCollision, l1, l1, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOn, 0xff, t2, ActionEnableCollision, p2, p2, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t2, ActionDisableCollision, p2, p2, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOn, 0xff, t2, ActionDisableCollision, p3, p3, 0xff, 0xff, 0xff);
	proxy3_addActor(EventSwitchOff, 0xff, t2, ActionEnableCollision, p3, p3, 0xff, 0xff, 0xff);
	proxy3_addActor(EventCollide, 0xff, t3, ActionShowObject, start1, end1, 0xff, 0xff, 0xff);
	// proxy3_addActor(EventCollide, 0xff, t1, ActionEnableCollision, p2, p2, 0xff, 0xff, 0xff);
	// proxy3_addActor(EventCollide, 0xff, t1, ActionDisableCollision, p3, p3, 0xff, 0xff, 0xff);


	//proxy3_addActor(EventCollide, 0xff, t3, ActionShowObject, start2, end2, 0xff, 0xff, 0xff);	


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);

}

void initScene15_5()
{
    registerCallBank(5);
    balls = 8; // Number of balls for this level

    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE3, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);

	int x = 6;
    float y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 70;
    int xa = x;
    float ya = y; 

    byte start1 = numObjects;
    for (byte i = 0; i < 12; i++)
    {
        x += 320 / 12;

        y = (cos(((x - 160) * 0.011) * (x - 160) * 0.011) + 1.5) * 70;

		if(rand() % 2 > 0)
		{
			proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 3, true);     
		}
		else
		{
			proxy3_setGameObjectTimedEdge(numObjects++, x, y, xa, ya, 3, true);			
		}
        xa = x;
        ya = y;

	}

	y = 50;
	for (byte i = 0; i < 5; i ++)
	{
		x = 20;
		for (byte j = 0; j < 8; j ++)
		{
			if((i == 1 || i == 2) && (j == 0 || j == 7) ||
				(i == 3 && ( j < 2 || j > 5)) ||
				(i == 4 && ( j < 3 || j > 4)) )
			{
			}
			else
			{
				byte r = rand() % 10;

				if ( r == 1)
				{
	                proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x, y, 6, true);
				}
				else if(r > 5)
				{
					proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
				}
				else
				{
					proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
				}
			}
			x += 40;
		}
		y += 25;
	}

    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene16_5()
{
    registerCallBank(5);
    balls = 8; // Number of balls for this level

    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
    proxy3_setGameObjectImage(numObjects++, IMG_LANDSCAPE3, 0, 22, 40, 3, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_ALIEN2, 33, 10, 7, 12, true, GCOL_LT_RED, true);

	const int size=24;
    float a = 18; // Amplitude8
    float k = 4.0; // Anzahl der Blütenblätter
    float theta;
    float x,y,r,xa,ya;
	byte p1=0;
	byte p2=0; 

   	proxy3_setGameObjectPortal(numObjects++, 165, 95, 8, true);
   	proxy3_setGameObjectPortal(numObjects++, 210, 20, 8, true);

	theta = 7 * ( 3 * PI / size);
	r = a * theta / 2.6;
	xa = 160 + r * cos(theta);
	ya = 102 + r * 0.7 * sin(theta);
	byte l=0;
 
	for (byte i = 8; i < 44; i++)
	{
		theta = (i) * ( 3 * PI / size);
		r = a * theta / 2.6;
		x = 160 + r * cos(theta);
		y = 102 + r * 0.7 * sin(theta);
		byte r = rand() % 10;

		if ( r == 1)
		{
            proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 5, 0.3, GCOL_RED, true);
		}
		else if(r > 5)
		{
			proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		}
		else
		{
			proxy3_setGameObjectTimedEdge(numObjects++, x, y, xa, ya, 5, true);

		}



		// proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		xa = x;
		ya = y;
	}


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene17_5()
{
    registerCallBank(5);
    balls = 12; // Number of balls for this level


    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYL, 0, 20, 11, 5, true, GCOL_DARK_GREY, true);
	proxy3_setGameObjectImage(numObjects++, IMG_VALLEYR, 29, 20, 11, 5, true, GCOL_DARK_GREY, true);

	const int size=48;
	float s = 62;
    float a = 4; // Amplitude8
    float b = 1; // Anzahl der Blütenblätter
    float theta;
    float x,y,r,xa,ya;
	byte p1=0;
	byte p2=0; 
	bombDistance = 40;

	theta = 0 * ( 2 * PI / size);
	r = s * sin((a/b)*theta);
	xa = 160 + r * 1.2 * cos(theta);
	ya = 110 + r * sin(theta);
	byte l=0;
 
	for (byte i = 0; i < 48; i++)
	{
		theta = (i) * ( 2 * PI / size);
		r = s * sin((a/b)*theta);
		x = 160 + r * 1.2 * cos(theta);
		y = 110 + r * sin(theta);

		if(!(abs(x - 160) < 1 && abs(y - 110) < 1))
		{
			byte r = rand() % 10;

			if ( r == 1)
			{
				proxy3_setGameObjectTimedBonusScoreCircle(numObjects++, x, y, 6, true);
			}
			else if(r > 5)
			{
				proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
			}
			else
			{
				proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
			}
		}

		// // if(i % 2 == 0)
		// // {
		// // 	proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 3, true);
		// if ( r == 1)
		// {

		// 	proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 5, 0.3, GCOL_RED, true);
		// }
		// else if(r > 5)
		// {
		// 	proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		// }
		// else
		// {
		// 	proxy3_setGameObjectTimedEdge(numObjects++, x, y, xa, ya, 5, true);

		// }

		// proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		xa = x;
		ya = y;

	}

	float step = 0;
	 xa = 96 * sin(step) + 160;
	 ya = 82 * cos(step) + 110;
	for (byte i = 0; i < 10; i++)
	{
		step += 36 * PI / 180;

		 x = 96 * sin(step) + 160;
		 y = 82 * cos(step) + 110;

		if (!(i == 0 || i == 9 || i == 4 || i == 5))
		{
			proxy3_setGameObjectValueableCrackEdge(numObjects++, x, y, xa, ya, 4);
		}
		xa = x;
		ya = y;
	}	

	proxy3_setGameObjectBombCircle(numObjects++, 160,110, 20, 0.8, GCOL_LT_RED);


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene18_5()
{
    registerCallBank(5);
    balls = 12; // Number of balls for this level

    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);


    float x,y,xa,ya;

	xa = 160 + 140 * cos(3 * PI * 0);
	ya = 70 - 40 * sin(6 * PI * 0);

    for (float t = 0; t <= 0.15; t += 0.02) 
	{
		x = 160 + 140 * cos(3 * PI * t);
		y = 70 - 40 * sin(6 * PI * t);
		proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		xa = x;
		ya = y;
    }
	xa = 160 + 140 * cos(3 * PI * 0.35);
	ya = 70 - 40 * sin(6 * PI * 0.35);
    for (float t = 0.35; t <= 0.5; t += 0.02) 
	{
		x = 160 + 140 * cos(3 * PI * t);
		y = 70 - 40 * sin(6 * PI * t);
		proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		xa = x;
		ya = y;
    }

    proxy3_setGameObjectBumpCircle(numObjects++, 260, 150, 12, 1.5, GCOL_CYAN);
    proxy3_setGameObjectBumpCircle(numObjects++, 240, 170, 12, 1.5, GCOL_YELLOW);
    proxy3_setGameObjectBumpCircle(numObjects++, 280, 170, 12, 1.5, GCOL_GREEN);

	xa = 160 + 140 * cos(3 * PI * 0);
	ya = 130 - 40 * sin(6 * PI * 0);

    proxy3_setGameObjectKillCircle(numObjects++, 80, 96, 20, GCOL_LT_RED); // Large central circle

	byte r=0;

    for (float t = 0; t <= 0.5; t += 0.05) 
	{
        x = 160 + 140 * cos(2 * PI * t);
        y = 130 - 40 * sin(5 * PI * t);

		r++;

		if(r % 2 == 0)
		{
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y + 20, 6, true);
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
			proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y - 20, 6, true);
		}
		else
		{
			proxy3_setGameObjectTimedCircle(numObjects++, x, y + 20, 6, true);
			proxy3_setGameObjectTimedCircle(numObjects++, x, y, 6, true);
			proxy3_setGameObjectTimedCircle(numObjects++, x, y - 20, 6, true);
		}


		//proxy3_setGameObjectValueableTimedEdge(numObjects++, x, y, xa, ya, 5, true);
		xa = x;
		ya = y;
    }


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}

void initScene19_5()
{
    registerCallBank(5);
    balls = 12; // Number of balls for this level
    proxy3_setGameObjectBall(numObjects++, 160, 22, 0, 0, 6, 0, true, true, VCOL_WHITE);
    proxy3_setGameObjectCannon(numObjects++, 160, 18, 18, ORIENTATION_DOWN, GCOL_DARK_GREY, true, true);

    proxy3_setGameObjectCannon(numObjects++, 12, 188, 12, ORIENTATION_UP, GCOL_MED_GREY, false, false);
    proxy3_setGameObjectCannon(numObjects++, 320-12, 188, 12, ORIENTATION_UP, GCOL_MED_GREY, false, false);

    float x,y,xa,ya;
	byte t1, t2;

	for(byte i = 0; i < 5; i++)
	{
		xa = 160 - (150 - (i*7)) * sin(4 * PI * 0.41);
		ya = 110 + i * 18 + 50 * cos(6 * PI * 0.41);

		byte j=0;
		if(i == 0 || i == 4)
		{
			j = 1;
		}
		for (float t = 0.41 ; t <= 0.59; t += 0.02) 
		{
			x = 160 - (150 - (i*7)) * sin(4 * PI * t);
			y = 110 + i * 18 + 50 * cos(6 * PI * t);
			if(i == 0 || i == 2 || i == 4)
			{
				if(j % 3 != 0)
				{
	 	       		proxy3_setGameObjectWallEdge(numObjects++, x, y, xa, ya, 5, 0.3, GCOL_RED, true);
				}
			}
			else
			{
				if(j == 3 && i == 3)
				{
					t1 = numObjects;
					proxy3_setGameObjectTimedDelayedCircle(numObjects++, x, y, 6, 0.8, GCOL_LT_GREEN, PAT_BONUS, true, false, 100);
				}
				else if(j == 6 && i == 3)
				{
					t2 = numObjects;
					proxy3_setGameObjectTimedDelayedCircle(numObjects++, x, y, 6, 0.8, GCOL_LT_GREEN, PAT_BONUS, true, false, 100);
				}
				else
				{	
					proxy3_setGameObjectTimedValuableCircle(numObjects++, x, y, 6, true);
				}

			}
			j++;
			xa = x;
			ya = y;
		}
	}

	for(byte i = 0; i < 5; i++)
	{
		xa = 160 - (150 - (i*7)) * sin(4 * PI * 0.41);
		ya = 110 + i * 18 + 50 * cos(6 * PI * 0.41);

		byte j=0;
		if(i == 0 || i == 4)
		{
			j = 1;
		}
		byte startL = numObjects;
		for (float t = 0.41 ; t <= 0.59; t += 0.02) 
		{
			x = 160 - (150 - (i*7)) * sin(4 * PI * t);
			y = 110 + i * 18 + 50 * cos(6 * PI * t);
			if(i == 0 || i == 2 || i == 4)
			{
				if(!(j % 3 != 0))
				{
					proxy3_setGameObjectLaser(numObjects++, x, y, xa, ya, GCOL_YELLOW, true);
				}
			}
			j++;
			xa = x;
			ya = y;
		}
		byte endL = numObjects - 1;
		if(i == 0)
		{
			proxy3_addActor(EventCollide, 0xff, t1, ActionDisableCollision, startL, endL, 0xff, 0xff, 0xff);
		}
		else if(i == 2)
		{
			proxy3_addActor(EventCollide, 0xff, t2, ActionDisableCollision, startL, endL, 0xff, 0xff, 0xff);

		}

	}
	proxy3_setGameObjectImage(numObjects++, IMG_MOON, 14, 20, 12, 5, true, GCOL_MED_GREY, false);


    // Adding large text to announce level start
    largeTextId = numObjects;
    largeTextTimer = LARGE_TEXT_TIMER;

    proxy3_setGameObjectLargeText(numObjects++, 8, true, ORIENTATION_LEFT, 0, GCOL_WHITE);
}



#pragma code(code)
#pragma data(data)

void rasterMenuInit(bool useKernal)
{
	rirq_stop();

	rirq_init(useKernal);
	rirq_build(&rmenu, 1);
	rirq_call(&rmenu, 0, menu_interrupt);
	rirq_set(0, 250, &rmenu);

	rirq_sort();

	rirq_start();	
}


void rasterInit(bool useKernal)
{

	rirq_stop();

	rirq_init(useKernal);
	rirq_build(&top, 4);
	rirq_delay(&top, 8);
	rirq_write(&top, 1, &vic.memptr, 0x68);
	rirq_write(&top, 2, &vic.ctrl1, VIC_CTRL1_BMM | VIC_CTRL1_DEN | VIC_CTRL1_RSEL | 3);
	rirq_call(&top, 3, joy_interrupt);

	rirq_build(&bottom, 3);
	rirq_delay(&bottom, 3);
	rirq_write(&bottom, 1, &vic.memptr, 0x22);
	rirq_write(&bottom, 2, &vic.ctrl1, VIC_CTRL1_DEN | VIC_CTRL1_RSEL | 3);

	rirq_set(0, 57, &top);
	rirq_set(1, 20, &bottom);
	rirq_sort();

	rirq_start();
}
void rasterSnd(bool useKernal)
{
	rirq_stop();

	rirq_init(useKernal);
	rirq_build(&rmenu, 1);
	rirq_call(&rmenu, 0, snd_interrupt);
	rirq_set(0, 250, &rmenu);

	rirq_sort();

	rirq_start();	
}

int main(void)
{
	// Activate trampoline
	mmap_trampoline();

	// Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();
	// timer = 0;
	// collideQueueFront = -1;
	// collideQueueRear = -1;
	// scoreQueueFront = -1;
	// scoreQueueRear = -1;
	// bombQueueFront = -1;
	// bombQueueRear = -1;
	// globalOrientation = ORIENTATION_DOWN;
	// gravity = Gravity;
	// traySprite = 5;
	// posYTray = 229;
	//sid.fmodevol = 15;
	//faceState = FA_WATCH;
	//faceTimer = 0xff;
	//ballSide = 0;
	//bumptimer = 0;

	// Copy assets

	mmap_set(MMAP_ROM);
	eflash.bank = 3;
	memcpy(Buffer, TextChars, 0x300);
	eflash.bank = 1;
	mmap_set(MMAP_RAM);
	memcpy(Charset, Buffer, 0x300);

	mmap_set(MMAP_ROM);
	eflash.bank = 6;
	memcpy(Buffer, ballsprites, 17 * 64);
	mmap_set(MMAP_RAM);
	memcpy(Sprites, Buffer, 17 * 64);
	mmap_set(MMAP_ROM);
	proxy_memcpy(8, 2, Buffer, Music, 0x800);

	// Switch to hires mode
	vic_setmode(VICM_HIRES, Color1, Hires);
	// vic.memptr = 0x2A;
	vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | VIC_CTRL1_RSEL | 3;
	spr_init(Color1);

	bm_init(&Screen, Hires, 40, 25);
	proxy0_spr_set(6, true, 160 - 23, posYTray, 64 + traySprite, VCOL_WHITE, false, true, false);
	proxy0_spr_set(7, true, 160 + 23, posYTray, 64 + traySprite + 1, VCOL_WHITE, false, true, false);

	mmap_set(MMAP_ROM);
//	string_write_2(0, 0, scoreTxt, VCOL_WHITE);

	// Screen color to black
	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLUE;
	mmap_set(MMAP_ROM);

	//rasterInit(true);
	eflash.bank = 2;
	initStarPolygon_2();
	eflash.bank = 1;
	highlight.timer = 0;
	level = 19;
	//breakMusic = false;

	state = GS_TITLE_INIT;
 
	// Forever
	for (;;)
	{
		// One game loop iteration
		frameLoop_1();
		rirq_wait();
	}

	// Never reached
	return 0;
}
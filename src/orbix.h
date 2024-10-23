struct Vect2
{
	float	v[2];
};

enum GameState
{
	GS_LEVEL_INIT = 1,	// InitLevel
	GS_LEVEL = 2,		// Show level message
	GS_FIRE = 3,		// Getting ready
	GS_RUN = 4,			// Playing the game
	GS_WAITCLEAN = 5,	// Wait till all clear
	GS_RUN_FINISH_INIT = 6,	// InitFinishRun
	GS_RUN_FINISH  = 7,	// Wait for restart
	GS_RUN_COMPLETED_INIT  = 8,	// All cleared
	GS_RUN_COMPLETED  = 9,	// All cleared
	GS_FINALIZE  = 10,	// Finalize run
	GS_GAMEOVER_INIT  = 11,	// GameOver
	GS_GAMEOVER  = 12,	// GameOver
	GS_MENU_INIT = 13,
	GS_MENU = 14,
	GS_GAME_INIT = 15,
	GS_TITLE_INIT = 16,
	GS_TITLE = 17,
	GS_TITLE_FADE_OUT = 18,
	GS_INTRO = 19,
	GS_SIMULATE = 20,
	GS_SIMULATE_FINISH = 21,
};

enum FaceState
{
	FA_WATCH,
	FA_ANGRY,
	FA_SUNNY,
	FA_THINK,
};

struct Box
{
	int x;
	int y;
	int width;
	int height;
};

struct CollisionTile
{
	byte position;
	byte colGameObject[8];
};

struct Actor
{
	byte event1;
	byte event2;
	byte senderId;
	byte action;
	byte receiverStart;
	byte receiverEnd;
	byte param0, param1;
	byte var0; 
};

struct Highlight
{
	byte x;
	byte y;
	byte w;
	byte h;
	bool isHires;
	byte restoreColor;
	byte timer;
};

struct GameObject
{
	byte comp0, comp1, comp2, comp3;
	byte spriteNum;
	float px, py;	// Edge: Start
	float vx, vy;
	float ax, ay;	
	float radius;
    float efficiency;
	struct Box boundingBox;
	sbyte orientation;
	sbyte crackLevel;
	byte color;
	byte pattern;
	struct Vect2 cannonVector;
	bool isOn;
	signed int baseScore;
};

struct CollidingPairs
{
	byte go1, go2, gofake;
};

struct ScoreDisplay
{
	int x,y;
	byte sourceId;
	signed int score;
	byte textId;
};

struct BombDisplay
{
	byte x,y;
	byte radius;
	byte origRadius;
	byte color;
};

struct ScoreBoard
{
	float threshold;
	byte multiplier;
	byte pegThreshold;
};

struct DepGameObject
{
	byte sourceId;
	byte targetId;
};

struct CollidingTimer
{
	byte id;
	byte timer;
};

struct Particle
{
	int				px, py, vx, vy;
	Particle	*	next;
};

void rasterInit(bool useKernal);
void push(byte x);
void pop();
float max_1(float x1, float x2);
float min_1(float x1, float x2);
void updateRender_1(byte id, bool init);
bool setGlobalOrientation_2(sbyte orientation, bool isInit);
void catchBall_2(byte id, int target);
void execActionEnableCollision_2(byte start, byte end);
void execActionDisableCollision_2(byte start, byte end);
void execActionLaser_2(byte start, byte end, bool val);
void execActionShow_2(byte start, byte end);
void execActionHide_2(byte start, byte end);
void execActionRoll_2(byte actorId, bool isCollideable);
void execActionRandom_2(byte actorId, bool isCollideable);
void dispatchEvent_2(byte event, byte senderId, byte p1, byte p2);
struct Box getBoundingBoxLine_3(float x1, float y1, float x2, float y2, float radius);
struct Box getBoundingBoxCircle_3(float x1, float y1, float radius, int radius2);
inline bool IsBitSet(const byte comp, const byte test);
void collideQueue_enqueue_2(byte insertItem);
sbyte collideQueue_dequeue_2(); 
bool collideQueue_isQueued_2(byte item);
void addFakeBalls_2(float px, float py, float vx, float vy, float radius, float efficiency); 
void addCollidingPairs_2(byte go1, byte go2, byte gofake); //
void addActor_3(byte event1, byte event2, byte senderId, byte action, byte receiverStart, byte receiverEnd, byte param0, byte param1, byte var0);
void colorChar(unsigned char x, unsigned char y, byte backc, char* col, bool onlyBack);
void colorAreaCircle_2(unsigned char x0, unsigned char y0, unsigned char radius, byte backc, bool onlyBack);
void setGameObjectSwitchCircle_3(byte id, float px, float py, float r, float efficiency, byte color, bool isOn);
void setGameObjectTimedDelayedCircle_3(byte id, float px, float py, float r, float efficiency, byte color, byte pattern, bool collideAble, bool valueable, signed int baseScore);
void setGameObjectTimedValuableCircle_3(byte id, float px, float py, float r, bool isCollideAble);
void setGameObjectBombWallCircle_3(byte id, float px, float py, float r, bool isCollideAble);
void setGameObjectTimedCircle_3(byte id, float px, float py, float r, bool isCollideAble);
void setGameObjectWallCircle_3(byte id, float px, float py, float r, float efficiency, byte color);
void setGameObjectKillCircle_3(byte id, float px, float py, float r, byte color);
void setGameObjectValuableCrackCircle_3(byte id, float px, float py, float r);
void setGameObjectCrackCircle_3(byte id, float px, float py, float r, float efficiency,int color, byte crackLevel, bool isValueable);
void setGameObjectCircleOrientation_3(byte id, float px, float py, float r, byte orientation, byte color, byte pattern, byte comp1);
void setGameObjectCannon_3(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon); 
void setGameObjectValueableTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable);
void setGameObjectBombWallEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable);
//void setGameObjectTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, float efficiency, byte color, byte pattern, bool isCollidable, bool isValueable, signed int baseScore);
void setGameObjectTimedEdge_3(byte id, float px, float py, float ax, float ay, float radius, bool isCollideable);
void setGameObjectLaser_3(byte id, float px, float py, float ax, float ay, byte color, bool isCollideable);
void setGameObjectWallEdge_3(byte id, float px, float py, float ax, float ay, float radius, float efficiency, byte color, bool isCollideable);
void setGameObjectCrackEdge_3(byte id, float px, float py, float ax, float ay, float radius, float efficiency, byte color, byte crackLevel, bool isValueable, signed int baseScore);
void setGameObjectValueableCrackEdge_3(byte id, float px, float py, float ax, float ay, float radius);
void setGameObjectActivateBallCircle_3(byte id, float px, float py, float r, float efficiency, byte color);
void setGameObjectPortal_3(byte id, float px, float py, float radius, bool isActive);
void setGameObjectTimedBonusScoreCircle_3(byte id, float px, float py, float r, bool isCollideAble);
void setGameObjectLargeText_3(byte id, char py, bool isOn, byte orientation, byte textId, byte color);
void setGameObjectBumpCircle_3(byte id, float px, float py, float r, float efficiency, byte color);
void setGameObjectBombCircle_3(byte id, float px, float py, float r, float efficiency, byte color);
void setGameObjectImage_3(byte id, byte imageId, char x, char y, char w, char h, bool isOn, byte color, bool deleteBonus);
void drawFrame();
void initScene_4();
void drawBackgroundImage_4(byte id);
void preInitScene_3();
void postInitScene_3();
inline void char_put(byte x, byte y, char ch, char color);
void string_write_2(char cx, char cy, const char * s, char color);
void vec2_rotate_2(Vect2* vec, float angle, float targetRadius); 
inline bool checkAabbCollision(const struct Box* b1, const struct Box* b2);
inline bool doCirclesOverlap_1(float x1, float y1, float r1, float x2, float y2, float r2);
inline bool isPointInCircle_1(float x1, float y1, float r1, float px, float py);
void colorBoundingBox_2(byte id); 
__interrupt void joy_interrupt();
__interrupt void menu_interrupt();
void quad_fill(const Bitmap * bm, const ClipRect * clip, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const char * pat);
void deleteRender_1(byte id, bool withUpdate);
void updateCannon_2(byte id);
void updatePhysics_1(byte id);
void updateCollisions_1();
void gameLoop_1();
void hideGameObject_2(byte id);
void resetBall_1(byte id);
struct ScoreDisplay scoreQueue_dequeue_2();
void scoreQueue_enqueue_2(ScoreDisplay insertItem);
void handleScore_2(byte id); //
void integer_write_2(char cx, char cy, int i); 
ScoreDisplay scoreQueue_get_2(byte item);
void score_inc_2(char digit, unsigned val);
void score_reset_2(void);
void activateSecondaryBall_1(byte id, byte targetId);
void addDepObject_3(byte sourceId, byte targetId);
void deactivateSecondaryBall_1(byte targetId);
void setGameObjectBall_3(byte id, float px, float py, float vx, float vy, float radius, byte spritenum, bool isMainBall, bool isOn, byte color);
void printLargeString_2(char cy, const char *s, bool withRoundScore, bool withLevelScore, bool withSubText);
//void printLargeString_2(char cy, const char *s, bool withRoundScore, bool withLevelScore, bool withSubText, const char *sub);
void char_put_2(char cx, char cy, char c); 
void executeLargeTextTimer_2();
struct Box getBoundingBoxChar_3(char x1, char y1, char w, char h);
void showBalls_2();
void showMultiplier_2(byte multiplier);
void playSfx(byte num);
void playSubtune(byte num);
void colorArea(byte x, byte y, byte w, byte h, byte fcol, byte bcol);
byte change_high_nibble(byte b, byte new_low_nibble);
byte change_low_nibble(byte b, byte new_low_nibble);
void copyCharset();
void setTunes_2(bool isOn);
void drawImageUnclipped(byte* byte_stream, byte x, byte y, byte width, byte height);
void colorAreaUnclipped(byte x, byte y, byte w, byte h, byte c, bool isHires); 
void startHighlight_2(byte x, byte y, byte w, byte h, bool isHires, byte restoreColor);
void updateDestroyed_2(byte id);
void pix_set(unsigned px, unsigned py, bool isConfetti);
void pix_clr_2(unsigned px, unsigned py);
void particle_init_2(void);
void particle_add_2(int px, int py, int vx, int vy);
void particle_remove_2(bool isConfetti);
void particle_move_2(byte grav, bool isConfetti);
int rnorm_2(byte wide);
void doFirework_2(byte type);
void initStarPolygon_2();
void setGameLetterCircle_3(byte id, float px, float py, float r, byte color, byte pattern);
void lchar_put_3(int cx, char cy, char c, char spray);
void initBonusLine_3();
float qsqrt_3(float number);
void updateFace_2();
void deleteRender_1(byte id);
void portalSwitch_2(byte id, byte targetId);
void execCollisionTimer_2(byte i);
void frameLoop_1();
void rasterMenuInit(bool useKernal);
void copyTitle();
void cleanScreen(bool withScrenn);
void decLevel_2();
void incLevel_2(bool round);
void paintLevMenu_1();
void explodeBomb_2(byte id);
void bombQueue_enqueue_2(BombDisplay insertItem);
struct BombDisplay bombQueue_dequeue_2();
BombDisplay bombQueue_get_2(byte item);
void handleDelete_1(byte id);
void execActionActivateBall_2(byte sourceId, byte mainBallId);
void execActionDeactivateBall_2(byte targetId);
void execActionKillBall_2(byte targetId);
__interrupt void snd_interrupt();
inline void registerCallBank(byte fromBank);
void killBall_2(byte id);
void titleFadeOut_2();
void intro_2();
byte simulatePhysics_3(byte id);
void restoreSimulationFinish_3(byte id);
void saveSimulationStart_3(byte id);
void paintMusicItem_1();

void initScene0_4();
void initScene1_4();
void initScene2_4();
void initScene3_4();
void initScene4_4();
void initScene5_4();
void initScene6_4();
void initScene7_4();
void initScene8_4();
void initScene9_4();
void initScene10_5();
void initScene11_5();
void initScene12_5();
void initScene13_5();
void initScene14_5();
void initScene15_5();
void initScene16_5();
void initScene17_5();
void initScene18_5();
void initScene19_5();


// proxy
__noinline void proxy12_dispatchEvent(byte event, byte senderId, byte p1, byte p2);
__noinline void proxy32_dispatchEvent(byte event, byte senderId, byte p1, byte p2);
__noinline void proxy12_addFakeBalls(float px, float py, float vx, float vy, float radius, float efficiency);
__noinline void proxy12_addCollidingPairs(byte go1, byte go2, byte gofake);
__noinline void proxy12_handleScore(byte id);
__noinline void proxy13_setGameObjectCannon(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon);
__noinline void proxy21_updateRender(byte id, bool init);
__noinline void proxy31_updateRender(byte id, bool init);
__noinline void proxy12_colorBoundingBox(byte id);
__noinline void proxy12_string_write(char cx, char cy, const char *s, char color);
__noinline void proxy0_drawFrame();
__noinline void proxy0_bm_circle_fill(const Bitmap *bm, const ClipRect *clip, int x, int y, char r, const char *pattern);
__noinline void proxy0_bm_line(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, char pattern, LineOp op);
__noinline void proxy0_bm_triangle_fill(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, int x2, int y2, const char *pat);
__noinline void proxy0_bm_quad_fill(const Bitmap *bm, const ClipRect *clip, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const char *pat);
__noinline void proxy0_bm_rect_clear(const Bitmap *dbm, const ClipRect *clip, int dx, int dy, int w, int h);
__noinline void proxy0_bm_put(const Bitmap *bm, int x, int y, bool c);
__noinline void proxy0_put_score(int x, int y, signed int score);
__noinline void proxy0_put_text(int x, int y, byte textId);
__noinline void proxy12_score_reset(void);
__noinline void proxy12_score_inc(char digit, unsigned val);
__noinline void proxy14_initScene();
__noinline void proxy0_pix_set(unsigned px, unsigned py);
__noinline void proxy12_collideQueue_enqueue(byte insertItem);
__noinline sbyte proxy12_collideQueue_dequeue();
__noinline bool proxy12_collideQueue_isQueued(byte item);
__noinline void proxy12_scoreQueue_enqueue(ScoreDisplay insertItem);
__noinline struct ScoreDisplay proxy12_scoreQueue_dequeue();
__noinline ScoreDisplay proxy12_scoreQueue_get(byte item);
__noinline void proxy12_showBalls();
__noinline void proxy12_showMultiplier(byte multiplier);
__noinline void proxy12_startHighlight(byte x, byte y, byte w, byte h, bool isHires, byte restoreColor);
__noinline void proxy12_doFirework(byte type);
__noinline void proxy31_setGlobalOrientation(sbyte orientation);
__noinline void proxy12_executeLargeTextTimer();
__noinline void proxy12_updateCannon(byte id);
__noinline void proxy21_deleteRender(byte id, bool withUpdate);
__noinline void proxy13_initBonusLine();
__noinline void proxy12_updateFace();
__noinline void proxy13_preInitScene();
__noinline void proxy13_postInitScene();
__noinline void proxy12_portalSwitch(byte id, byte targetId);
__noinline void proxy12_execCollisionTimer(byte i);
__noinline void proxy3_addActor(byte event1, byte event2, byte senderId, byte action, byte receiverStart, byte receiverEnd, byte param0, byte param1, byte var0);
__noinline void proxy3_setGameObjectSwitchCircle(byte id, float px, float py, float r, float efficiency, byte color, bool isOn);
__noinline void proxy3_setGameObjectTimedDelayedCircle(byte id, float px, float py, float r, float efficiency, byte color, byte pattern, bool collideAble, bool valueable, signed int baseScore);
__noinline void proxy3_setGameObjectTimedValuableCircle(byte id, float px, float py, float r, bool isCollideAble);
__noinline void proxy3_setGameObjectTimedExtraCircle(byte id, float px, float py, float r, bool isCollideAble);
__noinline void proxy3_setGameObjectTimedCircle(byte id, float px, float py, float r, bool isCollideAble);
__noinline void proxy3_setGameObjectWallCircle(byte id, float px, float py, float r, float efficiency, byte color);
__noinline void proxy3_setGameObjectKillCircle(byte id, float px, float py, float r, byte color);
__noinline void proxy3_setGameObjectValuableCrackCircle(byte id, float px, float py, float r);
__noinline void proxy3_setGameObjectCrackCircle(byte id, float px, float py, float r, float efficiency,int color, byte crackLevel, bool isValueable);
__noinline void proxy3_setGameObjectCircleOrientation(byte id, float px, float py, float r, byte orientation, byte color, byte pattern, byte comp1);
__noinline void proxy3_setGameObjectCannon(byte id, float px, float py, float r, byte orientation, byte color, bool useGlobalOrientation, bool isMainCannon);
__noinline void proxy3_setGameObjectValueableTimedEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable);
__noinline void proxy3_setGameObjectExtraTimedEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable);
__noinline void proxy3_setGameObjectTimedEdge(byte id, float px, float py, float ox, float oy, float radius, bool isCollideable);
__noinline void proxy3_setGameObjectLaser(byte id, float px, float py, float ox, float oy, byte color, bool isCollideable);
__noinline void proxy3_setGameObjectWallEdge(byte id, float px, float py, float ox, float oy, float radius, float efficiency, byte color, bool isCollideable);
__noinline void proxy3_setGameObjectCrackEdge(byte id, float px, float py, float ox, float oy, float radius, float efficiency, byte color, byte crackLevel, bool isValueable, signed int baseScore);
__noinline void proxy3_setGameObjectValueableCrackEdge(byte id, float px, float py, float ox, float oy, float radius);
__noinline void proxy3_setGameObjectActivateBallCircle(byte id, float px, float py, float r, float efficiency, byte color);
__noinline void proxy3_setGameObjectPortal(byte id, float px, float py, float radius, bool isActive);
__noinline void proxy3_setGameObjectBall(byte id, float px, float py, float vx, float vy, float radius, byte spritenum, bool isMainBall, bool isOn, byte color);
__noinline void proxy3_setGameObjectLargeText(byte id, char py, bool isOn, byte orientation, byte textId, byte color);
__noinline void proxy3_addDepObject(byte sourceId, byte targetId);
__noinline void proxy3_setGameObjectTimedBonusScoreCircle(byte id, float px, float py, float r, bool isCollideAble);
__noinline void proxy3_setGameObjectBumpCircle(byte id, float px, float py, float r, float efficiency, byte color);
__noinline void proxy34_initScene_4();
__noinline void proxy17_copyTitle();
__noinline void proxy12_updateDestroyed(byte id);


const char Patterns[15][8] = {
{
 0b11110111,
 0b11110111,
 0b11101011,
 0b11101011,
 0b11011101,
 0b11000001,
 0b11111111,
 0b11111111},
{
 0b11111111,
 0b11111111,
 0b10000011,
 0b10111011,
 0b11010111,
 0b11010111,
 0b11101111,
 0b11101111},
{
 0b11111101,
 0b11111101,
 0b11111101,
 0b00000000,
 0b10111111,
 0b10111111,
 0b10111111,
 0b00000000},
{
 0b11101111,
 0b11010111,
 0b10111011,
 0b01111101,
 0b10111110,
 0b11011101,
 0b11101011,
 0b11110111
},
{
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111
},
{
 0b10111011,
 0b01010101,
 0b01010101,
 0b10111011,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111
},
{
 0b10111011,
 0b00110011,
 0b10111011,
 0b10111011,
 0b11111111,
 0b11111111,
 0b11111111,
 0b11111111
},
{
 0b00111110,
 0b00011111,
 0b10001111,
 0b11000111,
 0b11100011,
 0b11110001,
 0b11111000,
 0b01111100
},
{
 0b00000000,
 0b00010000,
 0b00100000,
 0b00010000,
 0b00011000,
 0b00011100,
 0b00111110,
 0b11111110
},
{
 0b00000000,
 0b00011000,
 0b00011000,
 0b01111110,
 0b01111110,
 0b00011000,
 0b00011000,
 0b00000000
},
{
 0b11111111,
 0b11111101,
 0b11110011,
 0b11000111,
 0b11100011,
 0b11001111,
 0b10111111,
 0b11111111
},
{
 0b01100000,
 0b10010000,
 0b01100000,
 0b00000000,
 0b00001100,
 0b00010010,
 0b00001100,
 0b00000000
},
{
 0b10101010,
 0b01010101,
 0b10101010,
 0b01010101,
 0b10101010,
 0b01010101,
 0b10101010,
 0b01010101
},
{
 0b00001110,
 0b00010000,
 0b00011000,
 0b00111100,
 0b00111100,
 0b00011000,
 0b00000000,
 0b00000000
},
{
 0b10110101,
 0b11101101,
 0b11011101,
 0b00100000,
 0b10111011,
 0b10101110,
 0b10111101,
 0b00000000
}

};

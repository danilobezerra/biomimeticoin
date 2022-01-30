#include <genesis.h>
#include <resources.h>

#define P1_SPAWN_TILE 4
#define P2_SPAWN_TILE 5
#define SOLID_TILE 1
#define COIN_TILE 7
#define EXIT_TILE 6

#define TILESIZE 8
#define MAP_WIDTH 10
#define MAP_HEIGHT 10

#define P1_ANIM_DOWN 0
#define P1_ANIM_UP 1
#define P1_ANIM_SIDE 2
#define P2_ANIM_DOWN 3
#define P2_ANIM_UP 4
#define P2_ANIM_SIDE 5

#define MAX_COINS 3

#define SFX_COIN 64
#define SFX_UNLOCK 65

#define LEVEL_COUNT 5

enum GameScene { SCENE_SPLASH, SCENE_MENU, SCENE_GAMEPLAY };
enum GameScene scene;

typedef struct { u8 x; u8 y; } Point;
typedef enum { UP, DOWN, LEFT, RIGHT, NONE } MoveDirection;

typedef struct {
    Point position;
    Point tilePosition;
    int width;
    int height;
    bool isMoving;
    MoveDirection direction;
    Sprite *sprite;
    char name[7];
} Player;

Player player1 = {{0, 0}, {0, 0}, 8, 8, FALSE, NONE, NULL, "PLAYER1"};
Player player2 = {{0, 0}, {0, 0}, 8, 8, FALSE, NONE, NULL, "PLAYER2"};

typedef struct {
    Point position;
    u8 width;
    u8 height;
    Sprite *sprite;
    u8 health;
} Coin;

typedef u8 map[MAP_HEIGHT][MAP_WIDTH];

Coin coins[MAX_COINS];

u8 totalCoinsP1 = 0;
u8 coinsCollectedP1 = 0;
char scoreTextP1[10] = "";

//u8 totalCoinsP2 = 0;
//u8 coinsCollectedP2 = 0;
//char scoreTextP2[10] = "";

Point exitLocation = { 0, 0 };
bool exitUnlocked = FALSE;

u16 mainPalette[64];

u16 index;

map level1 = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 4, 0},
    {1, 5, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0, 0, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 7, 7},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 7},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 6, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

map level2 = {
    {1, 0, 0, 0, 6, 0, 0, 1, 7, 0},
    {1, 0, 1, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
    {0, 0, 0, 0, 1, 0, 0, 0, 4, 0},
    {0, 1, 0, 1, 1, 0, 1, 0, 1, 1},
    {0, 0, 1, 5, 1, 0, 1, 0, 0, 0},
    {1, 0, 0, 0, 1, 0, 7, 0, 1, 0},
    {0, 7, 0, 1, 1, 0, 1, 0, 1, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0}
};

map level3 = {
    {0, 7, 0, 0, 5, 0, 0, 1, 0, 1},
    {0, 0, 1, 1, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 7},
    {0, 1, 7, 1, 1, 0, 1, 0, 1, 1},
    {0, 1, 0, 6, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 4},
    {0, 1, 1, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

map level4 = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 7, 0, 0, 4, 0},
    {0, 0, 1, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 7, 0, 0, 0, 0, 0},
    {1, 0, 1, 1, 1, 0, 1, 1, 0, 0},
    {0, 0, 0, 5, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 7, 0, 0, 0, 0}
};

map level5 = {
    {0, 1, 0, 6, 0, 0, 1, 0, 1, 0},
    {0, 0, 0, 1, 1, 0, 1, 0, 0, 7},
    {0, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 1, 5, 0, 1, 1, 7, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 7, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 1, 1, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 4, 1, 0, 0, 0}
};

u8 *currentLevel;
static u8 currentLevelIndex = 0;
map *levelSelection[6];

void handleInputMenu(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1 || joy == JOY_2) {
        if (state & BUTTON_START) {
            scene = SCENE_GAMEPLAY;
        }
    }
}

int getTileAt(u8 x, u8 y) {
    return *(currentLevel + (y * MAP_HEIGHT + x));
}

void movePlayer1(MoveDirection direction) {
    if (player1.isMoving == FALSE) {
        switch (direction) {
            case UP:
                if (player1.tilePosition.y > 0 && getTileAt(player1.tilePosition.x, player1.tilePosition.y - 1) != SOLID_TILE) {
                    player1.tilePosition.y -= 1;
                    player1.isMoving = TRUE;
                    player1.direction = direction;

                    SPR_setAnim(player1.sprite, P1_ANIM_UP);
                }

                break;
            case DOWN:
                if (player1.tilePosition.y < MAP_HEIGHT - 1 && getTileAt(player1.tilePosition.x, player1.tilePosition.y + 1) != SOLID_TILE) {
                    player1.tilePosition.y += 1;
                    player1.isMoving = TRUE;
                    player1.direction = direction;

                    SPR_setAnim(player1.sprite, P1_ANIM_DOWN);
                }

                break;
            case LEFT:
                if (player1.tilePosition.x > 0 && getTileAt(player1.tilePosition.x - 1, player1.tilePosition.y) != SOLID_TILE) {
                    player1.tilePosition.x -= 1;
                    player1.isMoving = TRUE;
                    player1.direction = direction;

                    SPR_setAnim(player1.sprite, P1_ANIM_SIDE);
                    SPR_setHFlip(player1.sprite, TRUE);
                }

                break;
            case RIGHT:
                if (player1.tilePosition.x < MAP_WIDTH - 1 && getTileAt(player1.tilePosition.x + 1, player1.tilePosition.y) != SOLID_TILE) {
                    player1.tilePosition.x += 1;
                    player1.isMoving = TRUE;
                    player1.direction = direction;

                    SPR_setAnim(player1.sprite, P1_ANIM_SIDE);
                    SPR_setHFlip(player1.sprite, FALSE);
                }

                break;
            default:
                break;
        }
    }
}

void movePlayer2(MoveDirection direction) {
    if (player2.isMoving == FALSE) {
        switch (direction) {
            case UP:
                if (player2.tilePosition.y > 0 && getTileAt(player2.tilePosition.x, player2.tilePosition.y - 1) != SOLID_TILE) {
                    player2.tilePosition.y -= 1;
                    player2.isMoving = TRUE;
                    player2.direction = direction;

                    SPR_setAnim(player2.sprite, P2_ANIM_UP);
                }

                break;
            case DOWN:
                if (player2.tilePosition.y < MAP_HEIGHT - 1 && getTileAt(player2.tilePosition.x, player2.tilePosition.y + 1) != SOLID_TILE) {
                    player2.tilePosition.y += 1;
                    player2.isMoving = TRUE;
                    player2.direction = direction;

                    SPR_setAnim(player2.sprite, P2_ANIM_DOWN);
                }

                break;
            case LEFT:
                if (player2.tilePosition.x > 0 && getTileAt(player2.tilePosition.x - 1, player2.tilePosition.y) != SOLID_TILE) {
                    player2.tilePosition.x -= 1;
                    player2.isMoving = TRUE;
                    player2.direction = direction;

                    SPR_setAnim(player2.sprite, P2_ANIM_SIDE);
                    SPR_setHFlip(player2.sprite, TRUE);
                }

                break;
            case RIGHT:
                if (player2.tilePosition.x < MAP_WIDTH - 1 && getTileAt(player2.tilePosition.x + 1, player2.tilePosition.y) != SOLID_TILE) {
                    player2.tilePosition.x += 1;
                    player2.isMoving = TRUE;
                    player2.direction = direction;

                    SPR_setAnim(player2.sprite, P2_ANIM_SIDE);
                    SPR_setHFlip(player2.sprite, FALSE);
                }

                break;
            default:
                break;
        }
    }
}

void handleInputGameplay(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (state & BUTTON_UP) {
            movePlayer1(UP);
        } else if (state & BUTTON_DOWN) {
            movePlayer1(DOWN);
        } else if (state & BUTTON_LEFT) {  
            movePlayer1(LEFT);
        } else if (state & BUTTON_RIGHT) {
            movePlayer1(RIGHT);
        }
    }

    if (joy == JOY_2) {
        if (state & BUTTON_UP) {
            movePlayer2(UP);
        } else if (state & BUTTON_DOWN) {
            movePlayer2(DOWN);
        } else if (state & BUTTON_LEFT) {  
            movePlayer2(LEFT);
        } else if (state & BUTTON_RIGHT) {
            movePlayer2(RIGHT);
        }
    }
}

void updateScoreDisplay() {
    //sprintf(scoreTextP1, "P1 SCORE: %d", totalCoinsP1 + coinsCollectedP1);
    //VDP_clearText(20, 0, 15);
    //VDP_drawText(scoreTextP1, 20, 0);

    //sprintf(scoreTextP2, "P2 SCORE: %d", totalCoinsP2 + coinsCollectedP2);
    //VDP_clearText(20, 1, 15);
    //VDP_drawText(scoreTextP2, 20, 1);
}

void unlockExit() {
    exitUnlocked = TRUE;
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index + 2), exitLocation.x, exitLocation.y);
    XGM_startPlayPCM(SFX_UNLOCK, 1, SOUND_PCM_CH2);
}

void clearLevel() {
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearSprites();

    totalCoinsP1 = totalCoinsP1 + coinsCollectedP1;
    coinsCollectedP1 = 0;

    updateScoreDisplay();
    exitUnlocked = FALSE;
}

void loadLevel() {
    clearLevel();
    currentLevel = levelSelection[currentLevelIndex];

    u8 x = 0;
    u8 y = 0;
    u8 t = 0;
    
    u8 coinCount = 0;
    Coin *coin = coins;

    SPR_init();

    u8 i = 0;
    u8 total = MAP_HEIGHT * MAP_WIDTH;

    for (i = 0; i < total; i++) {
        t = *(currentLevel + i);

        if (t == P1_SPAWN_TILE) {
            player1.tilePosition.x = x;
            player1.tilePosition.y = y;

            player1.position.x = player1.tilePosition.x * TILESIZE;
            player1.position.y = player1.tilePosition.y * TILESIZE;

            player1.sprite = SPR_addSprite(&chars, player1.position.x, player1.position.y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index), x, y);
        } else if (t == P2_SPAWN_TILE) {
            player2.tilePosition.x = x;
            player2.tilePosition.y = y;

            player2.position.x = player2.tilePosition.x * TILESIZE;
            player2.position.y = player2.tilePosition.y * TILESIZE;

            player2.sprite = SPR_addSprite(&chars, player2.position.x, player2.position.y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
            SPR_setAnim(player2.sprite, P2_ANIM_DOWN);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index), x, y);
        } else if (t == COIN_TILE) {
            if (coinCount < MAX_COINS) {
                coin = &coins[coinCount];
                coin->position.x = x * TILESIZE;
                coin->position.y = y * TILESIZE;
                coin->width = 8;
                coin->height = 8;
                coin->health = 1;
                coin->sprite = SPR_addSprite(&spr_coin, coin->position.x, coin->position.y, TILE_ATTR(PAL3, 0, FALSE, FALSE));
                coinCount++;

                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index), x, y);
            }
        } else if (t == EXIT_TILE) {
            exitLocation.x = x;
            exitLocation.y = y;
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, index), x, y);
        } else {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index + t), x, y);
        }

        x++;
        if (x >= MAP_WIDTH) {
            y++;
            x = 0;
        }
    }
}

int main() {
    scene = SCENE_SPLASH;

    JOY_init();
    SYS_disableInts();
    SPR_init(0);

    PAL_setColors(0, palette_black, 64, DMA);

    XGM_setPCM(SFX_COIN, sfx_coin, sizeof(sfx_coin));
    XGM_setPCM(SFX_UNLOCK, sfx_unlock, sizeof(sfx_unlock));

    /* Load Splash */
    

    VDP_clearPlane(BG_A, TRUE);
    DMA_waitCompletion();

    VDP_clearPlane(BG_B, TRUE);
    DMA_waitCompletion();

    index = TILE_USERINDEX;
    VDP_drawImageEx(BG_B, &splash, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index), 0, 0, FALSE, TRUE);
    index += splash.tileset->numTile;

    memcpy(&mainPalette[0], splash.palette->data, 32);
    /* --- */

    SYS_enableInts();
    
    while(TRUE) {
        switch (scene) {
            case SCENE_SPLASH:
                PAL_fadeIn(0, 63, mainPalette, 20, FALSE);
                JOY_setEventHandler(NULL);

                waitTick(TICKPERSECOND * 3);

                PAL_fadeOutAll(20, FALSE);
                waitTick(TICKPERSECOND / 2);

                scene = SCENE_MENU;
                //scene = SCENE_GAMEPLAY;
                break;
            case SCENE_MENU:
                /* Load Menu */
                VDP_clearPlane(BG_A, TRUE);
                DMA_waitCompletion();

                VDP_clearPlane(BG_B, TRUE);
                DMA_waitCompletion();

                index = TILE_USERINDEX;

                // Draw images, prepare palettes, start music
                VDP_drawImageEx(BG_B, &title, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index), 1, 2, FALSE, TRUE);
                memcpy(&mainPalette[0], title.palette->data, 16);

                VDP_drawText("GGJ 2022 - Baixada Santista", 6, 22);
                VDP_drawText(" Programming by Danilo ", 8, 24);
                VDP_drawText("Game Design by Mitzrael", 8, 25);
                VDP_drawText("   Art by Thaynara     ", 8, 26);

                // TODO: Credits

                /* --- */
                
                PAL_fadeIn(0, 63, mainPalette, 60, FALSE);
                PAL_waitFadeCompletion();
                VDP_drawText("Press START button", 11, 16);

                JOY_setEventHandler(&handleInputMenu);

                while (scene == SCENE_MENU) {
                    SYS_doVBlankProcess();
                }

                VDP_clearTextArea(11, 16, 19, 1);

                PAL_fadeOutAll(20, FALSE);
                waitTick(TICKPERSECOND / 2);

                scene = SCENE_GAMEPLAY;
                break;
            case SCENE_GAMEPLAY:
                /* Load Game */
                //VDP_clearTextArea(10, 15, 21, 12);

                VDP_clearPlane(BG_A, TRUE);
                DMA_waitCompletion();

                VDP_clearPlane(BG_B, TRUE);
                DMA_waitCompletion();

                //index = TILE_USERINDEX;

                // Draw images, prepare palettes, start music
                VDP_loadTileSet(floortiles.tileset, index, DMA);

                memcpy(&mainPalette[16], floortiles.palette->data, 32);
                memcpy(&mainPalette[32], chars.palette->data, 32);
                memcpy(&mainPalette[48], spr_coin.palette->data, 18);

                levelSelection[0] = level1;
                levelSelection[1] = level2;
                levelSelection[2] = level3;
                levelSelection[3] = level4;
                levelSelection[4] = level5;
                /* --- */

                /* Load Level */
                loadLevel();
                /* --- */

                PAL_fadeIn(0, 63, mainPalette, 20, FALSE);
                JOY_setEventHandler(&handleInputGameplay);

                /*
                VDP_drawText("000000000000000000000000", 8, 4);
                VDP_drawText("000000000000000000000000", 8, 5);
                VDP_drawText("000000000000000000000000", 8, 6);
                VDP_drawText("000000000000000000000000", 8, 7);
                VDP_drawText("000000000000000000000000", 8, 8);
                VDP_drawText("000000000000000000000000", 8, 9);
                VDP_drawText("000000000000000000000000", 8, 10);
                VDP_drawText("000000000000000000000000", 8, 11);
                VDP_drawText("000000000000000000000000", 8, 12);
                VDP_drawText("000000000000000000000000", 8, 13);
                VDP_drawText("000000000000000000000000", 8, 14);
                VDP_drawText("000000000000000000000000", 8, 15);
                VDP_drawText("000000000000000000000000", 8, 16);
                VDP_drawText("000000000000000000000000", 8, 17);
                VDP_drawText("000000000000000000000000", 8, 18);
                VDP_drawText("000000000000000000000000", 8, 19);
                VDP_drawText("000000000000000000000000", 8, 20);
                VDP_drawText("000000000000000000000000", 8, 21);
                VDP_drawText("000000000000000000000000", 8, 22);
                VDP_drawText("000000000000000000000000", 8, 23);
                */

                Coin *coinToCheck;
                updateScoreDisplay();

                while (scene == SCENE_GAMEPLAY) {
                    /* Gameplay */

                    if (player1.isMoving == TRUE) {
                        switch (player1.direction) {
                            case UP:
                                player1.position.y -= 1;
                                break;
                            case DOWN:
                                player1.position.y += 1;
                                break;
                            case LEFT:
                                player1.position.x -= 1;
                                break;
                            case RIGHT:
                                player1.position.x += 1;
                                break;
                            default:
                                break;
                        }

                        if (player1.position.x % TILESIZE == 0 && player1.position.y % TILESIZE == 0) {
                            player1.isMoving = FALSE;

                            if (exitUnlocked == TRUE && player1.tilePosition.x == exitLocation.x && player1.tilePosition.y == exitLocation.y) {
                                currentLevelIndex++;
                                if (currentLevelIndex >= LEVEL_COUNT) {
                                    currentLevelIndex = 0;
                                }

                                loadLevel();
                            }
                        }
                    }

                    if (player2.isMoving == TRUE) {
                        switch (player2.direction) {
                            case UP:
                                player2.position.y -= 1;
                                break;
                            case DOWN:
                                player2.position.y += 1;
                                break;
                            case LEFT:
                                player2.position.x -= 1;
                                break;
                            case RIGHT:
                                player2.position.x += 1;
                                break;
                            default:
                                break;
                        }

                        if (player2.position.x % TILESIZE == 0 && player2.position.y % TILESIZE == 0) {
                            player2.isMoving = FALSE;

                            if (exitUnlocked == TRUE && player2.tilePosition.x == exitLocation.x && player2.tilePosition.y == exitLocation.y) {
                                currentLevelIndex++;
                                if (currentLevelIndex >= LEVEL_COUNT) {
                                    currentLevelIndex = 0;
                                }

                                loadLevel();
                            }
                        }
                    }

                    u8 i = 0;

                    for (i = 0; i < MAX_COINS; i++) {
                        coinToCheck = &coins[i];
                        
                        if ((player1.position.x < coinToCheck->position.x + coinToCheck->width 
                            && player1.position.x + player1.width > coinToCheck->position.x
                            && player1.position.y < coinToCheck->position.y + coinToCheck->height
                            && player1.position.y + player1.height > coinToCheck->position.y) == TRUE
                            || (player2.position.x < coinToCheck->position.x + coinToCheck->width 
                            && player2.position.x + player2.width > coinToCheck->position.x
                            && player2.position.y < coinToCheck->position.y + coinToCheck->height
                            && player2.position.y + player2.height > coinToCheck->position.y) == TRUE) {
                            if (coinToCheck->health > 0) {
                                coinToCheck->health = 0;

                                coinsCollectedP1++;
                                updateScoreDisplay();

                                XGM_startPlayPCM(SFX_COIN, 1, SOUND_PCM_CH2);
                                SPR_setVisibility(coinToCheck->sprite, HIDDEN);
                            }
                        }
                    }

                    if (coinsCollectedP1 == MAX_COINS) {
                        unlockExit();
                    }

                    SPR_setPosition(player1.sprite, player1.position.x, player1.position.y);
                    SPR_setPosition(player2.sprite, player2.position.x, player2.position.y);

                    SPR_update();
                    SYS_doVBlankProcess();
                }

                // TODO
                break;
            default:
                break;
        }
    }

    return 0;
}

#include <genesis.h>

enum GameScene { SCENE_MENU, SCENE_GAMEPLAY, SCENE_GAMEOVER };
enum GameScene scene;

void handleInput(u16 joystick, u16 changed, u16 state) {
    if (joystick == JOY_1) {
        switch (scene) {
            case SCENE_MENU:
                if (state & BUTTON_START) {
                    
                }
                break;
            case SCENE_GAMEPLAY:
                /* code */
                break;
            case SCENE_GAMEOVER:
                /* code */
                break;
            default:
                break;
        }

    }
}

int main() {
    VDP_drawText("Global Game Jam 2022 - Theme: Duality", 1, 13);

    JOY_init();
    JOY_setEventHandler(&handleInput);

    while(TRUE) {
        /* TODO: core loop */

        SYS_doVBlankProcess();
    }

    return 0;
}

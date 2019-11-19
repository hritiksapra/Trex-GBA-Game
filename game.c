#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gba.h"
#include "game.h"
#include "trex.h"
#include "background.h"
#include "startscreen.h"
#include "images/garbage.h"
#include "trexup.h"
#include "trexdown.h"
#include "horizon.h"
#include "cactus.h"
#include "cactii.h"
#include "gameover.h"
                    /* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"


                    /* TODO: */
// Add any additional states you need for your app.
typedef enum {
    START,
    START_NO_DRAW,
    PLAY,
    LOSE,
} GBAState;

int calcStringWidth(char *string) {
    return (int) ((strlen(string) - 1) * 6);
}

Trex ourTrex = {125, 1};
Cactus ourCac = {180, 134, cactus, 19, 10};
volatile int points = 0;
volatile int speed = 2;
int detectCollision(void);

int main(void) {
                    /* TODO: */
    // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = MODE3 | BG2_ENABLE;
    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial game state
    GBAState state = START;
    int frameCount = 0;
    int started = 0;
    while (1) {
        frameCount++;
        currentButtons = BUTTONS; // Load the current state of the buttons

        if (KEY_DOWN(BUTTON_SELECT, BUTTONS)) {
        	state = START; // If at any time they press Backspace, go back to beginning
            speed = 2;
            points = 0;
            ourTrex.row = 125;
            ourTrex.col = 1;
            ourCac.col = 180;
            ourCac.image = cactus;
            ourCac.width = 10;
        }

                    /* TODO: */
        // Manipulate the state machine below as needed //
        // NOTE: Call waitForVBlank() before you draw

        switch(state) {
            case START:
                waitForVBlank();
                drawFullScreenImageDMA(startscreen);
                drawString(20, 80, "Press Up", BLACK);
                drawString(25, 90, "to play", BLACK);
                state = START_NO_DRAW;
                break;
            case START_NO_DRAW:
                if (KEY_DOWN(BUTTON_UP, BUTTONS)) {
                    drawFullScreenImageDMA(backgroud);
                    drawImageDMA(ourTrex.col, ourTrex.row,26, 26, trexup);
                    drawImageDMA(0, 145,240, 24, horizon);
                    drawImageDMA(ourCac.col, ourCac.row, ourCac.width, ourCac.height, ourCac.image);
                    drawString(20, 30, "Score: 0", BLACK);
                    state = PLAY;
                }
                break;
            case PLAY:
                // drawBackPortionDMA(230, ourTrex.row, CACTUS_WIDTH, CACTUS_HEIGHT, cactus);
                if (detectCollision()) {
                    state = LOSE;
                    break;
                }
                waitForVBlank();
                if (started) {
                    drawBackPortionDMA(ourCac.col + ourCac.width - speed, ourCac.row, speed, ourCac.height, backgroud);
                    // if (frameCount % 20 == 0) {
                        ourCac.col -= speed;
                    // }
                    
                    // if (!detectCollision()) {

                    // }
                    if (points == 5 && speed == 2) {
                        ourCac.image = cactii;
                        ourCac.width = 20;
                        speed++;
                    }
                    if (ourCac.col < 1) {
                        drawBackPortionDMA(ourCac.col, ourCac.row, ourCac.width, ourCac.height , backgroud);
                        drawBackPortionDMA(20, 30, 60, 8, backgroud);
                        points++;
                        char buffer[128];
                        snprintf(buffer,128,"Score: %d", points);
                        drawString(20, 30, buffer, BLACK);
                        ourCac.col = 180;
                    }
                    drawImageDMA(ourCac.col, ourCac.row, ourCac.width, ourCac.height, ourCac.image);
                }
                if (KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons) ) {
                        started = 1;
                        // waitForVBlank();
                        drawBackPortionDMA(ourTrex.col, ourTrex.row, 26, 26, backgroud);
                        // if (frameCount != 0) {
                            ourTrex.row -= 80;
                            // ourTrex.col += 30;
                        // }
                        drawImageDMA(ourTrex.col, ourTrex.row, 26, 26, trexup);
                    } else {
                        // if (frameCount % 60 == 0) {
                            // waitForVBlank();
                            drawBackPortionDMA(ourTrex.col, ourTrex.row, 26/* + ourTrex.col*/, 3, backgroud);
                            ourTrex.row += 3;
                            // ourTrex.col -= 1;
                            // if (ourTrex.col < 1) {
                            //     ourTrex.col = 1;
                            // }
                            if (ourTrex.row > 125) //{
                                ourTrex.row = 125;
                            // }
                            drawImageDMA(ourTrex.col, ourTrex.row, 26, 26, trexup);
                        //}
                    }
                
                // state = ?
                break;
            case LOSE:
                waitForVBlank();
                drawFullScreenImageDMA(gameover);
                char bufferer[128];
                snprintf(bufferer,128,"%d", points);
                drawString(1, 80, "Score: ", RED);
                drawString(1, 100, bufferer, RED);
                // state = ?
                break;
        }
        previousButtons = currentButtons; // Store the current state of the buttons
    }
    return 0;
}

int detectCollision(void) {
    int x1 = ourTrex.col;
    int x2 = ourTrex.col + TREX_WIDTH;
    int y1 = ourTrex.row;
    int y2 = ourTrex.row + TREX_HEIGHT;

    int cactX1 = ourCac.col;
    int cactX2 = ourCac.col + ourCac.width;
    int cactY1 = ourCac.row;
    int cactY2 = ourCac.row + ourCac.height;
    
    if ((x1 >= cactX1 && x1 <= cactX2) && (y1 >= cactY1 && y1 <= cactY2))
        return 1;
    else if ((x2 >= cactX1 && x2 <= cactX2) && (y2 >= cactY1 && y2 <= cactY2))
        return 1;
    else {
        if ((cactX1 >= x1 && cactX1 <= x2) && (cactY1 >= y1 && cactY1 <= y2))
            return 1;
        else if ((cactX2 >= x1 && cactX2 <= x2) && (cactY2 >= y1 && cactY2 <= y2))
            return 1;
    }
    return 0;
}
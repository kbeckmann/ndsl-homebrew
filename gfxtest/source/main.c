#include <nds.h>
#include <gl2d.h>
#include <stdio.h>
#include <stdbool.h>

#define CLAMP(val, min, max) (((val) < (min)) ? (min) : (((val) > (max)) ? (max) : (val)))

typedef enum pattern_e
{
    PATTERN_BLACK,
    PATTERN_RED,
    PATTERN_GRADIENT_HORIZONTAL,
    PATTERN_GRADIENT_VERTICAL,
    PATTERN_STRIPE_2X,
    PATTERN_STRIPE_4X,

    PATTERN_COUNT,
} pattern_t;


bool inside_rect(touchPosition p, unsigned x, unsigned y, unsigned width, unsigned height) {
    return (p.px >= x) && (p.px < x + width) &&
           (p.py >= y) && (p.py < y + height);
}


int main(void)
{
    touchPosition touch = {0};
    touchPosition lastTouch = {0};
    PrintConsole bottomScreen;

    // Set mode 0, enable BG0 and set it to 3D
    videoSetMode(MODE_0_3D);

    // Bottom screen
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSelect(&bottomScreen);

    // Initialize gl
    glInit();

    // Enable antialiasing
    glEnable(GL_ANTIALIAS);

    // Setup the rear plane
    glClearColor(0, 63, 0, 31); // BG must be opaque for AA to work
    glClearPolyID(63); // BG must have a unique polygon ID for AA to work
    glClearDepth(0x7FFF);

    // This should work the same as the normal gl call
    glViewport(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    uint32_t frame = 0;
    uint32_t pattern = 0;
    uint32_t col_r = 31;
    uint32_t col_g = 0;
    uint32_t col_b = 0;

    while(pmMainLoop())
    {
        touchRead(&touch);

        if ((touch.px != lastTouch.px) || (touch.py != lastTouch.py))
        {
            if (touch.px == 0 && touch.py == 0)
            {
                // Release
                if (inside_rect(lastTouch, 0, 0, 100, SCREEN_HEIGHT))
                {
                    pattern = (pattern + 1) % PATTERN_COUNT;
                }
            }

            if (inside_rect(touch, 100, 0, 50, SCREEN_HEIGHT))
            {
                col_r = CLAMP((touch.py - 32) / 4, 0, 31);
            }
            if (inside_rect(touch, 150, 0, 50, SCREEN_HEIGHT))
            {
                col_g = CLAMP((touch.py - 32) / 4, 0, 31);;
            }
            if (inside_rect(touch, 200, 0, 50, SCREEN_HEIGHT))
            {
                col_b = CLAMP((touch.py - 32) / 4, 0, 31);;
            }
        }

        lastTouch = touch;

        consoleClear();
        iprintf("               R    G    B\n\n");
        iprintf("Frame = %ld\n", frame);
        iprintf("(%04i, %04i)\n", touch.px, touch.py);
        iprintf("Pattern = %ld\n", pattern);
        iprintf("R = %02lu\n", col_r);
        iprintf("G = %02lu\n", col_g);
        iprintf("B = %02lu\n", col_b);

        frame++;

        // This sets up glortho etc so we can easily draw each pixel on the screen
        glBegin2D();

        int col = RGB15( col_r, col_g, col_b );

        switch (pattern)
        {
            case PATTERN_BLACK:
            {
                // Black
                glBoxFilledGradient( 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,
                                        RGB15( 0, 0, 0 ),
                                        RGB15( 0, 0, 0 ),
                                        RGB15( 0, 0, 0 ),
                                        RGB15( 0, 0, 0 )
                                    );
                break;
            }

            case PATTERN_RED:
            {
                // Red
                glBoxFilledGradient( 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,
                                        col,
                                        col,
                                        col,
                                        col
                                    );
                break;
            }

            case PATTERN_GRADIENT_HORIZONTAL:
            {
                // Gradient Red to Black, left->right
                // Fill all of the 256 x 192 pixels.
                // Output is RGB666, but we define colors in RGB555.
                // 31 = UINT5_MAX
                glBoxFilledGradient( 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,
                                        col,
                                        col,
                                        RGB15( 0, 0, 0 ),
                                        RGB15( 0, 0, 0 )
                                    );
                break;
            }

            case PATTERN_GRADIENT_VERTICAL:
            {
                // Gradient Red to Black, left->right
                // Fill all of the 256 x 192 pixels.
                // Output is RGB666, but we define colors in RGB555.
                // 31 = UINT5_MAX
                glBoxFilledGradient( 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,
                                        col,
                                        RGB15( 0, 0, 0 ),
                                        RGB15( 0, 0, 0 ),
                                        col
                                    );
                break;
            }

            case PATTERN_STRIPE_2X:
            {
                // Striped pattern, every second pixel is black.
                for (int i = 0; i < SCREEN_WIDTH; i++)
                {
                    int col_stripe = 0;
                    if ((i & 0b1) == 0)
                    {
                        col_stripe = col;
                    }

                    glBoxFilledGradient( i, 0, i + 1, SCREEN_HEIGHT - 1,
                                            col_stripe,
                                            col_stripe,
                                            col_stripe,
                                            col_stripe
                                        );
                }
                break;
            }

            case PATTERN_STRIPE_4X:
            {
                // Striped pattern, every second pixel is black.
                for (unsigned i = 0; i < SCREEN_WIDTH; i++)
                {
                    int col_stripe = 0;
                    if ((i & 0b11) <= 0b01)
                    {
                        col_stripe = col;
                    }

                    glBoxFilledGradient( i, 0, i + 1, SCREEN_HEIGHT - 1,
                                            col_stripe,
                                            col_stripe,
                                            col_stripe,
                                            col_stripe
                                        );
                }
                break;
            }

            default:
                break;
        }

        glEnd2D();

        glFlush(0);

        swiWaitForVBlank();
    }

    return 0;
}

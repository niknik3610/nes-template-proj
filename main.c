#include "nesdoug/nesdoug.h"
#include "nesdoug/neslib.h"

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
 

//This is required because of some weird compiler/linker stuff
#pragma bss-name (push, "ZEROPAGE")
unsigned char oam_off;
#pragma bss-name (pop)

//black must be 0x0f, white must be 0x30
const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
};

#define BALL_LOCATION_TILES 1
#define L_WALL_LOCATION_TILES 2
#define R_WALL_LOCATION_TILES 3

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 30

#define PLAYER_LEN 3

// example of non-sequential vram data
unsigned char ball[] = {
0,
0,
BALL_LOCATION_TILES,
0,
0,
0,
NT_UPD_EOF}; // data must end in EOF

unsigned char wall_l[] = {
0,
0,
L_WALL_LOCATION_TILES,
NT_UPD_EOF};

unsigned char wall_r[] = {
0,
0,
R_WALL_LOCATION_TILES,
NT_UPD_EOF};

unsigned char player_buf[4 * PLAYER_LEN + 1];

unsigned char i;
unsigned char timer;

unsigned char player_0_location_y;
unsigned char player_1_location_y;

int main() {
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	ppu_on_all(); //	turn on screen
    
    player_0_location_y = SCREEN_HEIGHT >> 1;
    player_1_location_y = SCREEN_HEIGHT >> 1;

    for (i = 0; i < PLAYER_LEN; i++) {
        player_buf[4 * i + 0] = MSB(NTADR_A(0, player_0_location_y + i));
        player_buf[4 * i + 1] = LSB(NTADR_A(0, player_0_location_y + i));
        player_buf[4 * i + 2] = wall_l[2];
        player_buf[4 * i + 3] = wall_l[3];
    }
    player_buf[4 * PLAYER_LEN] = NT_UPD_EOF;

    set_vram_update(player_buf);
    ppu_wait_nmi();

    for (i = 0; i < PLAYER_LEN; i++) {
        player_buf[4 * i + 0] = MSB(NTADR_A(SCREEN_WIDTH - 1, player_1_location_y + i));
        player_buf[4 * i + 1] = LSB(NTADR_A(SCREEN_WIDTH - 1, player_1_location_y + i));
        player_buf[4 * i + 2] = wall_r[2];
        player_buf[4 * i + 3] = wall_r[3];
    }
    player_buf[4 * PLAYER_LEN] = NT_UPD_EOF;
    set_vram_update(player_buf);
    ppu_wait_nmi();

    i = 0;
    while(1) {
        ball[0] = MSB(NTADR_A(i, 14));
        ball[1] = LSB(NTADR_A(i, 14));
        set_vram_update(ball);
        ppu_wait_nmi();

        ball[3] = MSB(NTADR_A(i, 14));
        ball[4] = LSB(NTADR_A(i, 14));
        i = i % (SCREEN_WIDTH - 1) + 1;
    }
}

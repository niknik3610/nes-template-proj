#include "nesdoug/nesdoug.h"
#include "nesdoug/neslib.h"

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

#define EMPTY_SPRITE_LOC 0
#define BALL_SPRITE_LOC 1
#define L_WALL_SPRITE_LOC 2
#define R_WALL_SPRITE_LOC 3

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 30

#define PLAYER_LEN 3

#define X 0
#define Y 1

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

unsigned char ball_buf[] = {
    0,
    0,
    BALL_SPRITE_LOC,
    0,
    0,
    EMPTY_SPRITE_LOC,
    NT_UPD_EOF
};

//For use when you want to delete something from the screen
unsigned char empty_buf[] = {
    0,
    0,
    EMPTY_SPRITE_LOC,
};


unsigned char player_buf[3 * PLAYER_LEN + 1];

unsigned char i;
char negative;
unsigned char timer;

unsigned char player_0_loc[] = {0, (SCREEN_HEIGHT >> 1) - 1};
unsigned char player_1_loc[] = {SCREEN_WIDTH - 1, (SCREEN_HEIGHT >> 1) - 1};

unsigned char ball_loc[] = {0, 14};
unsigned char ball_movement_vec[] = {1, 0};

void update_player_video_buf(unsigned char wall_sprite, unsigned char x, unsigned char y) {
    for (i = 0; i < PLAYER_LEN; i++) {
        player_buf[3 * i + 0] = MSB(NTADR_A(x, y + i));
        player_buf[3 * i + 1] = LSB(NTADR_A(x, y + i));
        player_buf[3 * i + 2] = wall_sprite;
    }
    player_buf[3 * PLAYER_LEN] = NT_UPD_EOF;
}

void update_buf_location(unsigned char* buf, unsigned char x, unsigned char y) {
    buf[0] = MSB(NTADR_A(x, y));
    buf[1] = LSB(NTADR_A(x, y));
}

void update_ball_video_buf(unsigned char x, unsigned char y) {
    ball_buf[0] = MSB(NTADR_A(x, y));
    ball_buf[1] = LSB(NTADR_A(x, y));
}

void update_ball() {
    update_buf_location(ball_buf + 3, ball_loc[X], ball_loc[Y]);
    ball_loc[X] += ball_movement_vec[X];
    ball_loc[Y] += ball_movement_vec[Y];
    update_buf_location(ball_buf, ball_loc[X], ball_loc[Y]);
}

int main() {
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	ppu_on_all(); //	turn on screen
    
    update_player_video_buf(L_WALL_SPRITE_LOC, player_0_loc[X], player_0_loc[Y]);
    set_vram_update(player_buf);
    ppu_wait_nmi();

    update_player_video_buf(R_WALL_SPRITE_LOC, player_1_loc[X], player_1_loc[Y]);
    set_vram_update(player_buf);
    ppu_wait_nmi();

    i = 0;
    negative = 1;
    while(1) {
        // if (i > SCREEN_WIDTH - 1) {
        //     negative = 1;
        // }
        // else {
        //     negative = -1;
        // }

        // ball_movement_vec[X] *= negative;
        update_ball();
        set_vram_update(ball_buf);
        ppu_wait_nmi();
        // i = i + 1 % ((SCREEN_WIDTH - 1) * 2);
    }
}

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

#define MAXIMUM_VRAM_ITEMS 10
#define VRAME_ITEM_SIZE 3

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

struct  GameObject {
    unsigned char pos[2];
    unsigned char sprite_loc;
};

struct GameObject game_object_make(unsigned char loc[2], unsigned char sprite_loc) {
    struct GameObject obj = {
        .pos = {loc[0], loc[1]},
        .sprite_loc = sprite_loc
    };

    return obj;
}

typedef struct {
    unsigned char max_capacity, len;
    // + 1 for EOF
    unsigned char vram_buf[MAXIMUM_VRAM_ITEMS * VRAME_ITEM_SIZE + 1];
} VramBuf;

VramBuf vram_buf_make() {
    VramBuf buf = {
        .max_capacity = MAXIMUM_VRAM_ITEMS * VRAME_ITEM_SIZE + 1,
        .len = 0,
        .vram_buf = {}
    };
    return buf;
}
int vram_buf_push(VramBuf* buf, GameObject* obj) {
    unsigned char curr_len = buf->len;
    if (curr_len >= buf->max_capacity) {
        return -1;
    }

    buf->vram_buf[curr_len] = MSB(NTADR_A(obj->pos[X], obj->pos[Y]));
    buf->vram_buf[curr_len + 1] = LSB(NTADR_A(obj->pos[X], obj->pos[Y]));
    buf->vram_buf[curr_len + 2] = obj->sprite_loc;
    buf->len += 3;

    return 0;
}

void vram_buf_finish(VramBuf* buf) {
    buf->vram_buf[buf->len] = EMPTY_SPRITE_LOC;
    buf->len = 0;
}


// unsigned char player_buf[3 * PLAYER_LEN + 1];

unsigned char i;
char negative;
unsigned char timer;

unsigned char player_0_spawn_point[] = {0, (SCREEN_HEIGHT >> 1) - 1};
unsigned char player_1_spawn_point[] = {SCREEN_WIDTH - 1, (SCREEN_HEIGHT >> 1) - 1};
unsigned char ball_spawn_point[] = {1, 14};
//
// unsigned char ball_loc[] = {0, 14};

VramBuf vram_buf;
GameObject player_0_obj, player_1_obj;
GameObject ball_obj;

char ball_movement_vec[] = {-1, 0};

// void update_player_video_buf(unsigned char wall_sprite, unsigned char x, unsigned char y) {
//     for (i = 0; i < PLAYER_LEN; i++) {
//         player_buf[3 * i + 0] = MSB(NTADR_A(x, y + i));
//         player_buf[3 * i + 1] = LSB(NTADR_A(x, y + i));
//         player_buf[3 * i + 2] = wall_sprite;
//     }
//     player_buf[3 * PLAYER_LEN] = NT_UPD_EOF;
// }
//
// void update_buf_location(unsigned char* buf, unsigned char x, unsigned char y) {
//     buf[0] = MSB(NTADR_A(x, y));
//     buf[1] = LSB(NTADR_A(x, y));
// }
//
// void update_ball() {
//     update_buf_location(ball_buf + 3, ball_loc[X], ball_loc[Y]);
//     ball_loc[X] += ball_movement_vec[X];
//     ball_loc[Y] += ball_movement_vec[Y];
//     update_buf_location(ball_buf, ball_loc[X], ball_loc[Y]);
// }

int main() {
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	ppu_on_all(); //	turn on screen

    vram_buf = vram_buf_make();
    player_0_obj = game_object_make(player_0_spawn_point, L_WALL_SPRITE_LOC);
    player_1_obj = game_object_make(player_1_spawn_point, R_WALL_SPRITE_LOC);
    ball_obj = game_object_make(ball_spawn_point, BALL_SPRITE_LOC);

    vram_buf_push(&vram_buf, &player_0_obj);
    vram_buf_push(&vram_buf, &player_1_obj);
    vram_buf_finish(&vram_buf);

    set_vram_update(vram_buf.vram_buf);
    ppu_wait_nmi();


    
    // update_player_video_buf(L_WALL_SPRITE_LOC, player_0_loc[X], player_0_loc[Y]);
    // set_vram_update(player_buf);
    // ppu_wait_nmi();
    //
    // update_player_video_buf(R_WALL_SPRITE_LOC, player_1_loc[X], player_1_loc[Y]);
    // set_vram_update(player_buf);
    // ppu_wait_nmi();

    i = 0;
    negative = 1;
    while(1) {
        // update_ball();
        // set_vram_update(ball_buf);
        // ppu_wait_nmi();
    }
}

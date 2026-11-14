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

unsigned char i;
//black must be 0x0f, white must be 0x30
const unsigned char palette[]={
    BLACK, DK_GY, LT_GY, WHITE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

typedef struct {
    unsigned char pos[2];
    unsigned char sprite_loc;
} GameObject;

void game_object_make(GameObject* out, unsigned char loc[2], unsigned char sprite_loc) {
    out->pos[0] = loc[0];
    out->pos[1] = loc[1];
    out->sprite_loc = sprite_loc;
}

typedef struct {
    unsigned char size;
    GameObject inner[];
} GameObjectGroup;

/// Copies game_objects into the Group. Assumes out has been allocated with the correct size
void game_object_group_make(GameObjectGroup* out, GameObject* objects, unsigned char object_count) {
    for (i = 0; i < object_count; i++) {
        out->inner[i] = objects[i];
    }
    out->size = object_count;
}

void game_object_group_move(GameObjectGroup* group, unsigned char movement_vector[2]) {
    for (i = 0; i < group->size; i++) {
        GameObject* curr_obj = group->inner + i;
        curr_obj->pos[X] += movement_vector[X];
        curr_obj->pos[Y] += movement_vector[Y];
    }
}

/// Assumes group_one & group_two are the same size
void game_object_group_copy_positions(GameObjectGroup* dest_group, GameObjectGroup* src_group) {
    for (i = 0; i < dest_group->size; i++) {
        GameObject* dst_obj = dest_group->inner + i;
        GameObject* src_obj = src_group->inner + i;

        dst_obj->pos[X] = src_obj->pos[X];
        dst_obj->pos[Y] = src_obj->pos[Y];
    }
}

typedef struct {
    unsigned char max_capacity, len;
    // + 1 for EOF
    unsigned char vram_buf[MAXIMUM_VRAM_ITEMS * VRAME_ITEM_SIZE + 1];
} VramBuf;

void vram_buf_make(VramBuf* out) {
   out->max_capacity = MAXIMUM_VRAM_ITEMS * VRAME_ITEM_SIZE + 1;
   out->len = 0;
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
    buf->vram_buf[buf->len] = NT_UPD_EOF;
    buf->len = 0;
}

GameObject empty_obj;
VramBuf vram_buf;
/// Moves the game object & the empty object. Does not push
void game_object_move(GameObject* obj, char mov_vec[2]) {
    empty_obj.pos[X] = obj->pos[X];
    empty_obj.pos[Y] = obj->pos[Y];
    obj->pos[X] += mov_vec[X];
    obj->pos[Y] += mov_vec[Y];
}

unsigned char player_0_score;
unsigned char player_1_score;

char negative;
unsigned char timer;

unsigned char player_0_spawn_point[] = {0, (SCREEN_HEIGHT >> 1) - 1};
unsigned char player_1_spawn_point[] = {SCREEN_WIDTH - 1, (SCREEN_HEIGHT >> 1) - 1};
unsigned char ball_spawn_point[] = {2, 14};

GameObject player_0_obj, player_1_obj;
signed char player_0_mov_vec[] = {0, 0};
signed char player_1_mov_vec[] = {0, 0};

GameObject ball_obj;

char ball_movement_vec[] = {1, 0};
unsigned char zeroed_location[] = {0, 0};

unsigned char check_player_ball_collision() {
    unsigned char player_0_collision, player_1_collision;
    player_0_collision = ball_obj.pos[X] == player_0_obj.pos[X] && ball_obj.pos[Y] == player_0_obj.pos[Y];
    player_1_collision = ball_obj.pos[X] == player_1_obj.pos[X] && ball_obj.pos[Y] == player_1_obj.pos[Y];
    return player_0_collision || player_1_collision;
}

unsigned char pad_0, pad_1;
void pad_press_to_mov_vec(unsigned char pad_press, char* out) {
    out[Y] = 0;

    if (PAD_UP & pad_press) {
        out[Y] -= 1;
    }
    
    if (PAD_DOWN & pad_press) {
        out[Y] += 1;
    }
}

int main() {
	ppu_off(); // screen off

	pal_bg((char *)palette); //	load the BG palette
	ppu_on_all(); //	turn on screen

    vram_buf_make(&vram_buf);
    game_object_make(&player_0_obj, player_0_spawn_point, L_WALL_SPRITE_LOC);
    game_object_make(&player_1_obj, player_1_spawn_point, R_WALL_SPRITE_LOC);
    game_object_make(&ball_obj, ball_spawn_point, BALL_SPRITE_LOC);
    game_object_make(&empty_obj, zeroed_location, EMPTY_SPRITE_LOC);

    vram_buf_push(&vram_buf, &player_0_obj);
    vram_buf_push(&vram_buf, &player_1_obj);
    vram_buf_finish(&vram_buf);

    set_vram_update(vram_buf.vram_buf);
    ppu_wait_nmi();

    while(1) {
        pad_0 = pad_poll(0);
        pad_press_to_mov_vec(pad_0, (char*) player_0_mov_vec);
        game_object_move(&player_0_obj, (char*) player_0_mov_vec);
        vram_buf_push(&vram_buf, &empty_obj);
        vram_buf_push(&vram_buf, &player_0_obj);

        pad_1 = pad_poll(1);
        pad_press_to_mov_vec(pad_1, (char*) player_1_mov_vec);
        game_object_move(&player_1_obj, (char*) player_1_mov_vec);
        vram_buf_push(&vram_buf, &empty_obj);
        vram_buf_push(&vram_buf, &player_1_obj);
        
        game_object_move(&ball_obj, ball_movement_vec);
        
        if (check_player_ball_collision()) {
            ball_movement_vec[X] *= -1;
            game_object_move(&ball_obj, ball_movement_vec);
            game_object_move(&ball_obj, ball_movement_vec);
        }

        vram_buf_push(&vram_buf, &empty_obj);
        vram_buf_push(&vram_buf, &ball_obj);
        vram_buf_finish(&vram_buf);

        set_vram_update(vram_buf.vram_buf);
        ppu_wait_nmi();
    }
}

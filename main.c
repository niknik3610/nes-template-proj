#include "neslib/neslib.h"

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
 

//This is required because of some weird compiler/linker stuff
#pragma bss-name (push, "ZEROPAGE")
unsigned char oam_off;
#pragma bss-name (pop)

const unsigned char text[]="Hello World!"; 

//black must be 0x0f, white must be 0x30
const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
};


int main() {
    //If you define this as a global instead, it is much faster
	unsigned char i = 0;

	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
		
	// set a starting point on the screen
	// vram_adr(NTADR_A(x,y));
	vram_adr(NTADR_A(10,14)); // screen is 32 x 30 tiles


	while(text[i]){
		vram_put(text[i]); // this pushes 1 char to the screen
		++i;
	}	
	
	// vram_adr and vram_put only work with screen off
	// NOTE, you could replace everything between i = 0; and here with...
	// vram_write(text,sizeof(text));
	// does the same thing
	
	ppu_on_all(); //	turn on screen
	
	while (1){
		// infinite loop
		// game code can go here later.
		
	}
}

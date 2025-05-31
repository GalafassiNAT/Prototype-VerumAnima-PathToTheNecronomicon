#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>
#include <sprite_eng.h>
#include "globals.h"

#define MAX_TEXT_LINE 200

extern u8 buttons[NUMBER_OF_JOYPADS];
extern u8 buttons_old[NUMBER_OF_JOYPADS];

extern char line[MAX_TEXT_LINE];

////////////////////////////////////////////////////////////////////////////
// LOGIC

// #define wrap(X, L, H) ((X < L)? H : ((X > H)? : L: X))

#define WRAP(X, L, H) \
	if (X < L) \
		X = H; \
	else if (X > H) \
		X = L;

// #define clamp(X, L, H)   (min(max((X), (L)), (H)))

inline Box UTILS_get_box(int x, int y, int w, int h) {
	return (Box){x, x+w, y, y+h};
}

inline void print_bits(u32 value) {
	// HEX ->    12345678
	u32 mask = 0x0000000F;
	char str[33] = {0};
	for (u8 i = 7; i < 100; --i) {
		u16 nibble = (value >> (4*i)) & mask;
		switch (nibble) {
			case 0b0000: strcat(str, "0000"); break; //0
			case 0b0001: strcat(str, "0001"); break; //1
			case 0b0010: strcat(str, "0010"); break; //2
			case 0b0011: strcat(str, "0011"); break; //3
			case 0b0100: strcat(str, "0100"); break; //4
			case 0b0101: strcat(str, "0101"); break; //5
			case 0b0110: strcat(str, "0110"); break; //6
			case 0b0111: strcat(str, "0111"); break; //7
			case 0b1000: strcat(str, "1000"); break; //8
			case 0b1001: strcat(str, "1001"); break; //9
			case 0b1010: strcat(str, "1010"); break; //10
			case 0b1011: strcat(str, "1011"); break; //11
			case 0b1100: strcat(str, "1100"); break; //12
			case 0b1101: strcat(str, "1101"); break; //13
			case 0b1110: strcat(str, "1110"); break; //14
			case 0b1111: strcat(str, "1111"); break; //15
		}
	}
	kprintf("%s", str);
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

static inline void glow_color(u16 color_index, const u16* const color_vector, u8 n) {
	static u8 idx = 0;
	static u8 inc = 1;
	
	PAL_setColor(color_index, color_vector[idx]);
		
	idx += inc;
	if (idx == 0 || idx == n-1) {
		inc = -inc;
	}
}

static inline void text_add_int(u16 num) {
	static char text[6]; // INT_MAX: 65536
	intToStr(num, text, 2);
	if (strlen(text) + strlen(line) + 1 < MAX_TEXT_LINE) {
		strcat(line, text);
		strcat(line, ",");
	}
}

inline void text_print_and_clear() {
	kprintf("%s", line);
	line[0] = 0;
}

inline void rotate_colors(u8 first_index, u8 last_index, s8 direction) {
	u16 last_color = PAL_getColor(first_index);
	for (u8 i = first_index; i != last_index; i += direction) {
		PAL_setColor(i, PAL_getColor(i + direction));
	}
	PAL_setColor(last_index, last_color);
}
//  2 3 4 5 1
//  i        
//  last: 1

inline void rotate_colors_left(u8 left_index, u8 right_index) {
	rotate_colors(left_index, right_index, 1);
}

inline void rotate_colors_right(u8 left_index, u8 right_index) {
	rotate_colors(right_index, left_index, -1);
}

//////////////////////////////////////////////////////////////////////////
// GAMEPAD INPUT

inline void update_input() {
    // update joypad input
    for (int i = NUMBER_OF_JOYPADS-1; i >= 0; --i) {
		buttons_old[i] = buttons[i];
		buttons[i] = JOY_readJoypad(i);
	}
}

inline bool is_bitset(u8 value, u8 bit) {
    return ((value & bit) == bit);
}

inline bool key_down(u8 joy_id, u8 key) {
    return is_bitset(buttons[joy_id], key);
}

inline bool key_pressed(u8 joy_id, u8 key) {
    return is_bitset(buttons[joy_id], key) && !is_bitset(buttons_old[joy_id], key);
}

inline bool key_released(u8 joy_id, u8 key) {
    return !is_bitset(buttons[joy_id], key) && is_bitset(buttons_old[joy_id], key);
}

#endif // _STRUCTS_H_
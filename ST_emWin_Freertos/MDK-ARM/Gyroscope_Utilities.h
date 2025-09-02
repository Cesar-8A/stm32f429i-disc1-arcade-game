#include "spi.h"

extern unsigned char SPI_DATA[6]; //X,Y,Z values
extern unsigned char SPI_ADDRS[3]; //X,Y,Z register Addresses
extern SPI_HandleTypeDef hspi5;

extern int16_t DataX;
extern int16_t DataY;
extern int16_t DataZ;

typedef struct{
	int16_t current_x; //For drawing
	int16_t current_y; //For drawing
	int16_t prev_x; 
	int16_t prev_y;
	uint8_t height;
	uint8_t	width;
	int16_t direction_x;
	int16_t direction_y;
	
	// For keeping current track
	int16_t deltax, deltay, xinc1, xinc2 , 
	yinc1, yinc2, den, num, numadd;	
}fig_info;

void draw_map(void);

void bresenham_line_path(fig_info *figure);
void path_next_position(fig_info *figure);
void move_figure(fig_info *figure, uint16_t fig_number);
uint8_t check_colision(fig_info *figure);
void redraw_figure(fig_info*figure);



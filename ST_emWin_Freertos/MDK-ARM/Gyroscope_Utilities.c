#include "Gyroscope_Utilities.h"
#include <stdlib.h>
#include <math.h>
#include "main.h"

#define PI 3.1416

void bresenham_line_path(fig_info *figure){
	
  figure->deltax = 0;
	figure->deltay = 0; 
	figure->xinc1 = 0; 
	figure->xinc2 = 0; 
  figure->yinc1 = 0; 
	figure->yinc2 = 0;
	figure->den = 0; 
	figure->num=0;
	figure->numadd=0;
	
	figure->deltax = abs(figure->direction_x - figure->current_x);        /* The difference between the x's */
  figure->deltay = abs(figure->direction_y - figure->current_y);        /* The difference between the y's */
  
  if (figure->direction_x >= figure->current_x)                 /* The x-values are increasing */
  {
    figure->xinc1 = 1;
    figure->xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    figure->xinc1 = -1;
    figure->xinc2 = -1;
  }
  
  if (figure->direction_y >= figure->current_y)                 /* The y-values are increasing */
  {
    figure->yinc1 = 1;
    figure->yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    figure->yinc1 = -1;
    figure->yinc2 = -1;
  }
  
  if (figure->deltax >= figure->deltay)         /* There is at least one x-value for every y-value */
  {
    figure->xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    figure->yinc2 = 0;                  /* Don't change the y for every iteration */
    figure->den = figure->deltax;
    figure->num = figure->deltax / 2;
    figure->numadd = figure->deltay;
  }
  else                          /* There is at least one y-value for every x-value */
  {
    figure->xinc2 = 0;                  /* Don't change the x for every iteration */
    figure->yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    figure->den = figure->deltay;
    figure->num = figure->deltay / 2;
    figure->numadd = figure->deltax;
  }	
}

void move_figure(fig_info *figure, uint16_t fig_number){	
	uint16_t i=0;

	while(i<fig_number){	
		// Update position for each figure
		path_next_position(&figure[i]);
		if(check_colision(&figure[i])){
			
			// Prevent collision to loop
			figure[i].current_x = figure[i].prev_x;
			figure[i].current_y = figure[i].prev_y;
			//bresenham_line_path(&figure[i]);
			
		}
		// Check for collision, and if collided, update direction
		//if(fig_number < 50)
		//delay_software(1);
		i++;
	}
}

void path_next_position(fig_info *figure){
		//Redraw figure and erase previous
		//redraw_figure(figure);
		// Update previous position
		figure->prev_x = figure->current_x;
		figure->prev_y = figure->current_y;
	
	  figure->num += figure->numadd;              /* Increase the numerator by the top of the fraction */
    if (figure->num >= figure->den)             /* Check if numerator >= denominator */
    {
      figure->num -= figure->den;               /* Calculate the new numerator value */
      figure->current_x += figure->xinc1;               /* Change the x as appropriate */
      figure->current_y += figure->yinc1;               /* Change the y as appropriate */
    }
    figure->current_x += figure->xinc2;                 /* Change the x as appropriate */
    figure->current_y += figure->yinc2;                 /* Change the y as appropriate */
}

uint8_t check_colision(fig_info *figure){
	figure->direction_x = figure->current_x+400*cos(DataX*PI/180);
	figure->direction_y = figure->current_y+400*sin(DataY*PI/180);	
	return 0;
}

void redraw_figure(fig_info*figure){
	GUI_SetColor(GUI_RED);
	GUI_DrawCircle(figure->current_x, figure->current_y, 9);
}


void draw_map(void){
//Fourth square's row
	GUI_SetColor(GUI_WHITE);
	GUI_FillRect(0, 0, 240, 4);				  //Superior border Line
	GUI_FillRect(0, 316, 240, 320);				//Inferior border Line
	GUI_DrawLine(0, 0, 0, 320);				  //Left border Line
	GUI_DrawLine(239, 0, 239, 320);			//Right border Line
	
	//13th square's row (superior to inferior)
	GUI_DrawLine(0, 292, 24, 292);
	GUI_DrawLine(24, 292, 48, 316);
	
	GUI_DrawLine(72, 292, 96, 292);
	GUI_DrawLine(144, 292, 168, 292);
	
	GUI_DrawLine(216, 292, 240, 292);
	GUI_DrawLine(192, 316, 216, 292);
	
	//12th square's row
	GUI_DrawLine(96, 292, 120, 268);
	GUI_DrawLine(120, 268, 144, 292);
	
	//11th square's row
	GUI_DrawLine(24, 268, 24, 244);
	
	GUI_DrawLine(48, 268, 48, 244);
	GUI_DrawLine(48, 244, 72, 244);
	GUI_DrawLine(72, 268, 72, 244);
	
	GUI_DrawLine(120, 268, 120, 244);
	GUI_DrawLine(96, 244, 144, 244);
	
	GUI_DrawLine(168, 268, 168, 244);
	GUI_DrawLine(168, 244, 192, 244);
	GUI_DrawLine(192, 268, 192, 244);
	
	GUI_DrawLine(216, 268, 216, 244);
	
	//10th square's row
	
	//9th square's row
	GUI_DrawLine(0, 220, 24, 220);
	GUI_DrawLine(48, 220, 72, 220);
	GUI_DrawLine(96, 220, 120, 220);
	GUI_DrawLine(144, 220, 168, 220);
	GUI_DrawLine(192, 220, 216, 220);
	
	//8th square's row
	GUI_DrawLine(24, 196, 24, 172);
	GUI_DrawLine(24, 196, 48, 196);
	GUI_DrawLine(48, 196, 48, 172);
	
	GUI_DrawLine(72, 196, 72, 172);
	GUI_DrawLine(72, 196, 96, 196);
	GUI_DrawLine(96, 196, 96, 172);
	
	GUI_DrawLine(120, 196, 120, 172);
	GUI_DrawLine(120, 196, 144, 196);
	GUI_DrawLine(144, 196, 144, 172);
	
	GUI_DrawLine(168, 196, 168, 172);
	GUI_DrawLine(168, 196, 192, 196);
	GUI_DrawLine(192, 196, 192, 172);
	
	GUI_DrawLine(216, 196, 216, 172);
	GUI_DrawLine(216, 196, 240, 196);
	
	//7th square's row
	
	//6th square's row
	//GUI_DrawLine(24, 148, 24, 124);
	GUI_DrawLine(0, 124, 24, 124);
	GUI_DrawLine(24, 124, 24, 148);
	
	GUI_DrawLine(48, 124, 48, 148);
	GUI_DrawLine(48, 124, 72, 124);
	GUI_DrawLine(72, 124, 72, 148);
	
	GUI_DrawLine(96, 124, 96, 148);
	GUI_DrawLine(96, 124, 120, 124);
	GUI_DrawLine(120, 124, 120, 148);
	
	GUI_DrawLine(144, 124, 144, 148);
	GUI_DrawLine(144, 124, 168, 124);
	GUI_DrawLine(168, 124, 168, 148);
	
	GUI_DrawLine(192, 124, 192, 148);
	GUI_DrawLine(192, 124, 216, 124);
	GUI_DrawLine(216, 124, 216, 148);
	
	//Fifth square's row
	
	//Fourth square's row
	GUI_DrawLine(24, 100, 48, 100);
	GUI_DrawLine(72, 100, 96, 100);
	GUI_DrawLine(120, 100, 144, 100);
	GUI_DrawLine(168, 100, 192, 100);
	GUI_DrawLine(216, 100, 240, 100);
		
	//Third square's row
	GUI_DrawLine(24, 52, 24, 76);
	
	GUI_DrawLine(48, 52, 48, 76);
	GUI_DrawLine(48, 76, 72, 76);
	GUI_DrawLine(72, 52, 72, 76);
	
	GUI_DrawLine(120, 52, 120, 76);
	GUI_DrawLine(96, 76, 144, 76);
	
	GUI_DrawLine(168, 52, 168, 76);
	GUI_DrawLine(168, 76, 192, 76);
	GUI_DrawLine(192, 52, 192, 76);
	
	GUI_DrawLine(216, 52, 216, 76);
	
	//Second square's row
	GUI_DrawLine(96, 28, 120, 52);
	GUI_DrawLine(120, 52, 144, 28);
	
	//First square's row
	GUI_DrawLine(0, 28, 24, 28);
	GUI_DrawLine(24, 28, 48, 4);
	
	GUI_DrawLine(72, 28, 96, 28);
	GUI_DrawLine(144, 28, 168, 28);
	
	GUI_DrawLine(216, 28, 240, 28);
	GUI_DrawLine(192, 4, 216, 28);	
	GUI_SetColor(GUI_BLUE);
	
	GUI_SetColor(GUI_GRAY);
	GUI_FillCircle(120,28,12);
	GUI_SetColor(GUI_WHITE);
	GUI_FillCircle(120,28,8);
}


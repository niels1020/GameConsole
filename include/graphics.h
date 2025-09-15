#pragma once

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 240

//draw a pixel at the given coordinates in the given color
//also has code preventing goin of screen
void GraphicsDrawPixel(uint16_t x, uint16_t y, uint16_t color);
//draw a line inthe given color from (x0, y0) to (x1, y1)
void GraphicsDrawLine(int x0, int y0, int x1, int y1, uint16_t color);
//draw the outline of a rectangle starting top left
void GraphicsDrawRect(int x, int y, int w, int h, uint16_t color);
//draw a filled rect starting top left
void GraphicsFillRect(int x, int y, int w, int h, uint16_t color);
//draaw the outline of a cricle
void GraphicsDrawCircle(int x0, int y0, int radius, uint16_t color);
//draw a filled circle
void GraphicsFillCircle(int x0, int y0, int radius, uint16_t color);
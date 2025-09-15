#include "main.h"

void GraphicsDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }
    st7789_set_cursor(x, y);
    st7789_put(color);
}

void GraphicsDrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        GraphicsDrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void GraphicsDrawRect(int x, int y, int w, int h, uint16_t color) {
    GraphicsDrawLine(x, y, x + w - 1, y, color);           // Top
    GraphicsDrawLine(x, y, x, y + h - 1, color);           // Left
    GraphicsDrawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // Right
    GraphicsDrawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // Bottom
}

void GraphicsFillRect(int x, int y, int w, int h, uint16_t color) {
    for (int i = 0; i < h; ++i) {
        GraphicsDrawLine(x, y + i, x + w - 1, y + i, color);
    }
}

void GraphicsDrawCircle(int x0, int y0, int radius, uint16_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        GraphicsDrawPixel(x0 + x, y0 + y, color);
        GraphicsDrawPixel(x0 + y, y0 + x, color);
        GraphicsDrawPixel(x0 - y, y0 + x, color);
        GraphicsDrawPixel(x0 - x, y0 + y, color);
        GraphicsDrawPixel(x0 - x, y0 - y, color);
        GraphicsDrawPixel(x0 - y, y0 - x, color);
        GraphicsDrawPixel(x0 + y, y0 - x, color);
        GraphicsDrawPixel(x0 + x, y0 - y, color);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}

void GraphicsFillCircle(int x0, int y0, int radius, uint16_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // Draw horizontal lines between symmetric points
        GraphicsDrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        GraphicsDrawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
        GraphicsDrawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
        GraphicsDrawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}
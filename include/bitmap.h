#ifndef _BITMAP_H_
#define _BITMAP_H_

#define TEX_WIDTH 16
#define TEX_HEIGHT 16
#define TEX_CAP 100
#define BM_WIDTH 128
#define BM_HEIGHT 128
#define TEX_COUNT (BM_WIDTH*BM_HEIGHT) / (TEX_WIDTH*TEX_HEIGHT)

static const int INVISIBLE_COLOR = 0xffff00ff;

typedef struct {
    int pixels[TEX_WIDTH*TEX_HEIGHT][TEX_COUNT];
} Bitmap;


Bitmap bitmap_load(char* file_path);

#endif // _BITMAP_H_

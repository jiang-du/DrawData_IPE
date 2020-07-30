#ifndef DRAW_H
#define DRAW_H

#include <math.h>

// 定义欧式距离公式，返回值默认double类型。
#define Eclidian(x, y) sqrt((double)((x) * (x) + (y) * (y)))
#define L1_norm(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))

// 读取二进制数据的函数
int readData(short int *data, FILE *fp);
void StatusBar(int count_frame, int timeuse);

int DataPerpare(FILE *fp);
int getCircle();
int smoothCircle();

short getCurrentFrame(short count_frame, short **_cicle_center, char idx_actor = 0);
void getTextPosition(short count_frame, short *_text_x, short *_text_y, char idx_actor = 0);
void getTextColor(double* color_val, char idx_actor = 0);

#endif

#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <math.h>

// 定义欧式距离公式，返回值默认double类型。
#define Eclidian(x, y) sqrt((double)((x) * (x) + (y) * (y)))
#define L1_norm(x, y) ((x)>(y)?((x)-(y)):((y)-(x)))

// 读取二进制数据的函数
int readData(short int *data, FILE *fp);
void StatusBar(int count_frame);

int readData(short int *data, FILE *fp)
{
    int flag = 0;
    if (flag = fread(data, sizeof(short int), 20, fp) != 0)
    {
        // 读二进制数据成功
    }
    else
    {
        printf("Warning: No more data.\n");
    }

    return flag;
}

void StatusBar(int count_frame)
{
    if (!(count_frame % PRINT_INTERVAL))
    {
        printf("Processing %5.2f\%, %4d/%4d [", (100 * (float)count_frame / TOTAL_NUM_FRAME), count_frame, TOTAL_NUM_FRAME);
        for (int _t = 0; _t++ < (40 * (float)count_frame / TOTAL_NUM_FRAME); printf("="));
        printf(">");
        for (int _t = 0; _t++ < 40 - (40 * (float)count_frame / TOTAL_NUM_FRAME); printf(" "));
        printf("]\n");
    }
}

#endif

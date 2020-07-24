#ifndef DRAW_H
#define DRAW_H

// 是否保存视频文件输出
#define SAVEVIDEO 1 // 设置成0就不保存，默认为1
#define TOTAL_NUM_FRAME 1097
#define DISP_METHOD 3
// 设置文件路径
#define FILENAME "RawData.bin"
#define VIDEONAME "./../datasets/ori.avi"

#include <stdio.h>
#include <math.h>

// 定义欧式距离公式，返回值默认double类型。
#define Eclidian(x, y) sqrt((double)((x) * (x) + (y) * (y)))

// 读取二进制数据的函数
int readData(short int *data, FILE *fp);

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

#endif

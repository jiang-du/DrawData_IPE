#ifndef DRAW_H
#define DRAW_H

// 设置文件路径
#define FILENAME "RawData.bin"
#define VIDEONAME "./../datasets/ori.avi"

#include <stdio.h>

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

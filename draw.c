#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "config.h"
#include "draw.h"

// 框的坐标数据库
static short int database[TOTAL_NUM_FRAME][20];
static short int *data;
// 圆的中心点和半径
static short cicle_center[TOTAL_NUM_FRAME][2];
static short cicle_radius[TOTAL_NUM_FRAME];
// 文本框位置
static short text_x[TOTAL_NUM_FRAME], text_y[TOTAL_NUM_FRAME];

short getCurrentFrame(short count_frame, short **_cicle_center)
{
    *_cicle_center = cicle_center[count_frame];
    return cicle_radius[count_frame];
}

void getTextPosition(short count_frame, short *_text_x, short *_text_y)
{
    *_text_x = text_x[count_frame];
    *_text_y = text_y[count_frame];
}

int DataPerpare(FILE *fp)
{
    // 计时器专用变量
    struct timeval start, end;
    // 计时器开始
    gettimeofday(&start, NULL);
    // 预读取所有数据
    for (int i = 0; i < TOTAL_NUM_FRAME;)
        readData(database[i++], fp);
    // 计时器暂停
    gettimeofday(&end, NULL);
    int timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Data read at %d us.\n", timeuse);
    return 0;
}

int getCircle()
// 求解圆的中心点和半径
{
    // 计时器专用变量
    struct timeval start, end;
    // 计时器开始
    gettimeofday(&start, NULL);
    for (int i = 0; i < TOTAL_NUM_FRAME;)
    {
        // 取第几个演员的坐标
        short *pdata = database[i] + 4 * (ACTOR_NUM - 1);
        // 获取框的中心点；使用移位运算极限加速
        cicle_center[i][1] = (*pdata + *(pdata + 2)) >> 1;       // 水平方向
        cicle_center[i][0] = (*(pdata + 1) + *(pdata + 3)) >> 1; // 竖直方向
        // 获取圆的半径，并量化为整数
        cicle_radius[i++] = (short)Eclidian(*(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1)) >> 1;
    }
    // 计时器暂停
    gettimeofday(&end, NULL);
    int timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Calculate radius at %d us.\n", timeuse);
    return 0;
}

int smoothCircle()
{
    // 计时器专用变量
    struct timeval start, end;
    // 计时器开始
    gettimeofday(&start, NULL);
    for (short i = 0; ++i < TOTAL_NUM_FRAME;)
    {
        // 对半径的容差，注意变大和变小是分别处理的
        short _diff = cicle_radius[i] - cicle_radius[i - 1];
        // 半径最多增加多少
        short difference = cicle_radius[i - 1] >> 4; // 最多允许比上一帧增加1/16
        if (_diff > difference)
            cicle_radius[i] = cicle_radius[i - 1] + difference;
        else
        {
            // 半径最多减少多少(注意要用负的)
            difference = -(cicle_radius[i - 1] >> 5); // 最多允许比上一帧减少1/32
            if (_diff < difference)
                cicle_radius[i] = cicle_radius[i - 1] + difference;
        }
        // 对圆心位置的容差
        // 竖直方向平移量
        difference = 3;
        if (L1_norm(cicle_center[i][0], cicle_center[i - 1][0]) > difference)
            cicle_center[i][0] = cicle_center[i - 1][0] + ((cicle_center[i][0] > cicle_center[i - 1][0]) ? difference : (0 - difference));
        // 水平方向平移量，因为体操运动员运动速度快所以应该设置大一点
        difference = 20;
        if (L1_norm(cicle_center[i][1], cicle_center[i - 1][1]) > difference)
            cicle_center[i][1] = cicle_center[i - 1][1] + ((cicle_center[i][1] > cicle_center[i - 1][1]) ? difference : (0 - difference));

        // 提前获取运动员编号的文字显示位置初始值
        text_x[i] = cicle_center[i][1]; // 水平方向
        // 竖直方向稍微比圆的顶部向下一点点
        text_y[i] = cicle_center[i][0] - cicle_radius[i] + (cicle_radius[i] >> 2);
    }
    // 对文字位置的平滑操作，设置关键帧的距离smooth_d
    short smooth_d = 15;
    // 水平方向
    for (short i = 1; ++i < (TOTAL_NUM_FRAME / smooth_d);)
    {
        // 抛物线的方程(latex公式)：$y=ax^2+(\frac{y_R-y_L}{d}-da)x+y_L$
        float gradient = (float)(text_x[smooth_d * i] - text_x[smooth_d * i - smooth_d]) / smooth_d;
        for (register short j = -smooth_d; ++j < 0;)
            text_x[smooth_d * i + j] = text_x[smooth_d * i] + (short int)(j * gradient);
    }
    // 竖直方向
    smooth_d = 30;
    for (short i = 1; ++i < (TOTAL_NUM_FRAME / smooth_d);)
    {
        float gradient = (float)(text_y[smooth_d * i] - text_y[smooth_d * i - smooth_d]) / smooth_d;
        for (register short j = -smooth_d; ++j < 0;)
            text_y[smooth_d * i + j] = text_y[smooth_d * i] + (short int)(j * gradient);
    }
    // 计时器暂停
    gettimeofday(&end, NULL);
    int timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("Smooth at %d us.\n", timeuse);
    return 0;
}

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
        // 没有数据的话，用前一帧的数据来填充
        memcpy(data, data - 20, 20 * sizeof(short int));
    }

    return flag;
}

void StatusBar(int count_frame, int timeuse)
{
    if (!(count_frame % PRINT_INTERVAL))
    {
        printf("Processing %5.2f\%, %4d/%4d at %dfps[",
               (100 * (float)count_frame / TOTAL_NUM_FRAME), count_frame, TOTAL_NUM_FRAME, 1000000 * count_frame / timeuse);
        for (int _t = 0; _t++ < (40 * (float)count_frame / TOTAL_NUM_FRAME);)
            printf("=");
        printf(">");
        for (int _t = 0; _t++ < 39 - (40 * (float)count_frame / TOTAL_NUM_FRAME);)
            printf(" ");
        printf("]\n");
    }
}

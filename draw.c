#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "config.h"
#include "draw.h"

// 框的坐标数据库
static short int database[TOTAL_NUM_FRAME][TOTAL_PERSON * 4];
static short int *data;
// 圆的中心点和半径
static short all_cicle_center[TOTAL_PERSON][TOTAL_NUM_FRAME][2];
static short all_cicle_radius[TOTAL_PERSON][TOTAL_NUM_FRAME];
// 文本框位置
static short text_x[TOTAL_PERSON][TOTAL_NUM_FRAME], text_y[TOTAL_PERSON][TOTAL_NUM_FRAME];

// 每个人身上的编号文字颜色
static double color_bank[TOTAL_PERSON][3] = {
    {140, 240, 0},
    {80, 80, 255},
    {190, 50, 50},
    {255, 180, 80},
    {0, 180, 255},
};

short getCurrentFrame(short count_frame, short **_cicle_center, char idx_actor)
{
    *_cicle_center = all_cicle_center[idx_actor][count_frame];
    return all_cicle_radius[idx_actor][count_frame];
}

void getTextPosition(short count_frame, short *_text_x, short *_text_y, char idx_actor)
{
    *_text_x = text_x[idx_actor][count_frame];
    *_text_y = text_y[idx_actor][count_frame];
}

void getTextColor(double *color_val, char idx_actor)
{
    // 对文字颜色进行赋值
    memcpy(color_val, color_bank[idx_actor], 3 * sizeof(double));
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
    // 取第几个演员的坐标
    for (char idx_actor = -1; ++idx_actor < TOTAL_PERSON;)
    {
        short *cicle_radius = all_cicle_radius[idx_actor];
        for (int i = 0; i < TOTAL_NUM_FRAME;)
        {
            short *cicle_center = all_cicle_center[idx_actor][i];
            short *pdata = database[i] + 4 * idx_actor;
            // 获取框的中心点；使用移位运算极限加速
            cicle_center[1] = (*pdata + *(pdata + 2)) >> 1;       // 水平方向
            cicle_center[0] = (*(pdata + 1) + *(pdata + 3)) >> 1; // 竖直方向
            // 获取圆的半径，并量化为整数
            cicle_radius[i++] = (short)Eclidian(*(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1)) >> 1;
        }
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
    // 对第几个演员进行平滑
    for (char idx_actor = -1; ++idx_actor < TOTAL_PERSON;)
    {
        // 对圆形区域的平滑操作
        short *cicle_radius = all_cicle_radius[idx_actor];
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
            short *cicle_center = all_cicle_center[idx_actor][i];
            // 竖直方向平移量
            difference = 3;
            if (L1_norm(cicle_center[0], cicle_center[-2]) > difference)
                cicle_center[0] = cicle_center[-2] + ((cicle_center[0] > cicle_center[-2]) ? difference : (0 - difference));
            // 水平方向平移量，因为体操运动员运动速度快所以应该设置大一点
            difference = 20;
            if (L1_norm(cicle_center[1], cicle_center[-1]) > difference)
                cicle_center[1] = cicle_center[-1] + ((cicle_center[1] > cicle_center[-1]) ? difference : (0 - difference));

            // 提前获取运动员编号的文字显示位置初始值
            text_x[idx_actor][i] = cicle_center[1]; // 水平方向
            // 竖直方向稍微比圆的顶部向下一点点
            // text_y[idx_actor][i] = cicle_center[0] - cicle_radius[i] + (cicle_radius[i] >> 2);
            // 竖直方向的位置放在人的胸前
            text_y[idx_actor][i] = cicle_center[0] - (cicle_radius[i] >> 3);
        }

        // 对文字位置的平滑操作，设置关键帧的距离smooth_d
        short *x = text_x[idx_actor];
        // 水平方向
        short smooth_d = 18;
        for (short i = 1; ++i < (TOTAL_NUM_FRAME / smooth_d);)
        {
            // 抛物线的方程(latex公式)：$y=ax^2+(\frac{y_R-y_L}{d}-da)x+y_L$
            float gradient = (float)(x[smooth_d * i] - x[smooth_d * i - smooth_d]) / smooth_d;
            for (register short j = -smooth_d; ++j < 0;)
                x[smooth_d * i + j] = x[smooth_d * i] + (short int)(j * gradient);
        }
        // 竖直方向
        short *y = text_y[idx_actor];
        smooth_d = 30;
        for (short i = 1; ++i < (TOTAL_NUM_FRAME / smooth_d);)
        {
            float gradient = (float)(y[smooth_d * i] - y[smooth_d * i - smooth_d]) / smooth_d;
            for (register short j = -smooth_d; ++j < 0;)
                y[smooth_d * i + j] = y[smooth_d * i] + (short int)(j * gradient);
        }
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

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include "config.h"
#include "draw.h"

int main()
{
    const char *filename = FILENAME;
    const char *videoname = VIDEONAME;
    FILE *fp = fopen(filename, "rb");
    short int data[20];
    cv::Mat image;
    cv::VideoCapture capture;
    capture.open(videoname);
#if SAVE_VIDEO
    double fps = 25;
    cv::Size videoSize(1920, 1080);
    cv::VideoWriter writer("output.mp4", cv::VideoWriter::fourcc(FOUR_CC), fps, videoSize);
#endif
    if (capture.isOpened())
    {
        int count_frame = 0;
        while (1)
        {
            capture >> image;
            readData(data, fp);
            // 读到的坐标是(x1,y1,x2,y2)格式
            if (image.empty())
            {
                fclose(fp);
                cv::waitKey(0);
                break;
            }
            for (int i = 0; i < 5; i++)
            {
                short *pdata = data + i * 4;

#if (DISP_METHOD == 1)
                // Method 1: 直接显示框
                // 定义矩形框
                cv::Rect rect(*pdata, *(pdata + 1), *(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1));
                // 绘制框
                v::rectangle(image, rect, cv::Scalar(64 + i * 48, 128 + i * 32, 192 - i * 48, (double)0.5), 5, 8, 0);
                // image.at<u_char>(0,0,0) = 1;
#else
#if (DISP_METHOD == 2)
                // Method 2: 画圆
                // 获取框的中心点
                short cx = (*pdata + *(pdata + 2)) / 2;
                short cy = (*(pdata + 1) + *(pdata + 3)) / 2;
                // 获取圆的半径
                double radius = 0.5 * Eclidian(*(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1));
                cv::circle(image, cv::Point(cx, cy), radius, cv::Scalar(0, 255, 0), 3);
#else
#if (DISP_METHOD == 3)

                // Method 3: Attention
                if ((i - 3) != 0)
                    // 选择需要关注的person ID
                    continue;
                // 获取框的中心点
                short cx = (*pdata + *(pdata + 2)) / 2;
                short cy = (*(pdata + 1) + *(pdata + 3)) / 2;
                // 获取圆的半径
                double radius = 0.5 * Eclidian(*(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1));
                // 对图像进行逐像素处理
                if (image.isContinuous())
                {
                    // Mat在内存空间中连续，可用快速算法
                    // 其实不连续也可以用copy()函数，但是这样做没啥实际意义

                    // 将半径转成整数，加速计算
                    int _radius = (int)radius;

                    // int numberOfPixels = image.rows * image.cols * image.channels();
                    uchar *img_ptr = reinterpret_cast<uchar *>(image.data);
                    // 使用register变量，极限加速
                    for (register int xi = 0; xi < image.rows; xi++)
                    {
                        if (L1_norm(xi, cy) < _radius)
                        {
                            for (register int yi = 0; yi < image.cols; yi++)
                            {
                                // 判断是否在圆的外切矩形框内
                                if (L1_norm(yi, cx) < _radius)
                                {
                                    // 设置倍率
                                    float dist = Eclidian(xi - cy, yi - cx);
                                    float rate = MIN(3.5 - 3 * MIN(dist / radius, 1), 1);
                                    // 对3个channel写入内存地址
                                    for (register uchar _c = 0; _c++ < 3;)
                                        // 赋值运算右边执行之后左边的++返回的是加之前的结果
                                        *(img_ptr++) = (uchar)(rate * (float)(*img_ptr));
                                }
                                else
                                {
                                    for (register uchar _c = 0; _c++ < 3;)
                                        // 使用移位运算直接二进制操作
                                        *(img_ptr++) = (*(img_ptr)) >> 1;
                                }
                            }
                        }
                        else
                            // 剩下的都是背景
                            for (register int yi = 0; yi++ < image.cols;)
                                for (register uchar _c = 0; _c++ < 3;)
                                    *(img_ptr++) = (*(img_ptr)) >> 1;
                    }
                }
                else
                {
                    printf("Warning: You are using a slow algorithm.\n");
                    // 传统方法：遍历所有像素，并设置像素值
                    for (int xi = 0; xi < image.rows;)
                    {
                        //获取第i行首像素指针
                        cv::Vec3b *p = image.ptr<cv::Vec3b>(xi++);
                        for (int yi = 0; yi < image.cols; ++yi)
                        {
                            // 设置倍率
                            float dist = Eclidian(xi - cy, yi - cx);
                            float rate = MIN(3.5 - 3 * MIN(dist / radius, 1), 1);
                            // 对内存地址的数据直接进行修改
                            for (int _channel = 0; _channel < 3; ++_channel)
                                // BGR
                                p[yi][_channel] = (unsigned char)(rate * (float)p[yi][_channel]);
                        }
                    }
                }
                break;
#endif
#endif
#endif
            }
#if (!SAVE_VIDEO) or DISPLAY_VIDEO
// 只有在不保存视频的情况下才判断是否显示视频，否则强制显示
            imshow(WINDOW_TITLE, image);
#endif
#if SAVE_VIDEO
            writer << image;
#endif
#if (!SAVE_VIDEO) or DISPLAY_VIDEO
            if (cv::waitKey(1) >= 0)
            {
                fclose(fp);
                break;
            }
#endif
            // 打印状态信息
            StatusBar(count_frame++);
        }
    }
    return 0;
}
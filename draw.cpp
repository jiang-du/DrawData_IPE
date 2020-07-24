#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
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
#if SAVEVIDEO
    double fps = 25;
    cv::Size videoSize(1920, 1080);
    cv::VideoWriter writer("output.mp4", cv::VideoWriter::fourcc('X', '2', '6', '4'), fps, videoSize);
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
                //遍历所有像素，并设置像素值
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
                break;
#endif
#endif
#endif
            }
            imshow("Intelligent Physical Education System", image);
#if SAVEVIDEO
            writer << image;
#endif
            if (cv::waitKey(1) >= 0)
            {
                fclose(fp);
                break;
            }
            count_frame++;
            
            if (!(count_frame % 5))
            {
                printf("Processing %5.2f\%, %4d / %4d [", (100 * (float)count_frame / TOTAL_NUM_FRAME), count_frame, TOTAL_NUM_FRAME);
                for (int _t = 0; _t++ < (40 * (float)count_frame / TOTAL_NUM_FRAME); printf("="));
                printf(">");
                for (int _t = 0; _t++ < 40 - (40 * (float)count_frame / TOTAL_NUM_FRAME); printf(" "));
                printf("]\n");
            }
        }
    }
    return 0;
}
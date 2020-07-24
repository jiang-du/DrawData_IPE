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
    if (capture.isOpened())
    {
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
            for (int i = 0; i < 5;)
            {
                short *pdata = data + (i++) * 4;
                // 定义矩形框
                cv::Rect rect(*pdata, *(pdata + 1), *(pdata + 2) - *pdata, *(pdata + 3) - *(pdata + 1));
                // 绘制框
                cv::rectangle(image, rect, cv::Scalar(128, 255, 128, (double)0.5), 5, 8, 0);
            }
            imshow("Intelligent Physical Education System", image);
            if (cv::waitKey(1) >= 0)
            {
                fclose(fp);
                break;
            }
        }
    }
    return 0;
}
# 智慧体育项目画图专用程序

## 运行环境

- Ubuntu 20.04

- OpenCV 4.4

如果不会配置环境，请自己去谷歌搜索教程，也可以参考[相关博客(点击传送门直达)](https://www.jianshu.com/p/26dd452a362e)。

## 使用说明

1. 提前准备好`poseAdd2Id.json`放在该目录下。

2. 第一次运行时，需要先执行数据格式转换

    ```sh
    python3 convert_data.py
    ```

    执行之后会生成一个文件：

    > RawData.bin

3. 编辑相关参数

    ```sh
    vim draw.h
    ```

    根据自己的视频文件路径修改代码中的：

    ```c
    #define VIDEONAME "./../datasets/ori.avi"
    ```

4. 编译运行

    ```sh
    ./make.sh
    ```

# 智慧体育项目视频处理专用程序

## 运行环境

- Ubuntu 20.04

- OpenCV 4.4

首先需要从源码编译并配置opencv，如果不会配置环境，请自己去谷歌搜索教程，也可以参考[相关博客(点击传送门直达)](https://www.jianshu.com/p/26dd452a362e)，如果opencv源码压缩包下载太慢，请使用[码云镜像下载](https://gitee.com/jiangdu/opencv/repository/archive/4.4.0?format=tar.gz)。

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
    vim config.h
    ```

    根据自己的视频文件路径修改代码中的：

    ```c
    #define VIDEONAME "./../datasets/ori.avi"
    
    #define ACTOR_NUM 1 // 演员编号：范围1-5
    ```

4. 编译运行

    ```sh
    ./make.sh
    ```

## 功能介绍

2020.7.29更新：

1. 优化了代码结构，对数据的处理和opencv画图部分分别放在两个不同的函数文件中，数据采用了全局static变量以便共享。

2. 演员编号支持自由选择，方便生成分别关注5个人中每个人的独立视频。

2020.7.28更新：

基于抛物线的平滑原理：

设左关键帧的数值为$y_L$，右关键帧的数值为$y_R$，关键帧间隔为$d$，可以推导出抛物线的方程为：

![](data:image/png;base64,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)

其中a为待定系数，d在本程序中水平方向取$d=15$，竖直方向取$d=30$。

然后以左右关键帧为基准点，矫正中间每一帧的数值，实现平滑效果。

2020.7.28更新：

1. 对关注人的区域进行了平滑，采用了自适应非对称的阻尼门限方法。

2. 体操运动员头顶上的数字的位置设置更人性化。

3. 对计时器的运行机制进行了优化，输出平均帧率。由于H.264编码时间被统计进处理时间，fps显示会减少，但是实际上程序效率是提高的。

4. 左上角可以自由设置是否显示frame编号。

5. 写入视频机制优化，fps和视频尺寸也可以自适应了。

6. 视频处理结束后可调用ffmpeg以HEVC (`libx265`)进行二次编码压缩，进一步减少视频尺寸，如果安装了Nvidia显卡，可以用`hevc_nvenc`做编码器。

2020.7.27更新：

1. 改进底层操作方式，运行更流畅。

2. 代码结构优化，配置参数统一放在`config.h`文件中。

3. 支持自由选择是否保存视频，并支持设定格式(默认`H.264`)。

2020.7.24更新：

1. 已完成对单人的光照的简便渲染。

2. 对视频文件进行处理时，添加了滚动条动态显示进度的效果，更加人性化。

3. 目前提供了3种可视化方式：直接显示框、直接画框的外接圆、对关注的人的光照进行渐变渲染。默认使用方法3。

TODO:

- [ ] 基础功能
    - [x] 绘制人的框和圆形区域
    - [x] 对person的区域实现渐变的透明度
    - [X] 对人的ID编号进行标注
    - [X] 对数据进行平滑处理，解决跳动问题
    - [X] 演员编号支持自由选择，支持生成每个人的独立高亮视频
    - [ ] 对球的轨迹可视化
        - [ ] 实现动态透明度渐变(淡入淡出效果)
        - [ ] 优化视觉效果，自适应接球动作时轨迹的长度
- [ ] 性能优化
    - [x] 基于OpenCV和C语言的单线程CPU版
    - [x] 支持是否导出视频以及格式选择(H.264编码默认使用CPU多线程)
    - [ ] 调用C语言和python的接口，实现直接读json文件
    - [ ] 使用OpenCL或CUDA对视频处理进行速度优化

# LICENSE

[MIT License](./LICENSE)

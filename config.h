#ifndef CONFIG_H
#define CONFIG_H

// 是否保存视频文件输出
#define SAVE_VIDEO 1 // 设置成0就不保存，默认为1
#if SAVE_VIDEO
// 只有在保存视频的情况下才允许设置是否显示视频
#define DISPLAY_VIDEO 1 // 设置成0就不显示，默认为1
#endif
#define TOTAL_NUM_FRAME 1097
#define DISP_METHOD 3
// 是否显示左上角的frame编号
#define FRAME_NUM_DISP 1
// 设置多少个frame打印一次
#define PRINT_INTERVAL 30
// 编码格式选择X264更有利于视频质量好，但是速度慢一些
#define FOUR_CC 'X', '2', '6', '4'
//#define FOUR_CC 'P', 'I', 'M', '1'
#define WINDOW_TITLE "Intelligent Physical Education System"
// 设置文件路径
#define FILENAME "RawData.bin"
#define VIDEONAME "./../datasets/ori.avi"

#endif
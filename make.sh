g++ main.cpp draw.c -o main `pkg-config --cflags --libs opencv4`
./main
# 视频的二次封装压缩
rm final.mp4
# 使用GPU进行编码H.264
ffmpeg -i output.mp4 -i out.m4a -shortest -vcodec h264_nvenc -b:v 3600k final.mp4
rm output.mp4
ffplay final.mp4

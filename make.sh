g++ draw.cpp -o draw `pkg-config --cflags --libs opencv4`
./draw
# 视频的二次封装压缩
ffmpeg -i output.mp4 -vcodec libx265 -b:v 2000k final.mp4
rm output.mp4
ffplay final.mp4

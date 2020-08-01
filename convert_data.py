#!~/miniconda3/env/tf2/bin/python
import json
import numpy as np
import struct
import os

# 参数设置
num_frame = 1097
num_person = 5
person_lib = np.zeros([num_person, 4])
data_written = 0

# ---------- 对框的坐标点的处理 ----------
json_file = "./poseAdd2Id.json"
binary_file = "RawData.bin"
# 判断是否存在文件
if os.path.isfile(json_file):
    # 开始转数据格式
    with open(json_file, "r") as f:
        personDict = json.load(f)
else:
    # 没有json文件，加载个锤子啊
    raise Exception("No json file. Would you like to load with your hammer?")

print("Converting person data file ...")
# 写入二进制文件
with open(binary_file, "wb") as fp:
    for i in range(num_frame):
        personInfo = personDict[str(i)]
        for PersonIndex, PersonData in personInfo.items():
            # 不看pose点，只看框的位置(x1,y1,x2,y2)
            box = PersonData[0:4]
            # 把框的信息更新到数组
            person_lib[ord(PersonIndex) - ord("1"), :] = box
        # 把坐标点四舍五入，然后转换成int
        # buffer = np.round(person_lib).astype(int)
        # 不要用astype函数，因为会把int默认成<class 'numpy.int64'>，这点真的好烦呀，嘤～
        for n in range(num_person):
            for d in range(4):
                # 必须是int变量才允许写入
                data_int = int(round(person_lib[n, d]))
                if data_int < 0:
                    data_int = int(0)
                assert type(data_int)==int
                # 坐标范围不超过32767，因此封装成二进制格式只要2字节
                raw_int = struct.pack("h", np.int16(data_int))
                # 写入文件缓冲区
                fp.write(raw_int)
                # 统计写入的数据
                data_written += 1
print("Finish. %d data written." % data_written)

# ---------- 对小球轨迹的处理 ----------
json_file = "./ball_personid.json"
binary_file = "Ball.bin"
# 判断是否存在文件
if os.path.isfile(json_file):
    # 开始转数据格式
    with open(json_file, "r") as f:
        ballDict = json.load(f)
else:
    # 没有json文件，加载个锤子啊
    raise Exception("No json file. Would you like to load with your hammer?")

print("Converting ball tracking file ...")
# 格式转换
# 我的思路是开一个list()，5个成员对应5个person，每个人分别是list()对应track ID，每个track ID是个list()，存放frame num, x, y
ballDatabase = [list() for _ in range(5)]
for i in range(num_frame):
    ballInfo = ballDict[str(i)]
    for BallIndex, BallData in ballInfo.items():
        ballID = ord(BallIndex[0]) - 49 # 把小球ID(其实是运动员ID)换算成0-4
        trackID = ord(BallIndex[1]) - 49
        # 判断trackID是否存在
        if trackID >= len(ballDatabase[ballID]):
            # 不存在，新建track
            ballDatabase[ballID].append(list())
        # 这时候存在track ID，直接追加frame
        x = int((BallData[0] + BallData[2]) // 2)
        y = int((BallData[1] + BallData[3]) // 2)
        # 格式：frame num, x, y
        ballDatabase[ballID][trackID].append([i, x, y])

print("Writing ball tracking data ...")
# 写入二进制文件
data_written = 0
# 文件格式：
# 前5个数值，存放每个人的track ID数量，int16类型，共占用10 bytes
# 后面接下来是每个track ID的总帧数，int16类型
# 然后是具体的数据区，每个meta data依次以[frame num, x, y]的顺序存储为int16，占用6 bytes
with open(binary_file, "wb") as fp:
    # ---------- 1. 写入每个人的track ID数量 ----------
    for idx_personID in range(5):
        # 获取每人的track ID数量
        raw_int = struct.pack("h", np.int16(len(ballDatabase[idx_personID])))
        # 写入二进制数据
        fp.write(raw_int)
        data_written += 1
    # ---------- 2. 写入每个track ID的总帧数 ----------
    for idx_personID in range(5):
        for idx_trackID in range(len(ballDatabase[idx_personID])):
            # 获取每个track ID的帧数
            raw_int = struct.pack("h", np.int16(len(ballDatabase[idx_personID][idx_trackID])))
            # 写入二进制数据
            fp.write(raw_int)
            data_written += 1
    # ---------- 3. 写入每个frame meta data的[frame num, x, y]信息 ----------
    for idx_personID in range(5):
        for idx_trackID in range(len(ballDatabase[idx_personID])):
            for idx_frameID in range(len(ballDatabase[idx_personID][idx_trackID])):
                # 获取每个frame的meta data
                for i in range(3):
                    # 分别写入[frame num, x, y]
                    raw_int = struct.pack("h", np.int16(ballDatabase[idx_personID][idx_trackID][idx_frameID][i]))
                    # 写入二进制数据
                    fp.write(raw_int)
                    data_written += 1
print("Finish. %d data written." % data_written)

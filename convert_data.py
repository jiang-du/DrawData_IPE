#!~/miniconda3/env/tf2/bin/python
import json
import numpy as np
import struct
import os

# 参数设置
json_file = "./poseAdd2Id.json"
binary_file = "RawData.bin"
num_frame = 1097
num_person = 5
person_lib = np.zeros([num_person, 4])
data_written = 0

# 判断是否存在文件
if os.path.isfile(json_file):
    # 开始转数据格式
    with open(json_file, "r") as f:
        personDict = json.load(f)
else:
    # 没有json文件，加载个锤子啊
    raise Exception("No json file. Would you like to load with your hammer?")

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

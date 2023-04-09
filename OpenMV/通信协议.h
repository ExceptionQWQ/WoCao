
上位机发送一字节指令给摄像头
cmd = 0x00, 停止工作
cmd = 0x01, 检测二维码，反馈检测结果字符串
cmd = 0x02, 检测左黑线，反馈角度和截距
cmd = 0x03, 检测右黑线，反馈角度和截距
cmd = 0x04, 朝下摄像头检测圆（无论什么颜色），反馈坐标
cmd = 0x05, 检测红色圆环，反馈坐标
cmd = 0x06, 检测绿色圆环，反馈坐标
cmd = 0x07, 检测蓝色圆环，反馈坐标
cmd = 0x08, 检测红色色块，反馈坐标
cmd = 0x09, 检测绿色色块，反馈坐标
cmd = 0x0a, 检测蓝色色块，反馈坐标
cmd = 0x0b, 检测圆环颜色顺序


摄像头上报数据包格式：[0x12, 0x34, data_len, cmd, data...]

//存储str
cmd = 0x01;
data_len = strlen(str);
memcpy(data, str, data_len);

//存储pair
cmd = 0x02;
data_len = 4;
data[0] = (dx << 8) & 0xff;
data[1] = dx & 0xff;
data[2] = (dy << 8) & 0xff;
data[3] = dy & 0xff;

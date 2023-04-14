import sensor, image, time, math
import machine
from pyb import UART,LED

uart = UART(1,115200)
uart.init(115200, bits=8, parity=None, stop=1) # init with given parameters

cmdStatus = 0x00 #Stop

#色环阈值
redThresh = (0, 100, 19, 127, -18, 127)
greenThresh = (0, 100, -128, -20, -128, 127)
blueThresh = (0, 100, -128, 127, -128, -23)
targetThresh = (93, 100, -30, -15, 89, 104)

def SendStr(str):
    byteStr = bytearray(str)
    byteData = bytearray([0x12, 0x34, len(byteStr), 1]) + byteStr
    uart.write(byteData)

def SendPair(dx, dy):
    byteData = bytearray([0x12, 0x34, 4, 2, (dx >> 8) & 0xff, dx & 0xff, (dy >> 8) & 0xff, dy & 0xff])
    uart.write(byteData)

def UartReadCmd():
    len = uart.any()
    if len == 0:
        return None
    cmd = 0x00
    while len > 0:
        cmd = uart.readchar()
        len -= 1
    print("cmd:", cmd)
    return cmd

def ResetSensorForDetectLeftLine():
    sensor.reset()
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)
    sensor.set_windowing((0, 40, 200, 120))
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    #sensor.skip_frames(time = 500)

def DetectLeftLine():
    img = sensor.snapshot()
    m = 0
    aiml = None
    for l in img.find_lines(threshold = 1000, theta_margin = 45, rho_margin = 50):
        if (l.magnitude()>m) and ((15 > l.theta()) or (165 < l.theta())):
            m=l.magnitude()
            aiml=l
    if aiml:
        img.draw_line(aiml.line(), color = (255, 0, 0))
        print(aiml.theta(), aiml.rho())
        return [aiml.theta(), aiml.rho()]
    return None

def ResetSensorForDetectRightLine():
    sensor.reset()
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)
    sensor.set_windowing((0, 40, 250, 160))
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    #sensor.skip_frames(time = 500)

def DetectRightLine():
    img = sensor.snapshot()
    m = 0
    aiml = None
    for l in img.find_lines(threshold = 1000, theta_margin = 45, rho_margin = 50):
        if (l.magnitude()>m) and ((15 > l.theta()) or (165 < l.theta())):
            m=l.magnitude()
            aiml=l
    if aiml:
        img.draw_line(aiml.line(), color = (255, 0, 0))
        print(aiml.theta(), aiml.rho())
        return [aiml.theta(), aiml.rho()]
    return None


def ResetSensorForDetectColorCircle():
    sensor.reset()
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.VGA)
    sensor.set_windowing((180, 0, 300, 480))
    #sensor.skip_frames(time = 500)

def DetectColorCircle(thresh):
    img = sensor.snapshot()
    cnt = 0
    cx = 0
    cy = 0
    maxArea = 0
    for blob in img.find_blobs([thresh], invert = False, x_stride = 2, y_stride = 2, area_threshold = 2, pixels_threshold = 2, merge = True, margin = 50):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        cnt += 1
        if maxArea < blob.area():
            maxArea = blob.area()
            cx = blob.cx()
            cy = blob.cy()
    if cnt == 0:
        return [0, 0]

    img.flood_fill(int(cx) + 15, int(cy) + 15, seed_threshold = 0.3, floating_threshold = 0.05, color = (255, 255, 0), invert = False, clear_background = False)
    img.flood_fill(int(cx) + 15, int(cy) - 15, seed_threshold = 0.3, floating_threshold = 0.05, color = (255, 255, 0), invert = False, clear_background = False)
    img.flood_fill(int(cx) - 15, int(cy) + 15, seed_threshold = 0.3, floating_threshold = 0.05, color = (255, 255, 0), invert = False, clear_background = False)
    img.flood_fill(int(cx) - 15, int(cy) - 15, seed_threshold = 0.3, floating_threshold = 0.05, color = (255, 255, 0), invert = False, clear_background = False)

    img.draw_cross(int(cx) + 15, int(cy) + 15, color = (0, 255, 255))
    img.draw_cross(int(cx) + 15, int(cy) - 15, color = (0, 255, 255))
    img.draw_cross(int(cx) - 15, int(cy) + 15, color = (0, 255, 255))
    img.draw_cross(int(cx) - 15, int(cy) - 15, color = (0, 255, 255))

    cx = 0
    cy = 0
    cnt = 0
    for blob in img.find_blobs([targetThresh], invert = False, x_stride = 2, y_stride = 2, area_threshold = 2, pixels_threshold = 2, merge = True, margin = 50):
        img.draw_rectangle(blob.rect(), color = (255, 0, 0))
        cx += blob.cx()
        cy += blob.cy()
        cnt += 1
    if cnt != 1:
        return [0, 0]
    cx /= cnt
    cy /= cnt
    print(cx, cy)
    return [int(cx), int(cy)]

def GetColorCircleSeq():
    img = sensor.snapshot()
    cnt = 0
    colorHSeq = []
    colorMap = {}
    codeSum = 0
    for blob in img.find_blobs([redThresh, greenThresh, blueThresh], invert = False, x_stride = 2, y_stride = 2, area_threshold = 2, pixels_threshold = 2, merge = True, margin = 50):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        colorHSeq.append(blob.cy())
        colorMap[blob.cy()] = blob.code()
        codeSum += blob.code()
        cnt += 1
    if cnt != 3 or codeSum != 7:
        return [0, 0]
    colorHSeq.sort(reverse = True)
    ret = 100 * colorMap[colorHSeq[0]] + 10 * colorMap[colorHSeq[1]] + 1 * colorMap[colorHSeq[2]]
    print("colorSeq:" + str(ret))
    return [int(ret), 0]

def ResetSensorForDetectAllCircle():
    sensor.reset()
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA)
    sensor.set_windowing((40, 0, 120, 120))
    #sensor.skip_frames(time = 500)

def DetectAllCircle():
    img = sensor.snapshot()
    cnt = 0
    cx = 0
    cy = 0
    maxArea = 0
    for blob in img.find_blobs([redThresh, greenThresh, blueThresh], invert = False, x_stride = 2, y_stride = 2, area_threshold = 2, pixels_threshold = 2, merge = True, margin = 50):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        if maxArea < blob.area():
            maxArea = blob.area()
            cx = blob.cx()
            cy = blob.cy()
    print(cx, cy)
    return [int(cx), int(cy)]

def ResetSensorForDetectRedObject():
    sensor.reset()
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA)
    sensor.set_windowing((50, 0, 110, 120))
    #sensor.skip_frames(time = 500)

def DetectRedObject():
    img = sensor.snapshot()
    cnt = 0
    cx = 0
    cy = 0
    for blob in img.find_blobs([redThresh], invert = False, x_stride = 10, y_stride = 10, area_threshold = 100, pixels_threshold = 100, merge = True, margin = 10):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        cx += blob.cx()
        cy += blob.cy()
        cnt += 1
    if cnt == 0:
        return [0, 0]
    cx /= cnt
    cy /= cnt
    return [int(cx), int(cy)]

def ResetSensorForDetectGreenObject():
    sensor.reset()
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA)
    sensor.set_windowing((50, 0, 110, 120))
    #sensor.skip_frames(time = 500)

def DetectGreenObject():
    img = sensor.snapshot()
    cnt = 0
    cx = 0
    cy = 0
    for blob in img.find_blobs([greenThresh], invert = False, x_stride = 10, y_stride = 10, area_threshold = 100, pixels_threshold = 100, merge = True, margin = 10):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        cx += blob.cx()
        cy += blob.cy()
        cnt += 1
    if cnt == 0:
        return [0, 0]
    cx /= cnt
    cy /= cnt
    return [int(cx), int(cy)]

def ResetSensorForDetectBlueObject():
    sensor.reset()
    #sensor.set_auto_gain(False)
    #sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA)
    sensor.set_windowing((50, 0, 110, 120))
    #sensor.skip_frames(time = 500)

def DetectBlueObject():
    img = sensor.snapshot()
    cnt = 0
    cx = 0
    cy = 0
    for blob in img.find_blobs([blueThresh], invert = False, x_stride = 10, y_stride = 10, area_threshold = 100, pixels_threshold = 100, merge = True, margin = 10):
        img.draw_rectangle(blob.rect(), color = (0, 255, 0))
        cx += blob.cx()
        cy += blob.cy()
        cnt += 1
    if cnt == 0:
        return [0, 0]
    cx /= cnt
    cy /= cnt
    return [int(cx), int(cy)]

def UpdateStatus(cmd):
    global cmdStatus
    if cmdStatus == cmd:
        return None
    cmdStatus = cmd
    if cmd == 0x02:
        ResetSensorForDetectLeftLine()
    elif cmd == 0x03:
        ResetSensorForDetectRightLine()
    elif cmd == 0x04:
        ResetSensorForDetectAllCircle()
    elif cmd == 0x05:
        ResetSensorForDetectColorCircle()
    elif cmd == 0x06:
        ResetSensorForDetectColorCircle()
    elif cmd == 0x07:
        ResetSensorForDetectColorCircle()
    elif cmd == 0x08:
        ResetSensorForDetectRedObject()
    elif cmd == 0x09:
        ResetSensorForDetectGreenObject()
    elif cmd == 0x0a:
        ResetSensorForDetectBlueObject()
    elif cmd == 0x0b:
        ResetSensorForDetectColorCircle()



while True:
    cmd = UartReadCmd()
    if cmd:
        UpdateStatus(cmd)
    if cmdStatus == 0x02:
        ret = DetectLeftLine()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x03:
        ret = DetectRightLine()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x04:
        ret = DetectAllCircle()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x05:
        ret = DetectColorCircle(redThresh)
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x06:
        ret = DetectColorCircle(greenThresh)
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x07:
        ret = DetectColorCircle(blueThresh)
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x08:
        ret = DetectRedObject()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x09:
        ret = DetectGreenObject()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x0a:
        ret = DetectBlueObject()
        if ret:
            SendPair(ret[0], ret[1])
    elif cmdStatus == 0x0b:
        ret = GetColorCircleSeq()
        if ret:
            SendPair(ret[0], ret[1])


import sensor, image, time, math
import machine
from pyb import UART,LED

uart = UART(1,115200)
uart.init(115200, bits=8, parity=None, stop=1) # init with given parameters

cmdStatus = 0x00 #Stop

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

def ResetSensorForQrcodeScan():
    sensor.reset()
    sensor.set_auto_gain(False)
    sensor.set_auto_whitebal(False)
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.VGA)
    #sensor.skip_frames(time = 1000)

def ScanQrcode():
    img = sensor.snapshot()
    img.lens_corr(2.5) #修复畸变
    qrcodeScanInfo = img.find_qrcodes()
    if qrcodeScanInfo:
        print("qrcode scan info:", qrcodeScanInfo[0].payload())
        return str(qrcodeScanInfo[0].payload())
    return None


def UpdateStatus(cmd):
    global cmdStatus
    if cmdStatus == cmd:
        return None
    cmdStatus = cmd
    if cmd == 0x01:
        ResetSensorForQrcodeScan()



while True:
    cmd = UartReadCmd()
    if cmd:
        UpdateStatus(cmd)
    if cmdStatus == 0x01:
        ret = ScanQrcode()
        if ret:
            SendStr(ret)


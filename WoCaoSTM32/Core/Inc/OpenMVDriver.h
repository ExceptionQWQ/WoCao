#ifndef OPENMVDRIVER_H
#define OPENMVDRIVER_H

#include "main.h"


enum OPENMV_CMD
{
    OPENMV_CMD_STOP = 0x00,
    OPENMV_CMD_DETECT_QRCODE = 0x01,
    OPENMV_CMD_DETECT_LEFT_LINE = 0x02,
    OPENMV_CMD_DETECT_RIGHT_LINE = 0x03,
    OPENMV_CMD_DETECT_ALL_CIRCLE = 0x04,
    OPENMV_CMD_DETECT_RED_CIRCLE = 0x05,
    OPENMV_CMD_DETECT_GREEN_CIRCLE = 0x06,
    OPENMV_CMD_DETECT_BLUE_CIRCLE = 0x07,
    OPENMV_CMD_DETECT_RED_OBJECT = 0x08,
    OPENMV_CMD_DETECT_GREEN_OBJECT = 0x09,
    OPENMV_CMD_DETECT_BLUE_OBJECT = 0x0a,
    OPENMV_CMD_DETECT_COLOR_CIRCLE_SEQ = 0x0b,
};

enum OPENMV_HANDLE_CMD
{
    OPENMV_HANDLE_CMD_STR = 0x01,
    OPENMV_HANDLE_CMD_PAIR = 0x02,
};


#ifdef __cplusplus

#include <utility>
#include <vector>
#include <string>
#include <cstring>

class OpenMVDriver
{
private:
    int dx;
    int dy;
    char str[64];
    enum OPENMV_CMD cmdStatus;
    int updateTick = 0;
    void StoreStr(uint8_t* data, int dataLen);
    void StorePair(uint8_t* data, int dataLen);
public:
    int GetUpdateTick();
    void SendSignal(enum OPENMV_CMD cmd, int cnt = 3, bool isDelay = true);
    void HandleOpenMVPackage(int cmd, uint8_t* data, int dataLen); //处理串口驱动传输的数据包
    std::string GetStr();
    std::pair<int, int> GetPair();
};


#endif




#endif
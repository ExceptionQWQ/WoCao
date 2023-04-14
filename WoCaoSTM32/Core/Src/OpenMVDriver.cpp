#include "OpenMVDriver.h"

int OpenMVDriver::GetUpdateTick()
{
    return updateTick;
}

void OpenMVDriver::SendSignal(enum OPENMV_CMD cmd, int cnt, long delay)
{
    if (cmdStatus == cmd) return ;
    dx = 0; dy = 0;
    memset(str, 0, 64);
    cmdStatus = cmd;
    uint8_t cmdBuff = cmd;
    for (int i = 0; i < cnt; ++i) {
        HAL_UART_Transmit(&huart1, &cmdBuff, 1, 100);
        HAL_UART_Transmit(&huart2, &cmdBuff, 1, 100);
        HAL_Delay(10);
    }
    if (delay)
        HAL_Delay(delay); //等待传感器稳定
}

void OpenMVDriver::StoreStr(uint8_t* data, int dataLen)
{
    updateTick += 1;
    memcpy(str, data, dataLen);
    str[dataLen] = 0; //标记字符串结尾
}

void OpenMVDriver::StorePair(uint8_t* data, int dataLen)
{
    updateTick += 1;
    dx = (int16_t)(((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    dy = (int16_t)(((uint16_t)data[2] << 8) | (uint16_t)data[3]);
}

void OpenMVDriver::HandleOpenMVPackage(int cmd, uint8_t* data, int dataLen)
{
    switch (cmd) {
        case OPENMV_HANDLE_CMD_STR:
            StoreStr(data, dataLen);
            break;
        case OPENMV_HANDLE_CMD_PAIR:
            StorePair(data, dataLen);
            break;
    }
}

std::string OpenMVDriver::GetStr()
{
    return std::string(str);
}

std::pair<int, int> OpenMVDriver::GetPair()
{
    return std::make_pair(dx, dy);
}

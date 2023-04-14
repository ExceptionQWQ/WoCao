#include "RobotMain.h"

RobotMotion robotMotion;
ServoDriver servoDriver;

uint8_t tempChar;

//串口5调试
void HandleDebugSerialMessage(const char* message, int len)
{
    switch (message[0]) {
        case 'w':
            robotMotion.ClearWheelSpeed();
            robotMotion.MoveForward(20);
            robotMotion.UpdateWheelSpeed();
            break;
        case 's':
            robotMotion.ClearWheelSpeed();
            robotMotion.MoveBackward(20);
            robotMotion.UpdateWheelSpeed();
            break;
        case 'a':
            robotMotion.ClearWheelSpeed();
            robotMotion.MoveLeft(20);
            robotMotion.UpdateWheelSpeed();
            break;
        case 'd':
            robotMotion.ClearWheelSpeed();
            robotMotion.MoveRight(20);
            robotMotion.UpdateWheelSpeed();
            break;
        case 'p':
            robotMotion.ClearWheelSpeed();
            robotMotion.UpdateWheelSpeed();
            break;
    }
}

//分发处理通用传输传输数据包
void HandleUartTransmitPackage(int cmd, uint8_t* data, int dataLen)
{
    openMVDriver.HandleOpenMVPackage(cmd, data, dataLen);
}

//检查通用串口传输数据包
void CheckUartTransmitPackage(uint8_t* data, int& offset)
{
    if (offset < 4) return ;
    if (data[0] != 0x12 || data[1] != 0x34) { //不是帧头，则左移一字节
        for (int i = 0; i < offset - 1; ++i) {
            data[i] = data[i + 1];
        }
        offset -= 1;
        return ;
    }
    //是帧头
    int dataLen = data[2];
    int cmd = data[3];
    uint8_t* messageData = data + 4;
    if (offset >= dataLen + 4) {
        HandleUartTransmitPackage(cmd, messageData, dataLen);
        offset = 0;
    }
}

//检查舵机数据包
void CheckServoTransmitPackage(uint8_t* data, int& offset)
{
    if (offset < 4) return ;
    if (data[0] != 0xff || data[1] != 0xff) { //不是帧头，则左移一字节
        for (int i = 0; i < offset - 1; ++i) {
            data[i] = data[i + 1];
        }
        offset -= 1;
        return ;
    }
    //是帧头
    int dataLen = data[3];
    if (offset >= dataLen + 4) {
        servoDriver.HandleServoPackage(data, offset);
        offset = 0;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    static uint8_t debugUartRecvBuff[1] = {};
    static uint8_t debugUartBuff[128] = {};
    static int debugUartOffset = 0;
    static uint8_t echoEnd[] = "\r\n";

    static uint8_t uart1RecvBuff[1] = {};
    static uint8_t uart1Buff[128] = {};
    static int uart1Offset = 0;

    static uint8_t uart2RecvBuff[1] = {};
    static uint8_t uart2Buff[128] = {};
    static int uart2Offset = 0;

    static uint8_t uart3RecvBuff[1] = {};
    static uint8_t uart3Buff[128] = {};
    static int uart3Offset = 0;

    static uint8_t wifiRecvBuff[1] = {};

    if (huart->Instance == USART1) { //openmv接口

        uart1Buff[uart1Offset++] = uart1RecvBuff[0];
        CheckUartTransmitPackage(uart1Buff, uart1Offset);
        HAL_UART_Receive_IT(huart, uart1RecvBuff, 1);

    } else if (huart->Instance == USART2) { //openmv接口

        uart2Buff[uart2Offset++] = uart2RecvBuff[0];
        CheckUartTransmitPackage(uart2Buff, uart2Offset);
        HAL_UART_Receive_IT(huart, uart2RecvBuff, 1);

    }  else if (huart->Instance == USART3) { //总线舵机接口

        uart3Buff[uart3Offset++] = uart3RecvBuff[0];
        CheckServoTransmitPackage(uart3Buff, uart3Offset);
        HAL_UART_Receive_IT(huart, uart3RecvBuff, 1);

    } else if (huart->Instance == UART4) { //WiFi接收

        HAL_UART_Transmit(&huart5, wifiRecvBuff, 1, 10);
        HAL_UART_Receive_IT(huart, wifiRecvBuff, 1); //显示到串口调试

    } else if (huart->Instance == UART5) { //串口调试

        debugUartBuff[debugUartOffset] = debugUartRecvBuff[0];
        if (debugUartBuff[debugUartOffset] == ';') {
            HAL_UART_Transmit(huart, debugUartBuff, debugUartOffset + 1, 10); //数据回显
            HAL_UART_Transmit(huart, echoEnd, strlen(reinterpret_cast<char*>(echoEnd)), 10); //数据回显
            HandleDebugSerialMessage(reinterpret_cast<char*>(debugUartBuff), debugUartOffset + 1);
            debugUartOffset = 0;
        } else {
            ++debugUartOffset;
        }
        HAL_UART_Receive_IT(huart, debugUartRecvBuff, 1);

    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6) {
        WheelTick();
        int pid = 0;

        pid = GetWheelPID(WHEEL_TAG_FRONT_LEFT);
        if (pid >= 0) {
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1000 - pid);
            HAL_GPIO_WritePin(WHEEL_FRONT_LEFT_OUTPUT_B_GPIO_Port, WHEEL_FRONT_LEFT_OUTPUT_B_Pin, GPIO_PIN_SET);
        } else {
            pid = -pid;
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pid);
            HAL_GPIO_WritePin(WHEEL_FRONT_LEFT_OUTPUT_B_GPIO_Port, WHEEL_FRONT_LEFT_OUTPUT_B_Pin, GPIO_PIN_RESET);
        }

        pid = GetWheelPID(WHEEL_TAG_FRONT_RIGHT);
        if (pid >= 0) {
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 1000 - pid);
            HAL_GPIO_WritePin(WHEEL_FRONT_RIGHT_OUTPUT_B_GPIO_Port, WHEEL_FRONT_RIGHT_OUTPUT_B_Pin, GPIO_PIN_SET);
        } else {
            pid = -pid;
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pid);
            HAL_GPIO_WritePin(WHEEL_FRONT_RIGHT_OUTPUT_B_GPIO_Port, WHEEL_FRONT_RIGHT_OUTPUT_B_Pin, GPIO_PIN_RESET);
        }


        pid = GetWheelPID(WHEEL_TAG_BACK_LEFT);
        if (pid >= 0) {
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 1000 - pid);
            HAL_GPIO_WritePin(WHEEL_BACK_LEFT_OUTPUT_B_GPIO_Port, WHEEL_BACK_LEFT_OUTPUT_B_Pin, GPIO_PIN_SET);
        } else {
            pid = -pid;
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pid);
            HAL_GPIO_WritePin(WHEEL_BACK_LEFT_OUTPUT_B_GPIO_Port, WHEEL_BACK_LEFT_OUTPUT_B_Pin, GPIO_PIN_RESET);
        }

        pid = GetWheelPID(WHEEL_TAG_BACK_RIGHT);
        if (pid >= 0) {
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1000 - pid);
            HAL_GPIO_WritePin(WHEEL_BACK_RIGHT_OUTPUT_B_GPIO_Port, WHEEL_BACK_RIGHT_OUTPUT_B_Pin, GPIO_PIN_SET);
        } else {
            pid = -pid;
            if (pid > 1000) pid = 1000;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pid);
            HAL_GPIO_WritePin(WHEEL_BACK_RIGHT_OUTPUT_B_GPIO_Port, WHEEL_BACK_RIGHT_OUTPUT_B_Pin, GPIO_PIN_RESET);
        }
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case WHEEL_FRONT_RIGHT_INPUT_A_Pin:
            HandleWheelInterupt(WHEEL_TAG_FRONT_RIGHT, WHEEL_AB_TAG_A, HAL_GPIO_ReadPin(WHEEL_FRONT_RIGHT_INPUT_A_GPIO_Port, WHEEL_FRONT_RIGHT_INPUT_A_Pin));
            break;
        case WHEEL_FRONT_RIGHT_INPUT_B_Pin:
            HandleWheelInterupt(WHEEL_TAG_FRONT_RIGHT, WHEEL_AB_TAG_B, HAL_GPIO_ReadPin(WHEEL_FRONT_RIGHT_INPUT_B_GPIO_Port, WHEEL_FRONT_RIGHT_INPUT_B_Pin));
            break;

        case WHEEL_FRONT_LEFT_INPUT_A_Pin:
            HandleWheelInterupt(WHEEL_TAG_FRONT_LEFT, WHEEL_AB_TAG_A, HAL_GPIO_ReadPin(WHEEL_FRONT_LEFT_INPUT_A_GPIO_Port, WHEEL_FRONT_LEFT_INPUT_A_Pin));
            break;
        case WHEEL_FRONT_LEFT_INPUT_B_Pin:
            HandleWheelInterupt(WHEEL_TAG_FRONT_LEFT, WHEEL_AB_TAG_B, HAL_GPIO_ReadPin(WHEEL_FRONT_LEFT_INPUT_B_GPIO_Port, WHEEL_FRONT_LEFT_INPUT_B_Pin));
            break;

        case WHEEL_BACK_LEFT_INPUT_A_Pin:
            HandleWheelInterupt(WHEEL_TAG_BACK_LEFT, WHEEL_AB_TAG_A, HAL_GPIO_ReadPin(WHEEL_BACK_LEFT_INPUT_A_GPIO_Port, WHEEL_BACK_LEFT_INPUT_A_Pin));
            break;
        case WHEEL_BACK_LEFT_INPUT_B_Pin:
            HandleWheelInterupt(WHEEL_TAG_BACK_LEFT, WHEEL_AB_TAG_B, HAL_GPIO_ReadPin(WHEEL_BACK_LEFT_INPUT_B_GPIO_Port, WHEEL_BACK_LEFT_INPUT_B_Pin));
            break;

        case WHEEL_BACK_RIGHT_INPUT_A_Pin:
            HandleWheelInterupt(WHEEL_TAG_BACK_RIGHT, WHEEL_AB_TAG_A, HAL_GPIO_ReadPin(WHEEL_BACK_RIGHT_INPUT_A_GPIO_Port, WHEEL_BACK_RIGHT_INPUT_A_Pin));
            break;
        case WHEEL_BACK_RIGHT_INPUT_B_Pin:
            HandleWheelInterupt(WHEEL_TAG_BACK_RIGHT, WHEEL_AB_TAG_B, HAL_GPIO_ReadPin(WHEEL_BACK_RIGHT_INPUT_B_GPIO_Port, WHEEL_BACK_RIGHT_INPUT_B_Pin));
            break;
    }
}

void EnableWheelPWMOutput()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(MOTOR_DRIVER_EN_GPIO_Port, MOTOR_DRIVER_EN_Pin, GPIO_PIN_SET); //电机驱动使能
}
void DisableWheelPWMOutput()
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(MOTOR_DRIVER_EN_GPIO_Port, MOTOR_DRIVER_EN_Pin, GPIO_PIN_RESET); //电机驱动失能
}

//这会丢失第一帧数据
void EnableUart1SerialTransmit()
{
    HAL_UART_Receive_IT(&huart1, &tempChar, 1);
}
void EnableUart2SerialTransmit()
{
    HAL_UART_Receive_IT(&huart2, &tempChar, 1);
}
void EnableServoSerialTransmit()
{
    HAL_UART_Receive_IT(&huart3, &tempChar, 1);
}
void EnableWiFiTransmit()
{
    HAL_UART_Receive_IT(&huart4, &tempChar, 1);
}
void EnableDebugSerialTransmit()
{
    HAL_UART_Receive_IT(&huart5, &tempChar, 1);
}

//每隔1ms触发中断
void EnableTimeTick()
{
    HAL_TIM_Base_Start_IT(&htim6);
}


void RobotInit()
{
    robotMotion.Init(); //清空编码器脉冲和速度
    EnableTimeTick(); //编码器采集脉冲使能
    EnableDebugSerialTransmit(); //串口调试使能
    EnableUart1SerialTransmit(); //串口1接收使能
    EnableUart2SerialTransmit(); //串口2接收使能
    EnableServoSerialTransmit(); //舵机串口接收使能
    EnableWiFiTransmit(); //WiFi接收使能
    EnableWheelPWMOutput(); //电机驱动板使能

    LCD_Init();
    LCD_Clear(WHITE); //显示屏背景设置为白色
}

void LCDShowQrcodeInfo(std::string qrcode)
{
    if (qrcode.length() != 7) return ; //不符合二维码扫描结果
    LCD_Draw_Number(10, 10, 1, qrcode[0] - '0');
    LCD_Draw_Number(60, 10, 1, qrcode[1] - '0');
    LCD_Draw_Number(100, 10, 1, qrcode[2] - '0');
    LCD_Draw_Add_Char(150, 10, 1);
    LCD_Draw_Number(10, 90, 1, qrcode[4] - '0');
    LCD_Draw_Number(60, 90, 1, qrcode[5] - '0');
    LCD_Draw_Number(100, 90, 1, qrcode[6] - '0');
}

void DebugPrintQrcodeScanInfo()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_QRCODE, 3);
    while (true) {
        HAL_Delay(100);
        std::string info = openMVDriver.GetStr();
        char buff[1024] = {};
        snprintf(buff, 1024, "qrcode:%s\r\n", info.data());
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
    }
}

void DebugPrintDetectLeftLine()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_LEFT_LINE, 3);
    while (true) {
        HAL_Delay(100);
        auto ret = openMVDriver.GetPair();
        char buff[1024] = {};
        snprintf(buff, 1024, "theta:%d rho:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
    }
}

void DebugLeftKeepHrizAndDis()
{
    servoDriver.Init();
    servoDriver.MoveToTop();
    servoDriver.MoveLeftToFindLine();
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_LEFT_LINE, 3);
    MediumBlur m1(7), m2(7);
    while (true) {
        std::pair<int, int> ret = openMVDriver.GetPair();
        m1.Append(ret.first); m2.Append(ret.second);
        robotMotion.ClearWheelSpeed();
        robotMotion.KeepHrizLeft(m1);
        robotMotion.KeepVertLeft(m2);
        robotMotion.UpdateWheelSpeed();
        char buff[1024] = {};
        snprintf(buff, 1024, "angle:%d, dis:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
        HAL_Delay(5);
    }
}

void DebugMoveForwardAndTurnLeft()
{
    servoDriver.Init();
    servoDriver.MoveToTop();
    servoDriver.MoveLeftToFindLine();
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_LEFT_LINE, 3);
    MediumBlur m1(7), m2(7);

//    while (true) {
//        std::pair<int, int> ret = openMVDriver.GetPair();
//        char buff[1024] = {};
//        snprintf(buff, 1024, "angle:%d, dis:%d\r\n", ret.first, ret.second);
//        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
//        HAL_Delay(20);
//    }


    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDis(30, 80000);
    robotMotion.MoveForwardUntilCornerWithFindLeftLine(50);
    robotMotion.MoveForwardWithDis(50, 0);
}

void DebugRightKeepHriz()
{
    servoDriver.Init();
    servoDriver.MoveToTop();
    servoDriver.MoveRightToFindLine();
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_RIGHT_LINE, 3);
    MediumBlur m1(7), m2(7);
    while (true) {
        std::pair<int, int> ret = openMVDriver.GetPair();
        m1.Append(ret.first); m2.Append(ret.second);
        robotMotion.ClearWheelSpeed();
        robotMotion.KeepHrizRight(m1);
        robotMotion.UpdateWheelSpeed();
        char buff[1024] = {};
        snprintf(buff, 1024, "angle:%d, dis:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
        HAL_Delay(20);
    }
}

//测试转盘定位
void DebugFindRightCircle()
{
    servoDriver.Init();
    servoDriver.MoveToTop();
    servoDriver.MoveRightToFindObject();
    robotMotion.TrackCenterObject();
}
//等待红色物料坐标稳定
void WaitForRedObject()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_RED_OBJECT, 3);
    int tick = openMVDriver.GetUpdateTick();
    std::pair<int, int> lastPair = std::make_pair(0, 0);
    int status = 0; //状态变量， 0为初始值，还未识别到物料
    int initCnt = 0;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue; //等待摄像头传来新的数据
        tick = tick2;
        std::pair<int, int> ret = openMVDriver.GetPair(); //新的数据

        char buff[1024] = {};
        snprintf(buff, 1024, "WaitForRedObject dx:%d, dy:%d, tick:%d\r\n", ret.first, ret.second, tick);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 10);

        if (status == 0) { //最开始
            if (ret.first < 20 || ret.second < 20) ++initCnt;
            else status = 2; //如果最开始识别到物料，则等待4秒后再次识别，防止抓的时候跑掉
            if (initCnt > 5) { //如果未识别到要抓取的物料，则下一次识别到的时候就抓
                status = 1;
            }
        } else if (status == 1) { //等待物料坐标稳定后就抓取
            if (ret.first < 20 || ret.second < 20) continue;
            if (abs(ret.first - lastPair.first) < 2 && abs(ret.second - lastPair.second) < 2) break;
            lastPair = ret;
        } else if (status == 2) {
            HAL_Delay(6000); //等待6秒
            status = 1;
        }
    }
}
//等待绿色物料坐标稳定
void WaitForGreenObject()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_GREEN_OBJECT, 3);
    int tick = openMVDriver.GetUpdateTick();
    std::pair<int, int> lastPair = std::make_pair(0, 0);
    int status = 0; //状态变量， 0为初始值，还未识别到物料
    int initCnt = 0;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue; //等待摄像头传来新的数据
        tick = tick2;
        std::pair<int, int> ret = openMVDriver.GetPair(); //新的数据

        char buff[1024] = {};
        snprintf(buff, 1024, "WaitForGreenObject dx:%d, dy:%d, tick:%d\r\n", ret.first, ret.second, tick);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 10);

        if (status == 0) { //最开始
            if (ret.first < 20 || ret.second < 20) ++initCnt;
            else status = 2; //如果最开始识别到物料，则等待4秒后再次识别，防止抓的时候跑掉
            if (initCnt > 5) { //如果未识别到要抓取的物料，则下一次识别到的时候就抓
                status = 1;
            }
        } else if (status == 1) { //等待物料坐标稳定后就抓取
            if (ret.first < 20 || ret.second < 20) continue;
            if (abs(ret.first - lastPair.first) < 2 && abs(ret.second - lastPair.second) < 2) break;
            lastPair = ret;
        } else if (status == 2) {
            HAL_Delay(6000); //等待6秒
            status = 1;
        }
    }
}
//等待蓝色物料坐标稳定
void WaitForBlueObject()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_BLUE_OBJECT, 3);
    int tick = openMVDriver.GetUpdateTick();
    std::pair<int, int> lastPair = std::make_pair(0, 0);
    int status = 0; //状态变量， 0为初始值，还未识别到物料
    int initCnt = 0;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue; //等待摄像头传来新的数据
        tick = tick2;
        std::pair<int, int> ret = openMVDriver.GetPair(); //新的数据

        char buff[1024] = {};
        snprintf(buff, 1024, "WaitForBlueObject dx:%d, dy:%d, tick:%d\r\n", ret.first, ret.second, tick);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 10);

        if (status == 0) { //最开始
            if (ret.first < 20 || ret.second < 20) ++initCnt;
            else status = 2; //如果最开始识别到物料，则等待4秒后再次识别，防止抓的时候跑掉
            if (initCnt > 5) { //如果未识别到要抓取的物料，则下一次识别到的时候就抓
                status = 1;
            }
        } else if (status == 1) { //等待物料坐标稳定后就抓取
            if (ret.first < 20 || ret.second < 20) continue;
            if (abs(ret.first - lastPair.first) < 2 && abs(ret.second - lastPair.second) < 2) break;
            lastPair = ret;
        } else if (status == 2) {
            HAL_Delay(6000); //等待6秒
            status = 1;
        }
    }
}

//扫描二维码
std::string GetQrcode()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_QRCODE, 3);
    while (true) {
        HAL_Delay(100);
        std::string info = openMVDriver.GetStr();
        char buff[1024] = {};
        snprintf(buff, 1024, "qrcode:%s\r\n", info.data());
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
        if (info.length() == 7) {
            return info;
        }
    }
}

void SendQrcode(std::string qrcode)
{
    std::string data = "WoCao" + qrcode + "\n";
    uint8_t* cmd = reinterpret_cast<uint8_t *>(data.data());
    int len = strlen(reinterpret_cast<char*>(cmd));
    HAL_UART_Transmit(&huart4, cmd, len, 100);

}

void DebugGetAllServoPos()
{
    uint16_t yuntai = servoDriver.GetServoPos(SERVO_TAG_YUNTAI);
    HAL_Delay(5);
    uint16_t L1 = servoDriver.GetServoPos(SERVO_TAG_L1);
    HAL_Delay(5);
    uint16_t L2 = servoDriver.GetServoPos(SERVO_TAG_L2);
    HAL_Delay(5);
    uint16_t claw = servoDriver.GetServoPos(SERVO_TAG_CLAW);
    HAL_Delay(5);
    uint16_t s1 = servoDriver.GetServoPos(SERVO_TAG_S1);
    HAL_Delay(5);
    uint16_t s2 = servoDriver.GetServoPos(SERVO_TAG_S2);
    HAL_Delay(5);
    uint16_t s3 = servoDriver.GetServoPos(SERVO_TAG_S3);
    HAL_Delay(5);

    char buff[1024] = {};
    snprintf(buff, 1024, "yuntai:%d L1:%d L2:%d claw:%d s1:%d s2:%d s3:%d\r\n", yuntai, L1, L2, claw, s1, s2, s3);
    HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
}



void DebugForH2()
{
    servoDriver.Init();
    servoDriver.MoveRightToFindCircle();
    robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHL1();
    servoDriver.MoveToTop();

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHL2();
    servoDriver.MoveToTop();
}

void DebugPutH1H2()
{
    servoDriver.Init();
    servoDriver.MoveToTop();

    servoDriver.MoveRightToFindCircle();
    robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHM();
    servoDriver.MoveToHM1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHM1();
    servoDriver.MoveToTop();

    servoDriver.S2Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHR();
    servoDriver.MoveToHR1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHR1();
    servoDriver.MoveToTop();

    servoDriver.S3Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHL1();
    servoDriver.MoveToTop();

    HAL_Delay(6000);

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHM();
    servoDriver.MoveToHM2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHM2();
    servoDriver.MoveToTop();

    servoDriver.S3Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHL2();
    servoDriver.MoveToTop();

    servoDriver.S2Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHR();
    servoDriver.MoveToHR2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHR2();
    servoDriver.MoveToTop();


    while (true) {
        DebugGetAllServoPos();
        HAL_Delay(20);
    }
}

void DebugPutH1GetH1()
{
    servoDriver.MoveRightToFindCircle();
    robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHM();
    servoDriver.MoveToHM1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHM1();
    servoDriver.MoveToTop();

    servoDriver.S2Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHR();
    servoDriver.MoveToHR1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHR1();
    servoDriver.MoveToTop();

    servoDriver.S3Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
    servoDriver.UpdateServoPos();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromHL1();
    servoDriver.MoveToTop();

    servoDriver.S1Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.SpinToHM();
    servoDriver.MoveToHM1();
    servoDriver.ClawClose();
    servoDriver.MoveFromHM1();
    servoDriver.MoveToS1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS1();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
    servoDriver.UpdateServoPos();

    servoDriver.S2Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.SpinToHR();
    servoDriver.MoveToHR1();
    servoDriver.ClawClose();
    servoDriver.MoveFromHR1();
    servoDriver.MoveToS2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS2();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
    servoDriver.UpdateServoPos();

    servoDriver.S3Open();
    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.SpinToHL();
    servoDriver.MoveToHL1();
    servoDriver.ClawClose();
    servoDriver.MoveFromHL1();
    servoDriver.MoveToS3();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS3();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
    servoDriver.UpdateServoPos();




    while (true) {
        DebugGetAllServoPos();
        HAL_Delay(20);
    }
}



std::string GetColorCircleSeq()
{
    std::string colorSeq;
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_COLOR_CIRCLE_SEQ, 3);
    int tick = openMVDriver.GetUpdateTick();
    robotMotion.ClearWheelSpeed();
    robotMotion.MoveForward(5);
    robotMotion.UpdateWheelSpeed();
    int seq = 123; //default seq
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue;
        auto ret = openMVDriver.GetPair();
        if (ret.first == 0) continue;
        else seq = ret.first;
        break;
    }
    robotMotion.Stop();
    for (int i = 0; i < 3; ++i) { //转换成字符串
        int r = seq % 10;
        if (r == 4) r = 3;
        colorSeq.push_back('0' + r);
        seq /= 10;
    }
    std::reverse(std::begin(colorSeq), std::end(colorSeq)); //反转序列
    //Debug
    char buff[1024] = {};
    snprintf(buff, 1024, "ColorSeq:%s\r\n", colorSeq.c_str());
    HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
    return colorSeq;
}

//放物料到第一层
void PutObject1(char color, const std::string& colorSeq)
{
    if (colorSeq[0] == color) {
        servoDriver.SpinToHL();
        servoDriver.MoveToHL1();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHL1();
        servoDriver.MoveToTop();
    } else if (colorSeq[1] == color) {
        servoDriver.SpinToHM();
        servoDriver.MoveToHM1();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHM1();
        servoDriver.MoveToTop();
    } else if (colorSeq[2] == color) {
        servoDriver.SpinToHR();
        servoDriver.MoveToHR1();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHR1();
        servoDriver.MoveToTop();
    }
}

//放物料到第二层
void PutObject2(char color, const std::string& colorSeq)
{
    if (colorSeq[0] == color) {
        servoDriver.SpinToHL();
        servoDriver.MoveToHL2();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHL2();
        servoDriver.MoveToTop();
    } else if (colorSeq[1] == color) {
        servoDriver.SpinToHM();
        servoDriver.MoveToHM2();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHM2();
        servoDriver.MoveToTop();
    } else if (colorSeq[2] == color) {
        servoDriver.SpinToHR();
        servoDriver.MoveToHR2();
        servoDriver.ClawOpen();
        servoDriver.MoveFromHR2();
        servoDriver.MoveToTop();
    }
}

//从第一层取物料
void GetObject1(char color, const std::string& colorSeq)
{
    if (colorSeq[0] == color) {
        servoDriver.SpinToHL();
        servoDriver.ClawOpen();
        servoDriver.MoveToHL1();
        servoDriver.ClawClose();
        servoDriver.MoveFromHL1();
        servoDriver.MoveToTop();
    } else if (colorSeq[1] == color) {
        servoDriver.SpinToHM();
        servoDriver.ClawOpen();
        servoDriver.MoveToHM1();
        servoDriver.ClawClose();
        servoDriver.MoveFromHM1();
        servoDriver.MoveToTop();
    } else if (colorSeq[2] == color) {
        servoDriver.SpinToHR();
        servoDriver.ClawOpen();
        servoDriver.MoveToHR1();
        servoDriver.ClawClose();
        servoDriver.MoveFromHR1();
        servoDriver.MoveToTop();
    }
}

void RobotMain()
{
    constexpr double robotSpeed = 100;

    RobotInit();

//    servoDriver.MoveLeftToFindLine();
//    robotMotion.MoveForwardWithDisAndFindLeftLine(100, 250000);
//
//    DebugPutH1GetH1();

//    std::string caonima = "123+321";
//    LCDShowQrcodeInfo(caonima); //显示扫描结果
//
//    while (true) {
//        DebugGetAllServoPos();
//    }
//
//
//    return ;


    robotMotion.MoveLeftWithDis(30, 25000); //走出起停区

    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 113000); //走到二维码

    robotMotion.MoveLeftWithDis(30, 12000); //退一段距离扫描二维码
    servoDriver.ClawOpen2();

    servoDriver.MoveRightToFindQrcode();

    std::string qrcode = GetQrcode();
//    std::string qrcode = "123+321";

    servoDriver.MoveFromRightToFindQrcode();

    robotMotion.MoveRightWithDis(30, 12000); //回到赛道中间


    LCDShowQrcodeInfo(qrcode); //显示扫描结果
    SendQrcode(qrcode); //Wi-Fi发送扫描结果到场外

    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 130000); //走到转盘

    servoDriver.MoveRightToFindObject();
    robotMotion.TrackCenterObject(); //定位转盘

    //抓第一个物料并放入S1
    switch (qrcode[0]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
    }
    servoDriver.MoveRightToFindObject();
    //抓第二个物料并放入S2
    switch (qrcode[1]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
    }
    servoDriver.MoveRightToFindObject();
    //抓第三个物料并放入S3
    switch (qrcode[2]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
    }


    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 85000); //走到转弯处
    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 166000 - 18000); //走到粗加工区

    servoDriver.MoveRightToFindCircle();
    std::string colorSeq1 = GetColorCircleSeq();

    switch (colorSeq1[1]) { //定位中间的圆环
        case '1':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_RED_CIRCLE);
            break;
        case '2':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);
            break;
        case '3':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_BLUE_CIRCLE);
            break;
    }

    //放S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[0], colorSeq1);

    //放S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[1], colorSeq1);

    //放S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[2], colorSeq1);

    //取S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[0], colorSeq1);
    servoDriver.MoveToS1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS1();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();

    //取S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[1], colorSeq1);
    servoDriver.MoveToS2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS2();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();

    //取S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[2], colorSeq1);
    servoDriver.MoveToS3();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS3();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();


    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 150000); //走到转弯处

    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 165000 - 18000); //走到成品区


    servoDriver.MoveRightToFindCircle();
    std::string colorSeq2 = GetColorCircleSeq();

    switch (colorSeq2[1]) { //定位中间的圆环
        case '1':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_RED_CIRCLE);
            break;
        case '2':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);
            break;
        case '3':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_BLUE_CIRCLE);
            break;
    }

    //放S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[0], colorSeq2);

    //放S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[1], colorSeq2);

    //放S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[2], colorSeq2);

    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 148000); //走到转弯处
    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 316000); //走到起停区

    /*
     *
     * ###################################################################################################
     */


    //第二圈
    robotMotion.TurnLeft(30);

    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 233000); //走到转盘


    servoDriver.MoveRightToFindObject();
    robotMotion.TrackCenterObject(); //定位转盘

    //抓第一个物料并放入S1
    switch (qrcode[4]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S1Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS1();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS1();
            servoDriver.S1Close();
//            servoDriver.UpdateServoPos();
            break;
    }
    servoDriver.MoveRightToFindObject();
    //抓第二个物料并放入S2
    switch (qrcode[5]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S2Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS2();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS2();
            servoDriver.S2Close();
//            servoDriver.UpdateServoPos();
            break;
    }
    servoDriver.MoveRightToFindObject();
    //抓第三个物料并放入S3
    switch (qrcode[6]) {
        case '1':
            servoDriver.ClawOpen();
            WaitForRedObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
        case '2':
            servoDriver.ClawOpen();
            WaitForGreenObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
        case '3':
            servoDriver.ClawOpen();
            WaitForBlueObject();
            servoDriver.MoveToP1();
            servoDriver.ClawClose();
            servoDriver.MoveToTop();
            servoDriver.S3Open();
//            servoDriver.UpdateServoPos();
            servoDriver.MoveToS3();
            servoDriver.ClawOpen();
            servoDriver.MoveFromS3();
            servoDriver.S3Close();
//            servoDriver.UpdateServoPos();
            break;
    }

    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 85000); //走到转弯处
    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 163000); //走到粗加工区

    servoDriver.MoveRightToFindCircle();
    switch (colorSeq1[1]) { //定位中间的圆环
        case '1':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_RED_CIRCLE);
            break;
        case '2':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE);
            break;
        case '3':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_BLUE_CIRCLE);
            break;
    }


    //放S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[4], colorSeq1);

    //放S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[5], colorSeq1);

    //放S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();
    PutObject1(qrcode[6], colorSeq1);

    //取S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[4], colorSeq1);
    servoDriver.MoveToS1();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS1();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();

    //取S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[5], colorSeq1);
    servoDriver.MoveToS2();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS2();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();

    //取S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    GetObject1(qrcode[6], colorSeq1);
    servoDriver.MoveToS3();
    servoDriver.ClawOpen();
    servoDriver.MoveFromS3();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();


    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 150000); //走到转弯处

    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 165000); //走到成品区

    servoDriver.MoveRightToFindCircle();
    switch (colorSeq2[1]) { //定位中间的圆环
        case '1':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_RED_CIRCLE, 8);
            break;
        case '2':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_GREEN_CIRCLE, 8);
            break;
        case '3':
            robotMotion.TrackCenterCircle(OPENMV_CMD_DETECT_BLUE_CIRCLE, 8);
            break;
    }

    //放S1
    servoDriver.S1Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS1();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S1Close();
//    servoDriver.UpdateServoPos();
    PutObject2(qrcode[4], colorSeq2);

    //放S2
    servoDriver.S2Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS2();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S2Close();
//    servoDriver.UpdateServoPos();
    PutObject2(qrcode[5], colorSeq2);

    //放S3
    servoDriver.S3Open();
//    servoDriver.UpdateServoPos();
    servoDriver.ClawOpen();
    servoDriver.MoveToS3();
    servoDriver.ClawClose();
    servoDriver.MoveToTop();
    servoDriver.S3Close();
//    servoDriver.UpdateServoPos();
    PutObject2(qrcode[6], colorSeq2);

    servoDriver.MoveLeftToFindLine();
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 148000); //走到转弯处
    robotMotion.TurnLeft(30);
    robotMotion.MoveForwardWithDisAndFindLeftLine(robotSpeed, 316000); //走到起停区

    servoDriver.Init(); //复位
    robotMotion.MoveXYWithDis(30, 0, -18000); //回到起停区
    robotMotion.MoveXYWithDis(30, 40000, 0); //回到起停区
}
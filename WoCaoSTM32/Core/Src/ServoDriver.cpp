#include "ServoDriver.h"

uint8_t ServoDriver::CheckSum(uint8_t startNum, uint8_t endNum, uint8_t *inData)
{
    uint8_t count;
    uint16_t checkSum = 0;
    for (count = startNum; count < endNum + 1; ++count)
    {
        checkSum += inData[count];
    }
    return ((~checkSum) & 0xFF);
}

void ServoDriver::SetServoSpeed(uint16_t speed)
{
    servoSpeed = speed;
}

void ServoDriver::HandleServoPackage(uint8_t* data, int packageLen)
{
    if (packageLen != 8) return ;
    if (data[7] != CheckSum(2, 6, data)) return ;

    SERVO_TAG tag = (SERVO_TAG)data[2];
    uint8_t dataLen = data[3];
    uint16_t pos = (data[6] << 8) | data[5];

    switch (tag) {
        case SERVO_TAG_YUNTAI:
            yuntaiPos = pos;
            break;
        case SERVO_TAG_L1:
            l1Pos = pos;
            break;
        case SERVO_TAG_L2:
            l2Pos = pos;
            break;
        case SERVO_TAG_CLAW:
            clawPos = pos;
            break;
        case SERVO_TAG_S1:
            s1Pos = pos;
            break;
        case SERVO_TAG_S2:
            s2Pos = pos;
            break;
        case SERVO_TAG_S3:
            s3Pos = pos;
            break;
    }
}

void ServoDriver::SetServoPos(enum SERVO_TAG servoTag, uint16_t pos, uint16_t speed)
{
    switch (servoTag) {
        case SERVO_TAG_YUNTAI:
            yuntaiAngle = pos;
            break;
        case SERVO_TAG_L1:
            l1Angle = pos;
            break;
        case SERVO_TAG_L2:
            l2Angle = pos;
            break;
        case SERVO_TAG_CLAW:
            clawAngle = pos;
            break;
        case SERVO_TAG_S1:
            s1Angle = pos;
            break;
        case SERVO_TAG_S2:
            s2Angle = pos;
            break;
        case SERVO_TAG_S3:
            s3Angle = pos;
            break;
    }
    uint8_t writeTempCmd[] = {0xFF, 0xFF, 0x00, 0x09, 0x04, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    writeTempCmd[2] = servoTag;
    writeTempCmd[6] = pos & 0xff;
    writeTempCmd[7] = (pos >> 8) & 0xff;
    writeTempCmd[10] = speed & 0xff;
    writeTempCmd[11] = (speed >> 8) & 0xff;
    writeTempCmd[12] = CheckSum(2, 11, writeTempCmd);

    HAL_UART_Transmit(&huart3, writeTempCmd, 13, 10);
    HAL_Delay(1);
}

uint16_t ServoDriver::GetServoPos(enum SERVO_TAG servoTag)
{
    uint8_t cmd[] = {0xFF, 0xFF, 0x00, 0x04, 0x02, 0x38, 0x02, 0x00};
    cmd[2] = servoTag;
    cmd[7] = CheckSum(2, 6, cmd);
    uint8_t rx[8] = {0};
    HAL_UART_Transmit(&huart3, cmd, 8, 10);
    HAL_Delay(1);
    uint16_t pos = 0;
    switch (servoTag) {
        case SERVO_TAG_YUNTAI:
            pos = yuntaiPos;
            break;
        case SERVO_TAG_L1:
            pos = l1Pos;
            break;
        case SERVO_TAG_L2:
            pos = l2Pos;
            break;
        case SERVO_TAG_CLAW:
            pos = clawPos;
            break;
        case SERVO_TAG_S1:
            pos = s1Pos;
            break;
        case SERVO_TAG_S2:
            pos = s2Pos;
            break;
        case SERVO_TAG_S3:
            pos = s3Pos;
            break;
    }
    return pos;
}

void ServoDriver::UpdateServoPos() //blocking
{
    while (true) {
        if (yuntaiAngle != 0) SetServoPos(SERVO_TAG_YUNTAI, yuntaiAngle, servoSpeed);
        if (l1Angle != 0) SetServoPos(SERVO_TAG_L1, l1Angle, servoSpeed);
        if (l2Angle != 0) SetServoPos(SERVO_TAG_L2, l2Angle, servoSpeed);
        if (clawAngle != 0) SetServoPos(SERVO_TAG_CLAW, clawAngle, servoSpeed);
        if (s1Angle != 0) SetServoPos(SERVO_TAG_S1, s1Angle, servoSpeed);
        if (s2Angle != 0) SetServoPos(SERVO_TAG_S2, s2Angle, servoSpeed);
        if (s3Angle != 0) SetServoPos(SERVO_TAG_S3, s3Angle, servoSpeed);
        uint8_t writeTempCmd[] = {0xff, 0xff, 0xfe, 0x02, 0x05, 0xfa};
        HAL_UART_Transmit(&huart3, writeTempCmd, 6, 100); //为了解决摆动过程中信号线松动造成的机械臂死机

        bool flag = true;
        uint16_t pos = 0;
        if (yuntaiAngle != 0) {
            pos = GetServoPos(SERVO_TAG_YUNTAI);
            if (abs(pos - yuntaiAngle) > 100) flag = false;
        }
        if (l1Angle != 0) {
            pos = GetServoPos(SERVO_TAG_L1);
            if (abs(pos - l1Angle) > 100) flag = false;
        }
        if (l2Angle != 0) {
            pos = GetServoPos(SERVO_TAG_L2);
            if (abs(pos - l2Angle) > 100) flag = false;
        }
        if (clawAngle != 0) {
            pos = GetServoPos(SERVO_TAG_CLAW);
            if (abs(pos - clawAngle) > 100) flag = false;
        }
        if (s1Angle != 0) {
            pos = GetServoPos(SERVO_TAG_S1);
            if (abs(pos - s1Angle) > 100) flag = false;
        }
        if (s2Angle != 0) {
            pos = GetServoPos(SERVO_TAG_S2);
            if (abs(pos - s2Angle) > 100) flag = false;
        }
        if (s3Angle != 0) {
            pos = GetServoPos(SERVO_TAG_S3);
            if (abs(pos - s3Angle) > 100) flag = false;
        }
        if (flag) break;
        HAL_Delay(1);
    }
}

void ServoDriver::S1Open()
{
    SetServoPos(SERVO_TAG_S1, 2400, servoSpeed);
}

void ServoDriver::S2Open()
{
    SetServoPos(SERVO_TAG_S2, 2400, servoSpeed);
}

void ServoDriver::S3Open()
{
    SetServoPos(SERVO_TAG_S3, 2400, servoSpeed);
}

void ServoDriver::S1Close()
{
    SetServoPos(SERVO_TAG_S1, 1700, servoSpeed);
}

void ServoDriver::S2Close()
{
    SetServoPos(SERVO_TAG_S2, 1650, servoSpeed);
}

void ServoDriver::S3Close()
{
    SetServoPos(SERVO_TAG_S3, 1650, servoSpeed);
}

void ServoDriver::ClawOpen()
{
    SetServoPos(SERVO_TAG_CLAW, 2400, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::ClawOpen2()
{
    SetServoPos(SERVO_TAG_CLAW, 1684, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::ClawClose()
{
    SetServoPos(SERVO_TAG_CLAW, 2700, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveToTop()
{
    SetServoPos(SERVO_TAG_L1, 1500, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2900, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::Init() //初始位置
{
    S1Close();
    S2Close();
    S3Close();
    MoveToTop();
    ClawClose();
    SetServoPos(SERVO_TAG_YUNTAI, 3700, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1500, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2000, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveToS1()
{
    MoveToTop();
    SetServoPos(SERVO_TAG_YUNTAI, 2450, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1100, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1500, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2150, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromS1()
{
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToS2()
{
    MoveToTop();
    SetServoPos(SERVO_TAG_YUNTAI, 2850, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1100, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1500, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2150, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromS2()
{
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToS3()
{
    MoveToTop();
    SetServoPos(SERVO_TAG_YUNTAI, 3250, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2900, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1250, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2800, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1250, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2650, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromS3()
{
    SetServoPos(SERVO_TAG_L1, 1250, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2800, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 1300, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2900, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::SpinToRight()
{
    SetServoPos(SERVO_TAG_YUNTAI, 550, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::SpinToLeft()
{
    SetServoPos(SERVO_TAG_YUNTAI, 2590, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveToHL1()
{
    SetServoPos(SERVO_TAG_L1, 1490, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2300, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2211, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1777, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2529, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1780, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHL1()
{
    SetServoPos(SERVO_TAG_L1, 2211, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1777, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToHL2()
{
    SetServoPos(SERVO_TAG_L1, 1833, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1908, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2252, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1891, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHL2()
{
    SetServoPos(SERVO_TAG_L1, 1833, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1908, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToHM1()
{
    SetServoPos(SERVO_TAG_L1, 1490, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2300, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2200, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2533, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1573, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHM1()
{
    SetServoPos(SERVO_TAG_L1, 2200, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1500, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToHM2()
{
    SetServoPos(SERVO_TAG_L1, 1800, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1828, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2179, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1713, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHM2()
{
    SetServoPos(SERVO_TAG_L1, 1800, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1828, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToHR1()
{
    SetServoPos(SERVO_TAG_L1, 1490, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2300, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2200, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1500, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2513, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1724, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHR1()
{
    SetServoPos(SERVO_TAG_L1, 2200, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1700, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToHR2()
{
    SetServoPos(SERVO_TAG_L1, 1833, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1908, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2233, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1845, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromHR2()
{
    SetServoPos(SERVO_TAG_L1, 1833, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1908, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}

void ServoDriver::MoveToP1()
{
    SetServoPos(SERVO_TAG_L1, 1700, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1800, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2084, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1762, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromP1()
{

}

void ServoDriver::SpinToHL()
{
    SetServoPos(SERVO_TAG_YUNTAI, 178, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::SpinToHM()
{
    SetServoPos(SERVO_TAG_YUNTAI, 528, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::SpinToHR()
{
    SetServoPos(SERVO_TAG_YUNTAI, 873, servoSpeed);
    UpdateServoPos();
}



void ServoDriver::MoveRightToFindLine()
{
    MoveToTop();
    SpinToRight();
    SetServoPos(SERVO_TAG_L1, 1700, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2900, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveLeftToFindLine()
{
    MoveToTop();
    SpinToLeft();
    SetServoPos(SERVO_TAG_L1, 2000, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2900, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveRightToFindCircle()
{
    MoveToTop();
    SpinToRight();
    ClawOpen2();
    SetServoPos(SERVO_TAG_L1, 1900, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2500, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveRightToFindObject()
{
    MoveToTop();
    SpinToRight();
    SetServoPos(SERVO_TAG_L1, 1900, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 2800, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveRightToFindQrcode()
{
    MoveToTop();
    SpinToRight();
    SetServoPos(SERVO_TAG_L1, 1891, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1730, servoSpeed);
    UpdateServoPos();
    SetServoPos(SERVO_TAG_L1, 2284, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1379, servoSpeed);
    UpdateServoPos();
}

void ServoDriver::MoveFromRightToFindQrcode()
{
    SpinToRight();
    SetServoPos(SERVO_TAG_L1, 1891, servoSpeed);
    SetServoPos(SERVO_TAG_L2, 1730, servoSpeed);
    UpdateServoPos();
    MoveToTop();
}
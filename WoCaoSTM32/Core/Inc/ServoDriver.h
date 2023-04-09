#ifndef SERVODRIVER_H
#define SERVODRIVER_H

#include "main.h"

#ifdef __cplusplus
#include <cmath>
#include <cstring>
#include <cstdio>
#endif


enum SERVO_TAG
{
    SERVO_TAG_YUNTAI = 0, //顺时针
    SERVO_TAG_L1 = 1, //顺时针
    SERVO_TAG_L2 = 2, //顺时针
    SERVO_TAG_CLAW = 3,
    SERVO_TAG_S1 = 6,
    SERVO_TAG_S2 = 7,
    SERVO_TAG_S3 = 8,
};

#ifdef __cplusplus
class ServoDriver
{
public:
    void HandleServoPackage(uint8_t* data, int packageLen);
    void SetServoSpeed(uint16_t speed);
    void SetServoPos(enum SERVO_TAG servoTag, uint16_t pos, uint16_t speed);
    uint16_t GetServoPos(enum SERVO_TAG servoTag);
    void UpdateServoPos();
    void S1Open();
    void S2Open();
    void S3Open();
    void S1Close();
    void S2Close();
    void S3Close();
    void ClawOpen();
    void ClawOpen2();
    void ClawClose();
    void MoveToTop();
    void Init();
    void MoveToS1();
    void MoveFromS1();
    void MoveToS2();
    void MoveFromS2();
    void MoveToS3();
    void MoveFromS3();
    void SpinToRight();
    void SpinToLeft();
    void SpinToHL();
    void SpinToHM();
    void SpinToHR();
    void MoveToHL1();
    void MoveFromHL1();
    void MoveToHL2();
    void MoveFromHL2();
    void MoveToHM1();
    void MoveFromHM1();
    void MoveToHM2();
    void MoveFromHM2();
    void MoveToHR1();
    void MoveFromHR1();
    void MoveToHR2();
    void MoveFromHR2();
    void MoveToP1();
    void MoveFromP1();

    void MoveRightToFindLine();
    void MoveLeftToFindLine();
    void MoveRightToFindCircle();
    void MoveRightToFindObject();
    void MoveRightToFindQrcode();
    void MoveFromRightToFindQrcode();

private:
    uint16_t yuntaiAngle = 0;
    uint16_t l1Angle = 0;
    uint16_t l2Angle = 0;
    uint16_t clawAngle = 0;
    uint16_t s1Angle = 0;
    uint16_t s2Angle = 0;
    uint16_t s3Angle = 0;
    uint16_t yuntaiPos = 0;
    uint16_t l1Pos = 0;
    uint16_t l2Pos = 0;
    uint16_t clawPos = 0;
    uint16_t s1Pos = 0;
    uint16_t s2Pos = 0;
    uint16_t s3Pos = 0;
    uint16_t servoSpeed = 0;
    uint8_t CheckSum(uint8_t startNum, uint8_t endNum, uint8_t* inData);
};

#endif



#endif
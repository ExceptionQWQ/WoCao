#ifndef ROBOTMOTION_H
#define ROBOTMOTION_H

#include "main.h"
#include "WheelPWM.h"
#include "OpenMVDriver.h"
#include "MediumBlur.h"

#ifdef __cplusplus
#include <cmath>
#include <cstring>
#include <cstdio>
#endif

#ifdef __cplusplus
extern OpenMVDriver openMVDriver;
#endif

#ifdef __cplusplus

class RobotMotion
{
public:
    double wheelFrontLeftSpeed = 0;
    double wheelFrontRightSpeed = 0;
    double wheelBackLeftSpeed = 0;
    double wheelBackRightSpeed = 0;

    void Init();
    void ClearWheelSpeed();
    void UpdateWheelSpeed();
    void Stop();
    void KeepHrizRight(MediumBlur& mediumBlur);
    void KeepVertRight(MediumBlur& mediumBlur);
    void KeepHrizLeft(MediumBlur& mediumBlur);
    void KeepVertLeft(MediumBlur& mediumBlur);
    void MoveForward(double speed);
    void MoveBackward(double speed);
    void MoveLeft(double speed);
    void MoveRight(double speed);
    void MoveForwardWithDis(double speed, double dis); //blocking
    void MoveBackwardWithDis(double speed, double dis); //blocking
    void MoveForwardWithDisAndFindLeftLine(double speed, double dis); //blocking
    void MoveForwardUntilCornerWithFindLeftLine(double speed); //blocking
    void MoveLeftWithDis(double speed, double dis); //blocking
    void MoveRightWithDis(double speed, double dis); //blocking
    void MoveXYWithDis(double speed, double x, double y); //blocking
    void TurnRight(double speed); //blocking
    void TurnLeft(double speed); //blocking
    void TrackCenterCircle(enum OPENMV_CMD cmd, int eps = 5); //blocking
    void TrackCenterObject(); //blocking
};

#endif


#endif
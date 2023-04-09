#include "WheelPWM.h"

WheelPWM wheelFrontLeft;
WheelPWM wheelFrontRight;
WheelPWM wheelBackLeft;
WheelPWM wheelBackRight;

WheelPWM::WheelPWM()
{
    wheelSpeed = 0;
}
double WheelPWM::GetWheelSpeed()
{
    return this->lastWheelSpeed;
}
double WheelPWM::SetTargetSpeed(double tSpeed)
{
    this->targetSpeed = tSpeed;
    return this->targetSpeed;
}
void WheelPWM::AddSpeed()
{
    ++this->dis;
    ++this->wheelSpeed;
}
void WheelPWM::SubSpeed()
{
    --this->dis;
    --this->wheelSpeed;
}
double WheelPWM::GetPID()
{
    return this->pid;
}
void WheelPWM::Tick()
{
    this->errorP = this->targetSpeed - this->wheelSpeed;
    this->errorI += this->errorP;
    this->errorD = this->errorP - this->errorL;
    this->errorL = this->errorP;
    this->pid = P * this->errorP + I * this->errorI + D * this->errorD;
    this->lastWheelSpeed = this->wheelSpeed;
    this->wheelSpeed = 0;
}

void WheelReset(enum WHEEL_TAG tag)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            wheelFrontLeft.wheelSpeed = 0;
            wheelFrontLeft.errorP = 0;
            wheelFrontLeft.errorI = 0;
            wheelFrontLeft.errorD = 0;
            wheelFrontLeft.errorL = 0;
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            wheelFrontRight.wheelSpeed = 0;
            wheelFrontRight.errorP = 0;
            wheelFrontRight.errorI = 0;
            wheelFrontRight.errorD = 0;
            wheelFrontRight.errorL = 0;
            break;
        case WHEEL_TAG_BACK_LEFT:
            wheelBackLeft.wheelSpeed = 0;
            wheelBackLeft.errorP = 0;
            wheelBackLeft.errorI = 0;
            wheelBackLeft.errorD = 0;
            wheelBackLeft.errorL = 0;
            break;
        case WHEEL_TAG_BACK_RIGHT:
            wheelBackRight.wheelSpeed = 0;
            wheelBackRight.errorP = 0;
            wheelBackRight.errorI = 0;
            wheelBackRight.errorD = 0;
            wheelBackRight.errorL = 0;
            break;
        default:
            break;
    }
}
void ClearWheelDis(enum WHEEL_TAG tag)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            wheelFrontLeft.dis = 0;
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            wheelFrontRight.dis = 0;
            break;
        case WHEEL_TAG_BACK_LEFT:
            wheelBackLeft.dis = 0;
            break;
        case WHEEL_TAG_BACK_RIGHT:
            wheelBackRight.dis = 0;
            break;
        default:
            break;
    }
    return ;
}
double GetWheelDis(enum WHEEL_TAG tag)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            return wheelFrontLeft.dis;
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            return wheelFrontRight.dis;
            break;
        case WHEEL_TAG_BACK_LEFT:
            return wheelBackLeft.dis;
            break;
        case WHEEL_TAG_BACK_RIGHT:
            return wheelBackRight.dis;
            break;
        default:
            return 0;
            break;
    }
    return 0;
}


double GetWheelPID(WHEEL_TAG tag)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            return wheelFrontLeft.GetPID();
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            return wheelFrontRight.GetPID();
            break;
        case WHEEL_TAG_BACK_LEFT:
            return wheelBackLeft.GetPID();
            break;
        case WHEEL_TAG_BACK_RIGHT:
            return wheelBackRight.GetPID();
            break;
        default:
            return 0;
            break;
    }
    return 0;
}
double GetWheelSpeed(WHEEL_TAG tag)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            return wheelFrontLeft.GetWheelSpeed();
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            return wheelFrontRight.GetWheelSpeed();
            break;
        case WHEEL_TAG_BACK_LEFT:
            return wheelBackLeft.GetWheelSpeed();
            break;
        case WHEEL_TAG_BACK_RIGHT:
            return wheelBackRight.GetWheelSpeed();
            break;
        default:
            return 0;
            break;
    }
    return 0;
}
double SetWheelSpeed(WHEEL_TAG tag, double speed)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            return wheelFrontLeft.SetTargetSpeed(speed);
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            return wheelFrontRight.SetTargetSpeed(speed);
            break;
        case WHEEL_TAG_BACK_LEFT:
            return wheelBackLeft.SetTargetSpeed(speed);
            break;
        case WHEEL_TAG_BACK_RIGHT:
            return wheelBackRight.SetTargetSpeed(speed);
            break;
        default:
            return 0;
            break;
    }
    return 0;
}

void HandleABInterupt(WheelPWM& wheelPwm, WHEEL_AB_TAG ab, int updown)
{
    if (ab == WHEEL_AB_TAG_A && updown == 1) {
        wheelPwm.aStatus = true;
    } else if (ab == WHEEL_AB_TAG_A && updown == 0) {
        wheelPwm.aStatus = false;
    } else if (ab == WHEEL_AB_TAG_B && updown == 1) {
        wheelPwm.bStatus = true;
    } else if (ab == WHEEL_AB_TAG_B && updown == 0) {
        wheelPwm.bStatus = false;
    }
    if (ab == WHEEL_AB_TAG_A && wheelPwm.aStatus && wheelPwm.bStatus) {
        wheelPwm.AddSpeed();
    } else if (ab == WHEEL_AB_TAG_A && !wheelPwm.aStatus && !wheelPwm.bStatus) {
        wheelPwm.AddSpeed();
    } else if (ab == WHEEL_AB_TAG_B && wheelPwm.bStatus && wheelPwm.aStatus) {
        wheelPwm.SubSpeed();
    } else if (ab == WHEEL_AB_TAG_B && !wheelPwm.bStatus && !wheelPwm.aStatus) {
        wheelPwm.SubSpeed();
    }
}

void HandleWheelInterupt(WHEEL_TAG tag, WHEEL_AB_TAG ab, int updown)
{
    switch (tag) {
        case WHEEL_TAG_FRONT_LEFT:
            HandleABInterupt(wheelFrontLeft, ab, updown);
            break;
        case WHEEL_TAG_FRONT_RIGHT:
            HandleABInterupt(wheelFrontRight, ab, updown);
            break;
        case WHEEL_TAG_BACK_LEFT:
            HandleABInterupt(wheelBackLeft, ab, updown);
            break;
        case WHEEL_TAG_BACK_RIGHT:
            HandleABInterupt(wheelBackRight, ab, updown);
            break;
        default:
            break;
    }
}
void WheelTick()
{
    wheelFrontLeft.Tick();
    wheelFrontRight.Tick();
    wheelBackLeft.Tick();
    wheelBackRight.Tick();
}
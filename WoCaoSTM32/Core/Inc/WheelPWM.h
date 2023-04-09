#ifndef WHEELPWM_H
#define WHEELPWM_H

enum WHEEL_TAG
{
    WHEEL_TAG_FRONT_LEFT = 0,
    WHEEL_TAG_FRONT_RIGHT = 1,
    WHEEL_TAG_BACK_LEFT = 2,
    WHEEL_TAG_BACK_RIGHT = 3,
};

enum WHEEL_AB_TAG
{
    WHEEL_AB_TAG_A = 0,
    WHEEL_AB_TAG_B = 1,
};

#ifdef __cplusplus

class WheelPWM
{
public:
    double P = 40;
    double I = 5;
    double D = 0;
    double dis = 0;
    double lastWheelSpeed = 0;
    double wheelSpeed = 0;
    double targetSpeed = 0;
    double errorP = 0;
    double errorI = 0;
    double errorD = 0;
    double errorL = 0;
    double pid = 0;
    bool aStatus = false;
    bool bStatus = false;

    WheelPWM();
    double GetWheelSpeed();
    double SetTargetSpeed(double tSpeed);
    double GetPID();
    void Tick();
    void AddSpeed();
    void SubSpeed();
};

extern WheelPWM wheelFrontLeft;
extern WheelPWM wheelFrontRight;
extern WheelPWM wheelBackLeft;
extern WheelPWM wheelBackRight;

#endif


#ifdef __cplusplus
extern "C"
{
#endif
void WheelReset(enum WHEEL_TAG tag);
void ClearWheelDis(enum WHEEL_TAG tag);
double GetWheelDis(enum WHEEL_TAG tag);
double GetWheelPID(enum WHEEL_TAG tag);
double GetWheelSpeed(enum WHEEL_TAG);
double SetWheelSpeed(enum WHEEL_TAG tag, double speed);
void HandleWheelInterupt(enum WHEEL_TAG tag, enum WHEEL_AB_TAG ab, int updown);
void WheelTick();
#ifdef __cplusplus
}
#endif




#endif
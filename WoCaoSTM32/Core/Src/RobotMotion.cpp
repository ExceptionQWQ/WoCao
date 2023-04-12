#include "RobotMotion.h"

OpenMVDriver openMVDriver;

void RobotMotion::Init()
{
    wheelFrontLeft.dis = 0;
    wheelFrontLeft.wheelSpeed = 0;
    wheelFrontRight.dis = 0;
    wheelFrontRight.wheelSpeed = 0;
    wheelBackLeft.dis = 0;
    wheelBackLeft.wheelSpeed = 0;
    wheelBackRight.dis = 0;
    wheelBackRight.wheelSpeed = 0;
}

void RobotMotion::ClearWheelSpeed()
{
    wheelFrontLeftSpeed = 0;
    wheelFrontRightSpeed = 0;
    wheelBackLeftSpeed = 0;
    wheelBackRightSpeed = 0;
}

void RobotMotion::UpdateWheelSpeed()
{
    SetWheelSpeed(WHEEL_TAG_FRONT_LEFT, wheelFrontLeftSpeed);
    SetWheelSpeed(WHEEL_TAG_FRONT_RIGHT, wheelFrontRightSpeed);
    SetWheelSpeed(WHEEL_TAG_BACK_LEFT, wheelBackLeftSpeed);
    SetWheelSpeed(WHEEL_TAG_BACK_RIGHT, wheelBackRightSpeed);
}

void RobotMotion::Stop()
{
    ClearWheelSpeed();
    UpdateWheelSpeed();
}

void RobotMotion::KeepHrizRight(MediumBlur& mediumBlur)
{
    int angle = mediumBlur.GetValue();
    angle += 0;
    if (angle >= 0 && angle <= 15) {
        angle *= 0.8;
        wheelFrontLeftSpeed += angle;
        wheelFrontRightSpeed += angle;
        wheelBackLeftSpeed += angle;
        wheelBackRightSpeed += angle;
    }
    if (angle <= 180 && angle >= 165) {
        angle = 180 - angle;
        angle *= 0.8;
        wheelFrontLeftSpeed += -angle;
        wheelFrontRightSpeed += -angle;
        wheelBackLeftSpeed += -angle;
        wheelBackRightSpeed += -angle;
    }
}

void RobotMotion::KeepVertRight(MediumBlur& mediumBlur)
{
    int dis = abs(mediumBlur.GetValue()) - 130;
    if (abs(dis) < 120) {
        dis *= 0.5;
        if (dis > 40) dis = 20;
        if (dis < -40) dis = -20;
        wheelFrontRightSpeed += dis;
        wheelBackLeftSpeed += -dis;
    }
}

void RobotMotion::KeepHrizLeft(MediumBlur& mediumBlur)
{
    int angle = mediumBlur.GetValue();
    angle += 0;
    if (angle >= 0 && angle <= 15) {
        angle *= 0.8;
        wheelFrontLeftSpeed += angle;
        wheelFrontRightSpeed += angle;
        wheelBackLeftSpeed += angle;
        wheelBackRightSpeed += angle;
    }
    if (angle <= 180 && angle >= 165) {
        angle = 180 - angle;
        angle *= 0.8;
        wheelFrontLeftSpeed += -angle;
        wheelFrontRightSpeed += -angle;
        wheelBackLeftSpeed += -angle;
        wheelBackRightSpeed += -angle;
    }
}

void RobotMotion::KeepVertLeft(MediumBlur& mediumBlur)
{;
    int dis = abs(mediumBlur.GetValue()) - 130;
    if (abs(dis) < 120) {
        dis *= 0.5;
        if (dis > 40) dis = 20;
        if (dis < -40) dis = -20;
        wheelFrontRightSpeed += dis;
        wheelBackLeftSpeed += -dis;
    }
}

void RobotMotion::MoveForward(double speed)
{
    wheelFrontLeftSpeed += speed;
    wheelBackRightSpeed += -speed;
}

void RobotMotion::MoveBackward(double speed)
{
    MoveForward(-speed);
}

void RobotMotion::MoveLeft(double speed)
{
    wheelFrontRightSpeed += -speed;
    wheelBackLeftSpeed += speed;
}

void RobotMotion::MoveRight(double speed)
{
    MoveLeft(-speed);
}

void RobotMotion::MoveForwardWithDis(double speed, double dis)
{
    ClearWheelDis(WHEEL_TAG_FRONT_LEFT);
    ClearWheelSpeed();
    MoveForward(speed);
    UpdateWheelSpeed();
    while (fabs(GetWheelDis(WHEEL_TAG_FRONT_LEFT)) < dis) {}
    Stop();
}

void RobotMotion::MoveBackwardWithDis(double speed, double dis)
{
    MoveForwardWithDis(-speed, dis);
}

void RobotMotion::MoveForwardWithDisAndFindLeftLine(double speed, double dis)
{
    MediumBlur hrizBlur(7), vertBlur(7);
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_LEFT_LINE, 3);
    ClearWheelDis(WHEEL_TAG_FRONT_LEFT);
    int tick = openMVDriver.GetUpdateTick(), cnt = 0;
    double targetSpeed = 5; //初始速度为5
    bool flag = true;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        std::pair<int, int> ret = openMVDriver.GetPair();
        if (tick != tick2) {
            cnt = 0;
            hrizBlur.Append(ret.first); vertBlur.Append(ret.second);
        }
        tick = tick2;

        if (flag) { //起步加速
            targetSpeed += 0.5;
            if (targetSpeed > speed) targetSpeed = speed;
            double remainX = (std::pow(targetSpeed, 2) - std::pow(5, 2)) / (0.16); //计算出减速距离
            if (dis - fabs(GetWheelDis(WHEEL_TAG_FRONT_LEFT)) < remainX) flag = false;
        } else { //终点减速
            targetSpeed -= 0.5;
            if (targetSpeed < 5) targetSpeed = 5;
        }

        if (cnt < 4 || true) {
            cnt += 1;
            char buff[1024] = {};
            snprintf(buff, 1024, "angle:%d, dis:%d\r\n", ret.first, ret.second);
            HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);

            ClearWheelSpeed();
            MoveForward(targetSpeed);
            KeepHrizLeft(hrizBlur);
            KeepVertLeft(vertBlur);
            UpdateWheelSpeed();
        } else {
            ClearWheelSpeed();
            MoveForward(targetSpeed);
            UpdateWheelSpeed();
        }
        if (fabs(GetWheelDis(WHEEL_TAG_FRONT_LEFT)) > dis) break;
        HAL_Delay(5);
    }
    Stop();
}

void RobotMotion::MoveForwardUntilCornerWithFindLeftLine(double speed)
{
    int emptyCnt = 0, lastTick = -1;
    MediumBlur hrizBlur(7), vertBlur(7);
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_LEFT_LINE, 3);

    while (true) {
        std::pair<int, int> ret = openMVDriver.GetPair();
        hrizBlur.Append(ret.first); vertBlur.Append(ret.second);

        char buff[1024] = {};
        snprintf(buff, 1024, "angle:%d, dis:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);

        ClearWheelSpeed();
        MoveForward(speed);
        KeepHrizLeft(hrizBlur);
        KeepVertLeft(vertBlur);
        UpdateWheelSpeed();
        if (openMVDriver.GetUpdateTick() == lastTick) emptyCnt += 1;
        else emptyCnt = 0;
        if (emptyCnt > 40) break; //稳定后就跳出循环
        lastTick = openMVDriver.GetUpdateTick();
        HAL_Delay(5);
    }
    Stop();
}


void RobotMotion::MoveLeftWithDis(double speed, double dis)
{
    ClearWheelDis(WHEEL_TAG_BACK_LEFT);
    ClearWheelSpeed();
    MoveLeft(speed);
    UpdateWheelSpeed();
    while (fabs(GetWheelDis(WHEEL_TAG_BACK_LEFT)) < dis) {}
    Stop();
}

void RobotMotion::MoveRightWithDis(double speed, double dis)
{
    MoveLeftWithDis(-speed, dis);
}

void RobotMotion::MoveXYWithDis(double speed, double x, double y)
{
    ClearWheelDis(WHEEL_TAG_FRONT_LEFT);
    ClearWheelDis(WHEEL_TAG_BACK_LEFT);
    bool flagX = false, flagY = false;
    while (true) {
        ClearWheelSpeed();
        if (fabs(GetWheelDis(WHEEL_TAG_FRONT_LEFT)) < fabs(x)) {
            if (x > 0) {
                MoveForward(speed);
            } else {
                MoveBackward(speed);
            }
        } else {
            flagX = true;
        }
        if (fabs(GetWheelDis(WHEEL_TAG_BACK_LEFT)) < fabs(y)) {
            if (y > 0) {
                MoveLeft(speed);
            } else {
                MoveRight(speed);
            }
        } else {
            flagY = true;
        }
        UpdateWheelSpeed();
        if (flagX && flagY) break;
    }
    Stop();
}

void RobotMotion::TurnRight(double speed) //blocking
{
    ClearWheelDis(WHEEL_TAG_FRONT_LEFT);
    double dis = 34000;
    ClearWheelSpeed();
    wheelFrontLeftSpeed = speed;
    wheelFrontRightSpeed = speed;
    wheelBackLeftSpeed = speed;
    wheelBackRightSpeed = speed;
    UpdateWheelSpeed();
    while (fabs(GetWheelDis(WHEEL_TAG_FRONT_LEFT)) < dis) {}
    Stop();
}

void RobotMotion::TurnLeft(double speed) //blocking
{
    TurnRight(-speed);
}

void RobotMotion::TrackCenterCircle(enum OPENMV_CMD cmd, int eps)
{
    openMVDriver.SendSignal(cmd, 3);
    int tick = openMVDriver.GetUpdateTick();
    int targetX = 300 - 165, targetY = 233;
    MediumBlur xBlur(17), yBlur(17);
    double disScale = 1;
    int cnt = 0;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue;

        auto ret = openMVDriver.GetPair();
        if (ret.first == 0 || ret.second == 0) continue;

        xBlur.Append(ret.first); yBlur.Append(ret.second);

        char buff[1024] = {};
        snprintf(buff, 1024, "track dx:%d, dy:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);

        int offsetX = (xBlur.GetValue() - targetX) * disScale;
        int offsetY = (yBlur.GetValue() - targetY) * disScale;
        if (abs(xBlur.GetValue() - targetX) < eps && abs(yBlur.GetValue() - targetY) < eps) ++cnt;
        else cnt = 0;
        if (cnt > 200) break; //稳定后跳出
        MoveXYWithDis(3, offsetY, offsetX);
    }
}

void RobotMotion::TrackCenterObject()
{
    openMVDriver.SendSignal(OPENMV_CMD_DETECT_ALL_CIRCLE, 3);
    int tick = openMVDriver.GetUpdateTick();

    //TODO 需要添加检测坐标是否稳定

    int targetX = 60, targetY = 55;
    int disScale = 60;
    int cnt = 0;
    while (true) {
        int tick2 = openMVDriver.GetUpdateTick();
        if (tick == tick2) continue;
        tick = tick2;
        auto ret = openMVDriver.GetPair();
        if (ret.first == 0 || ret.second == 0) continue;
        int offsetX = (ret.first - targetX) * disScale;
        int offsetY = (ret.second - targetY) * disScale;
        if (abs(ret.first - targetX) < 8 && abs(ret.second - targetY) < 8) ++cnt;
        if (cnt > 10) break;
        MoveXYWithDis(8, offsetY, offsetX);
        char buff[1024] = {};
        snprintf(buff, 1024, "DFRC dx:%d, dy:%d\r\n", ret.first, ret.second);
        HAL_UART_Transmit(&huart5, reinterpret_cast<uint8_t*>(buff), strlen(buff), 100);
    }
}

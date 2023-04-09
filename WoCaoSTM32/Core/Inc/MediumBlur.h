#ifndef MEDIUMBLUR_H
#define MEDIUMBLUR_H

#ifdef __cplusplus
#include <vector>
#include <algorithm>
#include <iterator>
#endif

#ifdef __cplusplus

class MediumBlur
{
private:
    std::vector<int> data;
    std::vector<int> sorted;
    int kernelSize;
    void Sort();
public:
    MediumBlur();
    MediumBlur(int kernel);
    void SetKernel(int kernel);
    void Append(int value);
    int GetValue();
};

#endif

#endif
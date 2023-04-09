#include "MediumBlur.h"

void MediumBlur::Sort()
{
    sorted = data;
    std::sort(std::begin(sorted), std::end(sorted), std::less<int>());
}

MediumBlur::MediumBlur()
{

}

MediumBlur::MediumBlur(int kernel) : kernelSize {kernel}
{

}

void MediumBlur::SetKernel(int kernel)
{
    kernelSize = kernel;
}

void MediumBlur::Append(int value)
{
    if (data.size() >= kernelSize) {
        for (size_t i = 0; i < data.size() - 1; ++i) {
            data[i] = data[i + 1];
        }
        data.pop_back();
    }
    data.push_back(value);
    Sort();
}

int MediumBlur::GetValue()
{
    return sorted[sorted.size() >> 1];
}
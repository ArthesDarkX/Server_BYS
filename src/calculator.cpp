#include <iostream>

#include "calculator.h"

std::uint16_t Calculator::calcVector(const std::vector<std::uint16_t>& vector)
{
    std::uint16_t result{1};
    if (vector.empty())
    {
        result = 0;
    }

    for (auto numb : vector)
    {
        if (numb == 0) {
            result = 0;
            break;
        }
        if (result > std::numeric_limits<std::uint16_t>::max() / numb)
        {
            result = std::numeric_limits<std::uint16_t>::max();
        }
        else
        {
            result *= numb;
        }
    }
    return result;
}
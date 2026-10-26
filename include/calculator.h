#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

class Calculator
{
public:    
    std::uint16_t calcVector(const std::vector<std::uint16_t>& vector);
};
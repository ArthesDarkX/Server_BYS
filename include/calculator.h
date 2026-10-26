#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

class Calculator
{
private:
const std::uint32_t count_vectors;
std::vector<std::uint16_t> vectors_result;
public:
    Calculator(std::uint32_t count_vectors) : count_vectors(count_vectors) {};
    
    std::uint16_t calcVector(const std::vector<std::uint16_t>& vector);
};
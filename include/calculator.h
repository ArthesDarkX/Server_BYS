#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

class calculator
{
private:
const std::uint32_t count_vectors;
std::vector<std::uint16_t> vectors_result;
public:
    calculator(std::uint32_t count_vectors) : count_vectors(count_vectors) {};
    
    void check(const size_t vectors_count);

    std::pair<std::uint32_t, std::vector<std::uint16_t>> calc(const std::vector<std::vector<std::uint16_t>>& vectors);
};
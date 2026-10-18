#include "calculator.h"

void calculator::check(const size_t vectors_count)
{
    if (vectors_count != count_vectors) {
        throw std::invalid_argument("Передано меньше или больше векторов, чем требуется!");
    }
}

std::pair<std::uint32_t, std::vector<std::uint16_t>> calculator::calc(const std::vector<std::vector<std::uint16_t>>& vectors)
{
    vectors_result.clear();
    check(vectors.size());
    for (auto& vector : vectors)
    {
        std::uint16_t result{1};
        if (vector.empty())
        {
            result = 0;
            vectors_result.push_back(result);
            continue;
        }
        for (auto& numb : vector)
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
        vectors_result.push_back(result);
    }
    return std::make_pair(static_cast<std::uint32_t>(vectors_result.size()), vectors_result);
}
#pragma once

#include <random>

class Random {
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    // Function returns a random number between 0 and 1.
    static float Float()
    {
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }


    static float AlternatingOne() {

        if (s_Distribution(s_RandomEngine) % 2 == 0) {
            return -1.0f;
        }
        else {
            return 1.0f;
        }
    }


    // Function returns random number greater than x and smaller than y.
    static float InRange(float x, float y) {

        // Random number between 0 and 1.
        float n = Random::Float();

        return (x + n * (y - x));
    }

private:
    static std::mt19937 s_RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

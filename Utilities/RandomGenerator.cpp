#include "RandomGenerator.h"
int RandomGenerator::generate_int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine_);
}

double RandomGenerator::generate_double(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(engine_);
}
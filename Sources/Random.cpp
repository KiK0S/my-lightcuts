
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
    

float rand_between(float l, float r) {
    std::uniform_real_distribution<float> dist(l, r);
    return dist(gen);
}
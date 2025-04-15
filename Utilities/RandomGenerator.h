#include <random>

class RandomGenerator{
public:
    static RandomGenerator& get() {
        static RandomGenerator instance;
        return instance;
    };
    RandomGenerator() : engine_(std::random_device{}()) {}
    RandomGenerator(const RandomGenerator& other) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    ~RandomGenerator() = default;

    int generate_int(int min, int max);
 

    double generate_double(double min, double max);
  

private:
    std::default_random_engine engine_;
};
#pragma once

#include <random>
#include <cassert>

namespace internal {
    template <typename Floating,
        std::enable_if_t<std::is_floating_point<Floating>::value, int> = 0
    >
        Floating GetRandomValue(Floating min_range, Floating max_range) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<Floating> distribution(min_range, max_range);
        return distribution(rng);
    }
    template <typename Integer,
        std::enable_if_t<std::is_integral<Integer>::value, int> = 0
    >
        Integer GetRandomValue(Integer min_range, Integer max_range) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<Integer> distribution(min_range, max_range);
        return distribution(rng);
    }
}

template <typename T, typename D = double, std::enable_if_t<std::is_floating_point<T>::value || std::is_integral<T>::value, int> = 0>
class DifferentiableRandom {
public:
    DifferentiableRandom(T min, T max, T initial = 0) : min{ min }, max{ max }, value{ initial } {}
    ~DifferentiableRandom() {
        delete provider;
        provider = nullptr;
    }
    void AddDifferentiableLayer(T min, T max, T initial = 0) {
        ++level;
        if (provider) {
            provider->AddDifferentiableLayer(min, max, initial);
        } else {
            provider = new DifferentiableRandom<T, D>{ min, max, initial };
        }
    }
    void SetInitialValue(T val) {
        value = val;
    }
    T GetInitialValue() const {
        return initial_value;
    }
    T GetValue() const {
        return value;
    }
    void Advance(D step) {
        if (provider) {
            // Evaluate the derivative
            provider->Advance(step);
            initial_value = value;
            value += static_cast<T>(step * (provider->GetInitialValue() + 1.0 / static_cast<T>(level) * (provider->GetValue() - provider->GetInitialValue())));
            if (value < min)
                value = min;
            if (value > max)
                value = max;
        } else {
            value = internal::GetRandomValue<T>(min, max);
            initial_value = value;
        }
    }
private:
    DifferentiableRandom* provider = nullptr;
    int level{ 0 };
    T min;
    T max;
    T value;
    T initial_value;
};
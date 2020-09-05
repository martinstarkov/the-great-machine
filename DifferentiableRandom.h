#pragma once

#include <Engine/Utility.h>

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
    T initial_value{ 0 };
    T min;
    T max;
    T value;
};
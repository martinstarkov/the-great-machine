#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>

#define LOG(x) { std::cout << x << std::endl; }
#define LOG_(x) { std::cout << x; }

namespace internal {

}

namespace tgm {

    using Population = std::size_t;

    struct Genes {
        float death_chance;
        float replication_chance;
        float mutation_chance;
    };



}
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

    class Species {

    public:
        Species() = delete;
        Species(const std::string& name, Population population, Genes&& genes) : name{ name }, population{ population }, genes{ genes } {}

        const std::string& GetName() const {
            return name;
        }

        const Population GetPopulation() const {
            return population;
        }

        void Update() {

        }

    private:
        const std::string name;
        Population population;
        Genes genes;

    };

    class Ecosystem {

    public:
        Ecosystem() = default;

        template <typename ...TArgs>
        std::shared_ptr<Species> AddSpecies(TArgs&&... args) {
            auto species = std::make_shared<Species>(std::forward<TArgs&&>(args)...);
            all_species.emplace(species->GetName(), species);
            return species;
        }

        void RemoveSpecies(const std::string& name) {
            all_species.erase(name);
        }

        std::shared_ptr<Species> HasSpecies(const std::string& name) const {
            auto it = all_species.find(name);
            if (it != std::end(all_species)) {
                return it->second;
            }
            return nullptr;
        }

        void PrintAllSpecies() {
            for (auto& pair : all_species) {
                LOG(pair.first);
            }
        }

        void PrintSpeciesPopulations() {
            for (auto& pair : all_species) {
                LOG(pair.first << " : " << pair.second->GetPopulation());
            }
        }

        void Update() {
            for (auto& pair : all_species) {
                pair.second->Update();
            }
        }
    
    private:
        std::unordered_map<std::string, std::shared_ptr<Species>> all_species;
    };

}


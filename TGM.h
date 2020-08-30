#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include <cassert>

#define LOG(x) { std::cout << x << std::endl; }
#define LOG_(x) { std::cout << x; }

namespace tgm {

    using Population = std::size_t;
    
    class Ecosystem;

    namespace internal {

        Population BinomialSample(Population trials, float probability_of_success) {
            std::random_device seed;
            std::mt19937 gen(seed());
            std::binomial_distribution<Population> distribution(trials, probability_of_success);
            Population result = distribution(gen);
            assert(result <= trials);
            return result;
        }

        class MonteCarlo {

        public:
            MonteCarlo(Ecosystem& ecosystem, std::size_t trials, std::size_t steps);

        private:

        };


    } //namespace internal

    enum class State {
        FLUCTUATING,
        EXTINCT,
        CAPPED
    };

    struct Genes {
        float replication_chance;
        float death_chance;
        float mutation_chance;
    };

    class Species {

    public:
        Species() = delete;
        Species(const std::string& name, Population population, Genes&& genes) : name{ name }, population{ population }, genes{ genes }, mutated{ 0 } {}

        const std::string& GetName() const {
            return name;
        }

        const Population GetPopulation() const {
            return population;
        }

        const Population GetMutated() const {
            return mutated;
        }

        void Update() {
            ModifyPopulation();
        }

    private:

        void ModifyPopulation() {
            Population growth = internal::BinomialSample(population, genes.replication_chance);
            Population mutation = internal::BinomialSample(growth, genes.mutation_chance);
            Population decay = internal::BinomialSample(population, genes.death_chance);
            population += growth - mutation - decay;
            mutated += mutation;
        }

        const std::string name;
        Population population;
        Genes genes;
        Population mutated;

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

        const State GetState() const {
            return state;
        }

        void PrintAllSpecies() {
            for (auto& pair : all_species) {
                LOG(pair.first);
            }
        }

        void PrintSpeciesPopulations() {
            for (auto& pair : all_species) {
                LOG(pair.first << " : " << pair.second->GetPopulation() << ", Mutated: " << pair.second->GetMutated());
            }
        }

        void Update() {
            for (auto& pair : all_species) {
                pair.second->Update();
            }
            RecalculateState();
        }
    
    private:

        void RecalculateState() {
            for (auto& pair : all_species) {
                if (pair.second->GetPopulation() > 5000) {
                    state = State::CAPPED;
                    return;
                }
                else if (pair.second->GetPopulation() == 0) {
                    state = State::EXTINCT;
                    return;
                }
            }
            state = State::FLUCTUATING;
        }

        std::unordered_map<std::string, std::shared_ptr<Species>> all_species;
        State state{ State::FLUCTUATING };
    };

    internal::MonteCarlo::MonteCarlo(tgm::Ecosystem& ecosystem, std::size_t trials, std::size_t steps) {
        std::unordered_map<tgm::State, std::size_t> state_map;
        for (std::size_t i{ 0 }; i < trials; ++i) {
            for (std::size_t counter{ 0 }; counter < steps; ++counter) {
                ecosystem.Update();
                if (ecosystem.GetState() != tgm::State::FLUCTUATING) {
                    break;
                }
            }
            tgm::State final_state{ ecosystem.GetState() };
            auto it = state_map.find(final_state);
            if (it != std::end(state_map)) {
                ++(it->second);
            }
            else {
                state_map.emplace(final_state, 1);
            }
            //LOG(i);
        }
        for (auto& pair : state_map) {
            LOG(static_cast<int>(pair.first) << ": " << pair.second);
        }
    }

} //namespace tgm
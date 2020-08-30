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
    using SpeciesID = std::size_t;
    using ResourceIDs = std::vector<SpeciesID>;
    using Resources = std::vector<std::string>;
    
    class Ecosystem;

    constexpr SpeciesID NULL_SPECIES = 0;

    namespace internal {

        Population BinomialSample(Population trials, float probability_of_success) {
            assert(probability_of_success >= 0);
            assert(probability_of_success <= 1);
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
        Genes() = default;
        float replication_chance;
        float death_chance;
        float mutation_chance;
        float crowding_factor;
    };

    class Species {

    public:
        Species() = delete;
        Species(Ecosystem& ecosystem, SpeciesID id, const std::string& name, Population population, Genes&& genes, const Resources& resources);
     

        const std::string& GetName() const {
            return name;
        }

        const Population GetPopulation() const {
            return population;
        }

        const Population GetMutated() const {
            return mutated;
        }

        const ResourceIDs GetResources() const {
            return resources;
        }

        const SpeciesID GetID() const {
            return id;
        }

        void Update() {
            ModifyPopulation();
        }

    private:

        void ModifyPopulation() {
            Population density_death = internal::BinomialSample(population, genes.crowding_factor * population);
            Population growth = internal::BinomialSample(population, genes.replication_chance);
            Population mutation = internal::BinomialSample(growth, genes.mutation_chance);
            Population decay = internal::BinomialSample(population, genes.death_chance);
            population += growth - mutation - decay - density_death;
            mutated += mutation;
        }

        const std::string name;
        Population population;
        Genes genes;
        Population mutated;
        ResourceIDs resources;
        Ecosystem& ecosystem;
        SpeciesID id;

    };

    class Ecosystem {

    public:
        Ecosystem() : state{ State::FLUCTUATING }, total_population{ 0 } {
            all_species.emplace_back(std::make_unique<Species>(*this, NULL_SPECIES, "null species", 0, Genes{}, Resources{}));
        }

        template <typename ...TArgs>
        Species& AddSpecies(TArgs&&... args) {
            const std::string& name = ExtractName(std::forward<TArgs&&>(args)...);
            assert(!HasSpecies(name));
            static SpeciesID last_id{ NULL_SPECIES };
            all_species.emplace_back(std::make_unique<Species>(*this, ++last_id, std::forward<TArgs&&>(args)...));
            id_map.emplace(all_species[last_id]->GetName(), last_id);
            assert(last_id < all_species.size());
            return *all_species[last_id];
        }

        void RemoveSpecies(const std::string& name) {
            auto id = GetSpeciesID(name);
            all_species[id] = nullptr;
        }

        bool HasSpecies(const std::string& name) const {
            return GetSpeciesID(name) != NULL_SPECIES;
        }

        Species& GetSpecies(const std::string& name) const {
            auto id = GetSpeciesID(name);
            assert(HasSpecies(id));
            return *all_species[id];
        }

        const State GetState() const {
            return state;
        }

        const Population GetPopulation() const {
            return total_population;
        }

        const SpeciesID GetSpeciesID(const std::string& name) const {
            auto it = id_map.find(name);
            if (it != std::end(id_map)) {
                auto id = it->second;
                assert(id < all_species.size());
                return id;
            }
            return NULL_SPECIES;
        }

        void PrintAllSpecies() {
            for (auto& element : all_species) {
                LOG(element->GetName());
            }
        }

        void PrintSpeciesPopulations() {
            for (auto& element : all_species) {
                LOG(element->GetName() << " : " << element->GetPopulation() << ", Mutated: " << element->GetMutated());
            }
        }
        
        /*
        std::vector<Species&> GetCompetitors(const Species& species) {
            for (auto& resource : species.GetResources()) {
                for (auto& element : all_species) {
                }
            }
        }
        */

        void Update() {
            total_population = 0;
            for (auto& element : all_species) {
                element->Update();
                total_population += element->GetPopulation();
            }
            RecalculateState();
        }
    
    private:

        void RecalculateState() {
            for (auto& element : all_species) {
                if (element->GetPopulation() > 5000) {
                    state = State::CAPPED;
                    return;
                }
                else if (element->GetPopulation() == 0) {
                    state = State::EXTINCT;
                    return;
                }
            }
            state = State::FLUCTUATING;
        }

        bool HasSpecies(const SpeciesID id) const {
            return id != NULL_SPECIES && id < all_species.size();
        }

        template <typename T, typename ...TArgs>
        std::string ExtractName(T name, TArgs&&... args) {
            static_assert(std::is_same_v<T, const char*>);
            return name;
        }

        std::unordered_map<std::string, SpeciesID> id_map;
        std::vector<std::unique_ptr<Species>> all_species;
        State state;
        Population total_population;
    };

    Species::Species(Ecosystem& ecosystem, SpeciesID id, const std::string& name, Population population, Genes&& genes, const Resources& resources) : ecosystem{ ecosystem }, id{ id }, name{ name }, population{ population }, genes{ genes }, mutated{ 0 } {
        for (auto& element : resources) {
            SpeciesID id = ecosystem.GetSpeciesID(element);
            assert(id != NULL_SPECIES);
            this->resources.emplace_back(id);
        }
    }

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
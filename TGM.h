#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include <cassert>
#include <map>

#define LOG(x) { std::cout << x << std::endl; }
#define LOG_(x) { std::cout << x; }

namespace tgm {

    using Population = std::size_t;
    using SpeciesID = std::size_t;
    using ResourceBitset = std::vector<bool>;
    using Resources = std::vector<std::string>;
    
    class Ecosystem;

    constexpr SpeciesID NULL_SPECIES = 0;
    constexpr SpeciesID FIRST_VALID_SPECIES = NULL_SPECIES + 1;

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
            // TODO: Store some properties here later and allow printing them on request?
        };


    } //namespace internal

    enum class State {
        FLUCTUATING,
        EXTINCT,
        CAPPED
    };

    // Allows for printing states as long as you add new states to the INSERT_ELEMENT list
    std::ostream& operator<<(std::ostream& out, const State value) {
        static std::map<State, std::string> strings;
        if (strings.size() == 0) {
            #define INSERT_ELEMENT(p) strings[p] = #p
            INSERT_ELEMENT(State::FLUCTUATING);
            INSERT_ELEMENT(State::EXTINCT);
            INSERT_ELEMENT(State::CAPPED);
            #undef INSERT_ELEMENT
        }
        return out << strings[value];
    }

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
        Species(Ecosystem& ecosystem, SpeciesID id, const std::string& name, Population population, Genes&& genes, const Resources& resource_names) : ecosystem{ ecosystem }, id{ id }, name{ name }, original_population{ population }, population{ original_population }, original_genes{ genes }, genes{ original_genes }, mutated{ 0 }, death_count{ 0 } {
            AddInitialResources(resource_names);
        }
     
        const std::string& GetName() const {
            return name;
        }

        const Genes& GetGenes() const {
            return genes;
        }

        const Population GetPopulation() const {
            return population;
        }

        const Population GetOriginalPopulation() const {
            return original_population;
        }

        const Population GetMutated() const {
            return mutated;
        }

        const Population GetDeathCount() const {
            return death_count;
        }

        const auto GetNetGrowth() const {
            return static_cast<std::int64_t>(population - original_population);
        }

        const Population GetTheoreticalCarryingCapacity() const {
            float carrying_capacity = (genes.replication_chance * (1.0f - genes.mutation_chance) - genes.death_chance) / genes.crowding_factor;
            // if carrying capacity is negative, it the population can theoretically not support any life
            return carrying_capacity <= 0 ? 0 : static_cast<Population>(roundf(carrying_capacity));
        }

        void Reset() {
            population = original_population;
            genes = original_genes;
            death_count = 0;
            mutated = 0;
        }

        const ResourceBitset& GetResources() const {
            return resources;
        }

        bool HasResource(SpeciesID species_id) const {
            if (species_id < resources.size()) {
                return resources[species_id];
            }
            return false;
        }

        void AddResource(const std::string& name);

        void AddInitialResources(const Resources& resource_names);

        const SpeciesID GetID() const {
            return id;
        }

        void Update() {
            ModifyPopulation();
        }

    private:

        float GetWeightedCrowdingFactor();

        void ModifyPopulation() {
            // Regular crowding_factor
            //float crowding_factor = genes.crowding_factor * population;
            // Crowding_factors of all competitors weighted by their populations
            float w_crowding_factor = GetWeightedCrowdingFactor();
            Population crowding_coefficient = internal::BinomialSample(population, w_crowding_factor);
            Population growth = internal::BinomialSample(population, genes.replication_chance);
            Population mutation = internal::BinomialSample(growth, genes.mutation_chance);
            Population decay = internal::BinomialSample(population, genes.death_chance);
            Population net_growth = growth - mutation - crowding_coefficient;
            population += net_growth - decay;
            mutated += mutation;
            death_count += decay;
        }

        Ecosystem& ecosystem;
        SpeciesID id;
        const std::string name;
        const Population original_population;
        Population population;
        Population death_count;
        Population mutated;
        const Genes original_genes;
        Genes genes;
        ResourceBitset resources;

    };

    class Ecosystem {

    public:
        Ecosystem() : state{ State::FLUCTUATING }, total_population{ 0 } {
            all_species.emplace_back(std::make_unique<Species>(*this, NULL_SPECIES, "NULL_SPECIES", 0, Genes{}, Resources{}));
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
            for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                auto& species = *all_species[i];
                LOG(species.GetName());
            }
        }

        void PrintSpeciesStatus(const std::string& name) {
            SpeciesID id = GetSpeciesID(name);
            assert(HasSpecies(id));
            PrintSpeciesStatus(*all_species[id]);
        }

        void PrintAllSpeciesStatuses() {
            for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                auto& species = *all_species[i];
                LOG("---------------------");
                PrintSpeciesStatus(species);
            }
            LOG("---------------------");
        }
        
        auto GetCompetitors(const Species& species) {
            std::vector<std::reference_wrapper<Species>> competitors;
            const ResourceBitset& resources = species.GetResources();
            SpeciesID id = species.GetID();
            for (SpeciesID i = FIRST_VALID_SPECIES; i < resources.size(); ++i) {
                for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                    auto& species = *all_species[i];
                    // Do not add self to competitors
                    if (species.GetID() != id) {
                        if (species.HasResource(i)) {
                            competitors.emplace_back(species);
                        }
                    }
                }
            }
            return competitors;
        }

        void Update() {
            total_population = 0;
            for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                auto& species = *all_species[i];
                species.Update();
                total_population += species.GetPopulation();
            }
            RecalculateState();
        }

        void Reset() {
            for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                auto& species = *all_species[i];
                species.Reset();
            }
        }
    
    private:

        void PrintSpeciesStatus(const Species& species) {
            LOG("Name : " << species.GetName());
            LOG("Population : " << species.GetPopulation());
            LOG("Net Growth : " << species.GetNetGrowth());
            LOG("Mutated: " << species.GetMutated());
            LOG("Total Deaths: " << species.GetDeathCount());
            LOG("Theoretical Carrying Capacity : " << species.GetTheoreticalCarryingCapacity());
        }

        void RecalculateState() {
            for (SpeciesID i = FIRST_VALID_SPECIES; i < all_species.size(); ++i) {
                auto& species = *all_species[i];
                auto cap = species.GetTheoreticalCarryingCapacity();
                // TODO: Figure out something better for checking that population is not already at carrying capacity
                // cap exists and population starts below 80% of cap
                if (cap && species.GetOriginalPopulation() < cap * 0.8) {
                    if (species.GetPopulation() > cap) {
                        state = State::CAPPED;
                        return;
                    }
                } else if (species.GetPopulation() == 0) {
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

    void Species::AddInitialResources(const Resources& resource_names) {
        // Only run this function if resources has not been initialized otherwise it could resize to a smaller size
        assert(resources.size() == 0);
        if (resource_names.size() > 0) {
            std::vector<SpeciesID> ids;
            SpeciesID max_id = NULL_SPECIES;
            // Loop through names, add ids to vector for later use and figure out the highest id
            for (const auto& element : resource_names) {
                SpeciesID species_id = ecosystem.GetSpeciesID(element);
                assert(species_id != NULL_SPECIES);
                max_id = std::max(species_id, max_id);
                ids.push_back(species_id);
            }
            // Grow resources bitset to fit the highest id found (with default values of false)
            resources.resize(max_id + 1, false);
            // Set corresponding indexes of IDs to true
            for (auto& species_id : ids) {
                resources[species_id] = true;
            }
        }
    }

    void Species::AddResource(const std::string& name) {
        SpeciesID resource_id = ecosystem.GetSpeciesID(name);
        if (resource_id < resources.size()) {
            // Important that default value is false as if resource_id is the first resource added and has an id of e.g. 10, all other ids before should be set to false
            resources.resize(resource_id + 1, false);
        }
        resources[resource_id] = true;
    }

    float Species::GetWeightedCrowdingFactor() {
        auto competitors = ecosystem.GetCompetitors(*this);
        float crowding_factor = population * genes.crowding_factor;
        for (auto& element : competitors) {
            auto& species = element.get();
            crowding_factor += species.GetPopulation() * species.GetGenes().crowding_factor;
        }
        return crowding_factor;
    }

    internal::MonteCarlo::MonteCarlo(tgm::Ecosystem& ecosystem, std::size_t trials, std::size_t steps) {
        std::unordered_map<tgm::State, std::size_t> state_map;
        std::size_t average_days_to_extinction = 0;
        for (std::size_t i{ 0 }; i < trials; ++i) {
            std::size_t step{ 0 };
            while (step < steps) {
                ecosystem.Update();
                if (ecosystem.GetState() != tgm::State::FLUCTUATING) {
                    break;
                }
                ++step;
            }
            tgm::State final_state{ ecosystem.GetState() };
            if (final_state == State::EXTINCT) {
                average_days_to_extinction += step;
            }
            auto it = state_map.find(final_state);
            if (it != std::end(state_map)) {
                ++(it->second);
            }
            else {
                state_map.emplace(final_state, 1);
            }
            LOG("Monte Carlo trial #" << i);
            // During the last trial, print species information
            if (i == trials - 1) {
                LOG("Final trial detailed report: ");
                ecosystem.PrintAllSpeciesStatuses();
            }
            ecosystem.Reset();
        }
        LOG(steps << " days / trial");
        for (auto& pair : state_map) {
            LOG(pair.second << " trials ended in " << pair.first);
            if (pair.first == State::EXTINCT) {
                LOG("Average days to extinction : " << average_days_to_extinction / pair.second);
            }
        }
    }

} //namespace tgm
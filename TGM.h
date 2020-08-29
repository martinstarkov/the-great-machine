#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>

#define LOG(x) { std::cout << x << std::endl; }
#define LOG_(x) { std::cout << x; }

namespace internal {
    bool RandomBool() {
        return 0 + (rand() % (1 - 0 + 1)) == 1;
    }
}

namespace tgm {

    using Population = std::uint32_t;

    struct Rates {
        float growth = 1.0f;
        float decay = 1.0f;
    };

    class BaseSpecies {
    public:
        virtual const std::string GetName() const = 0;
        virtual const Population GetPopulation() const = 0;
        virtual const float GetImpactOn(std::string name) const = 0;
        virtual void Update() = 0;
        virtual void AddImpactOn(std::string name, float impact) = 0;
        virtual ~BaseSpecies() {}
    };

    class Ecosystem;

    class Species : public BaseSpecies {
    public:
        Species(Ecosystem& ecosystem, std::string name, Population population, Rates rates) : ecosystem{ ecosystem }, name{ name }, population{ population }, original_rates{ rates }, rates{ rates } {}
        ~Species() {}
        virtual const std::string GetName() const override final {
            return name;
        }
        virtual const Population GetPopulation() const override final {
            return population;
        }
        virtual const float GetImpactOn(std::string name) const override final {
            auto it = impact_on_others.find(name);
            if (it != std::end(impact_on_others)) {
                return it->second; // scale impact by population? figure this out later
            }
            return 0;
        }
        virtual void Update() override final;
        virtual void AddImpactOn(std::string name, float impact) override final {
            impact_on_others[name] = impact;
        }
    private:
        void GrowPopulation() {
            population = static_cast<Population>(round(population * rates.growth));
        }
        void DecayPopulation() {
            population = static_cast<Population>(round(population * rates.decay));
        }
        Ecosystem& ecosystem;
        const std::string name = "Unknown Species";
        Population population = 0;
        const Rates original_rates;
        Rates rates;
        std::unordered_map<std::string, float> impact_on_others;
    };

    class Ecosystem {
    public:
        Ecosystem() = default;
        std::shared_ptr<BaseSpecies> AddSpecies(std::string name, Population population, Rates rates) {
            auto shared = std::make_shared<Species>(*this, name, population, rates);
            all_species[name] = shared;
            return shared;
        }
        std::vector<std::shared_ptr<BaseSpecies>> GetPrey(std::unordered_map<std::string, float>& impacts_on_others) {
            std::vector<std::shared_ptr<BaseSpecies>> prey;
            for (auto& pair : impacts_on_others) {
                auto it = all_species.find(pair.first);
                if (it != std::end(all_species)) {
                    prey.push_back(it->second);
                }
            }
            return prey;
        }
        std::vector<std::shared_ptr<BaseSpecies>> GetPredators(std::string name) {
            std::vector<std::shared_ptr<BaseSpecies>> predators;
            for (auto& pair : all_species) {
                if (pair.second->GetImpactOn(name)) {
                    predators.push_back(pair.second);
                }
            }
            return predators;
        }
        void Update() {
            for (auto& species : all_species) {
                species.second->Update();
            }
        }
        void PrintSpeciesPopulation(std::string name) {
            auto it = all_species.find(name);
            if (it != std::end(all_species)) {
                LOG(name << ": " << it->second->GetPopulation());
            } else {
                LOG(name << " not added to ecosystem species");
            }
        }
        template <typename ...TArgs>
        void PrintSpeciesPopulations(TArgs... args) {
            (PrintSpeciesPopulation(args), ...);
        }
    private:
        std::unordered_map<std::string, std::shared_ptr<BaseSpecies>> all_species;
    };

    void Species::Update() {
        bool decision = internal::RandomBool();
        if (decision) {
            rates.growth = original_rates.growth;
            auto prey = ecosystem.GetPrey(impact_on_others);
            for (auto& p : prey) {
                rates.growth += p->GetImpactOn(name);
            }
            //LOG(name << " growth rate: " << rates.growth);
            GrowPopulation();
        } else {
            rates.decay = original_rates.decay;
            auto predators = ecosystem.GetPredators(name);
            for (auto& p : predators) {
                rates.decay -= p->GetImpactOn(name);
            }
            //LOG(name << " decay rate: " << rates.decay);
            DecayPopulation();
        }
    }

}
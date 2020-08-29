#include "TGM.h"

int main() {
	tgm::Ecosystem ecosystem;
	auto grass = ecosystem.AddSpecies("grass", 5000, { 1.08f, 0.99f });
	auto bunny = ecosystem.AddSpecies("bunny", 1000, { 1.01f, 0.99f });
	auto wolfy = ecosystem.AddSpecies("wolfy", 200, { 1.01f, 0.99f });
	auto human = ecosystem.AddSpecies("human", 10, { 1.9f, 0.99f });
	wolfy->AddImpactOn("bunny", 0.1f);
	wolfy->AddDependenceOn("bunny", 0.01f);
	bunny->AddImpactOn("grass", 0.05f);
	bunny->AddDependenceOn("grass", 0.05f);
	human->AddImpactOn("grass", 0.1f);
	human->AddImpactOn("wolf", 0.05f);
	ecosystem.PrintSpeciesPopulations();
	std::cin.get();
	while (true) {
		ecosystem.Update();
		ecosystem.PrintSpeciesPopulations();
		std::cin.get();
	}
}
#include "TGM.h"

int main() {
	tgm::Ecosystem ecosystem;
	auto grass = ecosystem.AddSpecies("grass", 100, { 1.01f, 1.0f });
	auto bunny = ecosystem.AddSpecies("bunny", 100, { 1.05f, 0.98f });
	bunny->AddImpactOn("grass", 0.01f);
	ecosystem.PrintSpeciesPopulations("bunny", "grass");
	std::cin.get();
	int counter = 0;
	while (counter < 100) {
		ecosystem.Update();
		ecosystem.PrintSpeciesPopulations("bunny", "grass");
		++counter;
		std::cin.get();
	}
}
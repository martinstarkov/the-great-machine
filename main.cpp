#include "TGM.h"

int main() {

	tgm::Ecosystem ecosystem;
	auto bunny = ecosystem.AddSpecies("bunny", 100, tgm::Genes{ 0.2f,0.3f,0.5f });
	auto wolf = ecosystem.AddSpecies("wolf", 100, tgm::Genes{ 0.2f,0.3f,0.5f });

	ecosystem.PrintSpeciesPopulations();

	while (true) {
		std::cin.get();
		ecosystem.Update();
		ecosystem.PrintSpeciesPopulations();
	}
}
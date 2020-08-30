#include "TGM.h"

int main() {

	tgm::Ecosystem ecosystem;
	auto bunny = ecosystem.AddSpecies("bunny", 1000, tgm::Genes{ 0.12f, 0.12f, 0.01f, 0.0001f }, tgm::Resources{});
	//auto wolf = ecosystem.AddSpecies("wolf", 100, tgm::Genes{ 0.2f,0.3f,0.5f });

	tgm::internal::MonteCarlo(ecosystem, 1000, 100);

	/*
	while (true) {
		std::cin.get();
		ecosystem.Update();
		ecosystem.PrintSpeciesPopulations();
	}*/
}
#include "TGM.h"

#include "DifferentiableRandom.h"

#include <windows.h>

constexpr int population = 20;
constexpr int width = 52;
constexpr int height = 26;

template <typename T, std::enable_if_t<std::is_floating_point<T>::value || std::is_integral<T>::value, int> = 0>
struct V2 {
	static V2 Random(T x_min, T x_max, T y_min, T y_max) {
		return V2{ internal::GetRandomValue<T>(x_min, x_max), internal::GetRandomValue<T>(y_min, y_max) };
	}
	T x = 0, y = 0;
	friend std::ostream& operator<<(std::ostream& os, const V2& obj) {
		os << "(" << obj.x << "," << obj.y << ")";
		return os;
	}
};

template <typename T, T W_min = 0, T W_max = width - 1, T H_min = 0, T H_max = height - 1, std::enable_if_t<std::is_floating_point<T>::value || std::is_integral<T>::value, int> = 0>
struct Individual {
	DifferentiableRandom<T> dx{ W_min, W_max };
	DifferentiableRandom<T> dy{ H_min, H_max };
	V2<T> pos;
	Individual() : Individual(V2<T>::Random(W_min, W_max, H_min, H_max)) {
	}
	Individual(V2<T> init_pos, V2<T> min_vel = { -1, -1 }, V2<T> max_vel = { 1, 1 }, V2<T> min_accel = { -2, -2 }, V2<T> max_accel = { 2, 2 }) {
		dx.SetInitialValue(init_pos.x);
		dy.SetInitialValue(init_pos.y);
		// order of layer addition matters
		dx.AddDifferentiableLayer(min_vel.x, max_vel.x);
		dx.AddDifferentiableLayer(min_accel.x, max_accel.x);
		dy.AddDifferentiableLayer(min_vel.y, max_vel.y);
		dy.AddDifferentiableLayer(min_accel.y, max_accel.y);
		SetNewPos();
	}
	void Update() {
		dx.Advance(1);
		dy.Advance(1);
		SetNewPos();
	}
	void SetNewPos() {
		pos.x = dx.GetValue();
		pos.y = dy.GetValue();
	}
};


int main() {

	tgm::Ecosystem ecosystem;
	auto bunny = ecosystem.AddSpecies("bunny", 1000, tgm::Genes{ 0.12f, 0.12f, 0.01f, 0.0001f }, tgm::Resources{});
	//auto wolf = ecosystem.AddSpecies("wolf", 100, tgm::Genes{ 0.2f,0.3f,0.5f });

	

	std::vector<std::vector<bool>> level(height, std::vector<bool>(width, false));

	std::vector<Individual<int>> individuals(population);

	while(true) {
		system("cls");
		for (auto& individual : individuals) {
			level[individual.pos.y][individual.pos.x] = true;
			individual.Update();
		}
		for (auto y = 0; y < level.size(); ++y) {
			for (auto x = 0; x < level[y].size(); ++x) {
				if (level[y][x]) {
					LOG_("#");
				} else {
					LOG_(" ");
				}
			}
			LOG("");
		}
		// clear previous locations
		std::fill(level.begin(), level.end(), std::vector<bool>(width, false));
	}

	//tgm::internal::MonteCarlo(ecosystem, 100, 700);

	/*
	while (true) {
		std::cin.get();
		ecosystem.Update();
		ecosystem.PrintSpeciesPopulations();
	}*/
}
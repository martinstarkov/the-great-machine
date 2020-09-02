#include "TGM.h"

#include "DifferentiableRandom.h"

#include <windows.h>
#include <set>

constexpr int grass_population = 50;
constexpr int rabbit_population = 20;
constexpr int fox_population = 10;
constexpr int lion_population = 5;

constexpr int width = 52;
constexpr int height = 26;

constexpr int min_x = 0;
constexpr int min_y = 0;
constexpr int max_x = width - 1;
constexpr int max_y = height - 1;

enum class TrophicLevel : std::size_t {
	PRIMARY_PRODUCER,
	PRIMARY_PREDATOR,
	SECONDARY_PREDATOR,
	TERTIARY_PREDATOR
};

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

using V2_int = V2<int>;

class BaseSpecies {
public:
	virtual const TrophicLevel GetTrophicLevel() const = 0;
	virtual const char GetCharacter() const = 0;
	virtual void Update() = 0;
};

class Species : public BaseSpecies {
public:
	Species(TrophicLevel trophic_level, char character) : trophic_level{ trophic_level }, character{ character } {}
	virtual const TrophicLevel GetTrophicLevel() const override final {
		return trophic_level;
	}
	virtual const char GetCharacter() const override final {
		return character;
	}
	virtual void Update() override {}
private:
	TrophicLevel trophic_level;
	char character;
	bool alive;
};


using IntegerRandomizer = DifferentiableRandom<int>;

class Individual : public Species {
public:
	Individual(TrophicLevel level, char character, V2_int init_pos = V2_int::Random(min_x, max_x, min_y, max_y)) : Species{ level, character } {
		dx.SetInitialValue(init_pos.x);
		dy.SetInitialValue(init_pos.y);
		// order of layer addition matters
		SetLayers(1, 2);
		UpdatePosition();
	}
	virtual void Update() override {
		dx.Advance(1);
		dy.Advance(1);
		UpdatePosition();
	}
	V2_int& GetPosition() {
		return pos;
	}
	const V2_int GetPosition() const {
		return pos;
	}
	const bool IsAlive() const {
		return alive;
	}
	void Kill() {
		alive = false;
	}
private:
	void SetLayers(int vel, int accel) {
		dx.AddDifferentiableLayer(-vel, vel);
		dy.AddDifferentiableLayer(-vel, vel);
		dx.AddDifferentiableLayer(-accel, accel);
		dy.AddDifferentiableLayer(-accel, accel);
	}
	void UpdatePosition() {
		pos.x = dx.GetValue();
		pos.y = dy.GetValue();
	}

	V2_int pos;
	bool alive = true;

	IntegerRandomizer dx{ min_x, max_x };
	IntegerRandomizer dy{ min_y, max_y };

};

class StaticIndividual : public Individual {
public:
	StaticIndividual(TrophicLevel level, char character, V2_int init_pos = V2_int::Random(min_x, max_x, min_y, max_y)) : Individual{ level, character, init_pos } {}
	virtual void Update() override final {

	}
private:
};

using SpeciesVector = std::vector<std::shared_ptr<Individual>>;

class Node {
public:
	Node() = default;
	Node(int state) : state{ state } {}
	const int GetState() const {
		return state;
	}
	void SetState(int new_state) {
		state = new_state;
	}
	void AddIndividual(std::shared_ptr<Individual> individual) {
		contents.emplace_back(individual);
	}
	void Update() {
		if (contents.size() > 1) {
			std::sort(contents.begin(), contents.end(), [](std::shared_ptr<Individual> a, std::shared_ptr<Individual> b) { return a->GetTrophicLevel() > b->GetTrophicLevel(); });
		}
		PrintChar();
		Clear();
	}
private:
	void PrintChar() {
		if (contents.size() == 0) {
			LOG_(" ");
		} else {
			auto& species = *contents.begin();
			assert(species != nullptr);
			LOG_(species->GetCharacter());
			// vector is sorted in ascending order of trophic levels;
			// TODO: For equal trophic level species, use traits to determine survivor
			// TODO: Add traits effect on chances of survival here
			// if highest trophic level is not a producer, kill the rest of the lower / equal trophic level species
			if (contents.size() > 1 && species->GetTrophicLevel() != TrophicLevel::PRIMARY_PRODUCER) {
				for (auto it = contents.begin() + 1; it != contents.end(); ++it) {
					auto& prey = *it;
					assert(prey != nullptr);
					prey->Kill();
					prey.reset();
				}
				contents.resize(1);
			}
		}
	}
	void Clear() {
		contents.clear();
	}
	int state = 0;
	SpeciesVector contents;
};

using Row = std::vector<Node>;
using Grid = std::vector<Row>;
using Individuals = std::vector<std::shared_ptr<Individual>>;

template<typename T>
static void UpdateIndividuals(std::vector<T>& container, Grid& grid) {
	for (auto it = std::begin(container); it != std::end(container); ) {
		auto& individual = *it;
		assert(individual != nullptr);
		if (!individual->IsAlive()) {
			it = container.erase(it);
		} else {
			auto& pos = individual->GetPosition();
			grid[pos.y][pos.x].AddIndividual(individual);
			individual->Update();
			++it;
		}
	}
}

static void UpdateGrid(Grid& grid) {
	for (auto y = 0; y < grid.size(); ++y) {
		for (auto x = 0; x < grid[y].size(); ++x) {
			auto& node = grid[y][x];
			node.Update();
		}
		LOG("");
	}
}

// Generates a population of a certain size with random positions on the grid
template <typename T>
static void GeneratePopulation(Individuals& container, int size, TrophicLevel trophic_level, char character) {
	container.reserve(size);
	for (auto i = 0; i < size; ++i) {
		container.emplace_back(std::make_shared<T>(trophic_level, character));
	}
}

static void Recursion(int value, int start) {
	value = value * start;
	if (value > 1000) {
		LOG("Finished at " << value);
	} else {
		LOG("Value: " << value);
		Recursion(value, ++start);
	}
}

int main() {

	Recursion(1, 1);


	/*Grid level(height, Row(width));

	Individuals grass;
	GeneratePopulation<StaticIndividual>(grass, grass_population, TrophicLevel::PRIMARY_PRODUCER, 'P');

	Individuals rabbits;
	GeneratePopulation<Individual>(rabbits, rabbit_population, TrophicLevel::PRIMARY_PREDATOR, '#');

	Individuals foxes;
	GeneratePopulation<Individual>(foxes, fox_population, TrophicLevel::SECONDARY_PREDATOR, 'F');

	Individuals lions;
	GeneratePopulation<Individual>(lions, lion_population, TrophicLevel::TERTIARY_PREDATOR, 'L');

	while (true) {
		system("cls");
		UpdateIndividuals(grass, level);
		UpdateIndividuals(rabbits, level);
		UpdateIndividuals(foxes, level);
		UpdateIndividuals(lions, level);
		UpdateGrid(level);
	}*/
}
#include "TGM.h"

#include "DifferentiableRandom.h"

#include <windows.h>
#include <set>

constexpr int rabbit_population = 30;
constexpr int grass_population = 30;
constexpr int weed_population = 50;

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

int main() {

	Grid level(height, Row(width));

	Individuals rabbits;
	rabbits.reserve(rabbit_population);
	for (auto i = 0; i < rabbit_population; ++i) {
		rabbits.emplace_back(std::make_shared<Individual>(TrophicLevel::PRIMARY_PREDATOR, '#'));
	}

	Individuals grass;
	grass.reserve(grass_population);
	for (auto i = 0; i < grass_population; ++i) {
		grass.emplace_back(std::make_shared<StaticIndividual>(TrophicLevel::PRIMARY_PRODUCER, 'X'));
	}

	Individuals weeds;
	weeds.reserve(weed_population);
	for (auto i = 0; i < weed_population; ++i) {
		weeds.emplace_back(std::make_shared<Individual>(TrophicLevel::SECONDARY_PREDATOR, '0'));
	}

	while (true) {
		system("cls");
		for (auto& weed : weeds) {
			if (weed) {
				if (!weed->IsAlive()) {
					weed.reset();
				} else {
					auto& pos = weed->GetPosition();
					level[pos.y][pos.x].AddIndividual(weed);
					weed->Update();
				}
			}
		}
		for (auto& g : grass) {
			if (g) {
				if (!g->IsAlive()) {
					g.reset();
				} else {
					auto& pos = g->GetPosition();
					level[pos.y][pos.x].AddIndividual(g);
					g->Update();
				}
			}
		}
		for (auto& rabbit : rabbits) {
			if (rabbit) {
				if (!rabbit->IsAlive()) {
					rabbit.reset();
				} else {
					auto& pos = rabbit->GetPosition();
					level[pos.y][pos.x].AddIndividual(rabbit);
					rabbit->Update();
				}
			}
		}
		for (auto y = 0; y < level.size(); ++y) {
			for (auto x = 0; x < level[y].size(); ++x) {
				auto& node = level[y][x];
				node.Update();
			}
			LOG("");
		}
	}
}
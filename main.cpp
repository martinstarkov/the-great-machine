
#include <engine/Engine.h>
#include <ecs/ECS.h>

#include "TGM.h"

#include "DifferentiableRandom.h"

//#include <windows.h>
//#include <set>
//
//constexpr int grass_population = 50;
//constexpr int rabbit_population = 20;
//constexpr int fox_population = 10;
//constexpr int lion_population = 5;
//
//constexpr int width = 52;
//constexpr int height = 26;
//
//constexpr int min_x = 0;
//constexpr int min_y = 0;
//constexpr int max_x = width - 1;
//constexpr int max_y = height - 1;
//
//enum class TrophicLevel : std::size_t {
//	PRIMARY_PRODUCER,
//	PRIMARY_PREDATOR,
//	SECONDARY_PREDATOR,
//	TERTIARY_PREDATOR
//};
//
//class BaseSpecies {
//public:
//	virtual const TrophicLevel GetTrophicLevel() const = 0;
//	virtual void Update() = 0;
//};
//
//class Species : public BaseSpecies {
//public:
//	Species(TrophicLevel trophic_level) : trophic_level{ trophic_level } {}
//	virtual const TrophicLevel GetTrophicLevel() const override final {
//		return trophic_level;
//	}
//	virtual void Update() override {}
//private:
//	TrophicLevel trophic_level;
//	bool alive = false;
//};

using IntegerRandomizer = DifferentiableRandom<int>;

//class Individual : public Species {
//public:
//	Individual(TrophicLevel level, V2_int init_pos = V2_int::Random(min_x, max_x, min_y, max_y)) : Species{ level } {
//		dx.SetInitialValue(init_pos.x);
//		dy.SetInitialValue(init_pos.y);
//		// order of layer addition matters
//		SetLayers(1, 2);
//		UpdatePosition();
//	}
//	virtual void Update() override {
//		dx.Advance(1);
//		dy.Advance(1);
//		UpdatePosition();
//	}
//	V2_int& GetPosition() {
//		return pos;
//	}
//	const V2_int GetPosition() const {
//		return pos;
//	}
//	const bool IsAlive() const {
//		return alive;
//	}
//	void Kill() {
//		alive = false;
//	}
//	std::size_t GetFoodCount() {
//		return food_count;
//	}
//	void SetFoodCount(int new_food_count) {
//		food_count = new_food_count;
//	}
//private:
//	void SetLayers(int vel, int accel) {
//		dx.AddDifferentiableLayer(-vel, vel);
//		dy.AddDifferentiableLayer(-vel, vel);
//		dx.AddDifferentiableLayer(-accel, accel);
//		dy.AddDifferentiableLayer(-accel, accel);
//	}
//	void UpdatePosition() {
//		pos.x = dx.GetValue();
//		pos.y = dy.GetValue();
//	}
//
//	V2_int pos;
//	bool alive = true;
//	std::size_t food_count = 0;
//
//	IntegerRandomizer dx{ min_x, max_x };
//	IntegerRandomizer dy{ min_y, max_y };
//
//};
//
//class StaticIndividual : public Individual {
//public:
//	StaticIndividual(TrophicLevel level, V2_int init_pos = V2_int::Random(min_x, max_x, min_y, max_y)) : Individual{ level, init_pos } {}
//	virtual void Update() override final {
//
//	}
//private:
//};
//
//using SpeciesVector = std::vector<std::shared_ptr<Individual>>;
//
//class Node {
//public:
//	Node() = default;
//	Node(int state) : state{ state } {}
//	const int GetState() const {
//		return state;
//	}
//	void SetState(int new_state) {
//		state = new_state;
//	}
//	void AddIndividual(std::shared_ptr<Individual> individual) {
//		contents.emplace_back(individual);
//	}
//	void Update() {
//		if (contents.size() > 1) {
//			std::sort(contents.begin(), contents.end(), [](std::shared_ptr<Individual> a, std::shared_ptr<Individual> b) { return a->GetTrophicLevel() > b->GetTrophicLevel(); });
//		}
//		MotherNature();
//		Clear();
//	}
//	void RemoveIndividual(std::shared_ptr<Individual> individual) {
//		for (auto it = std::begin(contents); it != std::end(contents);) {
//			if (*it == individual) {
//				contents.erase(it);
//				return;
//			} else {
//				++it;
//			}
//		}
//	}
//private:
//	void MotherNature() {
//		if (contents.size() == 0) {
//			//LOG_(" ");
//		} else {
//			auto& individual = *contents.begin();
//			assert(individual != nullptr);
//			//LOG_(species->GetCharacter());
//			// vector is sorted in ascending order of trophic levels;
//			// TODO: For equal trophic level species, use traits to determine survivor
//			// TODO: Add traits effect on chances of survival here
//			// if highest trophic level is not a producer, kill the rest of the lower / equal trophic level species
//			TrophicLevel predator = individual->GetTrophicLevel();
//			if (contents.size() > 1 && predator != TrophicLevel::PRIMARY_PRODUCER) {
//				bool food_found = false;
//				for (auto it = contents.begin() + 1; it != contents.end();) {
//					auto& prey = *it;
//					assert(prey != nullptr);
//					if (static_cast<int>(prey->GetTrophicLevel()) < static_cast<int>(predator)) {
//						prey->Kill();
//						prey.reset();
//						it = contents.erase(it);
//						food_found = true;
//					} else {
//						++it;
//					}
//				}
//				if (food_found) {
//					individual->SetFoodCount(individual->GetFoodCount() + 1);
//				}
//			}
//		}
//	}
//	void Clear() {
//		contents.clear();
//	}
//	int state = 0;
//	SpeciesVector contents;
//};
//
//using Row = std::vector<Node>;
//
//struct Grid {
//	Grid(int width, int height) : width{ width }, height{ height } {
//		contents = std::vector<Row>(height, Row(width)); 
//	}
//	std::vector<Row> contents;
//	int width;
//	int height;
//};
//
//using Individuals = std::vector<std::shared_ptr<Individual>>;
//
//template<typename T>
//static void UpdateIndividuals(std::vector<T>& container, Grid& grid) {
//	for (auto it = std::begin(container); it != std::end(container); ) {
//		auto& individual = *it;
//		assert(individual != nullptr);
//		if (!individual->IsAlive()) {
//			it = container.erase(it);
//		} else {
//			auto& pos = individual->GetPosition();
//			assert(pos.x < grid.width);
//			assert(pos.y < grid.height);
//			grid.contents[pos.y][pos.x].AddIndividual(individual);
//			individual->Update();
//			++it;
//		}
//	}
//}
//
//static void UpdateGrid(Grid& grid) {
//	for (auto y = 0; y < grid.contents.size(); ++y) {
//		for (auto x = 0; x < grid.contents[y].size(); ++x) {
//			auto& node = grid.contents[y][x];
//			node.Update();
//		}
//		//LOG("");
//	}
//}
//
//// Generates a population of a certain size with random positions on the grid
//template <typename T>
//static void GeneratePopulation(Individuals& container, int size, TrophicLevel trophic_level) {
//	container.reserve(size);
//	for (auto i = 0; i < size; ++i) {
//		container.emplace_back(std::make_shared<T>(trophic_level));
//	}
//}
//
//using namespace engine;

//#define TILE_SIZE V2_int{ Game::ScreenWidth() / width, Game::ScreenHeight() / height }
//
//class GameManager {
//public:
//	GameManager(int width, int height) : level{ width, height }, width{ width }, height{ height } {
//		Init();
//	}
//	void Init() {
//		GeneratePopulation<StaticIndividual>(grass, grass_population, TrophicLevel::PRIMARY_PRODUCER);
//		GeneratePopulation<Individual>(rabbits, rabbit_population, TrophicLevel::PRIMARY_PREDATOR);
//		TextureManager::Load("rabbit", "resources/hashbun.png");
//	}
//	void Update() {
//		static int day = 1;
//		//system("cls");
//		if (day == 8) {
//			LOG("Killing rabbits");
//			for (auto it = std::begin(rabbits); it != std::end(rabbits);) {
//				auto& rabbit = *(*it);
//				if (rabbit.GetFoodCount() == 0) {
//					rabbit.Kill();
//					it = rabbits.erase(it);
//				} else {
//					rabbit.SetFoodCount(0);
//					++it;
//				}
//			}
//			LOG("Rabbits left: " << rabbits.size());
//			day = 1;
//			//for (auto it = std::begin(rabbits); it != std::end(rabbits);) {
//			//	auto& rabbit= *it;
//			//	if (rabbit->GetFoodCount() == 0) {
//			//		auto& pos = rabbit->GetPosition();
//			//		level.contents[pos.y][pos.x].RemoveIndividual(rabbit);
//			//		rabbit->Kill();
//			//		/*rabbit.reset();
//			//		it = rabbits.erase(it);*/
//			//	} else {
//			//		++it;
//			//	}
//			//}
//		} else {
//			if (InputHandler::MousePressed(MouseButton::LEFT)) {
//				auto mouse_pos = InputHandler::GetMousePosition();
//				auto tile_pos = mouse_pos;
//				tile_pos.x = static_cast<int>(floor(tile_pos.x / TILE_SIZE.x));
//				tile_pos.y = static_cast<int>(floor(tile_pos.y / TILE_SIZE.y));
//				// Clamp tile_pos to grid
//				if (tile_pos.x < width && tile_pos.y < height) {
//					grass.emplace_back(std::make_shared<StaticIndividual>(TrophicLevel::PRIMARY_PRODUCER, tile_pos));
//				}
//			}
//			UpdateIndividuals(grass, level);
//			UpdateIndividuals(rabbits, level);
//			UpdateGrid(level);
//			LOG("Day: " << day);
//			++day;
//		}
//	}
//	void Render() {
//		for (auto& individual : grass) {
//			TextureManager::DrawRectangle(individual->GetPosition() * TILE_SIZE, TILE_SIZE, GREEN);
//		}
//		for (auto& individual : rabbits) {
//			TextureManager::DrawRectangle("rabbit", V2_int{ 0, 0 }, V2_int{ 238, 258 }, individual->GetPosition() * TILE_SIZE, TILE_SIZE);
//		}
//	}
//private:
//	int width, height;
//	Grid level;
//	Individuals grass;
//	Individuals rabbits;
//};


struct Size {
	Size(int radius) : radius{ radius } {}
	int radius;
};

struct Position {
	Position(V2_int center) : center{center} {}
	V2_int center;
};

struct Speed {
	Speed(int radius) : radius{ radius } {}
	int radius;
};

struct Range {
	Range(int radius) : radius{ radius } {}
	int radius;
	void AddInteraction(ecs::EntityId id) {
		potential_interactions.push_back(id);
	}
	void RemoveInteraction(ecs::EntityId id) {
		potential_interactions.remove(id);
	}
	std::size_t Interactions() const {
		return potential_interactions.size();
	}
private:
	std::list<ecs::EntityId> potential_interactions;
};

struct Color {
	Color(engine::Color color) : color{ color } {}
	engine::Color color;
};

struct RandomMovement {
	RandomMovement(V2_int initial_position, V2_int max_speed, V2_int max_acceleration) {
		dx.SetInitialValue(initial_position.x);
		dy.SetInitialValue(initial_position.y);
		dx.AddDifferentiableLayer(-max_speed.x, max_speed.x);
		dy.AddDifferentiableLayer(-max_speed.y, max_speed.y);
		dx.AddDifferentiableLayer(-max_acceleration.x, max_acceleration.x);
		dy.AddDifferentiableLayer(-max_acceleration.y, max_acceleration.y);
	}
	V2_int GetNewPosition() {
		dx.Advance(1);
		dy.Advance(1);
		return V2_int{ dx.GetValue(), dy.GetValue() };
	}
	bool randomize = true;
private:
	IntegerRandomizer dx{ 0, engine::Game::ScreenWidth() - 1 };
	IntegerRandomizer dy{ 0, engine::Game::ScreenHeight() - 1 };
};

constexpr std::size_t RABBITS = 10;

class GameManager {
public:
	GameManager() = delete;
	GameManager(ecs::Manager6& ecs) : ecs{ ecs } {
		Init();
	}
	ecs::EntityId MakeRabbit(V2_int pos) {
		static int size = 5;
		static int speed = 10;
		static int range = 20;
		ecs::EntityId id = ecs.CreateEntity();
		ecs.AddComponent<Position>(id, pos);
		ecs.AddComponent<Size>(id, size);
		ecs.AddComponent<Speed>(id, speed);
		ecs.AddComponent<Range>(id, range);
		ecs.AddComponent<Color>(id, engine::ORANGE);
		ecs.AddComponent<RandomMovement>(id, pos, V2_int{ size + speed, size + speed }, V2_int{ size + speed, size + speed });
		return id;
	}
	void Init() {
		for (auto i = 0; i < RABBITS; ++i) {
			ecs::EntityId id = MakeRabbit(V2_int::Random(0, engine::Game::ScreenWidth() - 1, 0, engine::Game::ScreenHeight() - 1));
		}
	}
	void Update() {
		static auto& cache = ecs.AddCache<Position, RandomMovement>();
		for (auto [id, pos, rng] : cache.GetEntities()) {
			// , speed, range, size, rabbit
			pos.center = rng.GetNewPosition(); //+= speed.radius * V2_int::Random(-1, 1, -1, 1);
		}
		static auto& cache2 = ecs.AddCache<Position, Range, Size>();
		for (auto [id, pos, range, size] : cache2.GetEntities()) {
			for (auto [id2, pos2, range2, size2] : cache2.GetEntities()) {
				if (id != id2) {
					if (engine::math::Distance(pos.center, pos2.center) < size.radius + range.radius + size2.radius + range2.radius) {
						range.AddInteraction(id2);
					} else {
						range.RemoveInteraction(id2);
					}
				}
			}
		}
		static auto& cache3 = ecs.AddCache<Color, Range>();
		for (auto [id, color, range] : cache3.GetEntities()) {
			if (range.Interactions() > 0) {
				color.color = engine::RED;
			} else {
				color.color = engine::ORANGE;
			}
		}
	}
	void Render() {
		static auto& cache = ecs.AddCache<Position, Size, Speed, Color, Range>();
		for (auto [id, pos, size, speed, color, range] : cache.GetEntities()) {
			engine::TextureManager::DrawCircle(pos.center, size.radius, color.color);
			engine::TextureManager::DrawCircle(pos.center, size.radius + range.radius, engine::GREEN);
		}
	}
private:
	ecs::Manager6& ecs;
};

int main(int argc, char* argv[]) {

	engine::Game::Init("The Great Machine", 800, 600, 10);

	ecs::Manager6 manager;

	GameManager game_manager{ manager };

	engine::Game::Loop([&]() {
		game_manager.Update();
	}, [&]() {
		game_manager.Render();
	});

	return 0;
}
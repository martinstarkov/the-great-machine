#include <engine/Engine.h>
#include <ecs/ECS.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

//#include "TGM.h"

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
	void AddInteraction(ecs::Entity entity) {
		interactions.push_back(entity);
	}
	void RemoveInteraction(ecs::Entity entity) {
		interactions.remove(entity);
	}
	std::size_t Interactions() const {
		return interactions.size();
	}
	std::list<ecs::Entity> interactions;
};

struct Color {
	Color(engine::Color color) : color{ color } {}
	engine::Color color;
};

struct Movement {
	Movement(V2_int initial_position, V2_int max_speed, V2_int max_acceleration) {
		dx.SetInitialValue(initial_position.x);
		dy.SetInitialValue(initial_position.y);
		dx.AddDifferentiableLayer(-max_speed.x, max_speed.x);
		dy.AddDifferentiableLayer(-max_speed.y, max_speed.y);
		dx.AddDifferentiableLayer(-max_acceleration.x, max_acceleration.x);
		dy.AddDifferentiableLayer(-max_acceleration.y, max_acceleration.y);
	}
	V2_int GetPosition() {
		dx.Advance(1);
		dy.Advance(1);
		return V2_int{ dx.GetValue(), dy.GetValue() };
	}
	void Disable() { enabled = false; }
	void Enable(V2_int current_position) {
		dx.SetValue(current_position.x);
		dy.SetValue(current_position.y);
		enabled = true;
	}
	bool IsEnabled() { return enabled; }
private:
	bool enabled = true;
	IntegerRandomizer dx{ 0, engine::Game::ScreenWidth() - 1 };
	IntegerRandomizer dy{ 0, engine::Game::ScreenHeight() - 1 };
};

struct AI {
	V2_int GetPosition(V2_int current_position, int max_speed) {
		V2_double direction = (current_position - target.GetComponent<Position>().center).unit<double>().opposite();
		V2_double new_position = static_cast<V2_double>(current_position) + direction * max_speed;
		return static_cast<V2_int>(new_position);

		//when running away
		//V2_double direction = (current_position - target.GetComponent<Position>().center).unit<double>();
		//V2_double new_position = static_cast<V2_double>(current_position) + direction * max_speed;
		//return static_cast<V2_int>(new_position);
	}
	void Enable(ecs::Entity new_target) {
		target = new_target;
		enabled = true;
	}
	void EnableRun(ecs::Entity new_pursuer) {
		pursuer = new_pursuer;
		run_enabled = true;
	}
	void Disable() { enabled = false; }
	void DisableRun() { run_enabled = false; }
	bool IsEnabled() { return enabled; }
	bool IsRunEnabled() { return run_enabled; }
private:
	bool enabled = false;
	bool run_enabled = false;
	ecs::Entity target;
	ecs::Entity pursuer;
};

struct Prey {
	Prey(std::list<std::string>&& prey_names) : prey_names{ std::move(prey_names) } {}
	void AddPrey(ecs::Entity entity) {
		prey.push_back(entity);
	}
	void RemovePrey(ecs::Entity entity) {
		prey.remove(entity);
	}
	std::size_t PreyCount() const {
		return prey.size();
	}
	void AddSpecies(const std::string& name) {
		prey_names.push_back(name);
	}
	/*
	void RemoveSpecies(const std::string& name) {
		prey_names.remove(name);
	} */
	bool HasSpecies(const std::string& name) const {
		for (auto& prey_name : prey_names) {
			if (name.compare(prey_name) == 0) return true;
		}
		return false;
	}

	std::list<ecs::Entity> prey;

private:
	std::list<std::string> prey_names;
};

struct Predator {
	Predator(std::list<std::string>&& predator_names) : predator_names{ std::move(predator_names) } {}
	void AddPredator(ecs::Entity entity) {
		predators.push_back(entity);
	}
	void RemovePredator(ecs::Entity entity) {
		predators.remove(entity);
	}
	std::size_t PredatorCount() const {
		return predators.size();
	}
	void AddSpecies(const std::string& name) {
		predator_names.push_back(name);
	}
	/*
	void RemoveSpecies(const std::string& name) {
		prey_names.remove(name);
	} */
	bool HasSpecies(const std::string& name) const {
		for (auto& predator_name : predator_names) {
			if (name.compare(predator_name) == 0) return true;
		}
		return false;
	}

	std::list<ecs::Entity> predators;

private:
	std::list<std::string> predator_names;
};

struct Species {
	Species(std::string&& name) : name{ std::move(name) } {}
	const std::string name;
};

constexpr std::size_t RABBITS = 10;
constexpr std::size_t GRASS = 40;
constexpr std::size_t FOXES = 5;

class GameManager {
public:
	GameManager() = delete;
	GameManager(ecs::Manager& ecs) : ecs{ ecs } {
		Init();
	}
	ecs::Entity MakeRabbit(V2_int pos) {
		static int size = 5;
		static int speed = 3;
		static int range = 60;
		ecs::Entity entity = ecs.CreateEntity();
		entity.AddComponent<Species>("rabbit");
		entity.AddComponent<AI>();
		entity.AddComponent<Position>(pos);
		entity.AddComponent<Size>(size);
		entity.AddComponent<Speed>(speed);
		entity.AddComponent<Range>(range);
		entity.AddComponent<Color>(engine::ORANGE);
		entity.AddComponent<Movement>(pos, V2_int{ size + speed, size + speed }, V2_int{ size + speed, size + speed });
		entity.AddComponent<Prey>(std::list<std::string>{ "grass" });
		//entity.AddComponent<Predator>(std::list<std::string>{ "fox" });
		return entity;
	}
	ecs::Entity MakeGrass(V2_int pos) {
		static int size = 5;
		ecs::Entity entity = ecs.CreateEntity();
		entity.AddComponent<Species>("grass");
		entity.AddComponent<Position>(pos);
		entity.AddComponent<Size>(size);
		entity.AddComponent<Color>(engine::GREEN);
		return entity;
	}
	ecs::Entity MakeFox(V2_int pos) {
		static int size = 5;
		static int speed = 3;
		static int range = 70;
		ecs::Entity entity = ecs.CreateEntity();
		entity.AddComponent<Species>("fox");
		entity.AddComponent<AI>();
		entity.AddComponent<Position>(pos);
		entity.AddComponent<Size>(size);
		entity.AddComponent<Speed>(speed);
		entity.AddComponent<Range>(range);
		entity.AddComponent<Color>(engine::BLUE);
		entity.AddComponent<Movement>(pos, V2_int{ size + speed, size + speed }, V2_int{ size + speed, size + speed });
		entity.AddComponent<Prey>(std::list<std::string>{ "rabbit" });
		return entity;
	}
	void Init() {
		for (auto i = 0; i < RABBITS; ++i) {
			MakeRabbit(V2_int::Random(0, engine::Game::ScreenWidth() - 1, 0, engine::Game::ScreenHeight() - 1));
		}
		for (auto i = 0; i < GRASS; ++i) {
			MakeGrass(V2_int::Random(0, engine::Game::ScreenWidth() - 1, 0, engine::Game::ScreenHeight() - 1));
		}
		for (auto i = 0; i < FOXES; ++i) {
			MakeFox(V2_int::Random(0, engine::Game::ScreenWidth() - 1, 0, engine::Game::ScreenHeight() - 1));
		}
	}
	void Update() {
		static auto& cache0 = ecs.AddCache<Position, Speed, AI>();
		for (auto [entity, pos, speed, ai] : cache0.GetEntities()) {
			if (ai.IsEnabled()) {
				pos.center = ai.GetPosition(pos.center, speed.radius);
			}
		}
		static auto& cache = ecs.AddCache<Position, Movement>();
		for (auto [entity, pos, rng] : cache.GetEntities()) {
			if (rng.IsEnabled()) {
				pos.center = rng.GetPosition();
			}
		}
		static auto& cache2 = ecs.AddCache<Position, Range, Size, Species>();
		static auto& cache3 = ecs.AddCache<Position, Size, Species>();
		for (auto [entity, pos, range, size, species] : cache2.GetEntities()) {
			for (auto [entity2, pos2, size2, species2] : cache3.GetEntities()) {
				if (entity != entity2 && species.name.compare(species2.name)) {
					if (engine::math::Distance(pos.center, pos2.center) < size.radius + range.radius + size2.radius) {
						range.AddInteraction(entity2);
						//ecs.GetComponent<Color>(id).color = engine::BLUE;
					} else {
						//ecs.GetComponent<Color>(id).color = engine::ORANGE;
						range.RemoveInteraction(entity2);
					}
				}
			}
		}
		static auto& cache4 = ecs.AddCache<Color, Range, Movement, Species>();
		for (auto [entity, color, range, movement, species] : cache4.GetEntities()) {
			if (range.Interactions() > 0) {
				//color.color = engine::RED;
				if (entity.HasComponent<Prey>()) {
					auto& prey = entity.GetComponent<Prey>();
					for (auto entity2 : range.interactions) {
						if (entity != entity2 && entity2 != ecs::null) {
							assert(entity2.HasComponent<Species>() && "Nicole's Check");
							auto& individual = entity2.GetComponent<Species>();
							if (species.name.compare(individual.name)) {
								//LOG(species.name << " interacting with " << individual.name);
								if (prey.HasSpecies(individual.name)) {
									prey.AddPrey(entity2);
								}
							}
						}
					}
				}
				//movement.randomize = false;
			} else {
				//color.color = engine::ORANGE;
			}
		}

		static auto& cache5 = ecs.AddCache<Prey, Color, Size, Speed, Position, Range, Movement, AI>();
		for (auto [entity, prey, color, size, speed, position, range, movement, ai] : cache5.GetEntities()) {
			if (prey.PreyCount() > 0) {
				auto shortest_distance = size.radius + range.radius;
				ecs::Entity closest_individual = ecs::null;
				for (auto entity2 : prey.prey) {
					if (entity != entity2) {
						if (entity2.HasComponent<Size>()) { //checks that component hasn't been "deleted"
							assert(entity2.HasComponent<Position>() && "Nicole's Check");
							assert(entity2.HasComponent<Size>() && "Nicole's Check");
							auto individual = entity2.GetComponent<Position>();
							auto individual_size = entity2.GetComponent<Size>();
							auto distance = engine::math::Distance(position.center, individual.center) - individual_size.radius - size.radius;
							if (distance <= shortest_distance) { //used to check if color was not black entity2.GetComponent<Color>().color != engine::BLACK
								shortest_distance = distance;
								closest_individual = entity2;
							}
						}
					}
				}
				if (closest_individual != ecs::null) {
					if (shortest_distance < speed.radius) {
						//eat it
						prey.RemovePrey(closest_individual);
						range.RemoveInteraction(closest_individual);
						closest_individual.RemoveComponent<Size>();
						//closest_individual.Destroy();
						//auto& prey_color = closest_individual.GetComponent<Color>();
						//prey_color.color = engine::BLACK;
						movement.Enable(position.center);
						ai.Disable();
					} else {
						movement.Disable();
						ai.Enable(closest_individual);
						//chase it
					}
				} else {
					movement.Enable(position.center);
					ai.Disable();
				}
			}
		}
	}
	void Render() {
		static auto& cache = ecs.AddCache<Position, Size, Color>();
		for (auto [entity, pos, size, color] : cache.GetEntities()) {
			engine::TextureManager::DrawCircle(pos.center, size.radius, color.color);
			if (entity.HasComponent<Range>()) {
				auto& range = entity.GetComponent<Range>();
				engine::TextureManager::DrawCircle(pos.center, size.radius + range.radius, engine::GREEN);
			}
			if (entity.HasComponent<Speed>()) {
				auto& speed = entity.GetComponent<Speed>();
				engine::TextureManager::DrawCircle(pos.center, size.radius + speed.radius, engine::RED);
			}
		}
	}
private:
	ecs::Manager& ecs;
};

int main(int argc, char* argv[]) {

	engine::Game::Init("The Great Machine", 800, 600, 10);

	ecs::Manager manager;

	GameManager game_manager{ manager };

	engine::Game::Loop([&]() {
		game_manager.Update();
		manager.Update();
	}, [&]() {
		game_manager.Render();
	});

	return 0;
}
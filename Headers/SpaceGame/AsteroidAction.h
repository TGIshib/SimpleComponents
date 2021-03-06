#ifndef ASTEROID_ACTION_H
#define ASTEROID_ACTION_H

#include "Actions/IAction.h"
#include "Components/PhysicsComponent.h"

class AsteroidAction : public IAction
{
private:

	unsigned int width; 
	unsigned int height; 

public:

	//-------------------------------------------------------------------------------
	// Name: AsteroidAction
	// Desc: 
	//-------------------------------------------------------------------------------
	AsteroidAction(unsigned int width, unsigned int height)
	{
		this->width = width; 
		this->height = height; 
	}
	//-------------------------------------------------------------------------------
	// Name: Update
	// Desc: 
	//-------------------------------------------------------------------------------
	void Update(IGameApp* gameApp) override final
	{
		auto componentCollectionRepository = gameApp->GetComponentCollectionRepository(); 
		this->SolveAsteroidPhysics(componentCollectionRepository); 
	}
	//-------------------------------------------------------------------------------
	// Name: SolveAsteroidPhysics
	// Desc: do logic/physics for the enemy asteroids 
	//-------------------------------------------------------------------------------
	static void SolveAsteroidPhysics(ComponentCollectionRepository* componentCollectionRepository) 
	{
		auto asteroidPhysicsComponents = componentCollectionRepository->SelectFromCollection<PhysicsComponent>("EnemyAsteroids");
		auto playerPhysicsComponents = componentCollectionRepository->SelectFromCollection<PhysicsComponent>("PlayerSpaceShip");

		if (asteroidPhysicsComponents == nullptr || asteroidPhysicsComponents->size() == 0) {
			return;
		}

		if (playerPhysicsComponents == nullptr || playerPhysicsComponents->size() == 0) {
			return;
		}

		auto playerPhysicsComponent = playerPhysicsComponents->front();

		for (auto physicsComponent : *asteroidPhysicsComponents) {

			auto distVector = playerPhysicsComponent.transformComponent->position - physicsComponent.transformComponent->position;

			// calculate acceleration on asteroids
			auto accel = 10000.0f * physicsComponent.mass / powf(distVector.Length(), 2.0f);
			auto angle = distVector.Angle();

			physicsComponent.velocity.x += accel*cosf(angle);
			physicsComponent.velocity.y += accel*sinf(angle);

			physicsComponent.transformComponent->position.x += physicsComponent.velocity.x;
			physicsComponent.transformComponent->position.y += physicsComponent.velocity.y;

			auto currentAngle = physicsComponent.transformComponent->orientation.Angle();
			physicsComponent.transformComponent->orientation = Vector2D(currentAngle + physicsComponent.angularVelocity);
		}
	}
};

#endif // ASTEROID_ACTION_H
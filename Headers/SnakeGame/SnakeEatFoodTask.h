#ifndef SNAKE_EAT_FOOD_TASK_H
#define SNAKE_EAT_FOOD_TASK_H

#include "Physics\IPhysicsTask.h"
#include "Components\TransformComponent.h"
#include "Events\IntersectionEventArgs.h"

class SnakeEatFoodTask : public IPhysicsTask
{
	void Task(ComponentCollectionRepository* componentCollectionRepository, const string& collection1, const string& collection2, EventObservable* eventObservable) override
	{
		auto snakeComponents = componentCollectionRepository->SelectFromCollection<TransformComponent>(collection1);
		auto foodComponents = componentCollectionRepository->SelectFromCollection<TransformComponent>(collection2); 
		
		auto snakeHead = snakeComponents->front();
		auto food = foodComponents->front(); 

		if (snakeHead.position == food.position) {
			eventObservable->Invoke<IntersectionEventArgs>(IntersectionEventArgs(&snakeHead, &food));
		}
	}

public:

	SnakeEatFoodTask() : IPhysicsTask("Snake", "Food")
	{
	}
};

#endif // SNAKE_EAT_FOOD_TASK_H
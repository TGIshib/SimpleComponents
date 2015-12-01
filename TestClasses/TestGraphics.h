//
// Created by Julian  on 22/11/15.
//

#ifndef SDLRTS_TESTGRAPHICS_H
#define SDLRTS_TESTGRAPHICS_H

#include "Graphics.h"
#include "Components/GraphicsComponent.h"
#include "Components/TransformComponent.h"
#include "GameApp.h"
#include "GraphicsResources/RectGraphicsResource.h"
#include "GraphicsResources/SpriteGraphicsResource.h"
#include <ctime>

// TestGraphics
class TestGraphics
{
private:

    Graphics* graphics;

    std::vector<IGraphicsResource*> graphicsResources;
    std::vector<BaseComponent*> graphicsComponents;
    std::vector<BaseComponent*> transformComponents;

public:

    TestGraphics()
    {
        this->graphics = new Graphics(640, 480, "GraphicsTest");
        this->TestRectGraphicsResource();
        this->TestAddSpriteGraphicResource();
    }

    bool TestRectGraphicsResource()
    {
        int numComponents = 100;

        srand(time(0));

        for ( int i = 0; i < numComponents; i++) {

            float randX  = static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/((float) 640)));
            float randY  = static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/((float) 480)));
            float angle  = static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/((float) 2.0*PI)));
            float scaleX = static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/((float) 50.0f)));
            float scaleY = static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/((float) 50.0f)));

            int graphicsId;
            int transformId = i;

            graphicsId = this->graphics->AddGraphicsResource(new RectGraphicsResource(i, "rectGraphicsResource", 10.0f, 10.0f, 0xff, 0x8f, 0x00, i*0x08));

            this->graphicsComponents.push_back(new GraphicsComponent(graphicsId, transformId));
            this->transformComponents.push_back(new TransformComponent(Vector2D(randX, randY), Vector2D(angle), Vector2D(scaleX,scaleY)));
        }

        this->graphics->UpdateGraphics(graphicsComponents, transformComponents);

        return true;
    }

    bool TestAddSpriteGraphicResource()
    {
        int graphicsId;
        int id = graphicsComponents.size();
        int transformId = graphicsComponents.size();

        graphicsId = this->graphics->LoadGraphicResource("../Resources/tech_wall2.png", "TestSprite");

        this->graphicsComponents.push_back(new GraphicsComponent(graphicsId, transformId));
        this->transformComponents.push_back(new TransformComponent(Vector2D(100.0f, 100.0f), Vector2D(PI), Vector2D(1.0f, 1.0f)));

        this->graphics->UpdateGraphics(graphicsComponents, transformComponents);

        return true;
    }

    ~TestGraphics()
    {
        delete this->graphics;

        for (IGraphicsResource* graphicsResource : this->graphicsResources) {
            delete graphicsResource;
        }
    }

};


#endif //SDLRTS_TESTGRAPHICS_H

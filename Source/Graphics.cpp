//
// Created by Julian  on 17/11/15.
//
#include "Graphics/IGraphicsResource.h"
#include "Components/GraphicsComponent.h"
#include "Components/TransformComponent.h"
#include "Graphics/GraphicsUtility.h"
#include "Graphics/Graphics.h"
#include "Graphics/SpriteGraphicsResource.h"

#include <iostream>
#include <vector>
#include <unordered_map>

//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
Graphics::Graphics(int windowWidth, int windowHeight, std::string appName)
{
	this->resourceId = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return;
    }

	cout << "SDL_Init() success." << endl; 

    // Create window
    this->window = SDL_CreateWindow(appName.c_str(), 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    if ( this->window == nullptr) {
        SDL_Quit();
        return;
    }

	cout << "SDL_CreateWindow() success." << endl; 

    // Create renderer
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (this->renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

	cout << "SDL_CreateRenderer() success." << endl; 

    Uint32 rmask, gmask, bmask, amask;

    // SDL interprets each pixel as a 32-bit number, so our masks must depend
    //  on the endianness (byte order) of the machine 
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    // Create surface
    this->sdlSurface = SDL_CreateRGBSurface(0, 20, 20, 32, rmask, gmask, bmask, amask);

    if (sdlSurface == nullptr) {

        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }

	cout << "SDL_CreateRGBSurface() success." << endl;

    // Create texture
    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 20, 20);

    if (sdlTexture == nullptr) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }

	cout << "SDL_CreateTexture() success." << endl; 

    // init sdl image
    int imgFlags = IMG_INIT_PNG;
    if ( !(IMG_Init(imgFlags) & imgFlags) ) {
		std::cout << IMG_GetError() << std::endl; 
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }  

	cout << "IMG_Init() success." << endl; 

	// init sdl ttf
	if (TTF_Init() == -1) {
		auto error = TTF_GetError(); 
		SDL_DestroyWindow(window); 
		SDL_DestroyRenderer(renderer); 
		SDL_Quit(); 

		return; 
	}

	cout << "TTF_Init() success." << endl; 

	this->consoleFont = TTF_OpenFont("Resources//Anonymous_Pro.ttf", 10);

	cout << "TTF_OpenFont() success." << endl; 
}
//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
Graphics::~Graphics()
{
    // Free the surfaces
    for (std::pair<std::string, SDL_Surface*> resourceSurfaceMapElement : this->resourceSurfaceMap ) {
        SDL_FreeSurface(resourceSurfaceMapElement.second);
    }

    for (std::pair<int, IGraphicsResource*> graphicsResourcePair : this->graphicsResourceMap) {
        delete graphicsResourcePair.second; 
    }

    SDL_DestroyWindow(this->window);
    SDL_FreeSurface(this->sdlSurface);
    SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
}
//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
SDL_Surface* Graphics::LoadSurface(std::string filename, SDL_PixelFormat* format)
{
    // The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(filename.c_str());
    if( loadedSurface == NULL )
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", filename.c_str(), IMG_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, format, NULL);
        if( optimizedSurface == NULL )
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", filename.c_str(), SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    return optimizedSurface;
}
//------------------------------------------------------------------------------------
// Name: LoadGraphicResource
// Desc:
//------------------------------------------------------------------------------------
int Graphics::LoadGraphicResource(std::string fileName, std::string resourceName)
{
    auto surface = this->LoadSurface(fileName, this->sdlSurface->format);

    if (surface != nullptr) {

        auto id = GetNextResourceId();

        auto spriteGraphicsResource = new SpriteGraphicsResource(surface);
        this->graphicsResourceMap.insert(std::pair<int, IGraphicsResource*>(id, spriteGraphicsResource));
		return id;
    }

    return -1;
}
//------------------------------------------------------------------------------------
// Name: AddGraphicsResource
// Desc:
//------------------------------------------------------------------------------------
int Graphics::AddGraphicsResource(IGraphicsResource* graphicsResource)
{
    auto resourceId = this->GetNextResourceId();
    this->graphicsResourceMap.insert(pair<int, IGraphicsResource*>(resourceId, graphicsResource));
    return resourceId;
}
//------------------------------------------------------------------------------------
// Name: Present
// Desc:
//------------------------------------------------------------------------------------
void Graphics::Present()
{
	auto yOffset = 5; 

	for (auto message : this->consoleMessages) {
		
		SDL_Color textColor = { 255, 255, 255 };
		SDL_Surface* surfaceText = TTF_RenderText_Solid(this->consoleFont, message.c_str(), textColor);
		SDL_Texture* textureText = SDL_CreateTextureFromSurface(this->renderer, surfaceText);

		SDL_Rect textRect = { 5, yOffset, surfaceText->w, surfaceText->h };

		yOffset += surfaceText->h + 2; 

		SDL_RenderCopy(this->renderer, textureText, NULL, &textRect);

		SDL_FreeSurface(surfaceText);
		SDL_DestroyTexture(textureText);
	}

	SDL_RenderPresent(this->renderer);
}
//------------------------------------------------------------------------------------
// Name: PrintConsoleText
// Desc:
//------------------------------------------------------------------------------------
void Graphics::PrintConsoleText(const string& message)
{
	this->consoleMessages.push_front(message); 

	while (this->consoleMessages.size() > maxConsoleMessages) {
		this->consoleMessages.pop_back();
	}
}
//------------------------------------------------------------------------------------
// Name: Clear
// Desc:
//------------------------------------------------------------------------------------
void Graphics::Clear()
{
	SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(this->renderer);
}
//------------------------------------------------------------------------------------
// Name: UpdateGraphics
// Desc:
//------------------------------------------------------------------------------------
void Graphics::UpdateGraphics(vector<GraphicsComponent>* graphicsComponents, vector<TransformComponent>* transformComponents)
{
	for (auto graphicsComponent : *graphicsComponents) {

		TransformComponent* transformComponent = graphicsComponent.transformComponent;

		IGraphicsResource* graphicsResource;
		int graphicsResourceId = graphicsComponent.resourceId;

		graphicsResource = this->graphicsResourceMap[graphicsResourceId];
		graphicsResource->Render(this->renderer, transformComponent);
	}
}
//------------------------------------------------------------------------------------
// Name: WindowWidth
// Desc:
//------------------------------------------------------------------------------------
int Graphics::WindowWidth() const
{
	auto w = 0; 
	auto h = 0;

	SDL_GetWindowSize(this->window, &w, &h); 

	return w; 
}
//------------------------------------------------------------------------------------
// Name: WindowWidth
// Desc:
//------------------------------------------------------------------------------------
int Graphics::WindowHeight() const
{
	auto w = 0;
	auto h = 0;

	SDL_GetWindowSize(this->window, &w, &h);

	return h;
}
//------------------------------------------------------------------------------------
// Name: UpdateGraphics
// Desc:
//------------------------------------------------------------------------------------
void Graphics::UpdateGraphicsPresentAndClear(SDL_Event* event, vector<BaseComponent*>* graphicsComponents, vector<BaseComponent*>* transformComponents)
{
#pragma message ("WARNING: "__FUNCTION__" is deprecated.")

	SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xff); 
    SDL_RenderClear(this->renderer);

    for (auto component : *graphicsComponents) {

		GraphicsComponent* graphicsComponent = component->As<GraphicsComponent*>(); 
		TransformComponent* transformComponent = static_cast<GraphicsComponent*>(graphicsComponent)->transformComponent; 

        IGraphicsResource* graphicsResource;
		int graphicsResourceId = static_cast<GraphicsComponent*>(graphicsComponent)->resourceId;

        graphicsResource = this->graphicsResourceMap[graphicsResourceId];
        graphicsResource->Render(this->renderer, transformComponent);
    }

	// Mouse debug stuff
	if (event!= nullptr && event->type == SDL_MOUSEMOTION) {

		SDL_Rect lastRect = {event->motion.x - 8, event->motion.y - 8, 16, 16};
		SDL_Rect currRect = { event->motion.x + event->motion.xrel - 8, event->motion.y + event->motion.yrel - 8, 16, 16 }; 
		SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0xff, 0xff); 
		SDL_RenderDrawRect(this->renderer, &lastRect); 
		SDL_RenderDrawRect(this->renderer, &currRect); 

		SDL_SetRenderDrawColor(this->renderer, 0x00, 0xff, 0x00, 0xff); 
		SDL_RenderDrawLine(this->renderer, lastRect.x + 8, lastRect.y + 8, currRect.x + 8, currRect.y + 8); 
	}

    SDL_RenderPresent(this->renderer);
}

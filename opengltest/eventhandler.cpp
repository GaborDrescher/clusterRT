#include <cstdlib>
#include <SDL.h>
#include <SDL_opengl.h>
#include "eventhandler.h"

EventHandler::EventHandler() {}

bool EventHandler::handleUserInput()
{
	SDL_Event event;

	//SDL_WaitEvent(&event);
	
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			return false;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					return false;
					break;

				default:
					break;
			}
		}

	}

	return true;
}


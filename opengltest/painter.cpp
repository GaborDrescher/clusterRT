#include <SDL.h>
#include <SDL_opengl.h>

#include "painter.h"

	Painter::Painter(int width, int height) : w(width), h(height)
	{
		//SDL_WM_SetCaption("opengl test", 0);
		SDL_SetVideoMode(width, height, 32, SDL_OPENGL);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
	}

	void Painter::paintImageData(unsigned char *data)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, data);

		SDL_GL_SwapBuffers();

	}



#ifndef _PAINTER_H
#define _PAINTER_H

class Painter {

	int w;
	int h;

	public:
	Painter(int width, int height);

	void paintImageData(unsigned char *data);
};

#endif

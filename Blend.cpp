#include "Blend.h"

int GetColor(unsigned int red, unsigned int  green, unsigned int blue, int  alpha)
{
	//unsigned char top = 0x;
	int color = (red << 24) | (green << 16) | (blue << 8) | alpha;
	return color;
}
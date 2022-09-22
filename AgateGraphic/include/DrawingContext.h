#pragma once

#include "Geometry.h"

class DrawingContext
{
public:
	void SetClip(Vector4 clip);

	void Draw(Geometry& geometry);
private:

};


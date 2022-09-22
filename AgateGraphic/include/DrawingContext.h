#pragma once

#include "Geometry.h"

class DrawingContext
{
public:

	void SetViewSize(uint32_t width, uint32_t height);

	void SetClip(Vector4 clip);

	void Draw(Geometry& geometry);
private:

};


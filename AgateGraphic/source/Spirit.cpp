#include "Spirit.h"

void Spirit::SetImage(std::shared_ptr<Image> img)
{
}

void Spirit::SetClip(const Vector4& clip)
{
}

void Spirit::SetBlendMode(BlendMode blend)
{
}

void Spirit::SetTransform(const Matrix3X2 matrix)
{
}

uint32_t Spirit::Rasterize()
{
    return 1;
}

const RasterizeData& Spirit::GetRasterizeData(uint32_t index)
{
    return _RasterData;
}

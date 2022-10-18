#pragma once
#include "SpiritBase.h"
#include <vector>
#include "Randomizer.h"
#include <bitserializer/bit_serializer.h>
#include <bitserializer/rapidjson_archive.h>

using namespace BitSerializer;
using JsonArchive = BitSerializer::Json::RapidJson::JsonArchive;

namespace agate
{
    class Program :
        public SpiritBase
    {
    public:
        Program();

        //int Update(int64_t time) override;

        //void Draw(DrawingContext& context) override;
  
        bool IsStoped();
    private:
        Randomizer  _Random;
    };
}

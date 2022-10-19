#pragma once
#include "SpiritBase.h"
#include <vector>
#include "Randomizer.h"

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

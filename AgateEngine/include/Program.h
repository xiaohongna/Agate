#pragma once
#include "SpiritBase.h"
#include <vector>
namespace agate
{
    class Program :
        public SpiritBase
    {
    public:
        Program();

        bool Update(int64_t time) override;

        void Draw(DrawingContext& context) override;

        void AddSpirit(std::shared_ptr<SpiritBase>&& spirit);
    private:
        std::vector<std::shared_ptr<SpiritBase>> _Spirits;
        std::vector<SpiritBase*> _DrawingSpirits;
    };
}

#include "SpiritBase.h"

namespace agate
{
    int SpiritBase::Update(int64_t time)
    {
        int ret = 1;
        for (auto& spirite : _Children)
        {
            if (spirite->Update(time) <= 0)
            {
                ret = 0;
            }
        }
        if (ret > 0)
        {
            if (time < _Begin)
            {
                return -1;
            }
            else if (time <= _End)
            {
                return 0;
            }
        }
        return ret;
    }
    
    void SpiritBase::Draw(DrawingContext& context)
    {
        for (auto& spirite : _Children)
        {
            spirite->Draw(context);
        }
    }
    
    void SpiritBase::AddSpirit(std::shared_ptr<SpiritBase>&& spirit)
    {
        spirit->_Parent = weak_from_this();
        _Children.emplace_back(spirit);
    }
}
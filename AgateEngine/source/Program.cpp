#include "Program.h"

namespace agate
{
	Program::Program()
	{
	}

	bool Program::Update(int64_t time)
	{
		_DrawingSpirits.clear();
		for (auto& spirit: _Spirits)
		{
			if (spirit->Update(time))
			{
				_DrawingSpirits.push_back(spirit.get());
			}
		}
		return true;
	}

	void Program::Draw(DrawingContext& context)
	{
		for (auto& spirit : _DrawingSpirits)
		{
			spirit->Draw(context);
		}
	}

	void Program::AddSpirit(std::shared_ptr<SpiritBase>&& spirit)
	{
		_Spirits.emplace_back(spirit);
	}
}

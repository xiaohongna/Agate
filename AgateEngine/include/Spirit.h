#pragma once
#include "SpiritBase.h"
#include "Image.h"
#include "RotationParameter.h"

namespace agate
{
	class Spirit:
		public SpiritBase
	{
	public:
		Spirit()
		{

		}

		void SetSource(const std::wstring& file);

		void SetRotation(const RotationAnimationParameter& param);

		bool Update(int64_t time) override;

		void Draw(DrawingContext& context) override;

	protected:
		Image  _Image;
		RotationAnimationParameter _RotationParam;

	};
}


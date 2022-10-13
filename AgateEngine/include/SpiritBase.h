#pragma once
#include <memory>
#include "DrawingContext.h"
#include <vector>
#include <list>

namespace agate
{
	class SpiritBase: public std::enable_shared_from_this<SpiritBase>
	{
	public:
		SpiritBase() :_Begin{ 0 }, _End{0} 
		{

		}
		void Range(int64_t b, int64_t e)
		{
			_Begin = b;
			_End = e;
		}
		/// <summary>
		/// ����Ԫ��
		/// </summary>
		/// <param name="time">ʱ�䣬����</param>
		/// <returns>С�� 0��Ԫ��δ����ʾʱ�䣬0�� ������ʾ�� ����0�� ������������</returns>
		virtual int Update(int64_t time);
		
		virtual void Draw(DrawingContext& context);
		
		void AddSpirit(std::shared_ptr<SpiritBase>&& spirit);

	protected:
		int64_t _Begin;
		int64_t _End;

		std::weak_ptr<SpiritBase> _Parent;
		std::vector<std::shared_ptr<SpiritBase>> _Children;

	};

}
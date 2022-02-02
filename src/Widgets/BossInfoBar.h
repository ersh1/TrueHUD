#pragma once
#include "Widgets/InfoBarBase.h"

namespace Scaleform
{
	class BossInfoBar : public InfoBarBase
	{
	public:
		BossInfoBar(RE::GPtr<RE::GFxMovieView> a_view, RE::ObjectRefHandle a_refHandle) :
			InfoBarBase(a_view, a_refHandle)
		{
			_barType = BarType::kBossInfoBar;
		}

		virtual void Update(float a_deltaTime) override;
		virtual void Initialize() override;

		int32_t GetIndex() const;
		void SetIndex(int32_t a_index);

	protected:
		virtual void UpdatePosition() override;
		virtual void LoadConfig() override;

		int32_t _index = 0;
	};
}

#pragma once
#include "Widgets/InfoBarBase.h"

namespace Scaleform
{
	class ActorInfoBar : public InfoBarBase
	{
	public:
		ActorInfoBar(RE::GPtr<RE::GFxMovieView> a_view, RE::ObjectRefHandle a_refHandle) :
			InfoBarBase(a_view, a_refHandle)
		{
			_barType = InfoBarBase::kActorInfoBar;
		}

		virtual void Update(float a_deltaTime) override;

	protected:
		virtual void UpdatePosition() override;
		virtual void LoadConfig() override;
	};
}

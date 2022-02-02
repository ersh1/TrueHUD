#pragma once
#include "TrueHUDAPI.h"

namespace Scaleform
{
	class ShoutIndicator : public TRUEHUD_API::WidgetBase
	{
	public:
		ShoutIndicator(RE::GPtr<RE::GFxMovieView> a_view) :
			WidgetBase(a_view)
		{}

		virtual void Update(float a_deltaTime) override;
		virtual void Initialize() override;
		virtual void Dispose() override;

		void SetPercent(float a_pct);
		void FlashShoutWidget();

	protected:
		virtual void UpdatePosition();
		virtual void LoadConfig();
		float GetPercent();

	private:
		float _cooldown = 0.f;
	};
}

#pragma once
#include "TrueHUDAPI.h"

namespace Scaleform
{
	class InfoBarBase : public TRUEHUD_API::WidgetBase
	{
	public:
		enum TargetType : std::uint8_t
		{
			kTarget = 0,
			kEnemy = 1,
			kTeammate = 2,
			kOther = 3,
		};

		enum TargetState : std::uint8_t
		{
			kAlive = 0,
			kStunned = 1,
			kBleedingOut = 2,
			kBleedingOutEssential = 3,
			kDead = 4
		};

		enum WidgetStateMode : std::uint8_t
		{
			kAdd = 0,
			kShow = 1,
			kHide = 2,
			kRemove = 3,
			kTargetKilled = 4
		};

		enum BarType : std::uint8_t
		{
			kActorInfoBar = 0,
			kBossInfoBar = 1
		};

		InfoBarBase(RE::GPtr<RE::GFxMovieView> a_view, RE::ObjectRefHandle a_refHandle) :
			WidgetBase(a_view, a_refHandle.native_handle()),
			_refHandle(a_refHandle)
		{}

		virtual void Initialize() override;
		virtual void Dispose() override;

		virtual void SetWidgetState(WidgetStateMode a_state);
		virtual void FlashActorValue(RE::ActorValue a_actorValue, bool a_bLong);
		virtual void FlashSpecial(bool a_bLong);
		virtual void RefreshColors();

		RE::ObjectRefHandle _refHandle;

	protected:
		virtual void UpdatePosition() = 0;
		virtual void LoadConfig() = 0;

		virtual void UpdateInfo();

		bool _bIsOffscreen = false;

		BarType _barType;
	};
}

#include "Widgets/ActorInfoBar.h"
#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"
#include "HUDHandler.h"
#include "Scaleform/Scaleform.h"

namespace Scaleform
{
	void ActorInfoBar::Update([[maybe_unused]] float a_deltaTime)
	{
		RE::GFxValue result;
		_object.Invoke("isReadyToRemove", &result);
		if (result.GetBool()) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		UpdatePosition();
		UpdateInfo();
	}

	void ActorInfoBar::UpdatePosition()
	{
		if (!_refHandle) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		// don't update position if alpha is zero
		/*RE::GFxValue::DisplayInfo dInfo;
		object.GetDisplayInfo(&dInfo);
		if (dInfo.GetAlpha() == 0) {
			return;
		}*/

		/*RE::GFxValue alpha;
		object.GetMember("_alpha", &alpha);
		if (alpha.GetNumber() == 0) {
			return;
		}*/

		RE::GRectF rect = _view->GetVisibleFrameRect();

		RE::NiPoint3 targetPos;
		Utils::GetTargetPos(_refHandle, targetPos, Settings::uInfoBarAnchor == WidgetAnchor::kBody);
		targetPos.z += Settings::fInfoBarOffsetZ;
		RE::NiPoint2 screenPos;

		RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, targetPos, screenPos.x, screenPos.y, _depth, 1e-5f);

		_bIsOffscreen = _depth < 0;

		if (_bIsOffscreen) {
			RE::GFxValue alpha;
			_object.GetMember("_alpha", &alpha);
			if (alpha.GetNumber() == 0) {
				return;
			}
		}

		float fNear = *g_fNear;
		float fFar = *g_fFar;
		float linearizedDepth = fNear * fFar / (fFar + _depth * (fNear - fFar));

		screenPos.y = 1.0f - screenPos.y;  // Flip y for Flash coordinate system
		screenPos.y = rect.top + (rect.bottom - rect.top) * screenPos.y;
		screenPos.x = rect.left + (rect.right - rect.left) * screenPos.x;

		float scale;

		if (Settings::bInfoBarScaleWithDistance) {
			float scaleMult = fmin(fmax(linearizedDepth, 500.f), 2000.f);
			float remapped = (((scaleMult - 500.f) * (50.f - 100.f)) / (2000.f - 500.f)) + 100.f;
			scale = remapped;
		} else {
			scale = 100.f;
		}

		scale *= Settings::fInfoBarScale;

		RE::GFxValue::DisplayInfo displayInfo;
		displayInfo.SetPosition(screenPos.x, screenPos.y);
		displayInfo.SetScale(scale, scale);
		_object.SetDisplayInfo(displayInfo);
	}

	void ActorInfoBar::LoadConfig()
	{
		using BarType = TRUEHUD_API::PlayerWidgetBarType;
		using ColorType = TRUEHUD_API::PlayerWidgetBarColorType;

		auto hudHandler = HUDHandler::GetSingleton();
		auto hudMenu = HUDHandler::GetTrueHUDMenu();

		RE::GFxValue args[44];
		args[0].SetNumber(static_cast<uint32_t>(Settings::uInfoBarDirection));
		args[1].SetNumber(static_cast<uint32_t>(Settings::uInfoBarDisplayName));
		args[2].SetBoolean(Settings::bInfoBarDisplayPhantomBars);
		args[3].SetBoolean(Settings::bInfoBarDisplaySpecialPhantomBar);
		args[4].SetNumber(static_cast<uint32_t>(Settings::uInfoBarDisplayDamageCounter));
		args[5].SetNumber(static_cast<uint32_t>(Settings::uInfoBarDisplayIndicator));
		args[6].SetNumber(static_cast<uint32_t>(Settings::uInfoBarDamageCounterAlignment));
		args[7].SetNumber(static_cast<uint32_t>(Settings::uInfoBarResourcesModeTarget));
		args[8].SetNumber(static_cast<uint32_t>(Settings::uInfoBarResourcesModeHostiles));
		args[9].SetNumber(static_cast<uint32_t>(Settings::uInfoBarResourcesModeTeammates));
		args[10].SetNumber(static_cast<uint32_t>(Settings::uInfoBarResourcesModeOthers));
		args[11].SetNumber(static_cast<uint32_t>(Settings::uInfoBarSpecialModeTarget));
		args[12].SetNumber(static_cast<uint32_t>(Settings::uInfoBarSpecialModeHostiles));
		args[13].SetNumber(static_cast<uint32_t>(Settings::uInfoBarSpecialModeTeammates));
		args[14].SetNumber(static_cast<uint32_t>(Settings::uInfoBarSpecialModeOthers));
		args[15].SetBoolean(hudHandler->bSpecialMode);
		args[16].SetNumber(static_cast<uint32_t>(Settings::uInfoBarIndicatorMode));	
		args[17].SetNumber(Settings::fInfoBarPhantomDuration);
		args[18].SetNumber(Settings::fInfoBarDamageCounterDuration);
		args[19].SetNumber((Settings::bInfoBarUseHUDOpacity ? *g_fHUDOpacity : Settings::fInfoBarOpacity) * 100.f);
		args[20].SetNumber(Settings::fInfoBarWidth);
		args[21].SetNumber(Settings::fInfoBarResourceWidth);
		args[22].SetNumber(Settings::fInfoBarTargetWidth);
		args[23].SetNumber(Settings::fInfoBarTargetResourceWidth);
		args[24].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::BarColor));
		args[25].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::PhantomColor));
		args[26].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::BackgroundColor));
		args[27].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::PenaltyColor));
		args[28].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::FlashColor));
		args[29].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::BarColor));
		args[30].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::PhantomColor));
		args[31].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::BackgroundColor));
		args[32].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::PenaltyColor));
		args[33].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::FlashColor));
		args[34].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::BarColor));
		args[35].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::PhantomColor));
		args[36].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::BackgroundColor));
		args[37].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::PenaltyColor));
		args[38].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::FlashColor));
		args[39].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::BarColor));
		args[40].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::PhantomColor));
		args[41].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::BackgroundColor));
		args[42].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::PenaltyColor));
		args[43].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::FlashColor));
		_object.Invoke("loadConfig", nullptr, args, 44);
	}
}

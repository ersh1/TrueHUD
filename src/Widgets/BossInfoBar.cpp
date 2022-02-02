#include "Widgets/BossInfoBar.h"
#include "Offsets.h"
#include "Settings.h"
#include "HUDHandler.h"

namespace Scaleform
{
	void BossInfoBar::Update([[maybe_unused]] float a_deltaTime)
	{
		RE::GFxValue result;
		_object.Invoke("isReadyToRemove", &result);
		if (result.GetBool()) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		UpdateInfo();
	}

	void BossInfoBar::Initialize()
	{
		InfoBarBase::Initialize();
		UpdatePosition();
	}

	int32_t BossInfoBar::GetIndex() const
	{
		return _index;
	}

	void BossInfoBar::SetIndex(int32_t a_index)
	{
		_index = a_index;
		UpdatePosition();
	}

	void BossInfoBar::UpdatePosition()
	{
		if (!_refHandle) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		RE::GRectF rect = _view->GetVisibleFrameRect();

		//bIsOffscreen = depth < 0;

		RE::NiPoint2 screenPos;
		float offsetY = _index * Settings::fMultipleBossBarsOffset;

		auto def = _view->GetMovieDef();
		if (def) {
			screenPos.x = def->GetWidth();
			screenPos.y = def->GetHeight();
			offsetY /= def->GetHeight();
		}

		if (Settings::uMultipleBossBarsStackDirection == BossBarStackDirection::kUp) {
			offsetY *= -1;
		}

		screenPos.x *= Settings::fBossBarAnchorX;
		screenPos.y *= Settings::fBossBarAnchorY + offsetY;

		float scale = Settings::fBossBarScale * 100.f;

		RE::GFxValue::DisplayInfo displayInfo;
		displayInfo.SetPosition(screenPos.x, screenPos.y);
		displayInfo.SetScale(scale, scale);
		_object.SetDisplayInfo(displayInfo);
	}

	void BossInfoBar::LoadConfig()
	{
		using BarType = TRUEHUD_API::PlayerWidgetBarType;
		using ColorType = TRUEHUD_API::PlayerWidgetBarColorType;

		auto hudHandler = HUDHandler::GetSingleton();
		auto hudMenu = HUDHandler::GetTrueHUDMenu();

		RE::GFxValue args[37];
		args[0].SetNumber(static_cast<uint32_t>(Settings::uBossBarDirection));
		args[1].SetBoolean(Settings::bBossBarDisplayPhantomBars);
		args[2].SetBoolean(Settings::bBossBarDisplaySpecialPhantomBar);
		args[3].SetBoolean(Settings::bBossBarDisplayDamageCounter);
		args[4].SetBoolean(Settings::bBossBarDisplayIndicator);
		args[5].SetNumber(static_cast<uint32_t>(Settings::uBossBarDamageCounterAlignment));
		args[6].SetNumber(static_cast<uint32_t>(Settings::uBossBarResourcesMode));
		args[7].SetNumber(static_cast<uint32_t>(Settings::uBossBarSpecialMode));
		args[8].SetBoolean(hudHandler->bSpecialMode);
		args[9].SetNumber(static_cast<uint32_t>(Settings::uBossBarResourceAlignment));
		args[10].SetNumber(static_cast<uint32_t>(Settings::uBossBarNameAlignment));
		args[11].SetNumber(static_cast<uint32_t>(Settings::uBossBarIndicatorMode));
		args[12].SetNumber(Settings::fBossBarPhantomDuration);
		args[13].SetNumber(Settings::fBossBarDamageCounterDuration);
		args[14].SetNumber((Settings::bBossBarUseHUDOpacity ? *g_fHUDOpacity : Settings::fBossBarOpacity) * 100.f);
		args[15].SetNumber(Settings::fBossBarWidth);
		args[16].SetNumber(Settings::fBossBarResourceWidth);
		args[17].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::BarColor));
		args[18].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::PhantomColor));
		args[19].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::BackgroundColor));
		args[20].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::PenaltyColor));
		args[21].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::HealthBar, ColorType::FlashColor));
		args[22].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::BarColor));
		args[23].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::PhantomColor));
		args[24].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::BackgroundColor));
		args[25].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::PenaltyColor));
		args[26].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::MagickaBar, ColorType::FlashColor));
		args[27].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::BarColor));
		args[28].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::PhantomColor));
		args[29].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::BackgroundColor));
		args[30].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::PenaltyColor));
		args[31].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::StaminaBar, ColorType::FlashColor));
		args[32].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::BarColor));
		args[33].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::PhantomColor));
		args[34].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::BackgroundColor));
		args[35].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::PenaltyColor));
		args[36].SetNumber(hudMenu->GetPlayerWidgetBarColor(BarType::SpecialBar, ColorType::FlashColor));
		_object.Invoke("loadConfig", nullptr, args, 37);
	}
}

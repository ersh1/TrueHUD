#include "Widgets/PlayerWidget.h"
#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"
#include "HUDHandler.h"

namespace Scaleform
{
	void PlayerWidget::Update([[maybe_unused]] float a_deltaTime)
	{
		if (!_bInitialized) {
			RE::GFxValue result;
			_object.Invoke("isInitialized", &result);
			if (result.GetBool()) {
				_bInitialized = true;
				UpdatePenalty();
			}
		}

		UpdateInfo();

		if (Settings::bPlayerWidgetHideVanillaBars && Settings::bPlayerWidgetForceHideVanillaBars) {
			ToggleVanillaBars(false);
		}

		if (Settings::bPlayerWidgetDisplayEnchantmentChargeMeter && Settings::bPlayerWidgetForceHideVanillaBars) {
			ToggleVanillaEnchantmentChargeMeters(false);
		}
	}

	void PlayerWidget::Initialize()
	{
		LoadConfig();

		if (Settings::bPlayerWidgetHideVanillaBars) {
			ToggleVanillaBars(false);
		} else if (_bVanillaBarsHidden) {
			ToggleVanillaBars(true);
		}

		if (Settings::bPlayerWidgetDisplayEnchantmentChargeMeter) {
			ToggleVanillaEnchantmentChargeMeters(false);
		} else if (_bVanillaEnchantmentChargeMetersHidden) {
			ToggleVanillaEnchantmentChargeMeters(true);
		}

		UpdatePosition();
	}

	void PlayerWidget::Dispose()
	{
		if (_bVanillaBarsHidden) {
			ToggleVanillaBars(true);
		}

		if (_bVanillaEnchantmentChargeMetersHidden) {
			ToggleVanillaEnchantmentChargeMeters(true);
		}

		_object.Invoke("cleanUp", nullptr, nullptr, 0);
	}

	void PlayerWidget::FlashActorValue(RE::ActorValue a_actorValue, bool a_bLong)
	{
		RE::GFxValue args[2];
		args[0].SetNumber(static_cast<int32_t>(a_actorValue));
		args[1].SetBoolean(a_bLong);

		_object.Invoke("flashAV", nullptr, args, 2);
	}

	void PlayerWidget::FlashSpecial(bool a_bLong)
	{
		RE::GFxValue arg;
		arg.SetBoolean(a_bLong);

		_object.Invoke("flashSpecial", nullptr, &arg, 1);
	}

	void PlayerWidget::RefreshColors()
	{
		using ColorType = TRUEHUD_API::BarColorType;
		
		auto hudMenu = HUDHandler::GetTrueHUDMenu();
		auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();

		RE::GFxValue args[20];
		args[0].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::BarColor));
		args[1].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::PhantomColor));
		args[2].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::BackgroundColor));
		args[3].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::PenaltyColor));
		args[4].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::FlashColor));
		args[5].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::BarColor));
		args[6].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::PhantomColor));
		args[7].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::BackgroundColor));
		args[8].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::PenaltyColor));
		args[9].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::FlashColor));
		args[10].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::BarColor));
		args[11].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::PhantomColor));
		args[12].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::BackgroundColor));
		args[13].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::PenaltyColor));
		args[14].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::FlashColor));
		args[15].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::BarColor));
		args[16].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::PhantomColor));
		args[17].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::BackgroundColor));
		args[18].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::PenaltyColor));
		args[19].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::FlashColor));
		_object.Invoke("setColors", nullptr, args, 20);
	}

	void PlayerWidget::UpdatePenaltyPercentages()
	{
		if (Settings::glob_survivalModeEnabled &&
			Settings::glob_survivalHealthPenaltyPercent &&
			Settings::glob_survivalMagickaPenaltyPercent &&
			Settings::glob_survivalStaminaPenaltyPercent)
		{
			if (Settings::glob_survivalModeEnabled->value == 1) {
				_healthPenaltyPct = Settings::glob_survivalHealthPenaltyPercent->value / 100.f;
				_magickaPenaltyPct = Settings::glob_survivalMagickaPenaltyPercent->value / 100.f;
				_staminaPenaltyPct = Settings::glob_survivalStaminaPenaltyPercent->value / 100.f;
			} else {
				_healthPenaltyPct = 0.f;
				_magickaPenaltyPct = 0.f;
				_staminaPenaltyPct = 0.f;
			}
			UpdatePenalty();
		}
	}

	void PlayerWidget::UpdatePlayerWidgetChargeMeters(float a_percent, bool a_bForce, bool a_bLeftHand, bool a_bShow)
	{
		RE::GFxValue args[4];
		args[0].SetNumber(a_percent);
		args[1].SetBoolean(a_bForce);
		args[2].SetBoolean(a_bLeftHand);
		args[3].SetBoolean(a_bShow);
		_object.Invoke("updateEnchantmentChargeMeter", nullptr, args, 4);
	}

	void PlayerWidget::UpdatePosition()
	{
		RE::GFxValue::DisplayInfo displayInfo;
		float scale = Settings::fPlayerWidgetScale * 100.f;
		displayInfo.SetScale(scale, scale);

		if (Settings::bPlayerWidgetCombined) {
			RE::GRectF rect = _view->GetVisibleFrameRect();

			RE::NiPoint2 screenPos;

			auto def = _view->GetMovieDef();
			if (def) {
				screenPos.x = def->GetWidth();
				screenPos.y = def->GetHeight();
			}

			screenPos.x *= Settings::fPlayerWidgetX;
			screenPos.y *= Settings::fPlayerWidgetY;

			displayInfo.SetPosition(screenPos.x, screenPos.y);
		} else {
			displayInfo.SetPosition(0, 0);
		}
		
		_object.SetDisplayInfo(displayInfo);
	}

	void PlayerWidget::LoadConfig()
	{
		using ColorType = TRUEHUD_API::BarColorType;

		auto hudHandler = HUDHandler::GetSingleton();
		auto hudMenu = HUDHandler::GetTrueHUDMenu();

		auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();

		RE::NiPoint2 stageSize;
		auto def = _view->GetMovieDef();
		if (def) {
			stageSize.x = def->GetWidth();
			stageSize.y = def->GetHeight();
		}

		RE::GFxValue args[54];
		args[0].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetHealthBarDirection));
		args[1].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetMagickaBarDirection));
		args[2].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetStaminaBarDirection));
		args[3].SetBoolean(Settings::bPlayerWidgetDisplayPhantomBars);
		args[4].SetBoolean(Settings::bPlayerWidgetDisplaySpecialPhantomBar);
		args[5].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetHealthMode));
		args[6].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetMagickaMode));
		args[7].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetStaminaMode));
		args[8].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetShoutIndicatorMode));
		args[9].SetNumber(static_cast<uint32_t>(Settings::uPlayerWidgetSpecialMode));
		args[10].SetBoolean(hudHandler->bSpecialMode);
		args[11].SetBoolean(Settings::bPlayerWidgetDisplayEnchantmentChargeMeter);
		args[12].SetNumber(Settings::fPlayerWidgetPhantomDuration);
		args[13].SetNumber(Settings::fPlayerWidgetScale);
		args[14].SetNumber((Settings::bPlayerWidgetUseHUDOpacity ? *g_fHUDOpacity : Settings::fPlayerWidgetOpacity) * 100.f);
		args[15].SetNumber(Settings::fPlayerWidgetBarWidth);
		args[16].SetNumber(Settings::fPlayerWidgetSpecialBarWidth);
		args[17].SetBoolean(Settings::bPlayerWidgetScaleBars);
		args[18].SetNumber(Settings::fPlayerWidgetMinBarWidth);
		args[19].SetNumber(Settings::fPlayerWidgetMaxBarWidth);
		args[20].SetNumber(Settings::fPlayerWidgetHealthBarScaleMult);
		args[21].SetNumber(Settings::fPlayerWidgetMagickaBarScaleMult);
		args[22].SetNumber(Settings::fPlayerWidgetStaminaBarScaleMult);
		args[23].SetNumber(stageSize.x * Settings::fPlayerWidgetX);
		args[24].SetNumber(stageSize.y * Settings::fPlayerWidgetY);
		args[25].SetNumber(stageSize.x * Settings::fPlayerWidgetEnchantmentChargeMeterX);
		args[26].SetNumber(stageSize.y * Settings::fPlayerWidgetEnchantmentChargeMeterY);
		args[27].SetNumber(Settings::bPlayerWidgetCombined);
		args[28].SetNumber(stageSize.x * Settings::fPlayerWidgetHealthX);
		args[29].SetNumber(stageSize.y * Settings::fPlayerWidgetHealthY);
		args[30].SetNumber(stageSize.x * Settings::fPlayerWidgetMagickaX);
		args[31].SetNumber(stageSize.y * Settings::fPlayerWidgetMagickaY);
		args[32].SetNumber(stageSize.x * Settings::fPlayerWidgetStaminaX);
		args[33].SetNumber(stageSize.y * Settings::fPlayerWidgetStaminaY);
		args[34].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::BarColor));
		args[35].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::PhantomColor));
		args[36].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::BackgroundColor));
		args[37].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::PenaltyColor));
		args[38].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kHealth, ColorType::FlashColor));
		args[39].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::BarColor));
		args[40].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::PhantomColor));
		args[41].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::BackgroundColor));
		args[42].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::PenaltyColor));
		args[43].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kMagicka, ColorType::FlashColor));
		args[44].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::BarColor));
		args[45].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::PhantomColor));
		args[46].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::BackgroundColor));
		args[47].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::PenaltyColor));
		args[48].SetNumber(hudMenu->GetBarColor(playerHandle, RE::ActorValue::kStamina, ColorType::FlashColor));
		args[49].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::BarColor));
		args[50].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::PhantomColor));
		args[51].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::BackgroundColor));
		args[52].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::PenaltyColor));
		args[53].SetNumber(hudMenu->GetSpecialBarColor(playerHandle, ColorType::FlashColor));
		_object.Invoke("loadConfig", nullptr, args, 54);
	}

	void PlayerWidget::UpdateInfo()
	{
		SetPercent(GetPercent());
		UpdatePenaltyPercentages();

		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		float permanentHealth, temporaryHealth, maxHealth, health, permanentMagicka, temporaryMagicka, maxMagicka, magicka, permanentStamina, temporaryStamina, maxStamina, stamina;

		permanentHealth = playerCharacter->GetPermanentActorValue(RE::ActorValue::kHealth);
		temporaryHealth = playerCharacter->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kHealth);
		maxHealth = permanentHealth + temporaryHealth;
		maxHealth /= (1.f - _healthPenaltyPct);
		if (playerCharacter->IsDead()) {
			health = 0;
		} else {
			health = playerCharacter->GetActorValue(RE::ActorValue::kHealth);
		}

		permanentMagicka = playerCharacter->GetPermanentActorValue(RE::ActorValue::kMagicka);
		temporaryMagicka = playerCharacter->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kMagicka);
		maxMagicka = permanentMagicka + temporaryMagicka;
		maxMagicka /= (1.f - _magickaPenaltyPct);
		magicka = playerCharacter->GetActorValue(RE::ActorValue::kMagicka);

		RE::ActorPtr mount;
		bool bIsInMountMode = false;
		if (Settings::bPlayerWidgetDisplayMountStamina && playerCharacter->GetMount(mount)) {
			permanentStamina = mount->GetPermanentActorValue(RE::ActorValue::kStamina);
			temporaryStamina = mount->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kStamina);
			maxStamina = permanentStamina + temporaryStamina;
			stamina = mount->GetActorValue(RE::ActorValue::kStamina);
			bIsInMountMode = true;
		} else {
			permanentStamina = playerCharacter->GetPermanentActorValue(RE::ActorValue::kStamina);
			temporaryStamina = playerCharacter->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kStamina);
			maxStamina = permanentStamina + temporaryStamina;
			maxStamina /= (1.f - _staminaPenaltyPct);
			stamina = playerCharacter->GetActorValue(RE::ActorValue::kStamina);
			bIsInMountMode = false;
		}

		bool bIsInCombat = playerCharacter->IsInCombat();

		RE::GFxValue args[12];
		args[0].SetNumber(ceil(maxHealth));
		args[1].SetNumber(ceil(health));
		args[2].SetNumber(Utils::GetPct(health, maxHealth));
		args[3].SetNumber(ceil(maxMagicka));
		args[4].SetNumber(ceil(magicka));
		args[5].SetNumber(Utils::GetPct(magicka, maxMagicka));
		args[6].SetNumber(ceil(maxStamina));
		args[7].SetNumber(ceil(stamina));
		args[8].SetNumber(Utils::GetPct(stamina, maxStamina));
		args[9].SetBoolean(bIsInCombat);
		args[10].SetBoolean(bIsInMountMode);
		args[11].SetBoolean(false);

		_object.Invoke("updateData", nullptr, args, 12);

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler && hudHandler->bSpecialFunctionsProvided) {
			RE::GFxValue specialArgs[3];
			auto maxSpecial = fmax(hudHandler->GetMaxSpecial(playerCharacter), 0.f);
			auto currentSpecial = fmax(hudHandler->GetCurrentSpecial(playerCharacter), 0.f);

			specialArgs[0].SetNumber(ceil(maxSpecial));
			specialArgs[1].SetNumber(ceil(currentSpecial));
			specialArgs[2].SetNumber(Utils::GetPct(currentSpecial, maxSpecial));
			_object.Invoke("updateSpecial", nullptr, specialArgs, 3);
		}
	}

	void PlayerWidget::UpdatePenalty()
	{
		RE::GFxValue penaltyArgs[4];

		penaltyArgs[0].SetNumber(_healthPenaltyPct);
		penaltyArgs[1].SetNumber(_magickaPenaltyPct);
		penaltyArgs[2].SetNumber(_staminaPenaltyPct);
		penaltyArgs[3].SetBoolean(false);

		_object.Invoke("updatePenalty", nullptr, penaltyArgs, 4);
	}

	void PlayerWidget::ToggleVanillaBars(bool a_bEnable)
	{
		auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
		auto& uiMovie = hud->uiMovie;
		if (uiMovie) {
			RE::GFxValue bEnable;
			bEnable.SetBoolean(a_bEnable);
			/*uiMovie->SetVariable("HUDMovieBaseInstance.Health._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Magica._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Stamina._visible", bEnable);*/
			uiMovie->SetVariable("HUDMovieBaseInstance.Health.HealthMeter_mc._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Health.HealthPenaltyMeter_mc._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Magica.MagickaMeter_mc._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Magica.MagickaFlashInstance._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Magica.MagickaPenaltyMeter_mc._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Stamina.StaminaMeter_mc._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Stamina.StaminaFlashInstance._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.Stamina.StaminaPenaltyMeter_mc._visible", bEnable);

			_bVanillaBarsHidden = !a_bEnable;
		}		
	}

	void PlayerWidget::ToggleVanillaEnchantmentChargeMeters(bool a_bEnable)
	{
		auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
		auto& uiMovie = hud->uiMovie;
		if (uiMovie) {
			RE::GFxValue bEnable;
			bEnable.SetBoolean(a_bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.BottomLeftLockInstance._visible", bEnable);
			uiMovie->SetVariable("HUDMovieBaseInstance.BottomRightLockInstance._visible", bEnable);

			_bVanillaEnchantmentChargeMetersHidden = !a_bEnable;
		}		
	}

}

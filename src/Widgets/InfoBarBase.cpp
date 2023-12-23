#include "Widgets/InfoBarBase.h"
#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"
#include "HUDHandler.h"
#include "NPCNameProvider.h"

namespace Scaleform
{
	void InfoBarBase::Initialize()
	{
		LoadConfig();
		SetWidgetState(WidgetStateMode::kShow);
	}

	void InfoBarBase::Dispose()
	{
		_object.Invoke("cleanUp", nullptr, nullptr, 0);
	}

	void InfoBarBase::SetWidgetState(WidgetStateMode a_state)
	{
		switch (a_state) {
		case WidgetStateMode::kAdd:
			if (_widgetState == kPendingHide || _widgetState == kPendingRemoval || _widgetState == kHidden) {
				_widgetState = kActive;
			}
		case WidgetStateMode::kShow:
			if (_widgetState == kPendingHide || _widgetState == kHidden) {
				_widgetState = kActive;
			}
			break;
		case WidgetStateMode::kHide:
			if (_widgetState == kActive) {
				_widgetState = kPendingHide;
			}
			break;
		case WidgetStateMode::kRemove:
			if (_widgetState < kPendingRemoval) {
				_widgetState = kPendingRemoval;
			}
			break;
		case WidgetStateMode::kTargetKilled:
			if (_widgetState < kDelayedRemoval) {
				_widgetState = kDelayedRemoval;
			}
			break;
		}
	}

	void InfoBarBase::FlashActorValue(RE::ActorValue a_actorValue, bool a_bLong)
	{
		RE::GFxValue args[2];
		args[0].SetNumber(static_cast<int32_t>(a_actorValue));
		args[1].SetBoolean(a_bLong);

		_object.Invoke("flashAV", nullptr, args, 2);
	}

	void InfoBarBase::FlashSpecial(bool a_bLong)
	{
		RE::GFxValue arg;
		arg.SetBoolean(a_bLong);

		_object.Invoke("flashSpecial", nullptr, &arg, 1);
	}

	void InfoBarBase::RefreshColors()
	{
		using ColorType = TRUEHUD_API::BarColorType;

		auto hudMenu = HUDHandler::GetTrueHUDMenu();

		RE::GFxValue args[20];
		args[0].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kHealth, ColorType::BarColor));
		args[1].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kHealth, ColorType::PhantomColor));
		args[2].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kHealth, ColorType::BackgroundColor));
		args[3].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kHealth, ColorType::PenaltyColor));
		args[4].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kHealth, ColorType::FlashColor));
		args[5].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kMagicka, ColorType::BarColor));
		args[6].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kMagicka, ColorType::PhantomColor));
		args[7].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kMagicka, ColorType::BackgroundColor));
		args[8].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kMagicka, ColorType::PenaltyColor));
		args[9].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kMagicka, ColorType::FlashColor));
		args[10].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kStamina, ColorType::BarColor));
		args[11].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kStamina, ColorType::PhantomColor));
		args[12].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kStamina, ColorType::BackgroundColor));
		args[13].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kStamina, ColorType::PenaltyColor));
		args[14].SetNumber(hudMenu->GetBarColor(_refHandle, RE::ActorValue::kStamina, ColorType::FlashColor));
		args[15].SetNumber(hudMenu->GetSpecialBarColor(_refHandle, ColorType::BarColor));
		args[16].SetNumber(hudMenu->GetSpecialBarColor(_refHandle, ColorType::PhantomColor));
		args[17].SetNumber(hudMenu->GetSpecialBarColor(_refHandle, ColorType::BackgroundColor));
		args[18].SetNumber(hudMenu->GetSpecialBarColor(_refHandle, ColorType::PenaltyColor));
		args[19].SetNumber(hudMenu->GetSpecialBarColor(_refHandle, ColorType::FlashColor));
		_object.Invoke("setColors", nullptr, args, 20);
	}

	void InfoBarBase::UpdateInfo()
	{	
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerLevel = playerCharacter->GetLevel();

		if (!_refHandle || !_refHandle.get()) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		auto actor = _refHandle.get()->As<RE::Actor>();
		if (!actor) {
			_widgetState = WidgetState::kRemoved;
			return;
		}
		
		auto hudMenu = HUDHandler::GetTrueHUDMenu();

		auto targetHandle = hudMenu->GetTarget();
		auto softTargetHandle = hudMenu->GetSoftTarget();

		bool bIsTeammate = Utils::IsPlayerTeammateOrSummon(actor);

		TargetType targetType;
		if (targetHandle && _refHandle == targetHandle) {
			targetType = kTarget;
		} else if (!targetHandle && _refHandle == softTargetHandle) {
			targetType = kTarget;
		} else if (actor->IsHostileToActor(playerCharacter)) {
			targetType = kEnemy;
		} else if (bIsTeammate) {
			targetType = kTeammate;
		} else {
			targetType = kOther;
		}

		if (targetType == kTarget && _widgetState == kPendingRemoval) {
			SetWidgetState(WidgetStateMode::kAdd);
		}

		TargetState targetState;
		if (actor->IsDead()) {
			targetState = kDead;
		} else if (actor->AsActorState()->IsBleedingOut()) {
			targetState = actor->IsEssential() ? kBleedingOutEssential : kBleedingOut;
		} else {
			targetState = kAlive;
		}

		if (_widgetState < kDelayedRemoval) {
			RE::GFxValue result;
			_object.Invoke("isHidden", &result);
			if (result.GetBool()) {
				_widgetState = WidgetState::kHidden;
			}

			const auto shouldBarBeDisplayed = [&]() {
				if (_barType == BarType::kBossInfoBar) {
					return !bIsTeammate && actor->IsInCombat() && actor->IsHostileToActor(playerCharacter);
				} else if (_barType == BarType::kActorInfoBar) {
					if (targetType > kTarget && !actor->IsInCombat()) {
						return false;
					}
					switch (targetType) {
					case TargetType::kTarget:
						return true;
					case TargetType::kEnemy:
						return Settings::uInfoBarDisplayHostiles > InfoBarsDisplayMode::kNever;
					case TargetType::kTeammate:
						return Settings::uInfoBarDisplayTeammates > InfoBarsDisplayMode::kNever;
					case TargetType::kOther:
						return Settings::uInfoBarDisplayOthers > InfoBarsDisplayMode::kNever;
					}
				}
				return false;
			};

			if (targetState > kBleedingOut) {
				SetWidgetState(WidgetStateMode::kTargetKilled);
			} else if (!shouldBarBeDisplayed()) {
				SetWidgetState(WidgetStateMode::kRemove);
			} else if (_barType == BarType::kActorInfoBar) {
				if (_bIsOffscreen) {
					SetWidgetState(WidgetStateMode::kHide);
				} else {
					bool r8 = false;
					bool bHasLOS = targetType == kTarget ? true : playerCharacter->HasLineOfSight(actor, r8);
					bool bVisible = bHasLOS && !(actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kInvisibility) > 0);
					if (bVisible) {
						SetWidgetState(WidgetStateMode::kShow);
					} else {
						SetWidgetState(WidgetStateMode::kHide);
					}
				}
			} 
		}

		auto targetName = NPCNameProvider::GetSingleton()->GetName(actor);
		auto targetLevel = actor->GetLevel();

		uint32_t levelColor = Settings::uDefaultColor;
		uint32_t outlineColor = Settings::uDefaultColorOutline;
		if (bIsTeammate) {
			levelColor = Settings::uTeammateColor;
			outlineColor = Settings::uTeammateColorOutline;
		} else if (playerLevel - targetLevel > Settings::uInfoBarLevelThreshold) {
			levelColor = Settings::uWeakerColor;
			if (Settings::bInfoBarLevelColorOutline) {
				outlineColor = Settings::uWeakerColorOutline;
			}
		} else if (targetLevel - playerLevel > Settings::uInfoBarLevelThreshold) {
			levelColor = Settings::uStrongerColor;
			if (Settings::bInfoBarLevelColorOutline) {
				outlineColor = Settings::uStrongerColorOutline;
			}
		}

		auto hudHandler = HUDHandler::GetSingleton();

		uint32_t soulType = 0;
		
		if (Settings::kywd_Dragon && actor->HasKeyword(Settings::kywd_Dragon)) {
			soulType = 7;
		} else if (Settings::kywd_noSoulTrap && actor->HasKeyword(Settings::kywd_noSoulTrap)) {
			soulType = 0;
		} else if (auto isSentient = IsSentient(actor)) {  // If sentient, then don't bother - all sentients have black soul gem levels
			soulType = 6;
		} else {
			soulType = GetSoulType(targetLevel, static_cast<uint8_t>(isSentient));
		}

		auto actorValueOwner = actor->AsActorValueOwner();
		float permanentHealth = actorValueOwner->GetPermanentActorValue(RE::ActorValue::kHealth);
		float temporaryHealth = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kHealth);
		float maxHealth = permanentHealth + temporaryHealth;
		float health;
		if (targetState == kDead || targetState == kBleedingOutEssential) {
			health = 0;
		} else {
			health = actorValueOwner->GetActorValue(RE::ActorValue::kHealth);
		}
		float permanentMagicka = actorValueOwner->GetPermanentActorValue(RE::ActorValue::kMagicka);
		float temporaryMagicka = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kMagicka);
		float maxMagicka = permanentMagicka + temporaryMagicka;
		float magicka = actorValueOwner->GetActorValue(RE::ActorValue::kMagicka);
		float permanentStamina = actorValueOwner->GetPermanentActorValue(RE::ActorValue::kStamina);
		float temporaryStamina = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kStamina);
		float maxStamina = permanentStamina + temporaryStamina;
		float stamina = actorValueOwner->GetActorValue(RE::ActorValue::kStamina);
		
		RE::GFxValue args[18];
		args[0].SetNumber(targetType);
		args[1].SetNumber(_widgetState);
		args[2].SetNumber(_depth);
		args[3].SetNumber(targetState);
		args[4].SetString(targetName);
		args[5].SetNumber(targetLevel);
		args[6].SetNumber(levelColor);
		args[7].SetNumber(outlineColor);
		args[8].SetNumber(soulType);
		args[9].SetNumber(ceil(maxHealth));
		args[10].SetNumber(ceil(health));
		args[11].SetNumber(Utils::GetPct(health, maxHealth));
		args[12].SetNumber(ceil(maxMagicka));
		args[13].SetNumber(ceil(magicka));
		args[14].SetNumber(Utils::GetPct(magicka, maxMagicka));
		args[15].SetNumber(ceil(maxStamina));
		args[16].SetNumber(ceil(stamina));
		args[17].SetNumber(Utils::GetPct(stamina, maxStamina));

		_object.Invoke("updateData", nullptr, args, 18);

		if (hudHandler && hudHandler->bSpecialFunctionsProvided) {
			RE::GFxValue specialArgs[3];
			auto maxSpecial = fmax(hudHandler->GetMaxSpecial(actor), 0.f);
			auto currentSpecial = fmax(hudHandler->GetCurrentSpecial(actor), 0.f);

			specialArgs[0].SetNumber(ceil(maxSpecial));
			specialArgs[1].SetNumber(ceil(currentSpecial));
			specialArgs[2].SetNumber(Utils::GetPct(currentSpecial, maxSpecial));
			_object.Invoke("updateSpecial", nullptr, specialArgs, 3);
		}
	}
}

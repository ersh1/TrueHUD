#include "Hooks.h"
#include "Settings.h"
#include "Offsets.h"
#include "HUDHandler.h"
#include "ModAPI.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Install()
	{
		logger::trace("Hooking...");

		HUDHook::Hook();
		//DamageHook::Hook();

		logger::trace("...success");
	}

	bool HUDHook::ProcessMessage(uintptr_t a_enemyHealth, RE::UIMessage* a_uiMessage)
	{
		bool bReturn = _ProcessMessage(a_enemyHealth, a_uiMessage);

		// if no other plugin controls targets, set the vanilla enemy target as the soft target
		auto trueHUDInterface = Messaging::TrueHUDInterface::GetSingleton();
		if (!trueHUDInterface->IsTargetControlTaken()) {
			RE::ActorHandle actorHandle = *(RE::ActorHandle*)(a_enemyHealth + 0x28);

			if (actorHandle) {
				HUDHandler::GetSingleton()->SetSoftTarget(actorHandle);
			} else {
				HUDHandler::GetSingleton()->SetSoftTarget(RE::ActorHandle());
			}
		}

		return bReturn;
	}

	void HUDHook::ProcessButton(RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		_ProcessButton(a_this, a_event, a_data);

		if (Settings::bPlayerWidgetDisplayMountStamina) {
			RE::ActorPtr playerMount = nullptr;
			bool bPlayerHasMount = RE::PlayerCharacter::GetSingleton()->GetMount(playerMount);
			if (bPlayerHasMount && playerMount->GetActorValue(RE::ActorValue::kStamina) <= 0.f) {
				FlashHUDMenuMeter(RE::ActorValue::kStamina);
			}
		}
	}

	void HUDHook::AddMessage_Flash(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data)
	{
		_AddMessage_Flash(a_this, a_menuName, a_type, a_data);

		RE::ActorValue actorValue = static_cast<RE::ActorValue>(a_data->discovery.underlying());
		if (actorValue == RE::ActorValue::kVoiceRate || actorValue == RE::ActorValue::kStamina || actorValue == RE::ActorValue::kMagicka || actorValue == RE::ActorValue::kHealth) {
			
			HUDHandler::GetSingleton()->FlashActorValue(RE::PlayerCharacter::GetSingleton()->GetHandle(), actorValue, true);
		}
	}

	void HUDHook::AddMessage_SetHUDCartMode(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data)
	{
		_AddMessage_SetHUDCartMode(a_this, a_menuName, a_type, a_data);

		if (a_data && a_data->text == "CartMode"sv) {
			HUDHandler::GetSingleton()->SetCartMode(a_data->unk40);
		}
	}

	bool HUDHook::Invoke_ChargeMeter1(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj)
	{
		bool bReturn = _Invoke_ChargeMeter1(a_this, a_data, a_result, a_name, a_args, a_numArgs, a_isDObj);

		if (Settings::bEnablePlayerWidget && Settings::bPlayerWidgetDisplayEnchantmentChargeMeter) {
			HUDHandler::GetSingleton()->UpdatePlayerWidgetChargeMeters(static_cast<float>(a_args[0].GetNumber()), a_args[1].GetBool(), a_args[2].GetBool(), a_args[3].GetBool());
		}		

		return bReturn;
	}

	bool HUDHook::Invoke_ChargeMeter2(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj)
	{
		bool bReturn = _Invoke_ChargeMeter2(a_this, a_data, a_result, a_name, a_args, a_numArgs, a_isDObj);

		if (Settings::bEnablePlayerWidget && Settings::bPlayerWidgetDisplayEnchantmentChargeMeter) {
			HUDHandler::GetSingleton()->UpdatePlayerWidgetChargeMeters(static_cast<float>(a_args[0].GetNumber()), a_args[1].GetBool(), a_args[2].GetBool(), a_args[3].GetBool());
		}		

		return bReturn;
	}

	bool HUDHook::IsPlayerOrPlayerMount(RE::Actor* a_actor)
	{
		if (Settings::bPlayerWidgetDisplayMountStamina) {
			RE::ActorPtr playerMount = nullptr;
			bool bPlayerHasMount = RE::PlayerCharacter::GetSingleton()->GetMount(playerMount);
			return a_actor->IsPlayerRef() || (bPlayerHasMount && a_actor == playerMount.get());
		} else {
			return a_actor->IsPlayerRef();
		}
	}

	void HUDHook::FlashHUDStaminaMountHook()
	{
#ifdef IS_SKYRIM_AE
		static REL::Relocation<uintptr_t> hook{ REL::ID(38022) };  // 636870
#else
		static REL::Relocation<uintptr_t> hook{ REL::ID(36994) };  // 60E820
#endif
		struct Patch : Xbyak::CodeGenerator
		{
			explicit Patch(uintptr_t a_funcAddr) {
				Xbyak::Label funcLabel;

				mov(rcx, rdi);
				call(ptr[rip + funcLabel]);
				cmp(rax, true);

				jmp(ptr[rip]);
				dq(hook.address() + 0x142);

				L(funcLabel);
				dq(a_funcAddr);
			}
		};

		Patch patch(reinterpret_cast<uintptr_t>(IsPlayerOrPlayerMount));
		patch.ready();

		REL::safe_fill(hook.address() + 0x13B, 0x90, 7);
		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<6>(hook.address() + 0x13B, trampoline.allocate(patch));
	}

	void DamageHook::HandleHealthDamage_Actor(RE::Actor* a_this, RE::Actor* a_attacker, float a_damage)
	{
		_HandleHealthDamage_Actor(a_this, a_attacker, a_damage);

		if (Settings::bEnableFloatingText && Settings::bEnableFloatingCombatText) {
			if (a_attacker == RE::PlayerCharacter::GetSingleton()) {
				HUDHandler::GetSingleton()->AddFloatingWorldTextWidget(std::to_string(static_cast<int32_t>(ceil(-a_damage))), 0xFFFFFF, 2.f, false, a_this->GetLookingAtLocation());
			}
		}
	}

	void DamageHook::HandleHealthDamage_Character(RE::Character* a_this, RE::Actor* a_attacker, float a_damage)
	{
		_HandleHealthDamage_Character(a_this, a_attacker, a_damage);

		if (Settings::bEnableFloatingText && Settings::bEnableFloatingCombatText) {
			if (a_attacker == RE::PlayerCharacter::GetSingleton()) {
				auto lastHitData = a_this->currentProcess->middleHigh->lastHitData;
				if (lastHitData && lastHitData->totalDamage == -a_damage) {  // physical attack
					HUDHandler::GetSingleton()->AddFloatingWorldTextWidget(std::to_string(static_cast<int32_t>(-ceil(a_damage))), 0xFFFFFF, 2.f, true, lastHitData->unk00);
				} else {  // spell/other
					HUDHandler::GetSingleton()->AddStackingDamageWorldTextWidget(a_this->GetHandle(), -a_damage);
					//HUDHandler::GetSingleton()->AddFloatingWorldTextWidget(std::to_string(static_cast<int32_t>(-ceil(a_damage))), 0xFFFFFF, 2.f, false, a_this->GetLookingAtLocation());
				}
			}
		}
	}

	void DamageHook::HandleHealthDamage_PlayerCharacter(RE::PlayerCharacter* a_this, RE::Actor* a_attacker, float a_damage)
	{
		_HandleHealthDamage_PlayerCharacter(a_this, a_attacker, a_damage);
		
		if (Settings::bEnableFloatingText && Settings::bEnableFloatingCombatText) {
			HUDHandler::GetSingleton()->AddFloatingWorldTextWidget(std::to_string(static_cast<int32_t>(ceil(-a_damage))), 0xFF0000, 2.f, false, a_this->GetLookingAtLocation());
		}
	}
}

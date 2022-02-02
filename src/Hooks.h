#pragma once

namespace Hooks
{	
	class HUDHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> EnemyHealthVtbl{ RE::VTABLE_EnemyHealth[0] };
			_ProcessMessage = EnemyHealthVtbl.write_vfunc(0x2, ProcessMessage);

			REL::Relocation<std::uintptr_t> SprintHandlerVtbl{ RE::VTABLE_SprintHandler[0] };
			_ProcessButton = SprintHandlerVtbl.write_vfunc(0x4, ProcessButton);

			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(52845) };  // 905A10 // FlashHudMenuMeter
			REL::Relocation<uintptr_t> hook3{ REL::ID(51642) };  // 8B0220 // SetHUDCartMode
			REL::Relocation<uintptr_t> hook4{ REL::ID(51666) };  // 8B1F40 // HUDChargeMeter::Update
			_AddMessage_Flash = trampoline.write_call<5>(hook1.address() + 0x83, AddMessage_Flash);  // 905A93
			_AddMessage_SetHUDCartMode = trampoline.write_call<5>(hook3.address() + 0xA3, AddMessage_SetHUDCartMode);  // 8B02C3
			_Invoke_ChargeMeter1 = trampoline.write_call<5>(hook4.address() + 0x168, Invoke_ChargeMeter1);  // 8B20A8
			_Invoke_ChargeMeter2 = trampoline.write_call<5>(hook4.address() + 0x2B3, Invoke_ChargeMeter2);  // 8B21F3
#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(51907) };  // 8D53D0 // FlashHudMenuMeter
			REL::Relocation<uintptr_t> hook3{ REL::ID(50747) };  // 87FFF0 // SetHUDCartMode
			REL::Relocation<uintptr_t> hook4{ REL::ID(50771) };  // 881CC0 // HUDChargeMeter::Update
			_AddMessage_Flash = trampoline.write_call<5>(hook1.address() + 0x83, AddMessage_Flash);  // 8D5453
			_AddMessage_SetHUDCartMode = trampoline.write_call<5>(hook3.address() + 0xA3, AddMessage_SetHUDCartMode);  // 880093
			_Invoke_ChargeMeter1 = trampoline.write_call<5>(hook4.address() + 0x168, Invoke_ChargeMeter1);  // 881E28
			_Invoke_ChargeMeter2 = trampoline.write_call<5>(hook4.address() + 0x2B3, Invoke_ChargeMeter2);  // 881F43
#endif
			FlashHUDStaminaMountHook();
		}

	private:
		static bool ProcessMessage(uintptr_t a_enemyHealth, RE::UIMessage* a_uiMessage);
		static inline REL::Relocation<decltype(ProcessMessage)> _ProcessMessage;

		static void ProcessButton(RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;

		static void AddMessage_Flash(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data);
		static void AddMessage_SetHUDCartMode(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data);
		static bool Invoke_ChargeMeter1(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj);
		static bool Invoke_ChargeMeter2(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj);

		static bool IsPlayerOrPlayerMount(RE::Actor* a_actor);
		static void FlashHUDStaminaMountHook();

		static inline REL::Relocation<decltype(AddMessage_Flash)> _AddMessage_Flash;
		static inline REL::Relocation<decltype(AddMessage_SetHUDCartMode)> _AddMessage_SetHUDCartMode;
		static inline REL::Relocation<decltype(Invoke_ChargeMeter1)> _Invoke_ChargeMeter1;
		static inline REL::Relocation<decltype(Invoke_ChargeMeter2)> _Invoke_ChargeMeter2;
	};

	class DamageHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> ActorVtbl{ RE::VTABLE_Actor[0] };
			_HandleHealthDamage_Actor = ActorVtbl.write_vfunc(0x104, HandleHealthDamage_Actor);

			REL::Relocation<std::uintptr_t> CharacterVtbl{ RE::VTABLE_Character[0] };
			_HandleHealthDamage_Character = CharacterVtbl.write_vfunc(0x104, HandleHealthDamage_Character);

			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
			_HandleHealthDamage_PlayerCharacter = PlayerCharacterVtbl.write_vfunc(0x104, HandleHealthDamage_PlayerCharacter);
		}

	private:
		static void HandleHealthDamage_Actor(RE::Actor* a_this, RE::Actor* a_attacker, float a_damage);
		static inline REL::Relocation<decltype(HandleHealthDamage_Actor)> _HandleHealthDamage_Actor;

		static void HandleHealthDamage_Character(RE::Character* a_this, RE::Actor* a_attacker, float a_damage);
		static inline REL::Relocation<decltype(HandleHealthDamage_Character)> _HandleHealthDamage_Character;

		static void HandleHealthDamage_PlayerCharacter(RE::PlayerCharacter* a_this, RE::Actor* a_attacker, float a_damage);
		static inline REL::Relocation<decltype(HandleHealthDamage_PlayerCharacter)> _HandleHealthDamage_PlayerCharacter;
	};

	void Install();
}

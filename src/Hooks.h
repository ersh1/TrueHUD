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

			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
			_AddObjectToContainer = PlayerCharacterVtbl.write_vfunc(0x5A, AddObjectToContainer);
			_PickUpObject = PlayerCharacterVtbl.write_vfunc(0xCC, PickUpObject);

			auto& trampoline = SKSE::GetTrampoline();
#
			REL::Relocation<uintptr_t> hook1{ RELOCATION_ID(51907, 52845) };  // 8D53D0, 905A10 // FlashHudMenuMeter
			REL::Relocation<uintptr_t> hook2{ RELOCATION_ID(50747, 51642) };  // 87FFF0, 8B0220 // SetHUDCartMode
			REL::Relocation<uintptr_t> hook3{ RELOCATION_ID(50771, 51666) };  // 881CC0, 8B1F40 // HUDChargeMeter::Update
			REL::Relocation<uintptr_t> hook4{ RELOCATION_ID(55946, 56490) };  // 9A42B0, 9CC5B0 // called by AddItemFunctor
			REL::Relocation<uintptr_t> hook5{ RELOCATION_ID(14692, 14864) };  // 19CA00, 1A7DE0 // called by ActivateFlora
			REL::Relocation<uintptr_t> hook6{ RELOCATION_ID(50476, 51369) };  // 86E2C0, 89C750 // called by ConstructibleObjectMenu
			REL::Relocation<uintptr_t> hook7{ RELOCATION_ID(50449, 51354) };  // 86B980, 899C10 // called by AlchemyMenu
			REL::Relocation<uintptr_t> hook8{ RELOCATION_ID(50450, 51355) };  // 86C640, 89A9C0 // called by EnchantConstructMenu
			REL::Relocation<uintptr_t> hook9{ RELOCATION_ID(15887, 16127) };  // 1E9900, 1F5130 // called by AddItem

			_AddMessage_Flash = trampoline.write_call<5>(hook1.address() + 0x83, AddMessage_Flash);                    // 8D5453, 905A93
			_AddMessage_SetHUDCartMode = trampoline.write_call<5>(hook2.address() + 0xA3, AddMessage_SetHUDCartMode);  // 880093, 8B02C3
			_Invoke_ChargeMeter1 = trampoline.write_call<5>(hook3.address() + 0x168, Invoke_ChargeMeter1);             // 881E28, 8B20A8
			_Invoke_ChargeMeter2 = trampoline.write_call<5>(hook3.address() + 0x2B3, Invoke_ChargeMeter2);             // 881F43, 8B21F3

			_AddItem_AddItemFunctor = trampoline.write_call<5>(hook4.address() + 0x15D, AddItem_AddItemFunctor);                                                       // 9A440D, 9CC70D
			_PlayPickupSoundAndMessage_AddItemFunctor = trampoline.write_call<5>(hook4.address() + 0x1C6, PlayPickupSoundAndMessage_AddItemFunctor);                   // 9A4476, 9CC776
#ifdef SKYRIM_SUPPORT_AE
			_PlayPickupSoundAndMessage_ActivateFlora1 = trampoline.write_call<5>(hook5.address() + 0x260, PlayPickupSoundAndMessage_ActivateFlora1);                   // 1A8040
			_PlayPickupSoundAndMessage_ActivateFlora2 = trampoline.write_call<5>(hook5.address() + 0x3CD, PlayPickupSoundAndMessage_ActivateFlora2);                   // 1A81AD
			_PlayPickupSoundAndMessage_ConstructibleObjectMenu = trampoline.write_call<5>(hook6.address() + 0x68, PlayPickupSoundAndMessage_ConstructibleObjectMenu);  // 89C7B8
			_PlayPickupSoundAndMessage_AlchemyMenu = trampoline.write_call<5>(hook7.address() + 0x229, PlayPickupSoundAndMessage_AlchemyMenu);                         // 899E39
			_PlayPickupSoundAndMessage_EnchantConstructMenu = trampoline.write_call<5>(hook8.address() + 0x2A3, PlayPickupSoundAndMessage_EnchantConstructMenu);       // 89AC63
			_PlayPickupSoundAndMessage_AddItem = trampoline.write_call<5>(hook9.address() + 0x182, PlayPickupSoundAndMessage_AddItem);                                 // 1F52B2
#else
			_PlayPickupSoundAndMessage_ActivateFlora1 = trampoline.write_call<5>(hook5.address() + 0x250, PlayPickupSoundAndMessage_ActivateFlora1);                   // 19CC50
			_PlayPickupSoundAndMessage_ActivateFlora2 = trampoline.write_call<5>(hook5.address() + 0x3C1, PlayPickupSoundAndMessage_ActivateFlora2);                   // 19CDC1
			_PlayPickupSoundAndMessage_ConstructibleObjectMenu = trampoline.write_call<5>(hook6.address() + 0x5D, PlayPickupSoundAndMessage_ConstructibleObjectMenu);  // 86E31D
			_PlayPickupSoundAndMessage_AlchemyMenu = trampoline.write_call<5>(hook7.address() + 0x22E, PlayPickupSoundAndMessage_AlchemyMenu);                         // 86BBAE
			_PlayPickupSoundAndMessage_EnchantConstructMenu = trampoline.write_call<5>(hook8.address() + 0x2A5, PlayPickupSoundAndMessage_EnchantConstructMenu);       // 86C8E5
			_PlayPickupSoundAndMessage_AddItem = trampoline.write_call<5>(hook9.address() + 0x178, PlayPickupSoundAndMessage_AddItem);			                       // 1E9A78
#endif
			FlashHUDStaminaMountHook();
		}

	private:
		static bool ProcessMessage(uintptr_t a_enemyHealth, RE::UIMessage* a_uiMessage);
		static inline REL::Relocation<decltype(ProcessMessage)> _ProcessMessage;

		static void ProcessButton(RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;

		static void AddObjectToContainer(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr);
		static inline REL::Relocation<decltype(AddObjectToContainer)> _AddObjectToContainer;

		static void PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound);
		static inline REL::Relocation<decltype(PickUpObject)> _PickUpObject;

		static void AddMessage_Flash(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data);
		static void AddMessage_SetHUDCartMode(RE::UIMessageQueue* a_this, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::HUDData* a_data);
		static bool Invoke_ChargeMeter1(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj);
		static bool Invoke_ChargeMeter2(RE::GFxValue::ObjectInterface* a_this, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, RE::UPInt a_numArgs, bool a_isDObj);

		static void AddItem_AddItemFunctor(RE::TESObjectREFR* a_this, RE::TESObjectREFR* a_object, int32_t a_count, bool a4, bool a5);
		static void PlayPickupSoundAndMessage_AddItemFunctor(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_ActivateFlora1(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_ActivateFlora2(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_ConstructibleObjectMenu(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_AlchemyMenu(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_EnchantConstructMenu(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);
		static void PlayPickupSoundAndMessage_AddItem(RE::TESBoundObject* a_object, int32_t a_count, bool a3, bool a4, void* a5);

		static bool IsPlayerOrPlayerMount(RE::Actor* a_actor);
		static void FlashHUDStaminaMountHook();

		static inline REL::Relocation<decltype(AddMessage_Flash)> _AddMessage_Flash;
		static inline REL::Relocation<decltype(AddMessage_SetHUDCartMode)> _AddMessage_SetHUDCartMode;
		static inline REL::Relocation<decltype(Invoke_ChargeMeter1)> _Invoke_ChargeMeter1;
		static inline REL::Relocation<decltype(Invoke_ChargeMeter2)> _Invoke_ChargeMeter2;

		static inline REL::Relocation<decltype(AddItem_AddItemFunctor)> _AddItem_AddItemFunctor;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_AddItemFunctor)> _PlayPickupSoundAndMessage_AddItemFunctor;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_ActivateFlora1)> _PlayPickupSoundAndMessage_ActivateFlora1;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_ActivateFlora2)> _PlayPickupSoundAndMessage_ActivateFlora2;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_ConstructibleObjectMenu)> _PlayPickupSoundAndMessage_ConstructibleObjectMenu;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_AlchemyMenu)> _PlayPickupSoundAndMessage_AlchemyMenu;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_EnchantConstructMenu)> _PlayPickupSoundAndMessage_EnchantConstructMenu;
		static inline REL::Relocation<decltype(PlayPickupSoundAndMessage_AddItem)> _PlayPickupSoundAndMessage_AddItem;
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

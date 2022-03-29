#pragma once

// variables
static float* g_deltaTime = (float*)RELOCATION_ID(523660, 410199).address();                         // 2F6B948, 30064C8
static float* g_deltaTimeRealTime = (float*)RELOCATION_ID(523661, 410200).address();                 // 2F6B94C, 30064CC
static float* g_DurationOfApplicationRunTimeMS = (float*)RELOCATION_ID(523662, 410201).address();    // 2F6B950, 30064D0
static float* g_fHUDOpacity = (float*)RELOCATION_ID(510579, 383659).address();                       // 1DF58F8, 1E89D38
static float* g_fShoutMeterEndDuration = (float*)RELOCATION_ID(510025, 382842).address();            // 1DF3ED8, 1E88290
static uintptr_t g_worldToCamMatrix = RELOCATION_ID(519579, 406126).address();                       // 2F4C910, 2FE75F0
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)RELOCATION_ID(519618, 406160).address();  // 2F4DED0, 2FE8B98
static float* g_fNear = (float*)(RELOCATION_ID(517032, 403540).address() + 0x40);                    // 2F26FC0, 2FC1A90
static float* g_fFar = (float*)(RELOCATION_ID(517032, 403540).address() + 0x44);                     // 2F26FC4, 2FC1A94


// functions
typedef uint32_t(_fastcall* tIsSentient)(RE::Actor* a_this);
static REL::Relocation<tIsSentient> IsSentient{ RELOCATION_ID(36889, 37913) };

typedef uint32_t(_fastcall* tGetSoulType)(uint16_t a_actorLevel, uint8_t a_isActorSentient);
static REL::Relocation<tGetSoulType> GetSoulType{ RELOCATION_ID(25933, 26520) };

typedef void(_fastcall* tSetHUDMode)(const char* a_mode, bool a_enable);
static REL::Relocation<tSetHUDMode> SetHUDMode{ RELOCATION_ID(50747, 51642) };

typedef void(_fastcall* tFlashHUDMenuMeter)(RE::ActorValue a_actorValue);
static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ RELOCATION_ID(51907, 52845) };

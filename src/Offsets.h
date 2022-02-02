#pragma once

// variables
#ifdef IS_SKYRIM_AE
static float* g_deltaTime = (float*)REL::ID(410199).address();                          // 30064C8
static float* g_deltaTimeRealTime = (float*)REL::ID(410200).address();                  // 30064CC
static float* g_DurationOfApplicationRunTimeMS = (float*)REL::ID(410201).address();     // 30064D0
static float* g_fHUDOpacity = (float*)REL::ID(383659).address();                        // 1E89D38
static float* g_fShoutMeterEndDuration = (float*)REL::ID(382842).address();             // 1E88290
static uintptr_t g_worldToCamMatrix = REL::ID(406126).address();                        // 2FE75F0
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)REL::ID(406160).address();   // 2FE8B98
static float* g_fNear = (float*)(REL::ID(403540).address() + 0x40);                     // 2FC1A90
static float* g_fFar = (float*)(REL::ID(403540).address() + 0x44);                      // 2FC1A94
#else
static float* g_deltaTime = (float*)REL::ID(523660).address();							// 2F6B948
static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C
static float* g_DurationOfApplicationRunTimeMS = (float*)REL::ID(523662).address();     // 2F6B950
static float* g_fHUDOpacity = (float*)REL::ID(510579).address();						// 1DF58F8
static float* g_fShoutMeterEndDuration = (float*)REL::ID(510025).address();             // 1DF3ED8
static uintptr_t g_worldToCamMatrix = REL::ID(519579).address();						// 2F4C910
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)REL::ID(519618).address();	// 2F4DED0
static float* g_fNear = (float*)(REL::ID(517032).address() + 0x40);						// 2F26FC0
static float* g_fFar = (float*)(REL::ID(517032).address() + 0x44);						// 2F26FC4
#endif

// functions
typedef uint32_t(_fastcall* tIsSentient)(RE::Actor* a_this);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tIsSentient> IsSentient{ REL::ID(37913) };
#else
static REL::Relocation<tIsSentient> IsSentient{ REL::ID(36889) };
#endif


typedef uint32_t(_fastcall* tGetSoulType)(uint16_t a_actorLevel, uint8_t a_isActorSentient);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tGetSoulType> GetSoulType{ REL::ID(26520) };
#else
static REL::Relocation<tGetSoulType> GetSoulType{ REL::ID(25933) };
#endif


typedef void(_fastcall* tSetHUDMode)(const char* a_mode, bool a_enable);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tSetHUDMode> SetHUDMode{ REL::ID(51642) };
#else
static REL::Relocation<tSetHUDMode> SetHUDMode{ REL::ID(50747) };
#endif

typedef void(_fastcall* tFlashHUDMenuMeter)(RE::ActorValue a_actorValue);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ REL::ID(52845) };
#else
static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ REL::ID(51907) };
#endif

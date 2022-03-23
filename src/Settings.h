#pragma once
#include <unordered_set>

enum class BarDirection : std::uint32_t
{
	kNormal = 0,
	kCenter = 1,
	kReverse = 2,
};

enum class InfoBarsDisplayMode : std::uint32_t
{
	kNever = 0,
	kOnHit = 1,
	kOnCombatStart = 2
};

enum class InfoBarsDisplayAttitude : std::uint32_t
{
	kNever = 0,
	kTargetOnly = 1,
	kHostiles = 2,
	kHostilesAndTeammates = 3,
	kEveryone = 4
};

enum class InfoBarsResourcesDisplayMode : std::uint32_t
{
	kNever = 0,
	kWhenChangedFromDefault = 1,
	kAlways = 2,
};

enum class PlayerWidgetDisplayMode : std::uint32_t
{
	kNever = 0,
	kDynamic = 1,
	kInCombat = 2,
	kWhenBarDisplayed = 3,
	kAlways = 4	
};

enum class ReticleStyle : std::uint32_t
{
	kCrosshair = 0,
	kNoCrosshair = 1,
	kDot = 2,
	kGlow = 3
};

enum class WidgetAnchor : std::uint32_t
{
	kBody = 0,
	kHead = 1,
};

enum class InfoBarIndicatorMode : std::uint32_t
{
	kSoulIcon = 0,
	kLevelText = 1,
	kSquareDifficultyIcon = 2,
};

enum class Alignment : std::uint32_t
{
	kCenter = 0,
	kLeft = 1,
	kRight = 2
};

enum class BossBarModifyHUD : std::uint32_t
{
	kNone = 0,
	kMoveSubtitles = 1,
	kHideCompass = 2
};

enum class VerticalDirection : std::uint32_t
{
	kUp = 0,
	kDown = 1,
};

struct Settings
{
	static void Initialize();
	static void ReadSettings();
	static void OnPostLoadGame();

	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);
	static void ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);
	static void ReadColorStringSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);

	// General
	static inline bool bEnableActorInfoBars = true;
	static inline bool bEnableBossBars = true;
	static inline bool bEnablePlayerWidget = true;
	static inline bool bEnableRecentLoot = true;
	static inline bool bEnableFloatingText = false;
	static inline bool bHideVanillaTargetBar = false;

	// Info Bar Widget
	static inline BarDirection uInfoBarDirection = BarDirection::kCenter;
	static inline InfoBarsDisplayMode uInfoBarDisplayHostiles = InfoBarsDisplayMode::kOnHit;
	static inline InfoBarsDisplayMode uInfoBarDisplayTeammates = InfoBarsDisplayMode::kOnCombatStart;
	static inline InfoBarsDisplayMode uInfoBarDisplayOthers = InfoBarsDisplayMode::kOnHit;
	static inline bool bInfoBarDisplayPhantomBars = true;
	static inline bool bInfoBarDisplaySpecialPhantomBar = false;
	static inline InfoBarsDisplayAttitude uInfoBarDisplayName = InfoBarsDisplayAttitude::kTargetOnly;
	static inline InfoBarsDisplayAttitude uInfoBarDisplayDamageCounter = InfoBarsDisplayAttitude::kEveryone;
	static inline InfoBarsDisplayAttitude uInfoBarDisplayIndicator = InfoBarsDisplayAttitude::kTargetOnly;
	static inline Alignment uInfoBarDamageCounterAlignment = Alignment::kRight;
	static inline InfoBarsResourcesDisplayMode uInfoBarResourcesModeTarget = InfoBarsResourcesDisplayMode::kWhenChangedFromDefault;
	static inline InfoBarsResourcesDisplayMode uInfoBarResourcesModeHostiles = InfoBarsResourcesDisplayMode::kNever;
	static inline InfoBarsResourcesDisplayMode uInfoBarResourcesModeTeammates = InfoBarsResourcesDisplayMode::kWhenChangedFromDefault;
	static inline InfoBarsResourcesDisplayMode uInfoBarResourcesModeOthers = InfoBarsResourcesDisplayMode::kNever;
	static inline InfoBarsResourcesDisplayMode uInfoBarSpecialModeTarget = InfoBarsResourcesDisplayMode::kWhenChangedFromDefault;
	static inline InfoBarsResourcesDisplayMode uInfoBarSpecialModeHostiles = InfoBarsResourcesDisplayMode::kNever;
	static inline InfoBarsResourcesDisplayMode uInfoBarSpecialModeTeammates = InfoBarsResourcesDisplayMode::kNever;
	static inline InfoBarsResourcesDisplayMode uInfoBarSpecialModeOthers = InfoBarsResourcesDisplayMode::kNever;
	static inline bool bInfoBarScaleWithDistance = true;
	static inline WidgetAnchor uInfoBarAnchor = WidgetAnchor::kHead;
	static inline float fInfoBarOffsetZ = 30.f;
	static inline bool bInfoBarLevelColorOutline = true;
	static inline uint16_t uInfoBarLevelThreshold = 10;
	static inline InfoBarIndicatorMode uInfoBarIndicatorMode = InfoBarIndicatorMode::kSoulIcon;
	static inline float fInfoBarPhantomDuration = 0.75f;
	static inline float fInfoBarDamageCounterDuration = 2.f;
	static inline float fInfoBarScale = 1.f;
	static inline bool bInfoBarUseHUDOpacity = true;
	static inline float fInfoBarOpacity = 1.f;
	static inline float fInfoBarWidth = 60.f;
	static inline float fInfoBarResourceWidth = 30.f;
	static inline float fInfoBarTargetWidth = 100.f;
	static inline float fInfoBarTargetResourceWidth = 70.f;

	// Boss Info Bar Widget
	static inline BarDirection uBossBarDirection = BarDirection::kCenter;
	static inline bool bBossBarDisplayPhantomBars = true;
	static inline bool bBossBarDisplaySpecialPhantomBar = false;
	static inline bool bBossBarDisplayDamageCounter = true;
	static inline bool bBossBarDisplayIndicator = true;
	static inline Alignment uBossBarDamageCounterAlignment = Alignment::kRight;
	static inline InfoBarsResourcesDisplayMode uBossBarResourcesMode = InfoBarsResourcesDisplayMode::kWhenChangedFromDefault;
	static inline InfoBarsResourcesDisplayMode uBossBarSpecialMode = InfoBarsResourcesDisplayMode::kWhenChangedFromDefault;
	static inline Alignment uBossBarResourceAlignment = Alignment::kLeft;
	static inline Alignment uBossBarNameAlignment = Alignment::kCenter;
	static inline bool bBossBarLevelColorOutline = true;
	static inline uint16_t uBossBarLevelThreshold = 10;
	static inline InfoBarIndicatorMode uBossBarIndicatorMode = InfoBarIndicatorMode::kSoulIcon;
	static inline float fBossBarPhantomDuration = 0.75f;
	static inline float fBossBarDamageCounterDuration = 2.f;
	static inline float fBossBarScale = 1.f;
	static inline bool bBossBarUseHUDOpacity = true;
	static inline float fBossBarOpacity = 1.f;
	static inline float fBossBarWidth = 400.f;
	static inline float fBossBarResourceWidth = 100.f;
	/*static inline float fBossBarAnchorX = 0.5f;
	static inline float fBossBarAnchorY = 0.87f;
	static inline BossBarModifyHUD uBossBarModifyHUD = BossBarModifyHUD::kMoveSubtitles;
	static inline uint8_t uBossBarMaxCount = 3;
	static inline float fMultipleBossBarsOffset = 50.f;
	static inline BossBarStackDirection uMultipleBossBarsStackDirection = BossBarStackDirection::kUp;*/
	static inline float fBossBarAnchorX = 0.5f;
	static inline float fBossBarAnchorY = 0.08f;
	static inline BossBarModifyHUD uBossBarModifyHUD = BossBarModifyHUD::kHideCompass;
	static inline bool bDisplayStandaloneShoutWidgetWhenHidingCompass = true;
	static inline uint32_t uBossBarMaxCount = 3;
	static inline float fMultipleBossBarsOffset = 50.f;
	static inline VerticalDirection uMultipleBossBarsStackDirection = VerticalDirection::kDown;

	// Player Widget
	static inline BarDirection uPlayerWidgetHealthBarDirection = BarDirection::kCenter;
	static inline BarDirection uPlayerWidgetMagickaBarDirection = BarDirection::kNormal;
	static inline BarDirection uPlayerWidgetStaminaBarDirection = BarDirection::kReverse;
	static inline bool bPlayerWidgetDisplayPhantomBars = true;
	static inline bool bPlayerWidgetDisplaySpecialPhantomBar = false;
	static inline PlayerWidgetDisplayMode uPlayerWidgetHealthMode = PlayerWidgetDisplayMode::kDynamic;
	static inline PlayerWidgetDisplayMode uPlayerWidgetMagickaMode = PlayerWidgetDisplayMode::kDynamic;
	static inline PlayerWidgetDisplayMode uPlayerWidgetStaminaMode = PlayerWidgetDisplayMode::kDynamic;
	static inline PlayerWidgetDisplayMode uPlayerWidgetShoutIndicatorMode = PlayerWidgetDisplayMode::kNever;
	static inline PlayerWidgetDisplayMode uPlayerWidgetSpecialMode = PlayerWidgetDisplayMode::kDynamic;
	static inline bool bPlayerWidgetHideVanillaBars = true;
	static inline bool bPlayerWidgetForceHideVanillaBars = false;
	static inline bool bPlayerWidgetDisplayEnchantmentChargeMeter = true;
	static inline bool bPlayerWidgetDisplayMountStamina = true;
	static inline float fPlayerWidgetPhantomDuration = 0.75f;
	static inline float fPlayerWidgetScale = 1.f;
	static inline bool bPlayerWidgetUseHUDOpacity = true;
	static inline float fPlayerWidgetOpacity = 1.f;
	static inline float fPlayerWidgetBarWidth = 245.f;
	static inline float fPlayerWidgetSpecialBarWidth = 70.f;
	static inline bool bPlayerWidgetScaleBars = false;
	static inline float fPlayerWidgetMinBarWidth = 50.f;
	static inline float fPlayerWidgetMaxBarWidth = 400.f;
	static inline float fPlayerWidgetHealthBarScaleMult = 1.f;
	static inline float fPlayerWidgetMagickaBarScaleMult = 1.f;
	static inline float fPlayerWidgetStaminaBarScaleMult = 1.f;
	static inline float fPlayerWidgetX = 0.045f;
	static inline float fPlayerWidgetY = 0.920f;
	static inline bool bPlayerWidgetCombined = false;	
	static inline float fPlayerWidgetHealthX = 0.500f;
	static inline float fPlayerWidgetHealthY = 0.940f;
	static inline float fPlayerWidgetMagickaX = 0.066f;
	static inline float fPlayerWidgetMagickaY = 0.940f;
	static inline float fPlayerWidgetStaminaX = 0.934f;
	static inline float fPlayerWidgetStaminaY = 0.940f;
	static inline float fPlayerWidgetEnchantmentChargeMeterX = 0.500f;
	static inline float fPlayerWidgetEnchantmentChargeMeterY = 0.970f;

	// Recent Loot
	static inline bool bRecentLootHideVanillaMessage = true;
	static inline bool bRecentLootHideInInventoryMenus = false;
	static inline bool bRecentLootHideInCraftingMenus = false;
	static inline uint32_t uRecentLootMaxMessageCount = 10;
	static inline float fRecentLootMessageDuration = 5.f;
	static inline float fRecentLootScale = 1.f;
	static inline bool bRecentLootUseHUDOpacity = true;
	static inline float fRecentLootOpacity = 1.f;
	static inline VerticalDirection uRecentLootListDirection = VerticalDirection::kUp;	
	static inline float fRecentLootX = 1.f;
	static inline float fRecentLootY = 0.85f;

	// Floating text
	static inline bool bEnableFloatingCombatText = true;
	static inline bool bFloatingTextScaleWithDistance = true;
	static inline float fFloatingTextScale = 1.f;

	// Colors
	static inline std::string sHealthColor;
	static inline std::string sHealthPhantomColor;
	static inline std::string sHealthBackgroundColor;
	static inline std::string sHealthPenaltyColor;
	static inline std::string sHealthFlashColor;

	static inline std::string sMagickaColor;
	static inline std::string sMagickaPhantomColor;
	static inline std::string sMagickaBackgroundColor;
	static inline std::string sMagickaPenaltyColor;
	static inline std::string sMagickaFlashColor;

	static inline std::string sStaminaColor;
	static inline std::string sStaminaPhantomColor;
	static inline std::string sStaminaBackgroundColor;
	static inline std::string sStaminaPenaltyColor;
	static inline std::string sStaminaFlashColor;

	static inline std::string sSpecialColor;
	static inline std::string sSpecialPhantomColor;
	static inline std::string sSpecialBackgroundColor;
	static inline std::string sSpecialPenaltyColor;
	static inline std::string sSpecialFlashColor;

	static inline std::string sDefaultColor;
	static inline std::string sWeakerColor;
	static inline std::string sWeakerColorOutline;
	static inline std::string sStrongerColor;
	static inline std::string sTeammateColor;

	// Colors
	static inline uint32_t uHealthColor = 0xDF2020;
	static inline uint32_t uHealthPhantomColor = 0xCBCBCB;
	static inline uint32_t uHealthBackgroundColor = 0x4D0F0F;
	static inline uint32_t uHealthPenaltyColor = 0x5F0000;
	static inline uint32_t uHealthFlashColor = 0xDF2020;

	static inline uint32_t uMagickaColor = 0x284BD7;
	static inline uint32_t uMagickaPhantomColor = 0xCBCBCB;
	static inline uint32_t uMagickaBackgroundColor = 0x0E1A4D;
	static inline uint32_t uMagickaPenaltyColor = 0x5F0000;
	static inline uint32_t uMagickaFlashColor = 0x284BD7;

	static inline uint32_t uStaminaColor = 0x007E00;
	static inline uint32_t uStaminaPhantomColor = 0xCBCBCB;
	static inline uint32_t uStaminaBackgroundColor = 0x004D00;
	static inline uint32_t uStaminaPenaltyColor = 0x5F0000;
	static inline uint32_t uStaminaFlashColor = 0x007E00;
	
	static inline uint32_t uSpecialColor = 0xFFA600;
	static inline uint32_t uSpecialPhantomColor = 0xCBCBCB;
	static inline uint32_t uSpecialBackgroundColor = 0x805300;
	static inline uint32_t uSpecialPenaltyColor = 0x5F0000;
	static inline uint32_t uSpecialFlashColor = 0xFFA600;

	static inline uint32_t uDefaultColor = 0xCBCBCB;
	static inline uint32_t uDefaultColorOutline = 0xCBCBCB;
	static inline uint32_t uWeakerColor = 0x1E88E5;
	static inline uint32_t uWeakerColorOutline = 0x1E88E5;
	static inline uint32_t uStrongerColor = 0xCC2929;
	static inline uint32_t uStrongerColorOutline = 0xCC2929;
	static inline uint32_t uTeammateColor = 0x66CC33;
	static inline uint32_t uTeammateColorOutline = 0x66CC33;

	// Non-MCM
	static inline std::unordered_set<RE::TESRace*> bossRaces;
	static inline std::unordered_set<RE::BGSLocationRefType*> bossLocRefTypes;
	static inline std::unordered_set<RE::TESActorBase*> bossNPCs;
	static inline std::unordered_set<RE::TESActorBase*> bossNPCBlacklist;

	static inline RE::BGSKeyword* kywd_noSoulTrap = nullptr;
	static inline RE::BGSKeyword* kywd_Dragon = nullptr;
	static inline RE::TESGlobal* glob_survivalModeEnabled = nullptr;
	static inline RE::TESGlobal* glob_survivalHealthPenaltyPercent = nullptr;
	static inline RE::TESGlobal* glob_survivalMagickaPenaltyPercent = nullptr;
	static inline RE::TESGlobal* glob_survivalStaminaPenaltyPercent = nullptr;
	static inline RE::TESGlobal* glob_trueHUDVersion = nullptr;
	static inline RE::TESGlobal* glob_trueHUDSpecialResourceBars = nullptr;
};

#include "Widgets/RecentLoot.h"
#include "Settings.h"
#include "Offsets.h"
#include "Utils.h"

namespace Scaleform
{
	void RecentLoot::Update([[maybe_unused]] float a_deltaTime)
	{
		_object.Invoke("update");
	}

	void RecentLoot::Initialize()
	{
		LoadConfig();
	}

	void RecentLoot::Dispose()
	{
		_object.Invoke("cleanUp");
	}

	void RecentLoot::AddMessage(RE::TESBoundObject* a_object, std::string_view a_name, uint32_t a_count)
	{
		RE::GFxValue args[4];

		std::string iconLabel = "default_misc";
		uint32_t iconColor = 0xFFFFFF;

		RE::FormType formType = a_object->GetFormType();
		
		switch (formType)
		{
		case RE::FormType::Scroll:
			iconLabel = "default_scroll"sv;
			break;
		case RE::FormType::Armor:
			ProcessArmorIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::Book:
			ProcessBookIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::Ingredient:
			iconLabel = "default_ingredient"sv;
			break;
		case RE::FormType::Light:
			iconLabel = "misc_torch"sv;
			break;
		case RE::FormType::Misc:
			ProcessMiscIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::Weapon:
			ProcessWeaponIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::Ammo:
			ProcessAmmoIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::KeyMaster:
			iconLabel = "default_key";
			break;
		case RE::FormType::AlchemyItem:
			ProcessPotionIcon(a_object, iconLabel, iconColor);
			break;
		case RE::FormType::SoulGem:
			ProcessSoulGemIcon(a_object, iconLabel, iconColor);
			break;
		}

		args[0].SetString(a_name);
		args[1].SetNumber(a_count);
		args[2].SetString(iconLabel);
		args[3].SetNumber(iconColor);
		_object.Invoke("addMessage", nullptr, args, 4);
	}

	void RecentLoot::UpdatePosition()
	{
		RE::GFxValue::DisplayInfo displayInfo;
		float scale = Settings::fRecentLootScale * 100.f;
		displayInfo.SetScale(scale, scale);

		RE::GRectF rect = _view->GetVisibleFrameRect();
		RE::NiPoint2 screenPos;

		auto def = _view->GetMovieDef();
		if (def) {
			screenPos.x = def->GetWidth();
			screenPos.y = def->GetHeight();
		}

		screenPos.x *= Settings::fRecentLootX;
		screenPos.y *= Settings::fRecentLootY;

		displayInfo.SetPosition(screenPos.x, screenPos.y);

		_object.SetDisplayInfo(displayInfo);
	}

	void RecentLoot::LoadConfig()
	{
		RE::NiPoint2 stageSize;
		auto def = _view->GetMovieDef();
		if (def) {
			stageSize.x = def->GetWidth();
			stageSize.y = def->GetHeight();
		}

		RE::GFxValue args[4];
		args[0].SetNumber(Settings::uRecentLootMaxMessageCount);
		args[1].SetNumber(Settings::fRecentLootMessageDuration);
		args[2].SetNumber((Settings::bRecentLootUseHUDOpacity ? *g_fHUDOpacity : Settings::fRecentLootOpacity) * 100.f);
		args[3].SetNumber(static_cast<uint32_t>(Settings::uRecentLootListDirection));
		_object.Invoke("loadConfig", nullptr, args, 4);

		UpdatePosition();
	}

	void RecentLoot::ProcessArmorIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		using Part = RE::BIPED_MODEL::BipedObjectSlot;
		static constexpr std::array<Part, 32> partMaskPrecedence{
			Part::kBody,
			Part::kHair,
			Part::kHands,
			Part::kForearms,
			Part::kFeet,
			Part::kCalves,
			Part::kShield,
			Part::kAmulet,
			Part::kRing,
			Part::kLongHair,
			Part::kEars,
			Part::kHead,
			Part::kCirclet,
			Part::kTail,
			Part::kModMouth,
			Part::kModNeck,
			Part::kModChestPrimary,
			Part::kModBack,
			Part::kModMisc1,
			Part::kModPelvisPrimary,
			Part::kDecapitateHead,
			Part::kDecapitate,
			Part::kModPelvisSecondary,
			Part::kModLegRight,
			Part::kModLegLeft,
			Part::kModFaceJewelry,
			Part::kModChestSecondary,
			Part::kModShoulder,
			Part::kModArmLeft,
			Part::kModArmRight,
			Part::kModMisc2,
			Part::kFX01
		};

		enum class SKYUI_EQUIPSLOT : uint32_t
		{
			kHead = 0,
			kHair = 1,
			kLongHair = 2,
			kBody = 3,
			kForearms = 4,
			kHands = 5,
			kShield = 6,
			kCalves = 7,
			kFeet = 8,
			kCirclet = 9,
			kAmulet = 10,
			kEars = 11,
			kRing = 12,
			kTail = 13
		};

		enum class SKYUI_ARMORWEIGHT : uint32_t
		{
			kLight = 0,
			kHeavy = 1,
			kNone = 2,
			kClothing = 3,
			kJewelry = 4
		};

		auto armor = a_object->As<RE::TESObjectARMO>();
		if (armor) {
			RE::FormID formID = a_object->GetFormID();
			auto baseID = formID & 0x00FFFFFF;

			RE::BIPED_MODEL::ArmorType weightClass = armor->bipedModelData.armorType.get();
			SKYUI_ARMORWEIGHT skyUIWeight = SKYUI_ARMORWEIGHT::kNone;

			switch (weightClass) {
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				skyUIWeight = SKYUI_ARMORWEIGHT::kLight;
				break;
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				skyUIWeight = SKYUI_ARMORWEIGHT::kHeavy;
				break;
			default:
				if (armor->HasKeywordString("VendorItemClothing"sv)) {
					skyUIWeight = SKYUI_ARMORWEIGHT::kClothing;
				} else if (armor->HasKeywordString("VendorItemJewelry"sv)) {
					skyUIWeight = SKYUI_ARMORWEIGHT::kJewelry;
				}
				break;
			}

			auto partMask = armor->bipedModelData.bipedObjectSlots;
			Part mainPartMask = Part::kNone;
			for (int i = 0; i < partMaskPrecedence.size(); i++) {
				if (partMask & partMaskPrecedence[i]) {
					mainPartMask = partMaskPrecedence[i];
					break;
				}
			}

			SKYUI_EQUIPSLOT subType;
			switch (mainPartMask) {
			case Part::kHead:
				subType = SKYUI_EQUIPSLOT::kHead;
				break;
			case Part::kHair:
				subType = SKYUI_EQUIPSLOT::kHair;
				break;
			case Part::kLongHair:
				subType = SKYUI_EQUIPSLOT::kLongHair;
				break;
			case Part::kBody:
				subType = SKYUI_EQUIPSLOT::kBody;
				break;
			case Part::kHands:
				subType = SKYUI_EQUIPSLOT::kHands;
				break;
			case Part::kForearms:
				subType = SKYUI_EQUIPSLOT::kForearms;
				break;
			case Part::kAmulet:
				subType = SKYUI_EQUIPSLOT::kAmulet;
				break;
			case Part::kRing:
				subType = SKYUI_EQUIPSLOT::kRing;
				break;
			case Part::kFeet:
				subType = SKYUI_EQUIPSLOT::kFeet;
				break;
			case Part::kCalves:
				subType = SKYUI_EQUIPSLOT::kCalves;
				break;
			case Part::kShield:
				subType = SKYUI_EQUIPSLOT::kShield;
				break;
			case Part::kCirclet:
				subType = SKYUI_EQUIPSLOT::kCirclet;
				break;
			case Part::kEars:
				subType = SKYUI_EQUIPSLOT::kEars;
				break;
			case Part::kTail:
				subType = SKYUI_EQUIPSLOT::kTail;
				break;
			default:
				subType = (SKYUI_EQUIPSLOT)0;
			}

			if (skyUIWeight == SKYUI_ARMORWEIGHT::kNone) {
				switch (mainPartMask) {
				case Part::kHead:
				case Part::kHair:
				case Part::kLongHair:
				case Part::kBody:
				case Part::kHands:
				case Part::kForearms:
				case Part::kFeet:
				case Part::kCalves:
				case Part::kShield:
				case Part::kTail:
					skyUIWeight = SKYUI_ARMORWEIGHT::kClothing;
					break;
				case Part::kAmulet:
				case Part::kRing:
				case Part::kCirclet:
				case Part::kEars:
					skyUIWeight = SKYUI_ARMORWEIGHT::kJewelry;
					break;
				}
			}

			switch (baseID) {
			case 0x08895A:  // BASEID_CLOTHESWEDDINGWREATH
				skyUIWeight = SKYUI_ARMORWEIGHT::kJewelry;
				break;
			case 0x011A84:  // BASEID_DLC1CLOTHESVAMPIRELORDARMOR
				subType = SKYUI_EQUIPSLOT::kBody;
				break;
			}

			// Select correct label
			a_outIconLabel = "default_armor"sv;
			a_outIconColor = 0xEDDA87;

			switch (skyUIWeight) {
			case SKYUI_ARMORWEIGHT::kLight:
				a_outIconColor = 0x756000;
				switch (subType) {
				case SKYUI_EQUIPSLOT::kHead:
				case SKYUI_EQUIPSLOT::kHair:
				case SKYUI_EQUIPSLOT::kLongHair:
					a_outIconLabel = "lightarmor_head"sv;
					break;
				case SKYUI_EQUIPSLOT::kBody:
				case SKYUI_EQUIPSLOT::kTail:
					a_outIconLabel = "lightarmor_body"sv;
					break;
				case SKYUI_EQUIPSLOT::kHands:
					a_outIconLabel = "lightarmor_hands"sv;
					break;
				case SKYUI_EQUIPSLOT::kForearms:
					a_outIconLabel = "lightarmor_forearms"sv;
					break;
				case SKYUI_EQUIPSLOT::kFeet:
					a_outIconLabel = "lightarmor_feet"sv;
					break;
				case SKYUI_EQUIPSLOT::kCalves:
					a_outIconLabel = "lightarmor_calves"sv;
					break;
				case SKYUI_EQUIPSLOT::kShield:
					a_outIconLabel = "lightarmor_shield"sv;
					break;
				case SKYUI_EQUIPSLOT::kAmulet:
					a_outIconLabel = "armor_amulet"sv;
					break;
				case SKYUI_EQUIPSLOT::kRing:
					a_outIconLabel = "armor_ring"sv;
					break;
				case SKYUI_EQUIPSLOT::kCirclet:
					a_outIconLabel = "armor_circlet"sv;
					break;
				}
				break;
			case SKYUI_ARMORWEIGHT::kHeavy:
				a_outIconColor = 0x6B7585;
				switch (subType) {
				case SKYUI_EQUIPSLOT::kHead:
				case SKYUI_EQUIPSLOT::kHair:
				case SKYUI_EQUIPSLOT::kLongHair:
					a_outIconLabel = "armor_head"sv;
					break;
				case SKYUI_EQUIPSLOT::kBody:
				case SKYUI_EQUIPSLOT::kTail:
					a_outIconLabel = "armor_body"sv;
					break;
				case SKYUI_EQUIPSLOT::kHands:
					a_outIconLabel = "armor_hands"sv;
					break;
				case SKYUI_EQUIPSLOT::kForearms:
					a_outIconLabel = "armor_forearms"sv;
					break;
				case SKYUI_EQUIPSLOT::kFeet:
					a_outIconLabel = "armor_feet"sv;
					break;
				case SKYUI_EQUIPSLOT::kCalves:
					a_outIconLabel = "armor_calves"sv;
					break;
				case SKYUI_EQUIPSLOT::kShield:
					a_outIconLabel = "armor_shield"sv;
					break;
				case SKYUI_EQUIPSLOT::kAmulet:
					a_outIconLabel = "armor_amulet"sv;
					break;
				case SKYUI_EQUIPSLOT::kRing:
					a_outIconLabel = "armor_ring"sv;
					break;
				case SKYUI_EQUIPSLOT::kCirclet:
					a_outIconLabel = "armor_circlet"sv;
					break;
				}
				break;
			case SKYUI_ARMORWEIGHT::kJewelry:
				switch (subType) {
				case SKYUI_EQUIPSLOT::kAmulet:
					a_outIconLabel = "armor_amulet"sv;
					break;
				case SKYUI_EQUIPSLOT::kRing:
					a_outIconLabel = "armor_ring"sv;
					break;
				case SKYUI_EQUIPSLOT::kCirclet:
					a_outIconLabel = "armor_circlet"sv;
					break;
				}
				break;
			case SKYUI_ARMORWEIGHT::kClothing:
			default:
				switch (subType) {
				case SKYUI_EQUIPSLOT::kHead:
				case SKYUI_EQUIPSLOT::kHair:
				case SKYUI_EQUIPSLOT::kLongHair:
					a_outIconLabel = "clothing_head"sv;
					break;
				case SKYUI_EQUIPSLOT::kBody:
				case SKYUI_EQUIPSLOT::kTail:
					a_outIconLabel = "clothing_body"sv;
					break;
				case SKYUI_EQUIPSLOT::kHands:
					a_outIconLabel = "clothing_hands"sv;
					break;
				case SKYUI_EQUIPSLOT::kForearms:
					a_outIconLabel = "clothing_forearms"sv;
					break;
				case SKYUI_EQUIPSLOT::kFeet:
					a_outIconLabel = "clothing_feet"sv;
					break;
				case SKYUI_EQUIPSLOT::kCalves:
					a_outIconLabel = "clothing_calves"sv;
					break;
				case SKYUI_EQUIPSLOT::kShield:
					a_outIconLabel = "clothing_shield"sv;
					break;
				}
				break;
			}
		}
	}

	void RecentLoot::ProcessBookIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, [[maybe_unused]] uint32_t& a_outIconColor)
	{
		enum class SKYUI_BOOKTYPE : uint32_t
		{
			kNone,
			kSpellTome,
			kNote,
			kRecipe,
		};

		auto book = a_object->As<RE::TESObjectBOOK>();
		if (book) {
			SKYUI_BOOKTYPE subType = SKYUI_BOOKTYPE::kNone;
			if (book->data.type == RE::OBJ_BOOK::Type::kNoteScroll) {
				subType = SKYUI_BOOKTYPE::kNote;
			} else if (book->HasKeywordString("VendorItemRecipe"sv)) {
				subType = SKYUI_BOOKTYPE::kRecipe;
			} else if (book->HasKeywordString("VendorItemSpellTome"sv)) {
				subType = SKYUI_BOOKTYPE::kSpellTome;
			}

			// Select correct label
			a_outIconLabel = "default_book"sv;

			switch (subType) {
			case SKYUI_BOOKTYPE::kRecipe:
			case SKYUI_BOOKTYPE::kNote:
				a_outIconLabel = "book_note"sv;
				break;
			case SKYUI_BOOKTYPE::kSpellTome:
				a_outIconLabel = "book_tome"sv;
				break;
			}
		}
	}

	void RecentLoot::ProcessMiscIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		enum class SKYUI_MISCTYPE : uint32_t
		{
			kNone,
			kGem,
			kDragonClaw,
			kArtifact,
			kLeather,
			kLeatherStrips,
			kHide,
			kRemains,
			kIngot,
			kChildrensClothes,
			kFirewood,
			kClutter,
			kLockpick,
			kGold
		};

		auto misc = a_object->As<RE::TESObjectMISC>();
		if (misc) {
			RE::FormID formID = a_object->GetFormID();
			auto baseID = formID & 0x00FFFFFF;

			SKYUI_MISCTYPE subType = SKYUI_MISCTYPE::kNone;
			if (misc->HasKeywordString("BYOHAdoptionClothesKeyword"sv)) {
				subType = SKYUI_MISCTYPE::kChildrensClothes;
			} else if (misc->HasKeywordString("VendorItemDaedricArtifact"sv)) {
				subType = SKYUI_MISCTYPE::kArtifact;
			} else if (misc->HasKeywordString("VendorItemGem"sv)) {
				subType = SKYUI_MISCTYPE::kGem;
			} else if (misc->HasKeywordString("VendorItemAnimalHide"sv)) {
				subType = SKYUI_MISCTYPE::kHide;
			} else if (misc->HasKeywordString("VendorItemAnimalPart"sv)) {
				subType = SKYUI_MISCTYPE::kRemains;
			} else if (misc->HasKeywordString("VendorItemOreIngot"sv)) {
				subType = SKYUI_MISCTYPE::kIngot;
			} else if (misc->HasKeywordString("VendorItemClutter"sv)) {
				subType = SKYUI_MISCTYPE::kClutter;
			} else if (misc->HasKeywordString("VendorItemFirewood"sv)) {
				subType = SKYUI_MISCTYPE::kFirewood;
			}

			switch (baseID) {
			case 0x06851E: // BASEID_GEMAMETHYSTFLAWLESS
				subType = SKYUI_MISCTYPE::kGem;
				break;
			case 0x04B56C: // BASEID_RUBYDRAGONCLAW
			case 0x0AB7BB: // BASEID_IVORYDRAGONCLAW
			case 0x07C260: // BASEID_GLASSCLAW
			case 0x05AF48: // BASEID_EBONYCLAW
			case 0x0ED417: // BASEID_EMERALDDRAGONCLAW
			case 0x0AB375: // BASEID_DIAMONDCLAW
			case 0x08CDFA: // BASEID_IRONCLAW
			case 0x0B634C: // BASEID_CORALDRAGONCLAW
			case 0x0999E7: // BASEID_E3GOLDENCLAW
			case 0x0663D7: // BASEID_SAPPHIREDRAGONCLAW
			case 0x039647: // BASEID_MS13GOLDENCLAW
				subType = SKYUI_MISCTYPE::kDragonClaw;
				break;
			case 0x00000A: // BASEID_LOCKPICK
				subType = SKYUI_MISCTYPE::kLockpick;
				break;
			case 0x00000F: // BASEID_GOLD001
				subType = SKYUI_MISCTYPE::kGold;
				break;
			case 0x0DB5D2: // BASEID_LEATHER01
				subType = SKYUI_MISCTYPE::kLeather;
				break;
			case 0x0800E4: // BASEID_LEATHERSTRIPS
				subType = SKYUI_MISCTYPE::kLeatherStrips;
				break;
			}

			// Select correct label
			a_outIconLabel = "default_misc"sv;

			switch (subType) {
			case SKYUI_MISCTYPE::kGem:
				a_outIconLabel = "misc_gem"sv;
				a_outIconColor = 0xFFB0D1;
				break;
			case SKYUI_MISCTYPE::kDragonClaw:
				a_outIconLabel = "misc_dragonclaw"sv;
				break;
			case SKYUI_MISCTYPE::kArtifact:
				a_outIconLabel = "misc_artifact"sv;
				break;
			case SKYUI_MISCTYPE::kLeather:
				a_outIconLabel = "misc_leather"sv;
				a_outIconColor = 0xBA8D23;
				break;
			case SKYUI_MISCTYPE::kLeatherStrips:
				a_outIconLabel = "misc_strips"sv;
				a_outIconColor = 0xBA8D23;
				break;
			case SKYUI_MISCTYPE::kHide:
				a_outIconLabel = "misc_hide"sv;
				a_outIconColor = 0xDBB36E;
				break;
			case SKYUI_MISCTYPE::kRemains:
				a_outIconLabel = "misc_remains"sv;
				break;
			case SKYUI_MISCTYPE::kIngot:
				a_outIconLabel = "misc_ingot"sv;
				a_outIconColor = 0x828282;
				break;
			case SKYUI_MISCTYPE::kChildrensClothes:
				a_outIconLabel = "clothing_body"sv;
				a_outIconColor = 0xEDDA87;
				break;
			case SKYUI_MISCTYPE::kFirewood:
				a_outIconLabel = "misc_wood"sv;
				a_outIconColor = 0xA89E8C;
				break;
			case SKYUI_MISCTYPE::kClutter:
				a_outIconLabel = "misc_clutter"sv;
				break;
			case SKYUI_MISCTYPE::kLockpick:
				a_outIconLabel = "misc_lockpick"sv;
				break;
			case SKYUI_MISCTYPE::kGold:
				a_outIconLabel = "misc_gold"sv;
				a_outIconColor = 0xCCCC33;
				break;
			default:
				break;

			}
		}
	}

	void RecentLoot::ProcessWeaponIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		enum class SKYUI_WEAPONTYPE : uint32_t
		{
			kMelee = 0,
			kSword = 1,
			kDagger = 2,
			kWaraxe = 3,
			kMace = 4,
			kGreatsword = 5,
			kBattleaxe = 6,
			kWarhammer = 7,
			kBow = 8,
			kCrossbow = 9,
			kStaff = 10,
			kPickaxe = 11,
			kWoodaxe = 12
		};

		auto weap = a_object->As<RE::TESObjectWEAP>();
		if (weap) {
			RE::FormID formID = a_object->GetFormID();
			auto baseID = formID & 0x00FFFFFF;

			auto weaponType = weap->GetWeaponType();
			SKYUI_WEAPONTYPE skyUIWeaponType = SKYUI_WEAPONTYPE::kMelee;
			switch (weaponType) {
			case RE::WEAPON_TYPE::kHandToHandMelee:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kMelee;
				break;
			case RE::WEAPON_TYPE::kOneHandSword:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kSword;
				break;
			case RE::WEAPON_TYPE::kOneHandDagger:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kDagger;
				break;
			case RE::WEAPON_TYPE::kOneHandAxe:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kWaraxe;
				break;
			case RE::WEAPON_TYPE::kOneHandMace:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kMace;
				break;
			case RE::WEAPON_TYPE::kTwoHandSword:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kGreatsword;
				break;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeywordString("WeapTypeWarhammer"sv)) {
					skyUIWeaponType = SKYUI_WEAPONTYPE::kWarhammer;
				}
				else {
					skyUIWeaponType = SKYUI_WEAPONTYPE::kBattleaxe;
				}
				break;
			case RE::WEAPON_TYPE::kBow:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kBow;
				break;
			case RE::WEAPON_TYPE::kStaff:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kStaff;
				break;
			case RE::WEAPON_TYPE::kCrossbow:
				skyUIWeaponType = SKYUI_WEAPONTYPE::kCrossbow;
				break;
			}

			switch (baseID) {
			case 0x0E3C16: // BASEID_WEAPPICKAXE
			case 0x06A707: // BASEID_SSDROCKSPLINTERPICKAXE
			case 0x1019D4: // BASEID_DUNVOLUNRUUDPICKAXE
				skyUIWeaponType = SKYUI_WEAPONTYPE::kPickaxe;
				break;
			case 0x02F2F4:  // BASEID_AXE01
			case 0x0AE086:  // BASEID_DUNHALTEDSTREAMPOACHERSAXE
				skyUIWeaponType = SKYUI_WEAPONTYPE::kWoodaxe;
				break;
			}

			// Select correct label
			a_outIconLabel = "default_weapon"sv;
			a_outIconColor = 0xA4A5BF;

			switch (skyUIWeaponType) {
			case SKYUI_WEAPONTYPE::kSword:
				a_outIconLabel = "weapon_sword"sv;
				break;
			case SKYUI_WEAPONTYPE::kDagger:
				a_outIconLabel = "weapon_dagger"sv;
				break;
			case SKYUI_WEAPONTYPE::kWaraxe:
				a_outIconLabel = "weapon_waraxe"sv;
				break;
			case SKYUI_WEAPONTYPE::kMace:
				a_outIconLabel = "weapon_mace"sv;
				break;
			case SKYUI_WEAPONTYPE::kGreatsword:
				a_outIconLabel = "weapon_greatsword"sv;
				break;
			case SKYUI_WEAPONTYPE::kBattleaxe:
				a_outIconLabel = "weapon_battleaxe"sv;
				break;
			case SKYUI_WEAPONTYPE::kWarhammer:
				a_outIconLabel = "weapon_hammer"sv;
				break;
			case SKYUI_WEAPONTYPE::kBow:
				a_outIconLabel = "weapon_bow"sv;
				break;
			case SKYUI_WEAPONTYPE::kCrossbow:
				a_outIconLabel = "weapon_crossbow"sv;
				break;
			case SKYUI_WEAPONTYPE::kStaff:
				a_outIconLabel = "weapon_staff"sv;
				break;
			case SKYUI_WEAPONTYPE::kPickaxe:
				a_outIconLabel = "weapon_pickaxe"sv;
				break;
			case SKYUI_WEAPONTYPE::kWoodaxe:
				a_outIconLabel = "weapon_woodaxe"sv;
				break;
			default:
				break;
			}
		}
	}

	void RecentLoot::ProcessAmmoIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		enum class SKYUI_AMMOTYPE : uint32_t
		{
			kArrow,
			kBolt
		};

		auto ammo = a_object->As<RE::TESAmmo>();
		if (ammo) {
			SKYUI_AMMOTYPE subType;

			auto flags = ammo->data.flags;
			if (flags.all(RE::AMMO_DATA::Flag::kNonBolt)) {
				subType = SKYUI_AMMOTYPE::kArrow;
			} else {
				subType = SKYUI_AMMOTYPE::kBolt;
			}

			// Select correct label
			a_outIconLabel = "weapon_arrow"sv;
			a_outIconColor = 0xA89E8C;

			switch (subType) {
			case SKYUI_AMMOTYPE::kArrow:
				a_outIconLabel = "weapon_arrow"sv;
				break;
			case SKYUI_AMMOTYPE::kBolt:
				a_outIconLabel = "weapon_bolt"sv;
				break;
			}
		}
	}

	void RecentLoot::ProcessPotionIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		enum class SKYUI_POTIONTYPE : uint32_t
		{
			kHealth,
			kHealRate,
			kHealRateMult,
			kMagicka,
			kMagickaRate,
			kMagickaRateMult,
			kStamina,
			kStaminaRate,
			kStaminaRateMult,
			kResistFire,
			kResistShock,
			kResistFrost,
			kPotion,
			kDrink,
			kFood,
			kPoison
		};

		auto alch = a_object->As<RE::AlchemyItem>();
		if (alch) {
			SKYUI_POTIONTYPE subType = SKYUI_POTIONTYPE::kPotion;

			auto flags = alch->data.flags;
			if (flags.all(RE::AlchemyItem::AlchemyFlag::kFoodItem)) {
				subType = SKYUI_POTIONTYPE::kFood;

				if (alch->data.consumptionSound && alch->data.consumptionSound->GetFormID() == 0x000B6435) { // FORMID_ITMPotionUse
					subType = SKYUI_POTIONTYPE::kDrink;
				}
			} else if (flags.all(RE::AlchemyItem::AlchemyFlag::kPoison)) {
				subType = SKYUI_POTIONTYPE::kPoison;
			} else {
				RE::ActorValue primaryAV = RE::ActorValue::kNone;
				if (alch->avEffectSetting) {
					primaryAV = alch->avEffectSetting->data.primaryAV;
				} else if (RE::Effect* effect = alch->effects[0]) {
					primaryAV = effect->baseEffect->data.primaryAV;
				}				
				switch (primaryAV) {
				case RE::ActorValue::kHealth:
					subType = SKYUI_POTIONTYPE::kHealth;
					break;
				case RE::ActorValue::kMagicka:
					subType = SKYUI_POTIONTYPE::kMagicka;
					break;
				case RE::ActorValue::kStamina:
					subType = SKYUI_POTIONTYPE::kStamina;
					break;
				case RE::ActorValue::kHealRate:
					subType = SKYUI_POTIONTYPE::kHealRate;
					break;
				case RE::ActorValue::kMagickaRate:
					subType = SKYUI_POTIONTYPE::kMagickaRate;
					break;
				case RE::ActorValue::kStaminaRate:
					subType = SKYUI_POTIONTYPE::kStaminaRate;
					break;
				case RE::ActorValue::kHealRateMult:
					subType = SKYUI_POTIONTYPE::kHealRateMult;
					break;
				case RE::ActorValue::kMagickaRateMult:
					subType = SKYUI_POTIONTYPE::kMagickaRateMult;
					break;
				case RE::ActorValue::kStaminaRateMult:
					subType = SKYUI_POTIONTYPE::kStaminaRateMult;
					break;
				case RE::ActorValue::kResistFire:
					subType = SKYUI_POTIONTYPE::kResistFire;
					break;
				case RE::ActorValue::kResistShock:
					subType = SKYUI_POTIONTYPE::kResistShock;
					break;
				case RE::ActorValue::kResistFrost:
					subType = SKYUI_POTIONTYPE::kResistFrost;
					break;
				}
			}

			// Select correct label
			a_outIconLabel = "default_potion"sv;

			switch (subType) {
			case SKYUI_POTIONTYPE::kHealth:
			case SKYUI_POTIONTYPE::kHealRate:
			case SKYUI_POTIONTYPE::kHealRateMult:
				a_outIconLabel = "potion_health"sv;
				a_outIconColor = 0xDB2E73;
				break;
			case SKYUI_POTIONTYPE::kMagicka:
			case SKYUI_POTIONTYPE::kMagickaRate:
			case SKYUI_POTIONTYPE::kMagickaRateMult:
				a_outIconLabel = "potion_magic"sv;
				a_outIconColor = 0x2E9FDB;
				break;
			case SKYUI_POTIONTYPE::kStamina:
			case SKYUI_POTIONTYPE::kStaminaRate:
			case SKYUI_POTIONTYPE::kStaminaRateMult:
				a_outIconLabel = "potion_stam"sv;
				a_outIconColor = 0x51DB2E;
				break;
			case SKYUI_POTIONTYPE::kResistFire:
				a_outIconLabel = "potion_fire"sv;
				a_outIconColor = 0xC73636;
				break;
			case SKYUI_POTIONTYPE::kResistShock:
				a_outIconLabel = "potion_shock"sv;
				a_outIconColor = 0xEAAB00;
				break;
			case SKYUI_POTIONTYPE::kResistFrost:
				a_outIconLabel = "potion_frost"sv;
				a_outIconColor = 0x1FFBFF;
				break;
			case SKYUI_POTIONTYPE::kDrink:
				a_outIconLabel = "food_wine"sv;
				break;
			case SKYUI_POTIONTYPE::kFood:
				a_outIconLabel = "default_food"sv;
				break;
			case SKYUI_POTIONTYPE::kPoison:
				a_outIconLabel = "potion_poison"sv;
				a_outIconColor = 0xAD00B3;
				break;
			default:
				break;
			}
			

		}
	}

	void RecentLoot::ProcessSoulGemIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor)
	{
		enum class SKYUI_SOULGEMTYPE : uint32_t
		{
			kNone,
			kPetty,
			kLesser,
			kCommon,
			kGreater,
			kGrand,
			kAzura
		};

		enum class SKYUI_SOULGEMSTATUS : uint32_t
		{
			kEmpty,
			kPartial,
			kFull
		};

		auto slgm = a_object->As<RE::TESSoulGem>();
		if (slgm) {
			RE::FormID formID = a_object->GetFormID();
			auto baseID = formID & 0x00FFFFFF;

			auto gemSize = slgm->GetMaximumCapacity();
			auto soulSize = slgm->GetContainedSoul();

			SKYUI_SOULGEMTYPE subType = (SKYUI_SOULGEMTYPE)gemSize;
			SKYUI_SOULGEMSTATUS status;

			if (gemSize == RE::SOUL_LEVEL::kNone || soulSize == RE::SOUL_LEVEL::kNone) {
				status = SKYUI_SOULGEMSTATUS::kEmpty;
			} else if (soulSize >= gemSize) {
				status = SKYUI_SOULGEMSTATUS::kFull;
			} else {
				status = SKYUI_SOULGEMSTATUS::kPartial;
			}

			switch (baseID) {
			case 0x063B29: // BASEID_DA01SOULGEMBLACKSTAR
			case 0x063B27: // BASEID_DA01SOULGEMAZURASSTAR
				subType = SKYUI_SOULGEMTYPE::kAzura;
				break;
			}

			// Select correct label
			a_outIconLabel = "misc_soulgem"sv;
			a_outIconColor = 0xE3E0FF;

			const auto ProcessSoulGemStatusIcon = [status, &a_outIconLabel]() {
				switch (status) {
				case SKYUI_SOULGEMSTATUS::kEmpty:
					a_outIconLabel = "soulgem_empty"sv;
					break;
				case SKYUI_SOULGEMSTATUS::kPartial:
					a_outIconLabel = "soulgem_partial"sv;
					break;
				case SKYUI_SOULGEMSTATUS::kFull:
					a_outIconLabel = "soulgem_full"sv;
					break;
				}
			};

			const auto ProcessGrandSoulGemStatusIcon = [status, &a_outIconLabel]() {
				switch (status) {
				case SKYUI_SOULGEMSTATUS::kEmpty:
					a_outIconLabel = "soulgem_grandempty"sv;
					break;
				case SKYUI_SOULGEMSTATUS::kPartial:
					a_outIconLabel = "soulgem_grandpartial"sv;
					break;
				case SKYUI_SOULGEMSTATUS::kFull:
					a_outIconLabel = "soulgem_grandfull"sv;
					break;
				}
			};

			switch (subType) {
			case SKYUI_SOULGEMTYPE::kPetty:
				a_outIconColor = 0xD7D4FF;
				ProcessSoulGemStatusIcon();
				break;
			case SKYUI_SOULGEMTYPE::kLesser:
				a_outIconColor = 0xC0BAFF;
				ProcessSoulGemStatusIcon();
				break;
			case SKYUI_SOULGEMTYPE::kCommon:
				a_outIconColor = 0xABA3FF;
				ProcessSoulGemStatusIcon();
				break;
			case SKYUI_SOULGEMTYPE::kGreater:
				a_outIconColor = 0x948BFC;
				ProcessGrandSoulGemStatusIcon();
				break;
			case SKYUI_SOULGEMTYPE::kGrand:
				a_outIconColor = 0x7569FF;
				ProcessGrandSoulGemStatusIcon();
				break;
			case SKYUI_SOULGEMTYPE::kAzura:
				a_outIconColor = 0x7569FF;
				a_outIconLabel = "soulgem_azura"sv;
				break;
			default:
				break;
			}

		}
	}

}

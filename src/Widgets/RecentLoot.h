#pragma once
#include "TrueHUDAPI.h"

namespace Scaleform
{
	class RecentLoot : public TRUEHUD_API::WidgetBase
	{
	public:
		RecentLoot(RE::GPtr<RE::GFxMovieView> a_view) :
			WidgetBase(a_view)
		{}

		virtual void Update(float a_deltaTime) override;
		virtual void Initialize() override;
		virtual void Dispose() override;

		void AddMessage(RE::TESBoundObject* a_object, std::string_view a_name, uint32_t a_count);

	protected:
		virtual void UpdatePosition();
		virtual void LoadConfig();

		// The process icon code was adapted, mostly without changes, from SkyUI's actionscript functions.
		void ProcessArmorIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessBookIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessMiscIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessWeaponIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessAmmoIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessPotionIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
		void ProcessSoulGemIcon(RE::TESBoundObject* a_object, std::string& a_outIconLabel, uint32_t& a_outIconColor);
	};
}

#pragma once
#include "TrueHUDAPI.h"

class HUDHandler;

namespace Messaging
{
	using APIResult = ::TRUEHUD_API::APIResult;
	using InterfaceVersion1 = ::TRUEHUD_API::IVTrueHUD1;
	using InterfaceVersion2 = ::TRUEHUD_API::IVTrueHUD2;
	using InterfaceVersion3 = ::TRUEHUD_API::IVTrueHUD3;
	using InterfaceVersion4 = ::TRUEHUD_API::IVTrueHUD4;
	using InterfaceVersion5 = ::TRUEHUD_API::IVTrueHUD5;
	using PlayerWidgetBarType = ::TRUEHUD_API::PlayerWidgetBarType;
	using BarColorType = ::TRUEHUD_API::BarColorType;
	using WidgetRemovalMode = ::TRUEHUD_API::WidgetRemovalMode;

	using SpecialResourceCallback = ::TRUEHUD_API::SpecialResourceCallback;
	using APIResultCallback = ::TRUEHUD_API::APIResultCallback;

	using WidgetBase = ::TRUEHUD_API::WidgetBase;

	class TrueHUDInterface : public InterfaceVersion5
	{
	private:
		TrueHUDInterface() noexcept;
		virtual ~TrueHUDInterface() noexcept;

	public:
		static TrueHUDInterface* GetSingleton() noexcept
		{
			static TrueHUDInterface singleton;
			return std::addressof(singleton);
		}

		// InterfaceVersion1
		virtual unsigned long GetTrueHUDThreadId() const noexcept override;
		virtual APIResult RequestTargetControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult RequestSpecialResourceBarsControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult SetTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept override;
		virtual APIResult SetSoftTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept override;
		virtual void AddActorInfoBar(RE::ActorHandle a_actorHandle) noexcept override;
		virtual void RemoveActorInfoBar(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept override;
		virtual void AddBoss(RE::ActorHandle a_actorHandle) noexcept override;
		virtual void RemoveBoss(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept override;
		virtual void FlashActorValue(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong) noexcept override;
		virtual APIResult FlashActorSpecialBar(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle, bool a_bLong) noexcept override;
		virtual APIResult RegisterSpecialResourceFunctions(SKSE::PluginHandle a_modHandle, SpecialResourceCallback&& a_getCurrentSpecialResource, SpecialResourceCallback&& a_getMaxSpecialResource, bool a_bSpecialMode, bool a_bDisplaySpecialForPlayer) noexcept override;
		virtual void LoadCustomWidgets(SKSE::PluginHandle a_modHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback) noexcept override;
		virtual void RegisterNewWidgetType(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType) noexcept override;
		virtual void AddWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget) noexcept override;
		virtual void RemoveWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode) noexcept override;
		virtual SKSE::PluginHandle GetTargetControlOwner() const noexcept override;
		virtual SKSE::PluginHandle GetPlayerWidgetBarColorsControlOwner() const noexcept override;
		virtual SKSE::PluginHandle GetSpecialResourceBarControlOwner() const noexcept override;
		virtual APIResult ReleaseTargetControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult ReleaseSpecialResourceBarControl(SKSE::PluginHandle a_modHandle) noexcept override;

		// InterfaceVersion2
		virtual void OverrideBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color) noexcept override;
		virtual void OverrideSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color) noexcept override;
		virtual void RevertBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType) noexcept override;
		virtual void RevertSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType) noexcept override;

		// InterfaceVersion3
		virtual void DrawLine(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawPoint(const RE::NiPoint3& a_position, float a_size, float a_duration, uint32_t a_color) noexcept override;
		virtual void DrawArrow(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_size, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawBox(const RE::NiPoint3& a_center, const RE::NiPoint3& a_extent, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawHalfCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawSphere(const RE::NiPoint3& a_origin, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawCylinder(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawCone(const RE::NiPoint3& a_origin, const RE::NiPoint3& a_direction, float a_length, float a_angleWidth, float a_angleHeight, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;
		virtual void DrawCapsule(const RE::NiPoint3& a_origin, float a_halfHeight, float a_radius, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness) noexcept override;

		virtual bool HasInfoBar(RE::ActorHandle a_actorHandle, bool a_bFloatingOnly = false) const noexcept override;

		// InterfaceVersion4
		virtual void DrawCapsule(const RE::NiPoint3& a_vertexA, const RE::NiPoint3& a_vertexB, float a_radius, float a_duration, uint32_t a_color, float a_thickness) noexcept override;

		//InterfaceVersion5
		virtual void DrawArc(const RE::NiPoint3& a_origin, float a_radius, float a_startRadian, float a_endRadian, const RE::NiMatrix3& a_matrix, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness) noexcept override;

		// Does a mod have control over the current target?
		bool IsTargetControlTaken() const noexcept;
		// Does a mod have control over the special resource bars?
		bool IsSpecialResourceBarsControlTaken() const noexcept;

	private:
		unsigned long apiTID = 0;

		std::atomic<SKSE::PluginHandle> targetControlOwner = SKSE::kInvalidPluginHandle;
		std::atomic<SKSE::PluginHandle> specialResourceBarsControlOwner = SKSE::kInvalidPluginHandle;
	};

}

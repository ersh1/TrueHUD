#pragma once
#include <queue>
#include <unordered_set>

#include "Scaleform/TrueHUDMenu.h"
#include "TrueHUDAPI.h"
#include "Widgets/ActorInfoBar.h"
#include "Widgets/BossInfoBar.h"

class HUDHandler :
	public RE::BSTEventSink<RE::TESCombatEvent>,
	public RE::BSTEventSink<RE::TESDeathEvent>,
	public RE::BSTEventSink<RE::TESEnterBleedoutEvent>,
	public RE::BSTEventSink<RE::TESHitEvent>,
	public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
private:
	using EventResult = RE::BSEventNotifyControl;
	using TrueHUDMenu = Scaleform::TrueHUDMenu;
	using BarColorType = ::TRUEHUD_API::BarColorType;
	using WidgetRemovalMode = TRUEHUD_API::WidgetRemovalMode;
	using SpecialResourceCallback = TRUEHUD_API::SpecialResourceCallback;
	using APIResultCallback = TRUEHUD_API::APIResultCallback;
	using WidgetBase = TRUEHUD_API::WidgetBase;
	using MenuVisibilityMode = TrueHUDMenu::MenuVisibilityMode;

public:
	static HUDHandler* GetSingleton()
	{
		static HUDHandler singleton;
		return std::addressof(singleton);
	}

	static void Register();

	virtual EventResult ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) override;
	virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;
	virtual EventResult ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>* a_eventSource) override;
	virtual EventResult ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override;
	virtual EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

	void OpenTrueHUDMenu();
	void CloseTrueHUDMenu();

	bool IsTrueHUDMenuOpen();
	static RE::GPtr<TrueHUDMenu> GetTrueHUDMenu();

	void Update();

	RE::ObjectRefHandle GetTarget() const;
	RE::ObjectRefHandle GetSoftTarget() const;
	void SetTarget(RE::ObjectRefHandle a_actorHandle);
	void SetSoftTarget(RE::ObjectRefHandle a_actorHandle);

	bool HasActorInfoBar(RE::ObjectRefHandle a_actorHandle);
	void AddActorInfoBar(RE::ObjectRefHandle a_actorHandle);
	void RemoveActorInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode = WidgetRemovalMode::Immediate);

	bool HasBossInfoBar(RE::ObjectRefHandle a_actorHandle);
	void AddBossInfoBar(RE::ObjectRefHandle a_actorHandle);
	void RemoveBossInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode = WidgetRemovalMode::Immediate);

	void AddShoutIndicator();
	void RemoveShoutIndicator();

	void AddPlayerWidget();
	void RemovePlayerWidget();
	void UpdatePlayerWidgetChargeMeters(float a_percent, bool a_bForce, bool a_bLeftHand, bool a_bShow);

	void AddRecentLootWidget();
	void RemoveRecentLootWidget();
	void AddRecentLootMessage(RE::TESBoundObject* a_object, std::string_view a_name, uint32_t a_count);

	void AddFloatingWorldTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint3 a_worldPosition);
	void AddFloatingScreenTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint2 a_screenPosition);
	void AddStackingDamageWorldTextWidget(RE::ObjectRefHandle a_refHandle, float a_damage);

	void OverrideBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color);
	void OverrideSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color);
	void RevertBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType);
	void RevertSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType);

	void LoadCustomWidgets(SKSE::PluginHandle a_pluginHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback);
	void RegisterNewWidgetType(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType);
	void AddWidget(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget);
	void RemoveWidget(SKSE::PluginHandle a_pluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode);

	void FlashActorValue(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong);
	void FlashActorSpecialBar(RE::ObjectRefHandle a_actorHandle, bool a_bLong);

	void DrawLine(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_duration, uint32_t a_color, float a_thickness);
	void DrawPoint(const RE::NiPoint3& a_position, float a_size, float a_duration, uint32_t a_color);
	void DrawArrow(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_size, float a_duration, uint32_t a_color, float a_thickness);
	void DrawBox(const RE::NiPoint3& a_center, const RE::NiPoint3& a_extent, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness);
	void DrawCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawHalfCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawArc(const RE::NiPoint3& a_origin, float a_radius, float a_startRadian, float a_endRadian, const RE::NiMatrix3& a_matrix, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawSphere(const RE::NiPoint3& a_origin, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawCylinder(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_radius, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawCone(const RE::NiPoint3& a_origin, const RE::NiPoint3& a_direction, float a_length, float a_angleWidth, float a_angleHeight, uint32_t a_segments, float a_duration, uint32_t a_color, float a_thickness);
	void DrawCapsule(const RE::NiPoint3& a_origin, float a_halfHeight, float a_radius, const RE::NiQuaternion& a_rotation, float a_duration, uint32_t a_color, float a_thickness);
	void DrawCapsule(const RE::NiPoint3& a_vertexA, const RE::NiPoint3& a_vertexB, float a_radius, float a_duration, uint32_t a_color, float a_thickness);

	void SetMenuVisibilityMode(MenuVisibilityMode a_mode);
	void SetCartMode(bool a_enable);

	void RemoveAllWidgets();

	bool CheckActorForBoss(RE::ObjectRefHandle a_refHandle);

	void OnPreLoadGame();
	void OnSettingsUpdated();

	void Initialize();

	bool bSpecialFunctionsProvided = false;
	SpecialResourceCallback GetCurrentSpecial = nullptr;
	SpecialResourceCallback GetMaxSpecial = nullptr;
	bool bSpecialMode;
	bool bDisplaySpecialForPlayer;

protected:
	friend class TrueHUDMenu;

	void Process(TrueHUDMenu& a_menu, float a_deltaTime);

private:
	using HUDTask = std::function<void(TrueHUDMenu&)>;
	using Lock = std::recursive_mutex;
	using Locker = std::lock_guard<Lock>;

	HUDHandler();
	HUDHandler(const HUDHandler&) = delete;
	HUDHandler(HUDHandler&&) = delete;

	~HUDHandler() = default;

	HUDHandler& operator=(const HUDHandler&) = delete;
	HUDHandler& operator=(HUDHandler&&) = delete;

	void AddHUDTask(HUDTask a_task);

	mutable Lock _lock;
	std::queue<HUDTask> _taskQueue;

	RE::ObjectRefHandle _currentTarget;
	RE::ObjectRefHandle _currentSoftTarget;

	struct DamageStack
	{
		DamageStack(float a_damage) :
			damage(a_damage)
		{}

		float damage = 0.f;
		float timeElapsed = 0.f;
	};
	std::unordered_map<RE::ObjectRefHandle, DamageStack> _stackingDamage;
	constexpr static float _stackingPeriodDuration = 0.5f;
};

#pragma once

#include "TrueHUDAPI.h"
#include "Widgets/ActorInfoBar.h"
#include "Widgets/BossInfoBar.h"
#include "Widgets/ShoutIndicator.h"
#include "Widgets/PlayerWidget.h"
#include "Widgets/FloatingText.h"
#include "Widgets/RecentLoot.h"
#include "Offsets.h"
#include <chrono>
#include <unordered_set>

namespace std
{
	template <class T>
	struct hash<RE::BSPointerHandle<T>>
	{
		uint32_t operator()(const RE::BSPointerHandle<T>& a_handle) const
		{
			uint32_t nativeHandle = const_cast<RE::BSPointerHandle<T>*>(&a_handle)->native_handle();  // ugh
			return nativeHandle;
		}
	};
}

namespace Scaleform
{
	class DebugLine
	{
	public:
		DebugLine(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, uint32_t a_color, float a_thickness, float a_duration) :
			start(a_start),
			end(a_end),
			color(a_color),
			thickness(a_thickness),
			duration(a_duration)
		{}

		RE::NiPoint3 start;
		RE::NiPoint3 end;
		uint32_t color;
		float thickness;
		float duration;
	};

	class DebugPoint
	{
	public:
		DebugPoint(const RE::NiPoint3& a_position, uint32_t a_color, float a_size, float a_duration) :
			position(a_position),
			color(a_color),
			size(a_size),
			duration(a_duration)
		{}

		RE::NiPoint3 position;
		uint32_t color;
		float size;
		float duration;
	};

	class TrueHUDMenu : public RE::IMenu
	{
	private:
		using Super = RE::IMenu;
		using BarColorType = ::TRUEHUD_API::BarColorType;
		using WidgetRemovalMode = TRUEHUD_API::WidgetRemovalMode;
		using WidgetBase = TRUEHUD_API::WidgetBase;
		using APIResultCallback = TRUEHUD_API::APIResultCallback;

		enum class TrueHUDWidgetType : uint8_t
		{
			kInfoBar = 0,
			kBossBar = 1,
			kStandaloneShoutIndicator = 2,
			kPlayerWidget = 3,
			kRecentLootWidget = 4,
			kFloatingText = 5
		};

		enum class BarType : uint8_t
		{
			kHealth,
			kMagicka,
			kStamina,
			kSpecial
		};

		struct BarColorOverride
		{
			uint32_t barColor = 0xFFFFFF;
			uint32_t phantomColor = 0xFFFFFF;
			uint32_t backgroundColor = 0xFFFFFF;
			uint32_t penaltyColor = 0xFFFFFF;
			uint32_t flashColor = 0xFFFFFF;

			bool bOverrideBarColor = false;
			bool bOverridePhantomColor = false;
			bool bOverrideBackgroundColor = false;
			bool bOverridePenaltyColor = false;
			bool bOverrideFlashColor = false;

			bool IsEmpty() const;
			bool GetColor(BarColorType a_colorType, uint32_t& a_outColor) const;
			void SetOverride(BarColorType a_colorType, uint32_t a_color);
			void RemoveOverride(BarColorType a_colorType);
		};

	public:
		enum class MenuVisibilityMode : uint8_t
		{
			kHidden,
			kPartial,
			kVisible
		};

		static constexpr std::string_view MenuName() noexcept { return MENU_NAME; }
		static constexpr std::int8_t SortPriority() noexcept { return SORT_PRIORITY; }

		static void Register();

		bool IsOpen() const;

		RE::GPtr<RE::GFxMovieView> GetView() const;

		void SetTarget(RE::ObjectRefHandle a_actorHandle);
		void SetSoftTarget(RE::ObjectRefHandle a_actorHandle);
		RE::ObjectRefHandle GetTarget() const;
		RE::ObjectRefHandle GetSoftTarget() const;
		
		bool HasActorInfoBar(RE::ObjectRefHandle a_actorHandle);
		bool AddActorInfoBar(RE::ObjectRefHandle a_actorHandle);
		bool RemoveActorInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode);

		bool HasBossInfoBar(RE::ObjectRefHandle a_actorHandle);
		bool AddBossInfoBar(RE::ObjectRefHandle a_actorHandle);
		bool RemoveBossInfoBar(RE::ObjectRefHandle a_actorHandle, WidgetRemovalMode a_removalMode);

		bool AddShoutIndicator();
		bool RemoveShoutIndicator();

		bool AddPlayerWidget();
		bool RemovePlayerWidget();
		void UpdatePlayerWidgetChargeMeters(float a_percent, bool a_bForce, bool a_bLeftHand, bool a_bShow);

		bool AddRecentLootWidget();
		bool RemoveRecentLootWidget();
		void AddRecentLootMessage(RE::TESBoundObject* a_object, std::string_view a_name, uint32_t a_count);

		bool AddFloatingWorldTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint3 a_worldPosition);
		bool AddFloatingScreenTextWidget(std::string a_text, uint32_t a_color, float a_duration, bool a_bSpecial, RE::NiPoint2 a_screenPosition);

		bool LoadCustomWidgets(SKSE::PluginHandle a_myPluginHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback);
		bool RegisterNewWidgetType(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType);
		bool AddWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget);
		bool RemoveWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode);

		void FlashActorValue(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong);
		void FlashActorSpecialBar(RE::ObjectRefHandle a_actorHandle, bool a_bLong);

		void OverrideBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color);
		void OverrideSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color);
		void RevertBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType);
		void RevertSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType);
		uint32_t GetBarColor(RE::ObjectRefHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType) const;
		uint32_t GetSpecialBarColor(RE::ObjectRefHandle a_actorHandle, BarColorType a_colorType) const;
		uint32_t GetDefaultColor(BarType a_barType, BarColorType a_barColorType) const;

		void DrawLine(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawPoint(const RE::NiPoint3& a_position, float a_size, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF);
		void DrawArrow(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_size = 10.f, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawBox(const RE::NiPoint3& a_center, const RE::NiPoint3& a_extent, const RE::NiQuaternion& a_rotation, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawHalfCircle(const RE::NiPoint3& a_center, const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, float a_radius, uint32_t a_segments, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawSphere(const RE::NiPoint3& a_origin, float a_radius, uint32_t a_segments = 16, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawCylinder(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, float a_radius, uint32_t a_segments, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawCone(const RE::NiPoint3& a_origin, const RE::NiPoint3& a_direction, float a_length, float a_angleWidth, float a_angleHeight, uint32_t a_segments, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);
		void DrawCapsule(const RE::NiPoint3& a_origin, float a_halfHeight, float a_radius, const RE::NiQuaternion& a_rotation, float a_duration = 0.f, uint32_t a_color = 0xFF0000FF, float a_thickness = 1.f);

		void SetMenuVisibilityMode(MenuVisibilityMode a_mode);
		void SetCartMode(bool a_enable);

		void UpdateSettings();
		void RemoveAllWidgets();

	protected:
		using UIResult = RE::UI_MESSAGE_RESULTS;

		TrueHUDMenu()
		{
			auto menu = static_cast<Super*>(this);
			menu->depthPriority = SortPriority();
			auto scaleformManager = RE::BSScaleformManager::GetSingleton();

			// do NOT use LoadMovieEx if you want skse scaleform hooks, it doesn't call the original function which is hooked by skse
			[[maybe_unused]] const auto success =
				scaleformManager->LoadMovieEx(menu, FILE_NAME, [](RE::GFxMovieDef* a_def) -> void {
					a_def->SetState(
						RE::GFxState::StateType::kLog,
						RE::make_gptr<Logger>().get());
				});

			// this works instead
			//[[maybe_unused]] const auto success = scaleformManager->LoadMovie(menu, menu->uiMovie, FILE_NAME.data());
			assert(success);
			auto def = menu->uiMovie->GetMovieDef();
			if (def) {
				def->SetState(
					RE::GFxState::StateType::kLog,
					RE::make_gptr<Logger>().get());
			}

			menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
			menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
			menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
			//menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);
			menu->inputContext = Context::kNone;

			_view = menu->uiMovie;
			_view->SetMouseCursorCount(0);	// disable input
		}

		TrueHUDMenu(const TrueHUDMenu&) = default;
		TrueHUDMenu(TrueHUDMenu&&) = default;

		~TrueHUDMenu() = default;

		TrueHUDMenu& operator=(const TrueHUDMenu&) = default;
		TrueHUDMenu& operator=(TrueHUDMenu&&) = default;

		static RE::stl::owner<RE::IMenu*> Creator() { return new TrueHUDMenu(); }

		// IMenu
		void PostCreate() {
			OnOpen();
			Super::PostCreate();
		}

		UIResult ProcessMessage(RE::UIMessage& a_message) override {
			using Type = RE::UI_MESSAGE_TYPE;

			switch (*a_message.type) {
			case Type::kShow:
				OnOpen();
				return Super::ProcessMessage(a_message);
			case Type::kHide:
				OnClose();
				return Super::ProcessMessage(a_message);
			default:
				return Super::ProcessMessage(a_message);
			}
		}

		void AdvanceMovie(float, std::uint32_t) override
		{
			auto currentTime = chrono_clock::now();
			auto dt = (currentTime - _movieLastTime) / 1000000000.f;

			_movieLastTime = currentTime;
			float deltaTime = static_cast<float>(dt.count());

			ProcessDelegate(deltaTime);
			Update(deltaTime);
			//Super::AdvanceMovie(a_interval, a_currentTime); ??

			if (uiMovie) {
				uiMovie->Advance(deltaTime);
			}
		}

	private:
		class Logger : public RE::GFxLog
		{
		public:
			void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
			{
				std::string fmt(a_fmt ? a_fmt : "");
				while (!fmt.empty() && fmt.back() == '\n') {
					fmt.pop_back();
				}

				std::va_list args;
				va_copy(args, a_argList);
				std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
				std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
				va_end(args);

				logger::info("{}: {}"sv, TrueHUDMenu::MenuName(), buf.data());
			}
		};

		using Lock = std::recursive_mutex;
		using Locker = std::lock_guard<Lock>;
		using chrono_clock = std::chrono::steady_clock;

		mutable Lock _lock;

		void OnOpen();
		void OnClose();

		void ProcessDelegate(float a_deltaTime);
		void Update(float a_deltaTime);

		void CacheData();

		void UpdateColors();
		void UpdateVisibility();

		bool AddBossInfoBarWidget(RE::ObjectRefHandle a_actorHandle);
		int32_t GetNextBossBarIndex() const;
		void RefreshBossBarIndexes(int32_t a_removedIndex);
		void UpdateBossQueue();
		void AddToDepthsArray(std::shared_ptr<TRUEHUD_API::WidgetBase> a_widget, uint32_t a_widgetType, RE::GFxValue& a_array);

		void UpdateDebugDraw(float a_deltaTime);

		void DrawLine2D(RE::NiPoint2& a_start, RE::NiPoint2& a_end, uint32_t a_color, float a_thickness);
		void DrawPoint2D(RE::NiPoint2& a_position, uint32_t a_color, float a_size);
		void DrawLine3D(RE::NiPoint3& a_start, RE::NiPoint3& a_end, uint32_t a_color, float a_thickness);
		void DrawPoint3D(RE::NiPoint3& a_position, uint32_t a_color, float a_size);
		void ClearLines();

		RE::NiPoint2 WorldToScreen(const RE::NiPoint3& a_worldPos) const;

		void ClampToScreen(RE::NiPoint2& a_point);

		bool IsOnScreen(RE::NiPoint2& a_start, RE::NiPoint2& a_end) const;
		bool IsOnScreen(RE::NiPoint2& a_point) const;

		bool _bIsOpen = false;

		bool _bCachedData = false;
		RE::NiPoint2 _screenRes;

		MenuVisibilityMode _menuVisibilityMode = MenuVisibilityMode::kVisible;
		bool _bCartMode = false;
		
		std::unordered_map<RE::ObjectRefHandle, std::shared_ptr<ActorInfoBar>> _actorInfoBarMap;
		std::unordered_map<RE::ObjectRefHandle, std::shared_ptr<BossInfoBar>> _bossInfoBarMap;
		std::shared_ptr<ShoutIndicator> _shoutIndicator;
		std::shared_ptr<PlayerWidget> _playerWidget;
		std::shared_ptr<RecentLoot> _recentLoot;
		std::unordered_map<uint32_t, std::shared_ptr<FloatingText>> _floatingTextMap;
		uint32_t _nextFloatingTextID = 0;

		using CustomWidgets = std::unordered_map<uint32_t, std::shared_ptr<WidgetBase>>;
		using CustomWidgetTypes = std::unordered_map<uint32_t, CustomWidgets>;
		using CustomWidgetMap = std::unordered_map<SKSE::PluginHandle, CustomWidgetTypes>;

		CustomWidgetMap _customWidgets;

		RE::ObjectRefHandle _targetHandle;
		RE::ObjectRefHandle _softTargetHandle;

		std::list<RE::ObjectRefHandle> _bossQueue;

		std::unordered_map<RE::ObjectRefHandle, std::unordered_map<BarType, BarColorOverride>> _colorOverrides;
		std::unordered_set<RE::ObjectRefHandle> _pendingColorChanges;

		std::vector<std::unique_ptr<DebugLine>> _linesToDraw;
		std::vector<std::unique_ptr<DebugPoint>> _pointsToDraw;

		bool _bVanillaEnemyHealthAlphaSaved = false;
		bool _bVanillaEnemyHealthHidden = false;
		RE::GFxValue _savedVanillaEnemyHealthAlpha;

		bool _bSubtitleYSaved = false;
		RE::GFxValue _savedSubtitleY;

		bool _bCompassAlphaSaved = false;
		RE::GFxValue _savedCompassAlpha;

		constexpr static std::string_view FILE_NAME{ "TrueHUD" };
		constexpr static std::string_view MENU_NAME{ "TrueHUD" };
		constexpr static std::int8_t SORT_PRIORITY{ 0 };

		RE::GPtr<RE::GFxMovieView> _view;

		chrono_clock::time_point _movieLastTime;
	};
}

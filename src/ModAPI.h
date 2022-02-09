#pragma once
#define TRUEHUD_API_COMMONLIB
#include "TrueHUDAPI.h"

class HUDHandler;

namespace Messaging
{
	using APIResult = ::TRUEHUD_API::APIResult;
	using InterfaceVersion1 = ::TRUEHUD_API::IVTrueHUD1;
	using InterfaceVersion2 = ::TRUEHUD_API::IVTrueHUD2;
	using PlayerWidgetBarType = ::TRUEHUD_API::PlayerWidgetBarType;
	using BarColorType = ::TRUEHUD_API::BarColorType;
	using WidgetRemovalMode = ::TRUEHUD_API::WidgetRemovalMode;
	using InterfaceContainer = ::TRUEHUD_API::InterfaceContainer;

	using InterfaceLoaderCallback = std::function<void(void* interfaceInstance, uint8_t interfaceVersion)>;
	using SpecialResourceCallback = ::TRUEHUD_API::SpecialResourceCallback;
	using APIResultCallback = ::TRUEHUD_API::APIResultCallback;
	
	using WidgetBase = ::TRUEHUD_API::WidgetBase;

	class TrueHUDInterface : public InterfaceVersion2
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
		virtual APIResult RequestPlayerWidgetBarColorsControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult RequestSpecialResourceBarsControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult SetTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept override;
		virtual APIResult SetSoftTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept override;
		virtual void AddActorInfoBar(RE::ActorHandle a_actorHandle) noexcept override;
		virtual void RemoveActorInfoBar(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept override;
		virtual void AddBoss(RE::ActorHandle a_actorHandle) noexcept override;
		virtual void RemoveBoss(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept override;
		virtual APIResult OverridePlayerWidgetBarColor(SKSE::PluginHandle a_modHandle, PlayerWidgetBarType a_playerWidgetBarType, BarColorType a_colorType, uint32_t a_color) noexcept override;
		virtual APIResult RevertPlayerWidgetBarColor(SKSE::PluginHandle a_modHandle, PlayerWidgetBarType a_playerWidgetBarType, BarColorType a_colorType) noexcept override;
		virtual void FlashActorValue(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong) noexcept override;
		virtual APIResult FlashActorSpecialBar(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle, bool a_bLong) noexcept override;
		virtual APIResult RegisterSpecialResourceFunctions(SKSE::PluginHandle a_modHandle, SpecialResourceCallback&& a_getCurrentSpecialResource, SpecialResourceCallback&& a_getMaxSpecialResource, bool a_bSpecialMode) noexcept override;
		virtual void LoadCustomWidgets(SKSE::PluginHandle a_modHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback) noexcept override;
		virtual void RegisterNewWidgetType(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType) noexcept override;
		virtual void AddWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget) noexcept override;
		virtual void RemoveWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode) noexcept override;
		virtual SKSE::PluginHandle GetTargetControlOwner() const noexcept override;
		virtual SKSE::PluginHandle GetPlayerWidgetBarColorsControlOwner() const noexcept override;
		virtual SKSE::PluginHandle GetSpecialResourceBarControlOwner() const noexcept override;
		virtual APIResult ReleaseTargetControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult ReleasePlayerWidgetBarColorsControl(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult ReleaseSpecialResourceBarControl(SKSE::PluginHandle a_modHandle) noexcept override;

		// InterfaceVersion2
		virtual void OverrideBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color) noexcept override;
		virtual void OverrideSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color) noexcept override;
		virtual void RevertBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType) noexcept override;
		virtual void RevertSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType) noexcept override;

		// Does a mod have control over the current target?
		bool IsTargetControlTaken() const noexcept;
		// Does a mod have control over the special resource bars?
		bool IsSpecialResourceBarsControlTaken() const noexcept; 

 public:
		using Consumers = std::vector<std::string>;

		void RegisterConsumer(const char* modName) noexcept;
		const Consumers& GetConsumers() const noexcept;

	private:
		Consumers consumers = {};
		unsigned long apiTID = 0;

		std::atomic<SKSE::PluginHandle> targetControlOwner = SKSE::kInvalidPluginHandle;
		std::atomic<SKSE::PluginHandle> specialResourceBarsControlOwner = SKSE::kInvalidPluginHandle;
	};

	void HandleInterfaceRequest(SKSE::MessagingInterface::Message* msg) noexcept;
	bool RegisterInterfaceListenerCallback(const SKSE::MessagingInterface* skseMessaging, const char* sender, InterfaceLoaderCallback&& callback) noexcept;
}

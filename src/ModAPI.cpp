#include "ModAPI.h"
#include "HUDHandler.h"
#include "Settings.h"

namespace Messaging
{
	TrueHUDInterface::TrueHUDInterface() noexcept
	{
		apiTID = GetCurrentThreadId();
	}

	TrueHUDInterface::~TrueHUDInterface() noexcept {}

	unsigned long TrueHUDInterface::GetTrueHUDThreadId() const noexcept
	{
		return apiTID;
	}

	APIResult TrueHUDInterface::RequestTargetControl(SKSE::PluginHandle a_modHandle) noexcept
	{
		const auto owner = targetControlOwner.load(std::memory_order::memory_order_acquire);
		if (owner != SKSE::kInvalidPluginHandle)
			if (owner == a_modHandle)
				return APIResult::AlreadyGiven;
			else
				return APIResult::AlreadyTaken;

		auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
		if (!targetControlOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
			return APIResult::AlreadyTaken;

		return APIResult::OK;
	}

	APIResult TrueHUDInterface::RequestPlayerWidgetBarColorsControl(SKSE::PluginHandle) noexcept
	{
		return APIResult::OK;
	}

	APIResult TrueHUDInterface::RequestSpecialResourceBarsControl(SKSE::PluginHandle a_modHandle) noexcept
	{
		const auto owner = specialResourceBarsControlOwner.load(std::memory_order::memory_order_acquire);
		if (owner != SKSE::kInvalidPluginHandle)
			if (owner == a_modHandle)
				return APIResult::AlreadyGiven;
			else
				return APIResult::AlreadyTaken;

		auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
		if (!specialResourceBarsControlOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
			return APIResult::AlreadyTaken;

		if (Settings::glob_trueHUDSpecialResourceBars) {
			Settings::glob_trueHUDSpecialResourceBars->value = true;
		}
		return APIResult::OK;
	}

	APIResult TrueHUDInterface::SetTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept
	{
		if (targetControlOwner != a_modHandle) {
			return APIResult::NotOwner;
		}

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->SetTarget(a_actorHandle);
		}

		return APIResult::OK;
	}

	APIResult TrueHUDInterface::SetSoftTarget(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle) noexcept
	{
		if (targetControlOwner != a_modHandle) {
			return APIResult::NotOwner;
		}

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->SetSoftTarget(a_actorHandle);
		}

		return APIResult::OK;
	}

	void TrueHUDInterface::AddActorInfoBar(RE::ActorHandle a_actorHandle) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->AddActorInfoBar(a_actorHandle);
		}
	}

	void TrueHUDInterface::RemoveActorInfoBar(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RemoveActorInfoBar(a_actorHandle, static_cast<TRUEHUD_API::WidgetRemovalMode>(a_removalMode));
		}
	}

	void TrueHUDInterface::AddBoss(RE::ActorHandle a_actorHandle) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->AddBossInfoBar(a_actorHandle);
		}
	}

	void TrueHUDInterface::RemoveBoss(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RemoveBossInfoBar(a_actorHandle, static_cast<TRUEHUD_API::WidgetRemovalMode>(a_removalMode));
		}
	}

	APIResult TrueHUDInterface::OverridePlayerWidgetBarColor(SKSE::PluginHandle, PlayerWidgetBarType a_playerWidgetBarType, BarColorType a_colorType, uint32_t a_color) noexcept
	{
		auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();
		switch (a_playerWidgetBarType) {
		case PlayerWidgetBarType::HealthBar:
			OverrideBarColor(playerHandle, RE::ActorValue::kHealth, a_colorType, a_color);
			break;
		case PlayerWidgetBarType::MagickaBar:
			OverrideBarColor(playerHandle, RE::ActorValue::kMagicka, a_colorType, a_color);
			break;
		case PlayerWidgetBarType::StaminaBar:
			OverrideBarColor(playerHandle, RE::ActorValue::kStamina, a_colorType, a_color);
			break;
		case PlayerWidgetBarType::SpecialBar:
			OverrideSpecialBarColor(playerHandle, a_colorType, a_color);
			break;
		}

		return APIResult::OK;
	}

	APIResult TrueHUDInterface::RevertPlayerWidgetBarColor(SKSE::PluginHandle, PlayerWidgetBarType a_playerWidgetBarType, BarColorType a_colorType) noexcept
	{
		auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();
		switch (a_playerWidgetBarType) {
		case PlayerWidgetBarType::HealthBar:
			RevertBarColor(playerHandle, RE::ActorValue::kHealth, a_colorType);
			break;
		case PlayerWidgetBarType::MagickaBar:
			RevertBarColor(playerHandle, RE::ActorValue::kMagicka, a_colorType);
			break;
		case PlayerWidgetBarType::StaminaBar:
			RevertBarColor(playerHandle, RE::ActorValue::kStamina, a_colorType);
			break;
		case PlayerWidgetBarType::SpecialBar:
			RevertSpecialBarColor(playerHandle, a_colorType);
			break;
		}

		return APIResult::OK;
	}

	void TrueHUDInterface::FlashActorValue(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->FlashActorValue(a_actorHandle, a_actorValue, a_bLong);
		}
	}

	APIResult TrueHUDInterface::FlashActorSpecialBar(SKSE::PluginHandle a_modHandle, RE::ActorHandle a_actorHandle, bool a_bLong) noexcept
	{
		if (specialResourceBarsControlOwner != a_modHandle) {
			return APIResult::NotOwner;
		}

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->FlashActorSpecialBar(a_actorHandle, a_bLong);
		}

		return APIResult::OK;
	}

	APIResult TrueHUDInterface::RegisterSpecialResourceFunctions(SKSE::PluginHandle a_modHandle, SpecialResourceCallback&& a_getCurrentSpecialResource, SpecialResourceCallback&& a_getMaxSpecialResource, bool a_bSpecialMode) noexcept
	{
		if (specialResourceBarsControlOwner != a_modHandle) {
			return APIResult::NotOwner;
		}

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->GetCurrentSpecial = a_getCurrentSpecialResource;
			hudHandler->GetMaxSpecial = a_getMaxSpecialResource;
			hudHandler->bSpecialMode = a_bSpecialMode;
			hudHandler->bSpecialFunctionsProvided = true;
		}

		return APIResult::OK;
	}

	void TrueHUDInterface::LoadCustomWidgets(SKSE::PluginHandle a_modHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->LoadCustomWidgets(a_modHandle, a_filePath, std::forward<APIResultCallback>(a_successCallback));
		}
	}

	void TrueHUDInterface::RegisterNewWidgetType(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RegisterNewWidgetType(a_modHandle, a_widgetType);
		}
	}

	void TrueHUDInterface::AddWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->AddWidget(a_modHandle, a_widgetType, a_widgetID, a_symbolIdentifier, a_widget);
		}
	}

	void TrueHUDInterface::RemoveWidget(SKSE::PluginHandle a_modHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RemoveWidget(a_modHandle, a_widgetType, a_widgetID, a_removalMode);
		}
	}

	SKSE::PluginHandle TrueHUDInterface::GetTargetControlOwner() const noexcept
	{
		return targetControlOwner;
	}

	SKSE::PluginHandle TrueHUDInterface::GetPlayerWidgetBarColorsControlOwner() const noexcept
	{
		return SKSE::kInvalidPluginHandle;
	}

	SKSE::PluginHandle TrueHUDInterface::GetSpecialResourceBarControlOwner() const noexcept
	{
		return specialResourceBarsControlOwner;
	}

	APIResult TrueHUDInterface::ReleaseTargetControl(SKSE::PluginHandle a_modHandle) noexcept
	{
		if (targetControlOwner != a_modHandle)
			return APIResult::NotOwner;
		targetControlOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

		return APIResult::OK;
	}

	APIResult TrueHUDInterface::ReleasePlayerWidgetBarColorsControl(SKSE::PluginHandle) noexcept
	{
		return APIResult::OK;
	}

	APIResult TrueHUDInterface::ReleaseSpecialResourceBarControl(SKSE::PluginHandle a_modHandle) noexcept
	{
		if (specialResourceBarsControlOwner != a_modHandle)
			return APIResult::NotOwner;
		specialResourceBarsControlOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->GetCurrentSpecial = nullptr;
			hudHandler->GetMaxSpecial = nullptr;
			hudHandler->bSpecialFunctionsProvided = false;
		}

		if (Settings::glob_trueHUDSpecialResourceBars) {
			Settings::glob_trueHUDSpecialResourceBars->value = false;
		}

		return APIResult::OK;
	}

	void TrueHUDInterface::OverrideBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType, uint32_t a_color) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->OverrideBarColor(a_actorHandle, a_actorValue, a_colorType, a_color);
		}
	}

	void TrueHUDInterface::OverrideSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType, uint32_t a_color) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->OverrideSpecialBarColor(a_actorHandle, a_colorType, a_color);
		}
	}

	void TrueHUDInterface::RevertBarColor(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, BarColorType a_colorType) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RevertBarColor(a_actorHandle, a_actorValue, a_colorType);
		}
	}

	void TrueHUDInterface::RevertSpecialBarColor(RE::ActorHandle a_actorHandle, BarColorType a_colorType) noexcept
	{
		auto hudHandler = HUDHandler::GetSingleton();
		if (hudHandler) {
			hudHandler->RevertSpecialBarColor(a_actorHandle, a_colorType);
		}
	}

	bool TrueHUDInterface::IsTargetControlTaken() const noexcept
	{
		return targetControlOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
	}

	bool TrueHUDInterface::IsSpecialResourceBarsControlTaken() const noexcept
	{
		return specialResourceBarsControlOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
	}

	void TrueHUDInterface::RegisterConsumer(const char* a_modName) noexcept
	{
		consumers.push_back(std::move(std::string(a_modName)));
		logger::info(FMT_STRING("Added API consumer '{}'"), a_modName);
	}

	const TrueHUDInterface::Consumers& TrueHUDInterface::GetConsumers() const noexcept
	{
		return consumers;
	}

	void HandleInterfaceRequest(SKSE::MessagingInterface::Message* a_msg) noexcept
	{
		if (a_msg->type != 0)
			return;
		constexpr const auto DispatchToPlugin = [](TRUEHUD_API::PluginMessage* packet, const char* to) noexcept {
			if (!SKSE::GetMessagingInterface()->Dispatch(0, packet, sizeof(TRUEHUD_API::PluginMessage), to))
				logger::warn(FMT_STRING("Failed to dispatch API message to '{}'"), to ? to : "unnamed");
		};

		TRUEHUD_API::PluginMessage packet = {};
		packet.type = TRUEHUD_API::PluginMessage::Type::Error;

		if (a_msg->dataLen != sizeof(TRUEHUD_API::PluginMessage)) {
			DispatchToPlugin(&packet, a_msg->sender);
			return;
		}

		const auto cmd = reinterpret_cast<const TRUEHUD_API::PluginMessage*>(a_msg->data);
		if (cmd->header != 'THUD' || cmd->type != TRUEHUD_API::PluginMessage::Type::RequestInterface) {
			//DispatchToPlugin(&packet, a_msg->sender);
			return;
		}

		const auto request = reinterpret_cast<const TRUEHUD_API::InterfaceRequest*>(cmd->messageData);
		if (!(request->interfaceVersion == TRUEHUD_API::InterfaceVersion::V1 ||
			request->interfaceVersion == TRUEHUD_API::InterfaceVersion::V2))
		{
			DispatchToPlugin(&packet, a_msg->sender);
			return;
		}

		auto api = TrueHUDInterface::GetSingleton();
		if (a_msg->sender)
			api->RegisterConsumer(a_msg->sender);
		else
			logger::info("Added unnamed API consumer");

		TRUEHUD_API::InterfaceContainer container = {};
		container.interfaceVersion = request->interfaceVersion;

		switch (request->interfaceVersion) {
		case TRUEHUD_API::InterfaceVersion::V1:
			[[fallthrough]];
		case TRUEHUD_API::InterfaceVersion::V2:
			container.interfaceInstance = static_cast<void*>(api);
			break;
		default:
			api->RegisterConsumer(a_msg->sender);
			return;
		}

		packet.type = TRUEHUD_API::PluginMessage::Type::InterfaceProvider;
		packet.messageData = &container;
		DispatchToPlugin(&packet, a_msg->sender);
	}

	bool RegisterInterfaceListenerCallback(const SKSE::MessagingInterface* skseMessaging, const char* sender, InterfaceLoaderCallback&& callback) noexcept
	{
		static InterfaceLoaderCallback storedCallback = callback;

		return skseMessaging->RegisterListener(sender, [](SKSE::MessagingInterface::Message* msg) {
			if (msg->type != 0) {
				return;
			}

			if (msg->dataLen == sizeof(TRUEHUD_API::PluginMessage)) {
				const auto resp = reinterpret_cast<TRUEHUD_API::PluginMessage*>(msg->data);
				if (resp->type == TRUEHUD_API::PluginMessage::Type::InterfaceProvider) {
					auto interfaceContainer = reinterpret_cast<InterfaceContainer*>(resp->messageData);
					storedCallback(
						interfaceContainer->interfaceInstance,
						static_cast<uint8_t>(interfaceContainer->interfaceVersion));
					return;
				}
			}

			HandleInterfaceRequest(msg);
		});
	}

}


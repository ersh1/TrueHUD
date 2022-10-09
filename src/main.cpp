#include "Hooks.h"
#include "Papyrus.h"
#include "Settings.h"
#include "HUDHandler.h"
#include "Scaleform/Scaleform.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Scaleform::Register();
		HUDHandler::Register();
		Settings::Initialize();
		Settings::ReadSettings();		
		HUDHandler::GetSingleton()->Initialize();
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		HUDHandler::GetSingleton()->OnPreLoadGame();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
	case SKSE::MessagingInterface::kNewGame:
		Settings::OnPostLoadGame();
		break;
	}
}

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Ersh");
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
	v.HasNoStructUse(true);

	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) { Sleep(100); }
#endif
	REL::Module::reset();  // Clib-NG bug workaround
	
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 8);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Hooks::Install();
	Papyrus::Register();

	return true;
}

extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI(const TRUEHUD_API::InterfaceVersion a_interfaceVersion)
{
	auto api = Messaging::TrueHUDInterface::GetSingleton();

	logger::info("TrueHUD::RequestPluginAPI called, InterfaceVersion {}", a_interfaceVersion);

	switch (a_interfaceVersion) {
	case TRUEHUD_API::InterfaceVersion::V1:
		[[fallthrough]];
	case TRUEHUD_API::InterfaceVersion::V2:
		[[fallthrough]];
	case TRUEHUD_API::InterfaceVersion::V3:
		[[fallthrough]];
	case TRUEHUD_API::InterfaceVersion::V4:
		logger::info("TrueHUD::RequestPluginAPI returned the API singleton");
		return static_cast<void*>(api);
	}

	logger::info("TrueHUD::RequestPluginAPI requested the wrong interface version");
	return nullptr;
}

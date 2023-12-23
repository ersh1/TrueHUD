
#pragma once

/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace NND_API
{
	constexpr auto NNDPluginName = "NPCsNamesDistributor";

	// Available NND interface versions
	enum class InterfaceVersion : uint8_t
	{
		kV1
	};

	enum class NameContext : uint8_t
	{
		kCrosshair = 1,
		kCrosshairMinion,

		kSubtitles,
		kDialogue,

		kInventory,

		kBarter,

		kEnemyHUD,

		kOther
	};

	// NND's modder interface
	class IVNND1
	{
	public:
		virtual std::string_view GetName(RE::ActorHandle, NameContext) noexcept = 0;
		virtual std::string_view GetName(const RE::Actor*, NameContext) noexcept = 0;
		virtual void RevealName(RE::ActorHandle) noexcept = 0;
		virtual void RevealName(RE::Actor*) noexcept = 0;
	};

	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the NND API interface.
	/// Recommended: Send your request during or after SKSEMessagingInterface::kMessage_PostLoad to make sure the dll has already been loaded
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::kV1)
	{
		const auto pluginHandle = GetModuleHandle("NPCsNamesDistributor.dll");
		if (const _RequestPluginAPI requestAPIFunction = reinterpret_cast<_RequestPluginAPI>(GetProcAddress(pluginHandle, "RequestPluginAPI"))) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}

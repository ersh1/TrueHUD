
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

		/// <summary>
		/// Retrieves a generated name for given actor appropriate in specified context.
		/// Note that NND might not have a name for the actor. In this case an empty string will be returned.
		/// </summary>
		/// <param name="actor">Actor for which the name should be retrieved.</param>
		/// <param name="context">Context in which the name needs to be displayed. Depending on context name might either shortened or formatted differently.</param>
		/// <returns>A name generated for the actor. If actor does not support generated names an empty string will be returned instead.</returns>
		virtual std::string_view GetName(RE::ActorHandle actor, NameContext context) noexcept = 0;

		/// <summary>
		/// Retrieves a generated name for given actor appropriate in specified context.
		/// Note that NND might not have a name for the actor. In this case an empty string will be returned.
		/// </summary>
		/// <param name="actor">Actor for which the name should be retrieved.</param>
		/// <param name="context">Context in which the name needs to be displayed. Depending on context name might either shortened or formatted differently.</param>
		/// <returns>A name generated for the actor. If actor does not support generated names an empty string will be returned instead.</returns>
		virtual std::string_view GetName(const RE::Actor* actor, NameContext context) noexcept = 0;

		/// <summary>
		/// Reveals a real name of the given actor to the player. If player already knows actor's name this method does nothing.
		/// This method can be used to programatically introduce an actor to the player.
		/// </summary>
		/// <param name="actor">Actor whose name should be revealed.</param>
		virtual void RevealName(RE::ActorHandle actor) noexcept = 0;
		
		/// <summary>
		/// Reveals a real name of the given actor to the player. If player already knows actor's name this method does nothing.
		/// This method can be used to programatically introduce an actor to the player.
		/// </summary>
		/// <param name="actor">Actor whose name should be revealed.</param>
		virtual void RevealName(RE::Actor* actor) noexcept = 0;
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

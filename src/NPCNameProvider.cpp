#include "NPCNameProvider.h"
#include "NND_API.h"

const char* NPCNameProvider::GetName(RE::Actor* actor) const
{
	if (NND) {
		return NND->GetName(actor, NND_API::NameContext::kEnemyHUD).data();
	}

	return actor->GetDisplayFullName();
}

void NPCNameProvider::RequestAPI()
{
	if (!NND) {
		NND = static_cast<NND_API::IVNND1*>(NND_API::RequestPluginAPI(NND_API::InterfaceVersion::kV1));
		if (NND) {
			logger::info("Obtained NND API - {0:x}", reinterpret_cast<uintptr_t>(NND));
		} else {
			logger::warn("Failed to obtain NND API");
		}
	}
}

#include "Utils.h"
#include <numbers>

bool Utils::GetNodePosition(RE::ActorPtr a_actor, const char* a_nodeName, RE::NiPoint3& point)
{
	bool bResult = false;

	if (a_nodeName[0]) {
		RE::NiAVObject* object = a_actor->Get3D2();
		if (object) {
			object = object->GetObjectByName(a_nodeName);
			if (object) {
				point.x = object->world.translate.x;
				point.y = object->world.translate.y;
				point.z = object->world.translate.z;
				bResult = true;
			}
		}
	}

	return bResult;
}

bool Utils::GetTorsoPos(RE::ActorPtr a_actor, RE::NiPoint3& point)
{
	RE::TESRace* race = a_actor->race;
	if (!race) {
		return false;
	}

	RE::BGSBodyPartData* bodyPartData = race->bodyPartData;
	if (!bodyPartData) {
		return false;
	}

	// bodyPart->part[0] body
	// bodyPart->part[1] head
	RE::BGSBodyPart* bodyPart = bodyPartData->parts[0];
	if (!bodyPart) {
		return false;
	}

	return GetNodePosition(a_actor, bodyPart->targetName.c_str(), point);
}

bool Utils::GetTargetPos(RE::ObjectRefHandle a_target, RE::NiPoint3& pos, bool bGetTorsoPos /*= true*/)
{
	auto target = a_target.get();
	if (!target) {
		return false;
	}

	if (target->Get3D2() == nullptr) {
		return false;
	}

	if (target->formType == RE::FormType::ActorCharacter) {
		auto actorPtr = RE::ActorPtr(target->As<RE::Actor>());
		if (!bGetTorsoPos || !GetTorsoPos(actorPtr, pos)) {
			pos = target->GetLookingAtLocation();
		}
	} else {
		pos = target->GetPosition();
	}

	return true;
}

bool Utils::IsPlayerTeammateOrSummon(RE::Actor* a_actor)
{
	if (a_actor) {
		if (bool bIsTeammate = a_actor->IsPlayerTeammate()) {
			return true;
		}

		if (a_actor->IsCommandedActor()) {
			auto commandingActor = a_actor->GetCommandingActor();
			if (commandingActor && (commandingActor.native_handle() == 0x100000 || commandingActor.get()->IsPlayerTeammate())) {
				return true;
			}
		}
	}

	return false;
}

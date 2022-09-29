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
	RE::TESRace* race = a_actor->GetRace();
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
			if (commandingActor && (commandingActor->IsPlayerRef() || commandingActor.get()->IsPlayerTeammate())) {
				return true;
			}
		}
	}

	return false;
}

bool Utils::IsBehindPlayerCamera(const RE::NiPoint3& a_pos)
{
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	if (!playerCamera) {
		return true;
	}

	RE::NiPoint3 cameraPosition = playerCamera->pos;

	RE::NiQuaternion cameraRotation{ 1.f, 0.f, 0.f, 0.f };
	auto cameraState = playerCamera->currentState.get();
	if (cameraState) {
		cameraState->GetRotation(cameraRotation);
	}

	auto direction = a_pos - cameraPosition;
	direction.Unitize();

	auto cameraForwardVector = GetForwardVector(cameraRotation);
	cameraForwardVector.Unitize();

	// root_two is the diagonal length of a 1x1 square. When comparing normalized forward
	// vectors, this accepts an angle of 90 degrees in all directions
	auto angleDif = (direction - cameraForwardVector).Length();
	return angleDif > ROOT_TWO;
}

RE::NiQuaternion Utils::QuatFromRotationMatrix(const RE::NiMatrix3& a_matrix)
{
	RE::NiQuaternion ret;

	float trace = a_matrix.entry[0][0] + a_matrix.entry[1][1] + a_matrix.entry[2][2];
	if (trace > 0.f) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		ret.w = 0.25f / s;
		ret.x = (a_matrix.entry[2][1] - a_matrix.entry[1][2]) * s;
		ret.y = (a_matrix.entry[0][2] - a_matrix.entry[2][0]) * s;
		ret.z = (a_matrix.entry[1][0] - a_matrix.entry[0][1]) * s;
	} else {
		if (a_matrix.entry[0][0] > a_matrix.entry[1][1] && a_matrix.entry[0][0] > a_matrix.entry[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a_matrix.entry[0][0] - a_matrix.entry[1][1] - a_matrix.entry[2][2]);
			ret.w = (a_matrix.entry[2][1] - a_matrix.entry[1][2]) / s;
			ret.x = 0.25f * s;
			ret.y = (a_matrix.entry[0][1] + a_matrix.entry[1][0]) / s;
			ret.z = (a_matrix.entry[0][2] + a_matrix.entry[2][0]) / s;
		} else if (a_matrix.entry[1][1] > a_matrix.entry[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a_matrix.entry[1][1] - a_matrix.entry[0][0] - a_matrix.entry[2][2]);
			ret.w = (a_matrix.entry[0][2] - a_matrix.entry[2][0]) / s;
			ret.x = (a_matrix.entry[0][1] + a_matrix.entry[1][0]) / s;
			ret.y = 0.25f * s;
			ret.z = (a_matrix.entry[1][2] + a_matrix.entry[2][1]) / s;
		} else {
			float s = 2.0f * sqrtf(1.0f + a_matrix.entry[2][2] - a_matrix.entry[0][0] - a_matrix.entry[1][1]);
			ret.w = (a_matrix.entry[1][0] - a_matrix.entry[0][1]) / s;
			ret.x = (a_matrix.entry[0][2] + a_matrix.entry[2][0]) / s;
			ret.y = (a_matrix.entry[1][2] + a_matrix.entry[2][1]) / s;
			ret.z = 0.25f * s;
		}
	}
	return ret;
}

RE::NiMatrix3 Utils::QuaternionToMatrix(const RE::NiQuaternion& a_quat)
{
	float sqw = a_quat.w * a_quat.w;
	float sqx = a_quat.x * a_quat.x;
	float sqy = a_quat.y * a_quat.y;
	float sqz = a_quat.z * a_quat.z;

	RE::NiMatrix3 ret;

	// invs (inverse square length) is only required if quaternion is not already normalised
	float invs = 1.f / (sqx + sqy + sqz + sqw);
	ret.entry[0][0] = (sqx - sqy - sqz + sqw) * invs;  // since sqw + sqx + sqy + sqz =1/invs*invs
	ret.entry[1][1] = (-sqx + sqy - sqz + sqw) * invs;
	ret.entry[2][2] = (-sqx - sqy + sqz + sqw) * invs;

	float tmp1 = a_quat.x * a_quat.y;
	float tmp2 = a_quat.z * a_quat.w;
	ret.entry[1][0] = 2.f * (tmp1 + tmp2) * invs;
	ret.entry[0][1] = 2.f * (tmp1 - tmp2) * invs;

	tmp1 = a_quat.x * a_quat.z;
	tmp2 = a_quat.y * a_quat.w;
	ret.entry[2][0] = 2.f * (tmp1 - tmp2) * invs;
	ret.entry[0][2] = 2.f * (tmp1 + tmp2) * invs;
	tmp1 = a_quat.y * a_quat.z;
	tmp2 = a_quat.x * a_quat.w;
	ret.entry[2][1] = 2.f * (tmp1 + tmp2) * invs;
	ret.entry[1][2] = 2.f * (tmp1 - tmp2) * invs;

	return ret;
}

Utils::Matrix4::Matrix4(const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, const RE::NiPoint3& a_z, const RE::NiPoint3& a_w)
{
	entry[0][0] = a_x.x;
	entry[0][1] = a_x.y;
	entry[0][2] = a_x.z;
	entry[0][3] = 0.f;
	entry[1][0] = a_y.x;
	entry[1][1] = a_y.y;
	entry[1][2] = a_y.z;
	entry[1][3] = 0.f;
	entry[2][0] = a_z.x;
	entry[2][1] = a_z.y;
	entry[2][2] = a_z.z;
	entry[2][3] = 0.f;
	entry[3][0] = a_w.x;
	entry[3][1] = a_w.y;
	entry[3][2] = a_w.z;
	entry[3][3] = 1.f;
}

void Utils::Matrix4::SetAxis(const RE::NiPoint3& a_axis0, const RE::NiPoint3& a_axis1, const RE::NiPoint3& a_axis2, const RE::NiPoint3& a_origin)
{
	entry[0][0] = a_axis0.x;
	entry[0][1] = a_axis0.y;
	entry[0][2] = a_axis0.z;
	entry[1][0] = a_axis1.x;
	entry[1][1] = a_axis1.y;
	entry[1][2] = a_axis1.z;
	entry[2][0] = a_axis2.x;
	entry[2][1] = a_axis2.y;
	entry[2][2] = a_axis2.z;
	entry[3][0] = a_origin.x;
	entry[3][1] = a_origin.y;
	entry[3][2] = a_origin.z;
}

RE::NiPoint3 Utils::Matrix4::TransformVector4(const RE::NiPoint3& a_vector, float a_w) const
{
	RE::NiPoint3 ret;
	ret.x = a_vector.x * entry[0][0] + a_vector.y * entry[1][0] + a_vector.z * entry[2][0] + a_w * entry[3][0];
	ret.y = a_vector.x * entry[0][1] + a_vector.y * entry[1][1] + a_vector.z * entry[2][1] + a_w * entry[3][1];
	ret.z = a_vector.x * entry[0][2] + a_vector.y * entry[1][2] + a_vector.z * entry[2][2] + a_w * entry[3][2];
	/*ret.x = a_vector.x * entry[0][0] + a_vector.y * entry[0][1] + a_vector.z * entry[0][2] + a_w * entry[0][3];
	ret.y = a_vector.x * entry[1][0] + a_vector.y * entry[1][1] + a_vector.z * entry[1][2] + a_w * entry[1][3];
	ret.z = a_vector.x * entry[2][0] + a_vector.y * entry[2][1] + a_vector.z * entry[2][2] + a_w * entry[2][3];*/
	return ret;
}

RE::NiPoint3 Utils::Matrix4::TransformPosition(const RE::NiPoint3& a_position) const
{
	return TransformVector4(a_position, 1.f);
}

RE::NiPoint3 Utils::Matrix4::TransformVector(const RE::NiPoint3& a_vector) const
{
	return TransformVector4(a_vector, 0.f);
}

Utils::Matrix4 Utils::Matrix4::operator*(const Matrix4& a_other) const
{
	Matrix4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.entry[i][j] = 0.f;
			for (int k = 0; k < 4; k++) {
				result.entry[i][j] += entry[i][k] * a_other.entry[k][j];
			}
		}
	}
	return result;
}

Utils::MatrixScale::MatrixScale(float a_scale)
{
	entry[0][0] = a_scale;
	entry[0][1] = 0.f;
	entry[0][2] = 0.f;
	entry[0][3] = 0.f;
	entry[1][0] = 0.f;
	entry[1][1] = a_scale;
	entry[1][2] = 0.f;
	entry[1][3] = 0.f;
	entry[2][0] = 0.f;
	entry[2][1] = 0.f;
	entry[2][2] = a_scale;
	entry[2][3] = 0.f;
	entry[3][0] = 0.f;
	entry[3][1] = 0.f;
	entry[3][2] = 0.f;
	entry[3][3] = 1.f;
}

Utils::MatrixScale::MatrixScale(const RE::NiPoint3& a_scale)
{
	entry[0][0] = a_scale.x;
	entry[0][1] = 0.f;
	entry[0][2] = 0.f;
	entry[0][3] = 0.f;
	entry[1][0] = 0.f;
	entry[1][1] = a_scale.y;
	entry[1][2] = 0.f;
	entry[1][3] = 0.f;
	entry[2][0] = 0.f;
	entry[2][1] = 0.f;
	entry[2][2] = a_scale.z;
	entry[2][3] = 0.f;
	entry[3][0] = 0.f;
	entry[3][1] = 0.f;
	entry[3][2] = 0.f;
	entry[3][3] = 1.f;
}

Utils::MatrixQuatRotation::MatrixQuatRotation(const RE::NiQuaternion& a_quat)
{
	const float x2 = a_quat.x + a_quat.x;
	const float y2 = a_quat.y + a_quat.y;
	const float z2 = a_quat.z + a_quat.z;
	const float xx = a_quat.x * x2;
	const float xy = a_quat.x * y2;
	const float xz = a_quat.x * z2;
	const float yy = a_quat.y * y2;
	const float yz = a_quat.y * z2;
	const float zz = a_quat.z * z2;
	const float wx = a_quat.w * x2;
	const float wy = a_quat.w * y2;
	const float wz = a_quat.w * z2;

	entry[0][0] = 1.f - (yy + zz);
	entry[1][0] = xy - wz;
	entry[2][0] = xz + wy;
	entry[3][0] = 0.f;
	entry[0][1] = xy + wz;
	entry[1][1] = 1.f - (xx + zz);
	entry[2][1] = yz - wx;
	entry[3][1] = 0.f;
	entry[0][2] = xz - wy;
	entry[1][2] = yz + wx;
	entry[2][2] = 1.f - (xx + yy);
	entry[3][2] = 0.f;
	entry[0][3] = 0.f;
	entry[1][3] = 0.f;
	entry[2][3] = 0.f;
	entry[3][3] = 1.f;
}

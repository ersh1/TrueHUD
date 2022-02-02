#pragma once

#define PI 3.1415926535897932f

namespace Utils
{
	// get world coordinates of nodeName for actor
	bool GetNodePosition(RE::ActorPtr a_actor, const char* a_nodeName, RE::NiPoint3& point);

	// acquire actor's torso position
	bool GetTorsoPos(RE::ActorPtr a_actor, RE::NiPoint3& point);

	bool GetTargetPos(RE::ObjectRefHandle a_target, RE::NiPoint3& pos, bool bGetTorsoPos = true);

	// returns true if actor is player's teammate, summon, or teammate's summon
	bool IsPlayerTeammateOrSummon(RE::Actor* a_actor);

	inline bool FloatCompare(const float a, const float b)
	{
		double delta = fabs(a - b);
		if (delta < std::numeric_limits<float>::epsilon() &&
			delta > -std::numeric_limits<float>::epsilon()) {
			return true;
		}
		return false;
	}

	inline float GetPct(const float a_current, const float a_max)
	{
		float percent = -1.f;

		if (a_max < 0.f) {
			return percent;
		}

		if (!FloatCompare(a_max, 0.f)) {
			percent = a_current / a_max;
			return fmin(1.f, fmax(percent, -1.f));  // negative indicates that the actor value is not used
		}

		return percent;
	}

	inline float InterpEaseIn(const float& A, const float& B, float alpha, float exp)
	{
		float const modifiedAlpha = std::pow(alpha, exp);
		return std::lerp(A, B, modifiedAlpha);
	}

	inline float InterpEaseOut(const float& A, const float& B, float alpha, float exp)
	{
		float const modifiedAlpha = 1.f - pow(1.f - alpha, exp);
		return std::lerp(A, B, modifiedAlpha);
	}

	inline float InterpEaseInOut(const float& A, const float& B, float alpha, float exp)
	{
		return std::lerp(A, B, (alpha < 0.5f) ? InterpEaseIn(0.f, 1.f, alpha * 2.f, exp) * 0.5f : InterpEaseOut(0.f, 1.f, alpha * 2.f - 1.f, exp) * 0.5f + 0.5f);
	}

	inline RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis)
	{
		float S = sin(angle);
		float C = cos(angle);

		const float XX = axis.x * axis.x;
		const float YY = axis.y * axis.y;
		const float ZZ = axis.z * axis.z;

		const float XY = axis.x * axis.y;
		const float YZ = axis.y * axis.z;
		const float ZX = axis.z * axis.x;

		const float XS = axis.x * S;
		const float YS = axis.y * S;
		const float ZS = axis.z * S;

		const float OMC = 1.f - C;

		return RE::NiPoint3(
			(OMC * XX + C) * vec.x + (OMC * XY - ZS) * vec.y + (OMC * ZX + YS) * vec.z,
			(OMC * XY + ZS) * vec.x + (OMC * YY + C) * vec.y + (OMC * YZ - XS) * vec.z,
			(OMC * ZX - YS) * vec.x + (OMC * YZ + XS) * vec.y + (OMC * ZZ + C) * vec.z);
	}

}

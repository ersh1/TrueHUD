#pragma once

#define PI       3.1415926535897932f
#define ROOT_TWO 1.4142135623730950f

namespace Utils
{
	// get world coordinates of nodeName for actor
	bool GetNodePosition(RE::ActorPtr a_actor, const char* a_nodeName, RE::NiPoint3& point);

	// acquire actor's torso position
	bool GetTorsoPos(RE::ActorPtr a_actor, RE::NiPoint3& point);

	bool GetTargetPos(RE::ObjectRefHandle a_target, RE::NiPoint3& pos, bool bGetTorsoPos = true);

	// returns true if actor is player's teammate, summon, or teammate's summon
	bool IsPlayerTeammateOrSummon(RE::Actor* a_actor);

	bool IsBehindPlayerCamera(const RE::NiPoint3& a_pos);

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

	inline RE::NiPoint3 RotateVector(const RE::NiPoint3& a_vec, const RE::NiQuaternion& a_quat)
	{
		//http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		const RE::NiPoint3 Q{ a_quat.x, a_quat.y, a_quat.z };
		const RE::NiPoint3 T = Q.Cross(a_vec) * 2.f;
		return a_vec + (T * a_quat.w) + Q.Cross(T);
	}

	inline RE::NiPoint3 GetForwardVector(const RE::NiQuaternion& a_quat) {
		return RotateVector({ 0.f, 1.f, 0.f }, a_quat);
	}

	inline bool IsNormalized(const RE::NiPoint3& a_vector) {
		return fabs(1.f - a_vector.SqrLength()) < 0.01f;
	}

	inline void FindBestAxisVectors(const RE::NiPoint3& a_vector, RE::NiPoint3& a_outAxis1, RE::NiPoint3& a_outAxis2) {
		float x = fabs(a_vector.x);
		float y = fabs(a_vector.y);
		float z = fabs(a_vector.z);

		if (z > x && z > y) {
			a_outAxis1 = { 1.f, 0.f, 0.f };
		} else {
			a_outAxis1 = { 0.f, 0.f, 1.f };
		}

		a_outAxis1 = a_outAxis1 - a_vector * (a_outAxis1.Dot(a_vector));
		a_outAxis1.Unitize();
		a_outAxis2 = a_outAxis1.Cross(a_vector);
	}

	RE::NiQuaternion QuatFromRotationMatrix(const RE::NiMatrix3& a_matrix);

	class Matrix4
	{
	public:
		enum class Axis : uint8_t
		{
			kX,
			kY,
			kZ
		};

		Matrix4() noexcept = default;
		Matrix4(const RE::NiPoint3& a_x, const RE::NiPoint3& a_y, const RE::NiPoint3& a_z, const RE::NiPoint3& a_w);

		void SetAxis(const RE::NiPoint3& a_axis0, const RE::NiPoint3& a_axis1, const RE::NiPoint3& a_axis2, const RE::NiPoint3& a_origin);
		RE::NiPoint3 TransformVector4(const RE::NiPoint3& a_vector, float a_w) const;
		RE::NiPoint3 TransformPosition(const RE::NiPoint3& a_position) const;
		RE::NiPoint3 TransformVector(const RE::NiPoint3& a_vector) const;
		
		inline RE::NiPoint3 GetOrigin() const
		{
			return { entry[3][0], entry[3][1], entry[3][2] };
		}

		inline RE::NiPoint3 GetScaledAxis(Axis a_axis)
		{
			switch (a_axis) {
			case Axis::kX:
				return { entry[0][0], entry[0][1], entry[0][2] };
			case Axis::kY:
				return { entry[1][0], entry[1][1], entry[1][2] };
			case Axis::kZ:
				return { entry[2][0], entry[2][1], entry[2][2] };
			default:
				return { 0.f, 0.f, 0.f };
			}
		}

		Matrix4 operator*(const Matrix4& a_other) const;

		// members
		float entry[4][4];
	};

	class MatrixScale : public Matrix4
	{
	public:
		MatrixScale(float a_scale);
		MatrixScale(const RE::NiPoint3& a_scale);
	};

	class MatrixQuatRotation : public Matrix4
	{
	public:
		MatrixQuatRotation(const RE::NiQuaternion& a_quat);
	};
}

namespace Scaleform
{
	inline void RegisterString(RE::GFxValue* dst, const char* name, const char* str)
	{
		RE::GFxValue fxValue;
		fxValue.SetString(str);
		dst->SetMember(name, fxValue);
	}

	inline void RegisterNumber(RE::GFxValue* dst, const char* name, double value)
	{
		RE::GFxValue fxValue;
		fxValue.SetNumber(value);
		dst->SetMember(name, fxValue);
	}

	inline void RegisterBoolean(RE::GFxValue* dst, const char* name, bool value)
	{
		RE::GFxValue fxValue;
		fxValue.SetBoolean(value);
		dst->SetMember(name, fxValue);
	}

}

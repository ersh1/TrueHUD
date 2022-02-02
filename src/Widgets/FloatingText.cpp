#include "Widgets/FloatingText.h"
#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"

namespace Scaleform
{
	void FloatingText::Update(float a_deltaTime)
	{
		RE::GFxValue::DisplayInfo displayInfo;
		if (!_object.GetDisplayInfo(&displayInfo)) {
			return;
		}

		UpdatePosition(a_deltaTime);
	}

	void FloatingText::Initialize()
	{
		if (_bSpecial) {
			_duration *= 1.5f;
		}

		if (_bIs3D) {
			if (_bSpecial) {
				_gravityAcceleration.z = 0.f;
				_text.append("!");
			} else {
				RE::NiPoint3 upVector{ 0.f, 0.f, 1.f };
				//_currentVelocity = Utils::RandomVectorInConeInDegrees(upVector, 30.f) * 150.f;
			}
		} else {
			auto def = _view->GetMovieDef();

			_screenPosition.x *= def->GetWidth();
			_screenPosition.y *= def->GetHeight();
		}

		LoadConfig();
	}

	void FloatingText::Dispose()
	{
		_object.Invoke("dispose", nullptr, nullptr, 0);
	}

	void FloatingText::UpdatePosition(float a_deltaTime)
	{
		float scale = 100.f;

		if (_bIs3D) {

			RE::GRectF rect = _view->GetVisibleFrameRect();

			RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, _worldPosition, _screenPosition.x, _screenPosition.y, _depth, 1e-5f);

			bool bIsOffscreen = _depth < 0;

			RE::GFxValue visible;
			visible.SetBoolean(!bIsOffscreen);
			_object.SetMember("_visible", &bIsOffscreen);

			float fNear = *g_fNear;
			float fFar = *g_fFar;
			float linearizedDepth = fNear * fFar / (fFar + _depth * (fNear - fFar));

			_screenPosition.y = 1.0f - _screenPosition.y;  // Flip y for Flash coordinate system
			_screenPosition.y = rect.top + (rect.bottom - rect.top) * _screenPosition.y;
			_screenPosition.x = rect.left + (rect.right - rect.left) * _screenPosition.x;

			if (Settings::bFloatingTextScaleWithDistance) {
				float min = 500.f;
				float max = 2000.f;
				float scaleMult = fmin(fmax(linearizedDepth, min), max);
				float remapped = (((scaleMult - min) * (50.f - 100.f)) / (max - min)) + 100.f;
				scale = remapped;
			}

			_currentVelocity *= 1.f - (a_deltaTime * 1.f);  // damping
			_currentVelocity += _gravityAcceleration * a_deltaTime;
			_worldPosition += _currentVelocity * a_deltaTime;
		}

		scale *= Settings::fFloatingTextScale;

		RE::GFxValue::DisplayInfo displayInfo;
		displayInfo.SetPosition(_screenPosition.x, _screenPosition.y);
		displayInfo.SetScale(scale, scale);
		_object.SetDisplayInfo(displayInfo);
	}

	void FloatingText::LoadConfig()
	{
		RE::GFxValue args[4];
		args[0].SetString(_text);
		args[1].SetNumber(_color);
		args[2].SetNumber(_duration);
		args[3].SetBoolean(_bSpecial);
		_object.Invoke("loadConfig", nullptr, args, 4);
	}
}

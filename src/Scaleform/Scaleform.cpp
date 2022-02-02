#include "Scaleform/Scaleform.h"

#include "Scaleform/TrueHUDMenu.h"
#include "Offsets.h"

namespace Scaleform
{
	//class SKSEScaleform_ActorInfoBar_UpdatePosition : public RE::GFxFunctionHandler
	//{
	//public:
	//	void Call(Params& a_params) override
	//	{
	//		assert(a_params.args);
	//		assert(a_params.argCount);

	//		ActorInfoBar::UpdatePosition(a_params);
	//	}
	//};

	//typedef std::map<const std::type_info*, RE::GFxFunctionHandler*> FunctionHandlerCache;
	//static FunctionHandlerCache g_functionHandlerCache;

	//template <typename T>
	//void RegisterFunction(RE::GFxValue* dst, RE::GFxMovieView* movie, const char* name)
	//{
	//	// either allocate the object or retrieve an existing instance from the cache
	//	RE::GFxFunctionHandler* fn = nullptr;

	//	// check the cache
	//	FunctionHandlerCache::iterator iter = g_functionHandlerCache.find(&typeid(T));
	//	if (iter != g_functionHandlerCache.end())
	//		fn = iter->second;

	//	if (!fn) {
	//		// not found, allocate a new one
	//		fn = new T;

	//		// add it to the cache
	//		// cache now owns the object as far as refcounting goes
	//		g_functionHandlerCache[&typeid(T)] = fn;
	//	}

	//	// create the function object
	//	RE::GFxValue fnValue;
	//	movie->CreateFunction(&fnValue, fn);

	//	// register it
	//	dst->SetMember(name, fnValue);
	//}

	//auto RegisterScaleformFunctions(RE::GFxMovieView* a_view, RE::GFxValue* a_root) -> bool
	//{
	//	RegisterFunction<SKSEScaleform_ActorInfoBar_UpdatePosition>(a_root, a_view, "ActorInfoBar_UpdatePosition");

	//	return true;
	//}

	void Register()
	{
		//auto scaleform = SKSE::GetScaleformInterface();
		//scaleform->Register(RegisterScaleformFunctions, "TrueHUD");

		TrueHUDMenu::Register();

		logger::info("Registered all movies");
	}
}


#pragma once
#include "NND_API.h"

class NPCNameProvider
{
public:
	static NPCNameProvider* GetSingleton()
	{
		static NPCNameProvider singleton;
		return std::addressof(singleton);
	}

	const char* GetName(RE::Actor* actor) const;

	void RequestAPI();

private:
	NND_API::IVNND1* NND = nullptr;

	NPCNameProvider() = default;
	NPCNameProvider(const NPCNameProvider&) = delete;
	NPCNameProvider(NPCNameProvider&&) = delete;

	~NPCNameProvider() = default;

	NPCNameProvider& operator=(const NPCNameProvider&) = delete;
	NPCNameProvider& operator=(NPCNameProvider&&) = delete;
};

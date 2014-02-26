#include "lib_reflection/Init.h"

bool Reflection::Init()
{
	Reflection::ObjectMirror::Init();
	Reflection::MirrorFactory::Init();
	Reflection::EnumRegistry::Init();
    return true;
}

void Reflection::Shutdown()
{
}


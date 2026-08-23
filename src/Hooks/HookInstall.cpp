#include "Hooks/Hooks.h"

namespace Hooks
{
	void Install()
	{
		Offsets::Init();
		HookManager::Hook();
	}
}

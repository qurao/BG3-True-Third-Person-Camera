#include "Hooks/Hooks.h"
#include "Settings/PresetScanner.h"
#include "Settings/Settings.h"
#include "Utils/Utils.h"

unsigned int __stdcall InitThread(void* param)
{
	Settings::Main::GetSingleton()->WatchForChanges();

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
#ifndef NDEBUG
		while (!IsDebuggerPresent()) {
			Sleep(100);
		}
#endif

		dku::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));
		INFO("TrueThirdPersonCamera DLL_PROCESS_ATTACH starting...");

		try {
			INFO("process : {}", dku::Hook::GetProcessName());
			const auto processPath = dku::Hook::GetProcessPath();
			INFO("process path : {}", processPath);

			std::string productVersion;
			if (Utils::GetProductVersion(processPath, productVersion)) {
				INFO("process version : {}", productVersion);
			}

			const auto settings = Settings::Main::GetSingleton();
			settings->Load();
			INFO("Settings::Load() completed.");

			Settings::ScanCustomPresets();

			_beginthreadex(NULL, 0, InitThread, NULL, 0, NULL);

			Hooks::Install();
			INFO("Hooks::Install() completed successfully.");
		} catch (const std::exception& e) {
			WARN("Exception in DllMain: {}", e.what());
		} catch (...) {
			WARN("Unknown exception in DllMain!");
		}
	}

	return TRUE;
}

#include "Utils.h"

#include "Hooks/Hooks.h"

#pragma comment(lib, "version")

namespace Utils
{
	bool GetProductVersion(std::string_view processPath, std::string& outProductVersion)
	{
		const auto file = fmt::detail::utf8_to_utf16(processPath);
		DWORD dwHandle;
		const DWORD size = GetFileVersionInfoSizeW(file.c_str(), &dwHandle);
		if (size != 0) {
			std::vector<BYTE> versionInfoBuffer(size);

			if (GetFileVersionInfoW(file.c_str(), 0, size, versionInfoBuffer.data())) {
				LPVOID versionInfo;
				UINT versionInfoSize;
				if (VerQueryValueW(versionInfoBuffer.data(), L"\\StringFileInfo\\000004B0\\ProductVersion", &versionInfo, &versionInfoSize)) {
					const auto productVersion = fmt::detail::to_utf8<wchar_t>(static_cast<const wchar_t*>(versionInfo));
					outProductVersion = productVersion.c_str();

					return true;
				}
			}
		}

		return false;
	}

	int16_t GetPlayerID(RE::UnkObject* a1)
	{
		auto currentPlayer = Utils::GetCurrentPlayer(a1);
		if (!currentPlayer) {
			return 1;
		}

		return currentPlayer->playerId_38;
	}

	RE::Player* GetCurrentPlayer(RE::UnkObject* a1)
	{
		return a1->currentPlayer;
	}

	RE::CameraDefinition* GetCurrentCameraDefinition(RE::CameraModeFlags cameraModeFlags)
	{
		if (reinterpret_cast<bool>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::cameraBoolOffset) {
			return reinterpret_cast<RE::CameraDefinition*>(reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::unkCameraOffset);
		} else if ((cameraModeFlags & 1) == 0) {
			return reinterpret_cast<RE::CameraDefinition*>(reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::explorationCameraOffset);
		}

		return reinterpret_cast<RE::CameraDefinition*>(reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::combatCameraOffset);
	}
}

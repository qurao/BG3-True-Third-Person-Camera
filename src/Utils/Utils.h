#pragma once
#include "RE/Camera.h"

namespace Utils
{
	bool GetProductVersion(std::string_view processPath, std::string& outProductVersion);
	int16_t GetPlayerID(RE::UnkObject* a1);
	RE::Player* GetCurrentPlayer(RE::UnkObject* a1);
	RE::CameraDefinition* GetCurrentCameraDefinition(RE::CameraModeFlags cameraModeFlags);
}

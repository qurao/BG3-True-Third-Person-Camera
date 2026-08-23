#pragma once

#include "RE/Camera.h"
#include <array>

namespace Hooks::AnchorState
{
	struct PerPlayerAnchorState
	{
		bool hasLastLockedAnchor = false;
		RE::Vector3 lastLockedAnchor{};
		bool hasEasedAnchor = false;

		RE::Vector3 easedAnchor{};

		bool wasCursorActive = false;
		RE::Vector3 idlePinAnchor{};

		float floorClipEasedZoom = -1.f;

		float cursorActivationRamp = 0.f;
	};

	extern std::array<PerPlayerAnchorState, 2> perPlayer;

	PerPlayerAnchorState& Get(int16_t playerId);
}

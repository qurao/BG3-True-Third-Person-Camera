#include "Camera/CameraController.h"

#include "Hooks/Hooks.h"
#include "Settings/Settings.h"
#include <windows.h>

namespace
{
	constexpr int kZoomToggleControllerButton = 16384; // XInput X button
	constexpr int kHorizontalFlipControllerButton = 8192;
	constexpr float kZoomToggleHoldTime = 0.25f;
}

void CameraController::CheckAndHandleHoldToggle(int16_t playerId, RE::CameraObject* cameraObject)
{
	auto& pd = GetPlayerData(playerId);

	if (!cameraObject) {
		pd.holdTime = 0.f;
		pd.holdTriggered = false;
		pd.keyboardWasPressed = false;
		return;
	}

	const int vkCode = Settings::Main::GetSingleton()->ZoomToggleKeyboardKey;
	if (vkCode > 0 && vkCode != VK_NUMLOCK && vkCode != VK_CAPITAL && vkCode != VK_SCROLL) {
		const bool isKeyPressed = (GetAsyncKeyState(vkCode) & 0x8000) != 0;
		if (isKeyPressed && !pd.keyboardWasPressed) {
			ToggleCloseView(cameraObject);
			pd.keyboardWasPressed = true;
		} else if (!isKeyPressed) {
			pd.keyboardWasPressed = false;
		}
	}

	if (!Hooks::Offsets::isInControllerMode || !*Hooks::Offsets::isInControllerMode) {
		pd.holdTime = 0.f;
		pd.holdTriggered = false;
		return;
	}

	const bool isButtonPressed = IsControllerButtonPressed(playerId, kZoomToggleControllerButton);

	if (isButtonPressed) {
		pd.holdTime += deltaTime_;

		if (pd.holdTime >= kZoomToggleHoldTime && !pd.holdTriggered) {
			ToggleCloseView(cameraObject);
			pd.holdTriggered = true;
		}
	} else {
		pd.holdTime = 0.f;
		pd.holdTriggered = false;
	}
}

void CameraController::CheckAndHandleHorizontalFlipToggle(int16_t playerId, RE::CameraObject* cameraObject)
{
	auto& pd = GetPlayerData(playerId);

	if (!cameraObject) {
		pd.flipHoldTime = 0.f;
		pd.flipHoldTriggered = false;
		pd.flipKeyboardWasPressed = false;
		return;
	}

	const int vkCode = Settings::Main::GetSingleton()->ZoomToggleHorizontalFlipKey;
	if (vkCode > 0 && vkCode != VK_NUMLOCK && vkCode != VK_CAPITAL && vkCode != VK_SCROLL) {
		const bool isKeyPressed = (GetAsyncKeyState(vkCode) & 0x8000) != 0;
		if (isKeyPressed && !pd.flipKeyboardWasPressed) {
			pd.horizontalOffsetFlipped = !pd.horizontalOffsetFlipped;
			pd.flipKeyboardWasPressed = true;
		} else if (!isKeyPressed) {
			pd.flipKeyboardWasPressed = false;
		}
	}

	if (!Hooks::Offsets::isInControllerMode || !*Hooks::Offsets::isInControllerMode) {
		pd.flipHoldTime = 0.f;
		pd.flipHoldTriggered = false;
		return;
	}

	const bool isButtonPressed = IsControllerButtonPressed(playerId, kHorizontalFlipControllerButton);

	if (isButtonPressed) {
		pd.flipHoldTime += deltaTime_;

		if (pd.flipHoldTime >= kZoomToggleHoldTime && !pd.flipHoldTriggered) {
			pd.horizontalOffsetFlipped = !pd.horizontalOffsetFlipped;
			pd.flipHoldTriggered = true;
		}
	} else {
		pd.flipHoldTime = 0.f;
		pd.flipHoldTriggered = false;
	}
}

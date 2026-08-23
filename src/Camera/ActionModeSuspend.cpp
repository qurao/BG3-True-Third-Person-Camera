#include "Camera/CameraController.h"

#include "Camera/CharacterStateProbes.h"
#include "Hooks/Hooks.h"
#include "Settings/Settings.h"
#include <cmath>
#include <windows.h>

namespace
{
	constexpr float kZoomToggleActionZoomValue = 7.0f;
}

void CameraController::SetActionModeSuspend(bool suspend)
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId == 0) {
		return;
	}
	auto& pd = GetPlayerData(playerId);

	if (pd.actionModeSuspend != suspend) {
		pd.actionModeSuspend = suspend;
		const auto settings = Settings::Main::GetSingleton();

		if (settings && settings->ZoomToggleImmersiveMode) {
			return;
		}

		const bool inNativeCombatOrTB = currentCamera_ && IsCombatOrTurnBasedOrTacticalMode(currentCamera_);
		if (pd.closeViewMode && currentCamera_ && settings && !inNativeCombatOrTB) {
			if (suspend) {
				currentCamera_->desiredZoom = kZoomToggleActionZoomValue;
			} else {
				currentCamera_->desiredZoom = settings->ZoomToggleCloseValue;
			}
		}
	}
}

bool CameraController::ShouldReleasePositionLock() const
{
	if (GetPlayerData(CurrentPlayerId()).actionModeSuspend) {
		return true;
	}

	const auto settings = Settings::Main::GetSingleton();
	if (!settings->ZoomToggleImmersiveMode || !currentCamera_) {
		return false;
	}

	return ((currentCamera_->cameraModeFlags & RE::CameraModeFlags::kCombat) != 0) || settings->ZoomToggleFTBActive;
}

bool CameraController::IsCharacterGenuinelyTargeting(uintptr_t characterPtr) const
{
	if (!characterPtr)
		return false;
	__try {
		if (currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kTactical)) {
			return true;
		}

		uintptr_t inputController = *reinterpret_cast<uintptr_t*>(characterPtr + 0xA0);
		if (inputController < 0x10000000 || inputController > 0x7FFF00000000ULL || (inputController & 7)) {
			return false;
		}

		uintptr_t previewTask = *reinterpret_cast<uintptr_t*>(inputController + 0x58);
		bool targetingHit = CameraControllerDetail::CheckCharacterTargetingTask(previewTask);

		const bool rawCombat = currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kCombat) != 0;

		if (targetingHit && !rawCombat) {
			bool selectMode = false, inputActive = true, enemyTurn = false;
			targetingHit = FindClientSnapshotForPlayer(selectMode, inputActive, enemyTurn) && selectMode;
		}

		const bool rawCombatOrTB = rawCombat || Settings::Main::GetSingleton()->ZoomToggleFTBActive;

		const bool moveOrderHit = rawCombatOrTB && CameraControllerDetail::CheckCharacterMovementOrderTask(previewTask);
		const bool itemHoverHit = rawCombatOrTB && CameraControllerDetail::CheckCharacterItemHoverPreviewTask(previewTask);
		bool result = targetingHit || moveOrderHit || itemHoverHit;

		if (rawCombatOrTB && !result) {
			bool selectMode = false, inputActive = true, enemyTurn = false;
			FindClientSnapshotForPlayer(selectMode, inputActive, enemyTurn);
			result = inputActive;
		}

		return result;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return false;
}

void CameraController::GetControlledCharacterTaskTypesForDiagnostics(uint32_t& outPreviewType, uint32_t& outRunningType) const
{
	outPreviewType = 0xFFFFFFFFu;
	outRunningType = 0xFFFFFFFFu;
	__try {
		if (!currentPlayer_ || !Hooks::Offsets::UnkPlayerSingletonPtr || !*Hooks::Offsets::UnkPlayerSingletonPtr || !Hooks::Offsets::GetCharacter) {
			return;
		}
		uintptr_t playerMgr = reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkPlayerSingletonPtr);
		uintptr_t characterPtr = Hooks::Offsets::GetCharacter(playerMgr, currentPlayer_->playerId_38);
		if (!characterPtr)
			return;

		uintptr_t inputController = *reinterpret_cast<uintptr_t*>(characterPtr + 0xA0);
		if (inputController < 0x10000000 || inputController > 0x7FFF00000000ULL || (inputController & 7)) {
			return;
		}

		uintptr_t runningTask = *reinterpret_cast<uintptr_t*>(inputController + 0x50);
		uintptr_t previewTask = *reinterpret_cast<uintptr_t*>(inputController + 0x58);

		if (previewTask >= 0x10000000 && previewTask <= 0x7FFF00000000ULL && !(previewTask & 7)) {
			outPreviewType = *reinterpret_cast<uint32_t*>(previewTask + 0x80);
		}
		if (runningTask >= 0x10000000 && runningTask <= 0x7FFF00000000ULL && !(runningTask & 7)) {
			outRunningType = *reinterpret_cast<uint32_t*>(runningTask + 0x80);
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

bool CameraController::IsCharacterInActionMode(uintptr_t characterPtr, int16_t nativePlayerId) const
{
	if (!characterPtr)
		return false;
	__try {
		if (currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kTactical)) {
			return true;
		}

		uintptr_t inputController = *reinterpret_cast<uintptr_t*>(characterPtr + 0xA0);
		if (inputController < 0x10000000 || inputController > 0x7FFF00000000ULL || (inputController & 7)) {
			return false;
		}

		bool selectorMode = *reinterpret_cast<bool*>(inputController + 0x20);
		if (selectorMode) {
			return true;
		}

		uintptr_t runningTask = *reinterpret_cast<uintptr_t*>(inputController + 0x50);
		(void)runningTask;
		uintptr_t previewTask = *reinterpret_cast<uintptr_t*>(inputController + 0x58);

		bool foundActive = CameraControllerDetail::CheckCharacterTargetingTask(previewTask);

		if (foundActive) {
			const bool rawCombatForGate = currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kCombat) != 0;
			if (!rawCombatForGate) {
				bool selectMode = false, inputActive = true, enemyTurn = false;
				foundActive = FindClientSnapshotForPlayer(selectMode, inputActive, enemyTurn) && selectMode;
			}
		}

		if (!foundActive) {
			const bool rawCombat = currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kCombat) != 0;
			const bool fTB = Settings::Main::GetSingleton()->ZoomToggleFTBActive;
			if ((rawCombat || fTB) && CameraControllerDetail::CheckCharacterHasTask(previewTask)) {
				foundActive = true;
			}
		}

		bool camBool = false;
		if (Hooks::Offsets::UnkCameraSingletonPtr && *Hooks::Offsets::UnkCameraSingletonPtr) {
			uintptr_t camSingleton = reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr);
			camBool = *reinterpret_cast<bool*>(camSingleton + Hooks::Offsets::cameraBoolOffset);
		}

		if (camBool) {
			foundActive = true;
		}

		const int16_t playerId = CurrentPlayerId();
		auto& pd = GetPlayerData(playerId);
		bool isL3Pressed = false;
		if (Hooks::Offsets::isInControllerMode && *Hooks::Offsets::isInControllerMode) {
			isL3Pressed = IsControllerButtonPressed(playerId, 0x0040); // XINPUT_GAMEPAD_LEFT_THUMB (L3)
		}

		if (Hooks::Offsets::isInControllerMode && *Hooks::Offsets::isInControllerMode) {
			if (isL3Pressed && !pd.wasL3Pressed) {
				pd.l3HoldUsedForZoom = false;
			}
			if (isL3Pressed && currentCamera_ && std::abs(currentCamera_->zoomDelta) > 0.01f) {
				pd.l3HoldUsedForZoom = true;
			}
			if (!isL3Pressed && pd.wasL3Pressed && !pd.l3HoldUsedForZoom) {
				pd.l3SelectorActive = !pd.l3SelectorActive;
			}
			pd.wasL3Pressed = isL3Pressed;

			bool isBPressed = IsControllerButtonPressed(playerId, 0x2000); // XINPUT_GAMEPAD_B (Cancel)
			if (isBPressed && !pd.wasBPressed && pd.l3SelectorActive) {
				pd.l3SelectorActive = false;
			}
			pd.wasBPressed = isBPressed;
		} else {
			pd.l3SelectorActive = false;
			pd.wasBPressed = false;
		}

		if (pd.l3SelectorActive) {
			foundActive = true;
		}

		return foundActive;
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return false;
}

void CameraController::UpdateActionModeSuspend(RE::Player* player)
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId == 0) {
		return;
	}
	auto& pd = GetPlayerData(playerId);

	bool actionMode = false;
	bool genuinelyTargetingNow = false;
	if (player && Hooks::Offsets::UnkPlayerSingletonPtr && *Hooks::Offsets::UnkPlayerSingletonPtr && Hooks::Offsets::GetCharacter) {
		uintptr_t playerMgr = reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkPlayerSingletonPtr);
		uintptr_t character = Hooks::Offsets::GetCharacter(playerMgr, player->playerId_38);
		genuinelyTargetingNow = IsCharacterGenuinelyTargeting(character);
		actionMode = IsCharacterInActionMode(character, player->playerId_38);
	}

	const bool nativeCombat = currentCamera_ && (currentCamera_->cameraModeFlags & RE::CameraModeFlags::kCombat) != 0;
	bool fTBActive = false;
	{
		const auto ftbSettings = Settings::Main::GetSingleton();
		ReadLocker locker(ftbSettings->Lock);
		fTBActive = ftbSettings->ZoomToggleFTBActive;
	}
	const bool inCombatOrTB = nativeCombat || fTBActive;
	if (inCombatOrTB) {
		pd.capsLockFreelook = false;
	} else {
		const bool capsLockPhysicallyDown = (GetAsyncKeyState(VK_CAPITAL) & 0x8000) != 0;
		if (capsLockPhysicallyDown && !pd.capsLockWasPhysicallyDown) {
			pd.capsLockFreelook = !pd.capsLockFreelook;
		}
		pd.capsLockWasPhysicallyDown = capsLockPhysicallyDown;
	}
	if (pd.capsLockFreelook) {
		actionMode = true;
	}

	if (actionMode) {
		pd.selectorCooldown = 0.15f;
		pd.selectorModeActive = true;
	} else if (pd.selectorCooldown > 0.f) {
		pd.selectorCooldown -= deltaTime_;
		pd.selectorModeActive = true;
	} else {
		pd.selectorModeActive = false;
	}

	if (!player || !pd.closeViewMode) {
		SetActionModeSuspend(false);
		pd.genuinelyTargeting = false;
		return;
	}

	if (actionMode) {
		pd.suspendCooldown = 0.15f;
		SetActionModeSuspend(true);
	} else if (pd.suspendCooldown > 0.f) {
		pd.suspendCooldown -= deltaTime_;
		SetActionModeSuspend(true);
	} else {
		SetActionModeSuspend(false);
	}

	if (genuinelyTargetingNow) {
		pd.genuinelyTargetingCooldown = 0.5f;
		pd.genuinelyTargeting = true;
	} else if (pd.genuinelyTargetingCooldown > 0.f) {
		pd.genuinelyTargetingCooldown -= deltaTime_;
		pd.genuinelyTargeting = true;
	} else {
		pd.genuinelyTargeting = false;
	}
}

#include "Camera/CameraController.h"

#include "Settings/Settings.h"

bool CameraController::FindClientSnapshotForPlayer(bool& outSelectMode, bool& outInputControllerActive, bool& outEnemyTurnActive) const
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId < 1 || playerId > static_cast<int16_t>(playerData_.size())) {
		return false;
	}

	const auto nativeId = GetPlayerData(playerId).nativePlayerId;
	if (!nativeId.has_value()) {
		return false;
	}

	const auto settings = Settings::Main::GetSingleton();
	ReadLocker locker(settings->Lock);
	const Settings::Main::ClientCameraSnapshot* match = nullptr;
	for (const auto& snap : settings->ClientCameraSnapshots) {
		if (snap.valid && snap.userId == static_cast<int>(*nativeId)) {
			match = &snap;
			break;
		}
	}
	if (!match) {
		return false;
	}

	outSelectMode = match->selectMode;
	outInputControllerActive = match->inputControllerActive;
	outEnemyTurnActive = match->enemyTurnActive;

	return true;
}

int16_t CameraController::GetPlayerIdFromCameraObject(RE::CameraObject* cameraObject) const
{
	if (activeSlot_ != 0) {
		return activeSlot_;
	}

	if (!cameraObject) {
		return 0;
	}
	for (int i = 0; i < playerData_.size(); i++) {
		if (playerData_[i].cameraObject == cameraObject) {
			return i + 1;
		}
	}

	return 0;
}

void CameraController::SetCameraObjectForPlayer(int16_t playerId, RE::CameraObject* cameraObject)
{
	if (playerId <= 0 || playerId > static_cast<int16_t>(playerData_.size()) || !cameraObject) {
		return;
	}

	for (int i = 0; i < playerData_.size(); i++) {
		if (i != playerId - 1 && playerData_[i].cameraObject == cameraObject) {
			playerData_[i].cameraObject = nullptr;
		}
	}

	playerData_[playerId - 1].cameraObject = cameraObject;
}

int16_t CameraController::GetOrAssignPlayerSlot(int16_t nativePlayerId)
{
	for (int i = 0; i < playerData_.size(); i++) {
		if (playerData_[i].nativePlayerId == nativePlayerId) {
			return i + 1;
		}
	}
	for (int i = 0; i < playerData_.size(); i++) {
		if (!playerData_[i].nativePlayerId.has_value()) {
			playerData_[i].nativePlayerId = nativePlayerId;
			return i + 1;
		}
	}
	return 0;
}

void CameraController::NotifyInputDeviceForPlayer(int16_t playerId, uint16_t deviceId)
{
	if (playerId < 1 || playerId > static_cast<int16_t>(playerData_.size())) {
		return;
	}

	for (int i = 0; i < playerData_.size(); i++) {
		if (i != playerId - 1 && playerData_[i].inputDeviceId.has_value() && *playerData_[i].inputDeviceId == deviceId) {
			return;
		}
	}

	playerData_[playerId - 1].inputDeviceId = deviceId;
}

void CameraController::NotifyPadIndexForPlayer(int16_t playerId, uint8_t padIndex)
{
	if (playerId < 1 || playerId > static_cast<int16_t>(playerData_.size())) {
		return;
	}
	if (padIndex >= 4) {
		return;
	}

	playerData_[playerId - 1].inputPlayerIndex = padIndex;
}

int16_t CameraController::GetPadIndexForPlayer(int16_t playerId) const
{
	if (playerId < 1 || playerId > static_cast<int16_t>(playerData_.size())) {
		return 0;
	}

	const auto& pd = playerData_[playerId - 1];
	if (pd.inputPlayerIndex.has_value()) {
		return static_cast<int16_t>(*pd.inputPlayerIndex);
	}

	return playerId - 1;
}

CameraController::CameraMode CameraController::GetCurrentCameraMode(RE::CameraObject* cameraObject)
{
	if (cameraObject) {
		return GetCurrentCameraMode(cameraObject->cameraModeFlags);
	}

	return CameraMode::kExploration;
}

CameraController::CameraMode CameraController::GetCurrentCameraMode(RE::CameraModeFlags cameraModeFlags)
{
	if ((cameraModeFlags & RE::CameraModeFlags::kCombat) != 0 && (cameraModeFlags & RE::CameraModeFlags::kTactical) == 0) {
		const auto settings = Settings::Main::GetSingleton();
		if (settings->ZoomToggleImmersiveMode) {
			return CameraMode::kExploration;
		}
	}

	if (cameraModeFlags & RE::CameraModeFlags::kCombat) {
		if (cameraModeFlags & RE::CameraModeFlags::kTactical) {
			return CameraMode::kCombatTactical;
		}
		return CameraMode::kCombat;
	}

	if (cameraModeFlags & RE::CameraModeFlags::kTactical) {
		return CameraMode::kExplorationTactical;
	}

	return CameraMode::kExploration;
}

bool CameraController::IsCombatOrTurnBasedOrTacticalMode(RE::CameraObject* cameraObject) const
{
	if (!cameraObject)
		return false;
	const uint32_t flags = cameraObject->cameraModeFlags;

	if ((flags & RE::CameraModeFlags::kTactical) != 0) {
		return true;
	}

	const auto settings = Settings::Main::GetSingleton();
	ReadLocker locker(settings->Lock);

	if (settings->ZoomToggleImmersiveMode) {
		return false;
	}

	if ((flags & (RE::CameraModeFlags::kCombat | RE::CameraModeFlags::kUnk2)) != 0) {
		return true;
	}

	if (settings->ZoomToggleFTBActive) {
		return true;
	}

	return false;
}

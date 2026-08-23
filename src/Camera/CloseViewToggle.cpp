#include "Camera/CameraController.h"

#include "Settings/Settings.h"

namespace
{
	constexpr float kCloseViewCenterPitch = 8.f;
	constexpr float kFarViewOverheadPitch = 40.f;
}

bool CameraController::IsCloseViewMode() const
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId == 0) {
		return false;
	}
	const auto& pd = GetPlayerData(playerId);

	if (!pd.closeViewMode || pd.actionModeSuspend) {
		return false;
	}

	if (currentCamera_ && IsCombatOrTurnBasedOrTacticalMode(currentCamera_)) {
		return false;
	}

	return true;
}

bool CameraController::ShouldApplyCloseViewFraming() const
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId == 0) {
		return false;
	}
	const auto& pd = GetPlayerData(playerId);

	if (!pd.closeViewMode) {
		return false;
	}

	if (pd.actionModeSuspend) {
		const auto settings = Settings::Main::GetSingleton();
		ReadLocker locker(settings->Lock);
		if (!settings->ZoomToggleImmersiveMode) {
			return false;
		}
	}

	if (currentCamera_ && IsCombatOrTurnBasedOrTacticalMode(currentCamera_)) {
		return false;
	}

	return true;
}

bool CameraController::GetBaseCloseViewMode() const
{
	const int16_t playerId = CurrentPlayerId();
	if (playerId == 0) {
		return false;
	}
	const auto& pd = GetPlayerData(playerId);

	if (!pd.closeViewMode) {
		return false;
	}

	if (currentCamera_ && IsCombatOrTurnBasedOrTacticalMode(currentCamera_)) {
		return false;
	}

	return true;
}

bool CameraController::IsVanillaFarCameraActive(int16_t playerId) const
{
	if (playerId == 0) {
		return false;
	}
	const auto& pd = GetPlayerData(playerId);
	if (pd.closeViewMode) {
		return false;
	}

	const auto settings = Settings::Main::GetSingleton();
	ReadLocker locker(settings->Lock);
	return settings->VanillaFarCamera;
}

void CameraController::ToggleCloseView(RE::CameraObject* cameraObject)
{
	if (!cameraObject) {
		return;
	}

	const int16_t playerId = GetPlayerIdFromCameraObject(cameraObject);
	if (playerId == 0) {
		return;
	}
	auto& pd = GetPlayerData(playerId);

	if (!pd.closeViewMode && IsCombatOrTurnBasedOrTacticalMode(cameraObject)) {
		return;
	}

	const auto settings = Settings::Main::GetSingleton();
	{
		ReadLocker locker(settings->Lock);

		pd.closeViewMode = !pd.closeViewMode;
		Settings::Main::SaveCameraState(pd.closeViewMode, pd.closeViewMode && settings->ZoomToggleImmersiveMode, pd.prevCombatActionPhase);

		cameraObject->zoomDelta = 0.f;

		pd.closeViewTransitionTimer = 2.0f;

		if (pd.closeViewMode) {
			const float closeZoom = settings->ZoomToggleCloseValue;
			cameraObject->desiredZoom = closeZoom;
			cameraObject->currentZoom_160 = closeZoom;
		} else {
			const float farZoom = settings->ZoomToggleFarValue;
			cameraObject->desiredZoom = farZoom;
			cameraObject->currentZoom_160 = farZoom;
		}

		pd.pitch = cameraObject->currentPitch_164;

		pd.pitchTransitionTarget = pd.closeViewMode ? kCloseViewCenterPitch : kFarViewOverheadPitch;
		pd.pitchTransitionSpeed = 4.f;
		pd.pitchTransitionCancellable = true;
	}

	{
		WriteLocker writeLocker(settings->Lock);
		settings->changed = true;
	}

	ApplyZoomToggleCameraOverrides();
}

#include "Camera/CameraController.h"

#include "Hooks/Hooks.h"
#include "Settings/Settings.h"
#include <algorithm>
#include <cmath>

namespace
{
	constexpr bool kExplorationUnlockPitch = true;
	constexpr bool kCombatUnlockPitch = false;
	constexpr bool kExplorationKeepTacticalPitchLocked = false;
	constexpr bool kCombatKeepTacticalPitchLocked = false;
	constexpr bool kOverrideRightStickDeadzone = true;
	constexpr float kNewDeadzone = 0.15f;
	constexpr float kExplorationUnlockedPitchMin = -85.f;
	constexpr float kExplorationUnlockedPitchMax = 85.f;
	constexpr float kCombatUnlockedPitchMin = -85.f;
	constexpr float kCombatUnlockedPitchMax = 85.f;
	constexpr float kUnlockedPitchInitialValue = 25.f;
	constexpr float kUnlockedPitchClampSpeed = 5.f;
	constexpr float kUnlockedPitchFloorOffset = 0.1f;
}

bool CameraController::IsCameraUnlocked(int16_t playerId, RE::CameraObject* cameraObject) const
{
	if (GetPlayerData(playerId).pitch.has_value()) {
		return true;
	}

	return CanAdjustPitch(cameraObject);
}

bool CameraController::CanAdjustPitch(RE::CameraObject* cameraObject) const
{
	const auto cameraMode = GetCurrentCameraMode(cameraObject);

	return CanAdjustPitch(cameraMode);
}

bool CameraController::CanAdjustPitch(CameraController::CameraMode cameraMode) const
{
	switch (cameraMode) {
	case CameraMode::kExploration:
		return kExplorationUnlockPitch;
	case CameraMode::kCombat:
		return kCombatUnlockPitch;
	case CameraMode::kExplorationTactical:
		return kExplorationUnlockPitch && !kExplorationKeepTacticalPitchLocked;
	case CameraMode::kCombatTactical:
		return kCombatUnlockPitch && !kCombatKeepTacticalPitchLocked;
	}

	return false;
}

void CameraController::SetControllerPitchDelta(int16_t playerId, float inputValue)
{
	const auto settings = Settings::Main::GetSingleton();
	ReadLocker locker(settings->Lock);

	const float deadzone = GetDeadzone();
	const float normalizeDeadzone = kOverrideRightStickDeadzone ? 1.f / (1.f - kNewDeadzone) : NORMALIZE_DEADZONE;

	if (fabs(inputValue) <= deadzone) {
		GetPlayerData(playerId).controllerPitchDelta = 0.f;
	} else {
		float sign = inputValue < 0.f ? -1.f : 1.f;
		float value = sign * (fabs(inputValue) - deadzone) * normalizeDeadzone;

		value *= settings->InvertPitch ? -1.f : 1.f;
		GetPlayerData(playerId).controllerPitchDelta = value * settings->ControllerCameraRotationMult;
	}
}

bool CameraController::CalculateCameraPitch(int16_t playerId, RE::CameraObject* cameraObject, float& outPitch)
{
	const auto settings = Settings::Main::GetSingleton();

	const auto cameraMode = GetCurrentCameraMode(cameraObject);
	bool isPitchUnlocked = CanAdjustPitch(cameraMode);

	auto& playerData = GetPlayerData(playerId);

	ReadLocker locker(settings->Lock);

	if (isPitchUnlocked) {
		float pitchMin, pitchMax;
		switch (cameraMode) {
		case CameraMode::kExploration:
		case CameraMode::kExplorationTactical:
			pitchMin = kExplorationUnlockedPitchMin;
			pitchMax = kExplorationUnlockedPitchMax;
			break;
		case CameraMode::kCombat:
		case CameraMode::kCombatTactical:
			pitchMin = kCombatUnlockedPitchMin;
			pitchMax = kCombatUnlockedPitchMax;
			break;
		}

		int32_t deltaY = 0;
		if (cameraObject->cameraModeFlags & RE::CameraModeFlags::kMouseRotation) {
			const float sign = settings->InvertPitch ? -1.f : 1.f;
			deltaY = deltaY_ * sign;
			deltaY_ = 0;
		} else {
			deltaY_ = 0;
		}

		float pitchDelta = 0.f;
		pitchDelta += deltaY * settings->MousePitchMult * settings->MouseCameraRotationMult;
		pitchDelta += playerData.controllerPitchDelta * deltaTime_ * cameraObject->rotationSpeed * settings->ControllerPitchMult;

		if (playerData.pitchTransitionTarget.has_value() && pitchDelta != 0.f && playerData.pitchTransitionCancellable) {
			playerData.pitchTransitionTarget.reset();
		}

		if (!playerData.pitch.has_value()) {
			playerData.pitch = std::clamp(kUnlockedPitchInitialValue, pitchMin, pitchMax);
		} else if (playerData.pitchTransitionTarget.has_value()) {
			playerData.pitch = InterpTo(*playerData.pitch, *playerData.pitchTransitionTarget, deltaTime_, playerData.pitchTransitionSpeed);
			if (std::abs(*playerData.pitch - *playerData.pitchTransitionTarget) < 0.05f) {
				playerData.pitch = playerData.pitchTransitionTarget;
				playerData.pitchTransitionTarget.reset();
			}
		} else if (*playerData.pitch + pitchDelta > pitchMax) {
			playerData.pitch = std::min(*playerData.pitch, pitchMax);
		} else if (*playerData.pitch + pitchDelta < pitchMin) {
			playerData.pitch = std::max(*playerData.pitch, pitchMin);
		} else {
			playerData.pitch = *playerData.pitch + pitchDelta;
		}

		if (playerData.pitch > pitchMax) {
			playerData.pitch = InterpTo(*playerData.pitch, pitchMax, deltaTime_, kUnlockedPitchClampSpeed);
		} else if (playerData.pitch < pitchMin) {
			playerData.pitch = InterpTo(*playerData.pitch, pitchMin, deltaTime_, kUnlockedPitchClampSpeed);
		}

		outPitch = *playerData.pitch;

		return true;
	} else {
		if (playerData.pitch.has_value()) {
			if (playerData.pitch != outPitch) {
				playerData.pitch = InterpTo(*playerData.pitch, outPitch, deltaTime_, kUnlockedPitchClampSpeed);
			} else {
				playerData.pitch.reset();
			}
		}

		if (playerData.pitch.has_value()) {
			outPitch = *playerData.pitch;
		}
		return false;
	}
}

void CameraController::AdjustCameraZoomForPitch(uint64_t unkContext1, uint64_t unkContext2, RE::CameraObject* cameraObject)
{
	constexpr float minZoom = 0.5f;

	if (!cameraObject || cameraObject->currentZoomB <= minZoom) {
		return;
	}

	const float floorOffset = kUnlockedPitchFloorOffset;

	bool isUnderFloorLevel;

	const auto skipSteps = std::truncf((cameraObject->desiredZoom - cameraObject->currentZoomB) / ZOOM_ADJUST_STEP);
	float finalZoom = cameraObject->desiredZoom - (skipSteps * ZOOM_ADJUST_STEP);
	do {
		RE::Vector3 finalCameraPos;
		finalCameraPos.x = cameraObject->desiredCameraRootPos.x + cameraObject->cameraRotation.x * finalZoom;
		finalCameraPos.y = cameraObject->desiredCameraRootPos.y + cameraObject->cameraRotation.y * finalZoom;
		finalCameraPos.z = cameraObject->desiredCameraRootPos.z + cameraObject->cameraRotation.z * finalZoom;

		RE::FloorLevelStruct floorLevelStruct;

		bool unkFlag = false;
		RE::CameraDefinition* cameraDefinition = Hooks::Offsets::GetCurrentCameraDefinition(cameraObject);
		Hooks::Offsets::GetFloorLevel(floorLevelStruct, unkContext2, unkFlag, cameraDefinition, nullptr, finalCameraPos, *reinterpret_cast<uint64_t*>(unkContext1 + 0x118));

		if (floorLevelStruct.unk08) {
			isUnderFloorLevel = false;
		} else {
			isUnderFloorLevel = finalCameraPos.y < floorLevelStruct.floorLevel + floorOffset;

			if (isUnderFloorLevel) {
				finalZoom = std::max(finalZoom - ZOOM_ADJUST_STEP, minZoom);
				cameraObject->currentZoomB = finalZoom;
				cameraObject->currentZoomA = finalZoom;

				if (finalZoom <= minZoom) {
					return;
				}
			}
		}
	} while (isUnderFloorLevel);
}

float CameraController::AdjustInputValueForDeadzone(float inputValue, bool applyMult)
{
	const float deadzone = GetDeadzone();

	if (inputValue > deadzone) {
		float normalizedValue = (inputValue - deadzone) * (1.f / (1.f - deadzone));
		if (applyMult) {
			const auto settings = Settings::Main::GetSingleton();
			ReadLocker locker(settings->Lock);
			normalizedValue *= settings->ControllerCameraRotationMult;
		}
		return Denormalize(VANILLA_DEADZONE, 1.f, normalizedValue);
	}

	return 0.f;
}

float CameraController::GetDeadzone()
{
	return kOverrideRightStickDeadzone ? kNewDeadzone : VANILLA_DEADZONE;
}

float CameraController::NormalizeWithinRange(float min, float max, float value)
{
	return fminf(max - min, fmaxf(min, fminf(max, value)) - min) / (max - min);
}

float CameraController::Denormalize(float min, float max, float value)
{
	float normalizedRange = max - min;
	float scaledValue = value * normalizedRange;
	return scaledValue + min;
}

float CameraController::DegreesToRadians(float degrees)
{
	static constexpr float PI = 3.1415926535897932;
	static constexpr float DEG_TO_RADIAN = PI / 180.f;

	return degrees * DEG_TO_RADIAN;
}

float CameraController::InterpTo(float current, float target, float deltaTime, float interpSpeed)
{
	if (interpSpeed <= 0.f) {
		return target;
	}

	const float distance = target - current;

	if (std::abs(distance) < 1e-5f) {
		return target;
	}

	const float delta = distance * std::clamp(deltaTime * interpSpeed, 0.f, 1.f);

	return current + delta;
}

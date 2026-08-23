#include "Camera/CameraController.h"

#include "Hooks/Hooks.h"
#include "Settings/Settings.h"

namespace
{
	constexpr bool kExplorationOverrideLockedPitch = false;
	constexpr bool kExplorationUnlockPitch = true;
	constexpr bool kExplorationKeepTacticalPitchLocked = false;
	constexpr float kExplorationLockedPitchClose = 19.05f;
	constexpr float kExplorationLockedPitchFar = 40.71f;
	constexpr float kExplorationLockedTacticalPitchClose = 85.55f;
	constexpr float kExplorationLockedTacticalPitchFar = 85.55f;
	constexpr float kExplorationLockedAltPitchClose = 32.69f;
	constexpr float kExplorationLockedAltPitchFar = 39.7f;

	constexpr bool kExplorationOverrideZoom = true;
	constexpr float kExplorationZoomMin = 0.5f;
	constexpr float kExplorationZoomMax = 20.f;
	constexpr float kExplorationTacticalZoomMin = 10.f;
	constexpr float kExplorationTacticalZoomMax = 50.f;
	constexpr float kExplorationAltZoomMin = 10.f;
	constexpr float kExplorationAltZoomMax = 40.f;

	constexpr bool kExplorationOverrideFOV = false;
	constexpr float kExplorationFOVClose = 55.f;
	constexpr float kExplorationFOVFar = 55.f;
	constexpr float kExplorationTacticalFOV = 25.f;
	constexpr float kExplorationAltFOVClose = 45.f;
	constexpr float kExplorationAltFOVFar = 45.f;

	constexpr bool kExplorationOverrideOffset = false;
	constexpr float kExplorationHorizontalOffsetMult = 0.f;
	constexpr float kExplorationVerticalOffsetMult = 0.8f;

	constexpr bool kCombatOverrideLockedPitch = false;
	constexpr bool kCombatUnlockPitch = false;
	constexpr bool kCombatKeepTacticalPitchLocked = false;
	constexpr float kCombatLockedPitchClose = 32.73f;
	constexpr float kCombatLockedPitchFar = 52.42f;
	constexpr float kCombatLockedTacticalPitchClose = 85.55f;
	constexpr float kCombatLockedTacticalPitchFar = 85.55f;
	constexpr float kCombatLockedAltPitchClose = 32.69f;
	constexpr float kCombatLockedAltPitchFar = 39.7f;

	constexpr bool kCombatOverrideZoom = false;
	constexpr float kCombatZoomMin = 1.f;
	constexpr float kCombatZoomMax = 15.f;
	constexpr float kCombatTacticalZoomMin = 10.f;
	constexpr float kCombatTacticalZoomMax = 50.f;
	constexpr float kCombatAltZoomMin = 10.f;
	constexpr float kCombatAltZoomMax = 40.f;

	constexpr bool kCombatOverrideFOV = false;
	constexpr float kCombatFOVClose = 55.f;
	constexpr float kCombatFOVFar = 55.f;
	constexpr float kCombatTacticalFOV = 25.f;
	constexpr float kCombatAltFOVClose = 45.f;
	constexpr float kCombatAltFOVFar = 45.f;

	constexpr bool kCombatOverrideOffset = false;
	constexpr float kCombatHorizontalOffsetMult = 0.f;
	constexpr float kCombatVerticalOffsetMult = 0.8f;
}

void CameraController::SetCameraSettings()
{
	const auto settings = Settings::Main::GetSingleton();

	{
		ReadLocker locker(settings->Lock);

		if (!settings->changed) {
			return;
		}

		{
			RE::CameraDefinition* camera = reinterpret_cast<RE::CameraDefinition*>(reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::explorationCameraOffset);

			if (kExplorationOverrideLockedPitch) {
				if (!kExplorationUnlockPitch) {
					camera->pitchClose_164 = kExplorationLockedPitchClose;
					camera->pitchFar_160 = kExplorationLockedPitchFar;
					camera->tacticalPitchClose_174 = kExplorationLockedTacticalPitchClose;
					camera->tacticalPitchFar_170 = kExplorationLockedTacticalPitchFar;
					camera->pitchCloseAlt_17C = kExplorationLockedAltPitchClose;
					camera->pitchFarAlt_178 = kExplorationLockedAltPitchFar;
				} else if (kExplorationKeepTacticalPitchLocked) {
					camera->tacticalPitchClose_174 = kExplorationLockedTacticalPitchClose;
					camera->tacticalPitchFar_170 = kExplorationLockedTacticalPitchFar;
				}
			}

			if (kExplorationOverrideZoom) {
				camera->minZoom_2C = kExplorationZoomMin;
				camera->maxZoom_28 = kExplorationZoomMax;
				camera->tactMinZoom_C8 = kExplorationTacticalZoomMin;
				camera->tactMaxZoom_CC = kExplorationTacticalZoomMax;
				camera->altMinZoomController_34 = kExplorationAltZoomMin;
				camera->altMaxZoomController_30 = kExplorationAltZoomMax;
			}

			if (kExplorationOverrideFOV) {
				camera->fovClose_84 = kExplorationFOVClose;
				camera->fovFar_88 = kExplorationFOVFar;
				camera->tacticalFov_D0 = kExplorationTacticalFOV;
				camera->fovCloseAlt_8C = kExplorationAltFOVClose;
				camera->fovFarAlt_90 = kExplorationAltFOVFar;
			}

			if (kExplorationOverrideOffset) {
				camera->camVerticalOffsetMult_68 = kExplorationVerticalOffsetMult;
				camera->camHorizontalOffsetMult_64 = kExplorationHorizontalOffsetMult;
			}
		}

		{
			RE::CameraDefinition* camera = reinterpret_cast<RE::CameraDefinition*>(reinterpret_cast<uintptr_t>(*Hooks::Offsets::UnkCameraSingletonPtr) + Hooks::Offsets::combatCameraOffset);

			if (kCombatOverrideLockedPitch) {
				if (!kCombatUnlockPitch) {
					camera->pitchCombatClose_16C = kCombatLockedPitchClose;
					camera->pitchCombatFar_168 = kCombatLockedPitchFar;
					camera->tacticalPitchClose_174 = kCombatLockedTacticalPitchClose;
					camera->tacticalPitchFar_170 = kCombatLockedTacticalPitchFar;
					camera->pitchCloseAlt_17C = kCombatLockedAltPitchClose;
					camera->pitchFarAlt_178 = kCombatLockedAltPitchFar;
				} else if (kCombatKeepTacticalPitchLocked) {
					camera->tacticalPitchClose_174 = kCombatLockedTacticalPitchClose;
					camera->tacticalPitchFar_170 = kCombatLockedTacticalPitchFar;
				}
			}

			if (kCombatOverrideZoom || settings->ZoomToggleImmersiveMode) {
				camera->minZoom_2C = kCombatZoomMin;
				camera->maxZoom_28 = kCombatZoomMax;
				camera->tactMinZoom_C8 = kCombatTacticalZoomMin;
				camera->tactMaxZoom_CC = kCombatTacticalZoomMax;
				camera->altMinZoomController_34 = kCombatAltZoomMin;
				camera->altMaxZoomController_30 = kCombatAltZoomMax;
			}

			if (kCombatOverrideFOV || settings->ZoomToggleImmersiveMode) {
				camera->fovClose_84 = kCombatFOVClose;
				camera->fovFar_88 = kCombatFOVFar;
				camera->tacticalFov_D0 = kCombatTacticalFOV;
				camera->fovCloseAlt_8C = kCombatAltFOVClose;
				camera->fovFarAlt_90 = kCombatAltFOVFar;
			}

			if (kCombatOverrideOffset) {
				camera->camVerticalOffsetMult_68 = kCombatVerticalOffsetMult;
				camera->camHorizontalOffsetMult_64 = kCombatHorizontalOffsetMult;
			}
		}
	}

	WriteLocker locker(settings->Lock);
	settings->changed = false;
}

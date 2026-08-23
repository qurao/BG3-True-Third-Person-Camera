#pragma once

namespace CameraControllerDetail
{
	bool CheckEclSneaking(uintptr_t characterPtr);
	float GetCharacterHeightRatio(uintptr_t characterPtr, float* outRawHeight = nullptr);
	float RawHeightToRatio(float height, float baseline);
	bool CheckCharacterTargetingTask(uintptr_t taskPtr);
	bool CheckCharacterMovementOrderTask(uintptr_t taskPtr);
	bool CheckCharacterItemHoverPreviewTask(uintptr_t taskPtr);
	bool CheckCharacterHasTask(uintptr_t taskPtr);
}

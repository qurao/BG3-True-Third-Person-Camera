#include "Camera/CharacterStateProbes.h"

namespace CameraControllerDetail
{
	namespace
	{
		bool CheckStatusArrayAtOffset(uintptr_t statusMgr, uintptr_t arrOffset)
		{
			__try {
				struct RawArr
				{
					uintptr_t* buf;
					uint32_t cap;
					uint32_t sz;
				};
				RawArr* arr = reinterpret_cast<RawArr*>(statusMgr + arrOffset);
				if (!arr->buf || arr->sz == 0 || arr->sz > 64 || arr->cap < arr->sz)
					return false;
				if (reinterpret_cast<uintptr_t>(arr->buf) < 0x10000000)
					return false;

				for (uint32_t i = 0; i < arr->sz && i < 32; ++i) {
					uintptr_t obj = arr->buf[i];
					if (obj < 0x10000000 || obj > 0x7FFF00000000ULL)
						continue;

					uintptr_t* vtbl = *reinterpret_cast<uintptr_t**>(obj);
					if (!vtbl || reinterpret_cast<uintptr_t>(vtbl) < 0x10000000)
						continue;

					typedef uint32_t (*tGetStatusId)(uintptr_t);
					uint32_t id = reinterpret_cast<tGetStatusId>(vtbl[1])(obj);
					if (id == 8) { // StatusType::SNEAKING = 8
						return true;
					}
				}
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				return false;
			}

			return false;
		}

		bool RawCheckEclSneaking(uintptr_t characterPtr)
		{
			static const uintptr_t kStatusMgrOffset = 0xB8;
			static const uintptr_t kActionStatusesOffset = 0x1E0; // Individual Hide
			static const uintptr_t kStatusesOffset = 0x268; // Group Hide
			static const uintptr_t kExternalStatusesOffset = 0x278; // External status

			uintptr_t statusMgr = 0;
			__try {
				statusMgr = *reinterpret_cast<uintptr_t*>(characterPtr + kStatusMgrOffset);
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				return false;
			}
			if (statusMgr < 0x10000000 || statusMgr > 0x7FFF00000000ULL || (statusMgr & 7))
				return false;

			if (CheckStatusArrayAtOffset(statusMgr, kActionStatusesOffset))
				return true;
			if (CheckStatusArrayAtOffset(statusMgr, kStatusesOffset))
				return true;
			if (CheckStatusArrayAtOffset(statusMgr, kExternalStatusesOffset))
				return true;

			return false;
		}
	}

	bool CheckEclSneaking(uintptr_t characterPtr)
	{
		if (!characterPtr)
			return false;
		return RawCheckEclSneaking(characterPtr);
	}

	float RawHeightToRatio(float height, float baseline)
	{
		if (baseline < 0.5f || baseline > 4.0f) {
			baseline = 2.09f;
		}
		if (height > 0.5f && height < 4.0f) {
			return height / baseline;
		} else if (height > 50.0f && height < 400.0f) {
			return (height / 100.0f) / baseline;
		}
		return 1.0f;
	}

	float GetCharacterHeightRatio(uintptr_t characterPtr, float* outRawHeight)
	{
		if (!characterPtr)
			return 1.0f;
		float ratio = 1.0f;
		__try {
			uintptr_t clothVisual = *reinterpret_cast<uintptr_t*>(characterPtr + 0xD0);
			if (clothVisual && clothVisual >= 0x10000000 && clothVisual <= 0x7FFF00000000ULL) {
				float scaleY = *reinterpret_cast<float*>(clothVisual + 0x28); // Transform.Scale.Y
				float minY = *reinterpret_cast<float*>(clothVisual + 0x5C); // BaseBound.Min.Y
				float maxY = *reinterpret_cast<float*>(clothVisual + 0x68); // BaseBound.Max.Y
				float height = (maxY - minY) * (scaleY > 0.01f ? scaleY : 1.0f);
				if (outRawHeight) {
					*outRawHeight = height;
				}
				ratio = RawHeightToRatio(height, 2.09f);
			}
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			ratio = 1.0f;
		}
		return ratio;
	}

	bool CheckCharacterTargetingTask(uintptr_t taskPtr)
	{
		if (taskPtr < 0x10000000 || taskPtr > 0x7FFF00000000ULL || (taskPtr & 7))
			return false;
		__try {
			uint32_t taskInfoType = *reinterpret_cast<uint32_t*>(taskPtr + 0x4C);

			uint32_t taskType = *reinterpret_cast<uint32_t*>(taskPtr + 0x80);

			return taskType == 4 || taskType == 5 || taskType == 6 ||
			       taskInfoType == 4 || taskInfoType == 5 || taskInfoType == 6;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		}
		return false;
	}

	bool CheckCharacterMovementOrderTask(uintptr_t taskPtr)
	{
		if (taskPtr < 0x10000000 || taskPtr > 0x7FFF00000000ULL || (taskPtr & 7))
			return false;
		__try {
			uint32_t taskInfoType = *reinterpret_cast<uint32_t*>(taskPtr + 0x4C);
			uint32_t taskType = *reinterpret_cast<uint32_t*>(taskPtr + 0x80);
			return taskType == 0 || taskType == 3 || taskInfoType == 0 || taskInfoType == 3;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		}
		return false;
	}

	bool CheckCharacterItemHoverPreviewTask(uintptr_t taskPtr)
	{
		if (taskPtr < 0x10000000 || taskPtr > 0x7FFF00000000ULL || (taskPtr & 7))
			return false;
		__try {
			uint32_t taskInfoType = *reinterpret_cast<uint32_t*>(taskPtr + 0x4C);
			uint32_t taskType = *reinterpret_cast<uint32_t*>(taskPtr + 0x80);
			return taskType == 7 || taskType == 20 || taskInfoType == 7 || taskInfoType == 20;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		}
		return false;
	}

	bool CheckCharacterHasTask(uintptr_t taskPtr)
	{
		if (taskPtr < 0x10000000 || taskPtr > 0x7FFF00000000ULL || (taskPtr & 7))
			return false;
		__try {
			volatile uint32_t probe = *reinterpret_cast<uint32_t*>(taskPtr + 0x80);
			(void)probe;
			return true;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		}
		return false;
	}
}

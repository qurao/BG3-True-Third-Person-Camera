#pragma once
#include "RE/Camera.h"

namespace Hooks
{
	using namespace DKUtil::Alias;

	class Offsets
	{
	public:
		static bool Init()
		{
			bool success = true;

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"48 8B 05 ?? ?? ?? ?? 80 B8 32 13 00 00 00 74 07">());
				if (scan) {
					auto singletonOffset = *reinterpret_cast<int32_t*>(scan + 3);
					UnkCameraSingletonPtr = reinterpret_cast<void**>(scan + 7 + singletonOffset);
					GetCurrentCameraDefinition = reinterpret_cast<tGetCurrentCameraDefinition>(scan);
					INFO("GetCurrentCameraDefinition found: {:X}", AsAddress(GetCurrentCameraDefinition) - dku::Hook::Module::get().base())
				} else {
					ERROR("GetCurrentCameraDefinition not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"48 8B 0D ?? ?? ?? ?? 0F B7 D0 E8 ?? ?? ?? ?? 84 C0 75 1B">());
				if (scan) {
					auto unkSingletonOffset = *reinterpret_cast<int32_t*>(scan + 3);
					auto funcOffset = *reinterpret_cast<int32_t*>(scan + 0xB);
					UnkSingletonPtr = reinterpret_cast<void**>(scan + 7 + unkSingletonOffset);
					ShouldShowSneakCones = reinterpret_cast<tShouldShowSneakCones>(scan + 0xF + funcOffset);

					auto inputSingletonOffset = *reinterpret_cast<int32_t*>(AsAddress(ShouldShowSneakCones) + 0x82 + 3);
					UnkInputSingletonPtr = reinterpret_cast<void**>(AsAddress(ShouldShowSneakCones) + 0x82 + 7 + inputSingletonOffset);

					auto getInputValueCallsite = AsAddress(ShouldShowSneakCones) + 0x9B;
					auto getInputValueOffset = *reinterpret_cast<int32_t*>(getInputValueCallsite + 1);
					GetInputValue = reinterpret_cast<tGetInputValue>(getInputValueCallsite + 5 + getInputValueOffset);

					INFO("Input related functions found: {:X}", AsAddress(ShouldShowSneakCones) - dku::Hook::Module::get().base())
				} else {
					ERROR("Input related functions not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"48 8B 0D ?? ?? ?? ?? 0F B7 D7 E8 ?? ?? ?? ?? 3C FF">());
				if (scan) {
					auto playerSingletonOffset = *reinterpret_cast<int32_t*>(scan + 3);
					UnkPlayerSingletonPtr = reinterpret_cast<void**>(scan + 7 + playerSingletonOffset);

					auto getPlayerControllerCallsite = scan + 0xA;
					auto getPlayerControllerOffset = *reinterpret_cast<int32_t*>(getPlayerControllerCallsite + 1);
					GetPlayerController = reinterpret_cast<tGetPlayerController>(getPlayerControllerCallsite + 5 + getPlayerControllerOffset);

					INFO("Player controller related functions found: {:X}", AsAddress(GetPlayerController) - dku::Hook::Module::get().base())
				} else {
					ERROR("Player controller functions not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"80 3D ?? ?? ?? ?? ?? 74 22">());
				if (scan) {
					auto offset = *reinterpret_cast<int32_t*>(scan + 2);
					isInControllerMode = reinterpret_cast<bool*>(scan + 7 + offset);
					INFO("isInControllerMode found: {:X}", AsAddress(isInControllerMode) - dku::Hook::Module::get().base())
				} else {
					ERROR("isInControllerMode not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 41 0F 28 45 40">());
				if (scan) {
					auto offset = *reinterpret_cast<int32_t*>(scan + 1);
					GetCharacter = reinterpret_cast<tGetCharacter>(scan + 5 + offset);
					INFO("GetCharacter found: {:X}", AsAddress(GetCharacter) - dku::Hook::Module::get().base())
				} else {
					ERROR("GetCharacter not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"48 89 5C 24 08 57 48 83 EC 30 48 8B 41 10 48 8B D9">());
				if (scan) {
					GetCharacterHeight = reinterpret_cast<tGetCharacterHeight>(scan);
					INFO("GetCharacterHeight found: {:X}", AsAddress(GetCharacterHeight) - dku::Hook::Module::get().base())
				} else {
					ERROR("GetCharacterHeight not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? EB 11 33 C9">());
				if (scan) {
					auto offset = *reinterpret_cast<int32_t*>(scan + 1);
					GetFloorLevel = reinterpret_cast<tGetFloorLevel>(scan + 5 + offset);
					INFO("GetFloorLevel found: {:X}", AsAddress(GetFloorLevel) - dku::Hook::Module::get().base())
				} else {
					ERROR("GetFloorLevel not found!")
					success = false;
				}
			}

			{
				auto scan = static_cast<uint8_t*>(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 0F 28 DA F3 0F 5C E5">());
				if (scan) {
					auto offset = *reinterpret_cast<int32_t*>(scan + 1);
					GetCameraMinZoom = reinterpret_cast<tGetCameraMinZoom>(scan + 5 + offset);
					INFO("GetCameraMinZoom found: {:X}", AsAddress(GetCameraMinZoom) - dku::Hook::Module::get().base())
				} else {
					ERROR("GetCameraMinZoom not found!")
					success = false;
				}
			}

			return success;
		}

		enum class InputID : int32_t
		{
			kZoomIn = 104,
			kZoomOut = 105,
			kRotateLeft = 107,
			kRotateRight = 108,
			kMouseRotateLeft = 109,
			kMouseRotateRight = 110,
			kToggleInputMode = 0xC0
		};

		constexpr static inline uint32_t cameraBoolOffset = 0x1332;
		constexpr static inline uint32_t unkCameraOffset = 0xC58;
		constexpr static inline uint32_t explorationCameraOffset = 0x79C;
		constexpr static inline uint32_t combatCameraOffset = 0x930;

		using tGetCurrentCameraDefinition = RE::CameraDefinition* (*)(RE::CameraObject* a1);
		using tShouldShowSneakCones = bool (*)(void* a1, int16_t playerId);
		using tGetCharacter = uintptr_t (*)(uintptr_t a1, int16_t playerId);
		using tGetCharacterHeight = float (*)(uintptr_t character);
		using tGetPlayerController = void* (*)(void* a1, int16_t playerId);
		using tGetInputValue = RE::InputValue* (*)(void* a1, RE::InputValue& outValue, InputID& inputId, void* a4);
		using tGetCurrentPlayerInternal = RE::Player* (*)(uint64_t a1, uint64_t a2);
		using tGetFloorLevel = RE::FloorLevelStruct* (*)(RE::FloorLevelStruct& outFloorLevelStruct, uint64_t a2, bool a3, RE::CameraDefinition* cameraDefinition, void* a5, RE::Vector3& cameraPos, uint64_t a7);
		using tGetCameraMinZoom = float (*)(RE::CameraModeFlags flags, bool a2);

		static inline tGetCurrentCameraDefinition GetCurrentCameraDefinition;
		static inline tShouldShowSneakCones ShouldShowSneakCones;
		static inline tGetCharacter GetCharacter;
		static inline tGetCharacterHeight GetCharacterHeight;
		static inline tGetPlayerController GetPlayerController;
		static inline tGetInputValue GetInputValue;
		static inline tGetCurrentPlayerInternal GetCurrentPlayerInternal;
		static inline tGetCameraMinZoom GetCameraMinZoom;
		static inline tGetFloorLevel GetFloorLevel;

		static inline void** UnkSingletonPtr = nullptr;
		static inline void** UnkCameraSingletonPtr = nullptr;
		static inline void** UnkPlayerSingletonPtr = nullptr;
		static inline void** UnkInputSingletonPtr = nullptr;
		static inline bool* isInControllerMode = nullptr;
	};

	class HookManager
	{
	public:
		static bool Hook()
		{
			bool success = true;

			dku::Hook::Trampoline::AllocTrampoline(1 << 7);

			const auto UpdateCameraCallAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 48 8D 8D F8 04 00 00 E8 ?? ?? ?? ?? E9 AF FD FF FF">());
			if (UpdateCameraCallAddress) {
				updateCamera_ = dku::Hook::write_call<5>(UpdateCameraCallAddress, Hook_UpdateCamera);
				INFO("Hooked UpdateCamera: {:X}", AsAddress(UpdateCameraCallAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("UpdateCamera not found!")
				success = false;
			}

			const auto HandleCameraInputAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 0F B7 08 66 89 0B 80 3B 00">());
			if (HandleCameraInputAddress) {
				handleCameraInput_ = dku::Hook::write_call<5>(HandleCameraInputAddress, Hook_HandleCameraInput);
				INFO("Hooked HandleCameraInput: {:X}", AsAddress(HandleCameraInputAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("HandleCameraInput not found!")
				success = false;
			}

			const auto CalculateCameraPitchAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 80 BF 4C 01 00 00 00">());
			if (CalculateCameraPitchAddress) {
				calculateCameraPitch_ = dku::Hook::write_call<5>(CalculateCameraPitchAddress, Hook_CalculateCameraPitch);
				INFO("Hooked CalculateCameraPitch: {:X}", AsAddress(CalculateCameraPitchAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("CalculateCameraPitch not found!")
				success = false;
			}

			const auto UpdateCameraPitchAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 48 8B 46 70 4C 8D 45 90 0F 28 46 30">());
			if (UpdateCameraPitchAddress) {
				updateCameraPitch_ = dku::Hook::write_call<5>(UpdateCameraPitchAddress, Hook_UpdateCameraPitch);
				INFO("Hooked UpdateCameraPitch: {:X}", AsAddress(UpdateCameraPitchAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("UpdateCameraPitch not found!")
				success = false;
			}

			const auto AfterUpdateCameraZoomAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 80 BF 54 02 00 00 00">());
			if (AfterUpdateCameraZoomAddress) {
				afterUpdateCameraZoom_ = dku::Hook::write_call<5>(AfterUpdateCameraZoomAddress, Hook_AfterUpdateCameraZoom);
				INFO("Hooked AfterUpdateCameraZoom: {:X}", AsAddress(AfterUpdateCameraZoomAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("AfterUpdateCameraZoom not found!")
				success = false;
			}

			const auto HandleToggleInputModeCallAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 0F B7 00 84 C0">());
			if (HandleToggleInputModeCallAddress) {
				handleToggleInputMode_ = dku::Hook::write_call<5>(HandleToggleInputModeCallAddress, Hook_HandleToggleInputMode);
				INFO("Hooked HandleToggleInputModeCall: {:X}", AsAddress(HandleToggleInputModeCallAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("HandleToggleInputModeCall not found!")
				success = false;
			}

			const auto SetDefaultZoomCallAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 48 8B CF E8 ?? ?? ?? ?? E9 81 02 00 00">());
			if (SetDefaultZoomCallAddress) {
				setDefaultZoom_ = dku::Hook::write_call<5>(SetDefaultZoomCallAddress, Hook_SetDefaultZoom);
				INFO("Hooked SetDefaultZoom: {:X}", AsAddress(SetDefaultZoomCallAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("SetDefaultZoom not found!")
				success = false;
			}

			const auto SDLMouseYHookAddress = AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? ?? 48 8B 8D F8 00 00 00 48 33 CC E8 ?? ?? ?? ?? 4C 8D 9C 24 40 02 00 00">());
			if (SDLMouseYHookAddress) {
				struct Stub : Xbyak::CodeGenerator
				{
					Stub()
					{
						mov(r9d, r12d);
						mov(rax, (uintptr_t)&Hook_SDLMouseYHook);
						jmp(rax);
					}
				};
				static Stub stub;

				sdlMouseYHook_ = dku::Hook::write_call<5>(SDLMouseYHookAddress, (bool (*)(uint64_t, uint64_t, bool, int))stub.getCode());

				INFO("Hooked SDLMouseYHook: {:X}", AsAddress(SDLMouseYHookAddress) - dku::Hook::Module::get().base())
			} else {
				ERROR("SDLMouseYHook not found!")
				success = false;
			}

			return success;
		}

	private:
		static void Hook_UpdateCamera(uint64_t unused1, uint64_t unused2, uint64_t unused3, RE::UnkObject* cameraContext);
		static void* Hook_HandleCameraInput(uint64_t unused1, uint64_t unused2, RE::UnkObject* cameraContext, uintptr_t inputEvent);
		static float Hook_CalculateCameraPitch(RE::CameraObject* cameraObject, uint8_t unused1, uint8_t unused2);
		static void Hook_UpdateCameraPitch(uint64_t unused1, uint64_t unused2, RE::CameraObject* cameraObject, uint64_t frameContext);
		static void Hook_AfterUpdateCameraZoom(uint64_t unkContext1, uint64_t unkContext2, RE::UnkObject* cameraContext, uint64_t unused4);
		static int16_t* Hook_HandleToggleInputMode(uint64_t unkContext, int16_t& outResult, Offsets::InputID* inputId);
		static void Hook_SetDefaultZoom(RE::CameraObject* cameraObject);
		static bool Hook_SDLMouseYHook(uint64_t unused1, uint64_t unused2, bool unused3, int deltaY);

		static inline std::add_pointer_t<decltype(Hook_UpdateCamera)> updateCamera_;
		static inline std::add_pointer_t<decltype(Hook_HandleCameraInput)> handleCameraInput_;
		static inline std::add_pointer_t<decltype(Hook_CalculateCameraPitch)> calculateCameraPitch_;
		static inline std::add_pointer_t<decltype(Hook_UpdateCameraPitch)> updateCameraPitch_;
		static inline std::add_pointer_t<decltype(Hook_AfterUpdateCameraZoom)> afterUpdateCameraZoom_;
		static inline std::add_pointer_t<decltype(Hook_HandleToggleInputMode)> handleToggleInputMode_;
		static inline std::add_pointer_t<decltype(Hook_SetDefaultZoom)> setDefaultZoom_;
		static inline std::add_pointer_t<decltype(Hook_SDLMouseYHook)> sdlMouseYHook_;
	};

	void Install();
}

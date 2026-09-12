   #pragma once

   // Minimal external integration point for companion mods that want to force
   // the camera-rotate input into a "held" state without needing to hook or
   // pattern-scan anything in this DLL or the game binary themselves.
   //
   // A companion mod (e.g. RotateToggle.dll) resolves this DLL's module handle
   // and looks up TTPC_SetRotateOverride via GetProcAddress, then calls it
   // whenever it wants to force (or release) the toggle-rotate input state.
   // This is the *only* integration surface such a mod should depend on; the
   // internal InputID/offset machinery in Hooks.h is not a stable API and may
   // change between versions.

   namespace API
   {
   	// When true, Hook_HandleToggleInputMode reports "pressed" for
   	// InputID::kToggleInputMode every frame, regardless of the real
   	// physical input state.
   	inline volatile bool g_rotateOverrideActive = false;
   }

   extern "C" __declspec(dllexport) void __cdecl TTPC_SetRotateOverride(bool active);

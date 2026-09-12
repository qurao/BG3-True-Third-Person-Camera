   #include "API/RotateOverride.h"

   extern "C" __declspec(dllexport) void __cdecl TTPC_SetRotateOverride(bool active)
   {
   	API::g_rotateOverrideActive = active;
   }

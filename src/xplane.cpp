#include <XPLMPlugin.h>

#include <cstring>
#include <memory>

#include "draw.h"

constexpr auto PLUGIN_NAME = "cairoxp";
constexpr auto PLUGIN_SIG = "aeroc7.cairoxp.example";
constexpr auto PLGUIN_DESC =
    "An example plugin showing a method of using Cairo from a background thread to draw on the "
    "panel";
static std::unique_ptr<Draw> panel_draw;

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {
    std::strcpy(outName, PLUGIN_NAME);
    std::strcpy(outSig, PLUGIN_SIG);
    std::strcpy(outDesc, PLGUIN_DESC);

    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    panel_draw = std::make_unique<Draw>();

    return 1;
}

PLUGIN_API void XPluginStop() { panel_draw.reset(); }

PLUGIN_API void XPluginDisable() {}

PLUGIN_API int XPluginEnable() { return 1; }

PLUGIN_API void XPluginReceiveMessage([[maybe_unused]] XPLMPluginID inFrom,
    [[maybe_unused]] int inMsg, [[maybe_unused]] void *inParam) {}

#if IBM
#include <windows.h>

BOOL WINAPI DllMain([[maybe_unused]] HINSTANCE hinst, [[maybe_unused]] DWORD reason,
    [[maybe_unused]] LPVOID resvd) {
    return TRUE;
}

#endif
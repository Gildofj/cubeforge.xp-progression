#include "HUDFormatter.h"
#include "../../core/Constants.h"
#include <cwchar>
#include <cstdio>

namespace pyro {

    std::wstring HUDFormatter::FormatLevelPrefix(double level) {
        wchar_t buffer[64];
        if (level > 1e6) {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV %.2fM ", level / 1e6);
        } else if (level > 1e3) {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV %.2fK ", level / 1e3);
        } else {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV %.0f ", level);
        }
        return std::wstring(buffer);
    }

    std::wstring HUDFormatter::FormatRegionBracket(int regionDistance) {
        const int64_t upper = static_cast<int64_t>(regionDistance + 1) * kLevelsPerRegion;
        const int64_t lower = static_cast<int64_t>(regionDistance) * kLevelsPerRegion + 1;
        wchar_t buffer[64];

        if (upper < 1000) {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV.%lld-%lld ", lower, upper);
        } else if (upper < 1000000) {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV.%lldK ", (upper / 1000));
        } else {
            swprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), L"LV.%lldM ", (upper / 1000000));
        }
        return std::wstring(buffer);
    }

} // namespace pyro

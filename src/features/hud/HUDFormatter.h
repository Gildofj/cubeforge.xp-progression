#pragma once

#include <string>
#include "cwsdk.h"

namespace pyro {

    class HUDFormatter {
    public:
        /**
         * @brief Formats a numeric level value into a human-readable string (e.g., "LV 5 ", "LV 1.20K ", "LV 3.50M ").
         */
        [[nodiscard]] static std::wstring FormatLevelPrefix(double level);

        /**
         * @brief Formats a regional level bracket based on region distance (e.g., "LV.1-5 ", "LV.50K ", "LV.2M ").
         */
        [[nodiscard]] static std::wstring FormatRegionBracket(int regionDistance);
    };

} // namespace pyro

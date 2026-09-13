#include "test_framework.h"

// Forward declare registration functions
void RegisterProgressionFormulasTests();
void RegisterDisplayFormattingTests();
void RegisterMemoryHelperTests();
void RegisterStatScalingTests();
void RegisterNetworkSyncTests();

int main() {
    RegisterProgressionFormulasTests();
    RegisterDisplayFormattingTests();
    RegisterMemoryHelperTests();
    RegisterStatScalingTests();
    RegisterNetworkSyncTests();

    return xp_progression::testing::TestRunner::Instance().RunAllTests();
}

#include "../test_framework.h"
#include <vector>
#include <string>
#include <sstream>

inline std::vector<uint8_t> ParsePatternString(const std::string& pattern) {
    std::vector<uint8_t> pattern_bytes;
    std::istringstream iss(pattern);
    for (std::string s; iss >> s;) {
        if (s == "?" || s == "??") {
            pattern_bytes.push_back(0); // Wildcard marker
        }
        else {
            pattern_bytes.push_back((uint8_t)std::stoi(s, 0, 16));
        }
    }
    return pattern_bytes;
}

inline bool MatchPatternInMemory(const uint8_t* memory, size_t memSize, const std::string& pattern, size_t& foundOffset) {
    std::vector<uint8_t> patternBytes = ParsePatternString(pattern);
    if (patternBytes.empty() || patternBytes.size() > memSize) {
        return false;
    }

    for (size_t i = 0; i <= memSize - patternBytes.size(); ++i) {
        bool match = true;
        for (size_t b = 0; b < patternBytes.size(); ++b) {
            if (patternBytes[b] == 0) continue; // Wildcard
            if (patternBytes[b] != memory[i + b]) {
                match = false;
                break;
            }
        }
        if (match) {
            foundOffset = i;
            return true;
        }
    }
    return false;
}

TEST_FUNC(MemoryHelper, PatternParsingHexAndWildcards) {
    std::string pattern = "48 8B 05 ?? ?? ?? ?? 48 85 C0";
    std::vector<uint8_t> parsed = ParsePatternString(pattern);

    ASSERT_EQ(parsed.size(), (size_t)10);
    ASSERT_EQ(parsed[0], (uint8_t)0x48);
    ASSERT_EQ(parsed[1], (uint8_t)0x8B);
    ASSERT_EQ(parsed[2], (uint8_t)0x05);
    ASSERT_EQ(parsed[3], (uint8_t)0x00); // Wildcard
    ASSERT_EQ(parsed[4], (uint8_t)0x00); // Wildcard
    ASSERT_EQ(parsed[5], (uint8_t)0x00); // Wildcard
    ASSERT_EQ(parsed[6], (uint8_t)0x00); // Wildcard
    ASSERT_EQ(parsed[7], (uint8_t)0x48);
    ASSERT_EQ(parsed[8], (uint8_t)0x85);
    ASSERT_EQ(parsed[9], (uint8_t)0xC0);
}

TEST_FUNC(MemoryHelper, BufferPatternMatching) {
    uint8_t dummyCodeBlock[] = {
        0x90, 0x90, 0x55, 0x48, 0x89, 0xE5,
        0x48, 0x8B, 0x05, 0x12, 0x34, 0x56, 0x78, 0x48, 0x85, 0xC0,
        0x74, 0x05, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xC3
    };

    size_t offset = 0;
    bool found = MatchPatternInMemory(dummyCodeBlock, sizeof(dummyCodeBlock), "48 8B 05 ?? ?? ?? ?? 48 85 C0", offset);

    ASSERT_TRUE(found);
    ASSERT_EQ(offset, (size_t)6);

    // Non-matching pattern
    bool notFound = MatchPatternInMemory(dummyCodeBlock, sizeof(dummyCodeBlock), "FF FF FF FF", offset);
    ASSERT_FALSE(notFound);
}

void RegisterMemoryHelperTests() {
    REGISTER_TEST(MemoryHelper, PatternParsingHexAndWildcards);
    REGISTER_TEST(MemoryHelper, BufferPatternMatching);
}

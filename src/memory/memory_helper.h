#pragma once

#include <Windows.h>
#include <psapi.h>
#include <sstream>
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include "../main.h"

#define CUBE_EXE_NAME "cubeworld.exe"

class MemoryHelper
{
public:
    static MODULEINFO GetModuleInfo(HMODULE module_handle)
    {
        MODULEINFO module_info{};
        if (module_handle != nullptr)
        {
            GetModuleInformation(GetCurrentProcess(), module_handle, &module_info, sizeof(MODULEINFO));
        }
        return module_info;
    }

    static MODULEINFO GetModuleInfo(const char* module)
    {
        return GetModuleInfo(GetModuleHandleA(module));
    }

    static uint64_t GetCubeBase()
    {
        static const auto cube_info = GetModuleInfo(CUBE_EXE_NAME);
        return reinterpret_cast<uint64_t>(cube_info.lpBaseOfDll);
    }

    /**
     * Searches the cubeworld module for any instance of a specified string, and replaces it in memory.
     */
    static void FindAndReplaceString(const std::wstring& search, const std::wstring& replace)
    {
        HMODULE module_handle = GetModuleHandleA(CUBE_EXE_NAME);
        if (!module_handle) return;

        MODULEINFO module_info{};
        GetModuleInformation(GetCurrentProcess(), module_handle, &module_info, sizeof(MODULEINFO));

        const uint64_t base_address = reinterpret_cast<uint64_t>(module_info.lpBaseOfDll);
        const uint64_t module_size = module_info.SizeOfImage;

        const wchar_t* search_cstr = search.c_str();
        const size_t search_len = search.size();

        if (module_size < search_len * 2) return;

        for (uint64_t i = 0; i < module_size - search_len * 2; i++)
        {
            auto* check_addr = reinterpret_cast<uint16_t*>(base_address + i);

            bool found = true;
            for (size_t b = 0; b < search_len; b++)
            {
                if (static_cast<uint16_t>(search_cstr[b]) != check_addr[b])
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                PatchMemory(reinterpret_cast<void*>(check_addr), const_cast<wchar_t*>(replace.c_str()), (replace.size() + 1) * sizeof(wchar_t));
            }
        }
    }

    // Default to cubeworld module
    static uint64_t FindPattern(const std::string& pattern, bool get_end = false)
    {
        return FindPattern(CUBE_EXE_NAME, pattern, get_end);
    }

    static uint64_t FindPattern(const std::string& module, const std::string& pattern, bool get_end = false)
    {
        std::vector<int16_t> pattern_bytes; // Use int16_t where -1 denotes wildcard '?'

        std::istringstream iss(pattern);
        for (std::string s; iss >> s;)
        {
            if (s == "?" || s == "??")
            {
                pattern_bytes.push_back(-1);
            }
            else
            {
                pattern_bytes.push_back(static_cast<int16_t>(std::stoi(s, nullptr, 16)));
            }
        }

        HMODULE module_handle = GetModuleHandleA(module.c_str());
        if (!module_handle) return 0;

        MODULEINFO module_info{};
        GetModuleInformation(GetCurrentProcess(), module_handle, &module_info, sizeof(MODULEINFO));

        const uint64_t base_address = reinterpret_cast<uint64_t>(module_info.lpBaseOfDll);
        const uint64_t module_size = module_info.SizeOfImage;

        if (module_size < pattern_bytes.size()) return 0;

        for (uint64_t i = 0; i < module_size - pattern_bytes.size(); i++)
        {
            const uint64_t check_addr = base_address + i;
            bool found = true;

            for (size_t b = 0; b < pattern_bytes.size(); b++)
            {
                const int16_t p = pattern_bytes[b];
                if (p == -1) continue; // Wildcard

                const uint8_t check_byte = *reinterpret_cast<const uint8_t*>(check_addr + b);
                if (static_cast<uint8_t>(p) != check_byte)
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                return get_end ? (check_addr + pattern_bytes.size()) : check_addr;
            }
        }

        return 0;
    }

    static void PatchMemory(void* dst, const void* src, size_t size)
    {
        if (!dst || !src || size == 0) return;

        DWORD old_protection = 0;
        if (VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protection))
        {
            memcpy(dst, src, size);
            VirtualProtect(dst, size, old_protection, &old_protection);
        }
    }

    template<typename T>
    static void PatchMemory(uint64_t dst, const T& src)
    {
        PatchMemory(reinterpret_cast<void*>(dst), &src, sizeof(T));
    }

    template<typename T>
    static void PatchMemory(void* dst, const T& src)
    {
        PatchMemory(dst, &src, sizeof(T));
    }
};

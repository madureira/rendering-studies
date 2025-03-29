#pragma once

#include <GL/glew.h>
#include <string>

#if defined(__APPLE__)
#include <mach/mach.h>
#include <sstream>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(__linux__)
#include <fstream>
#elif defined(_WIN32)
#include <intrin.h>
#include <windows.h>
#endif

std::string get_sysctl_value(const char *name);

class HardwareUtil final
{
public:
    static std::string GetOS()
    {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
        return "MacOS";
#elif __linux__
        return "Linux";
#else
        return "Unknown OS";
#endif
    }

    static std::string GetCPUModel()
    {
#if defined(__APPLE__) || defined(__linux__)
        return get_sysctl_value("machdep.cpu.brand_string");
#elif defined(_WIN32)
        char buffer[128];
        DWORD size = sizeof(buffer);
        if (RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", RRF_RT_REG_SZ, NULL, buffer, &size) == ERROR_SUCCESS)
        {
            return std::string(buffer);
        }
        return "Unknown CPU";
#else
        return "Unknown CPU";
#endif
    }

    static std::string GetCPUArchitecture()
    {
#if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
        return "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
        return "ARM64 (Apple Silicon)";
#elif defined(__arm__) || defined(_M_ARM)
        return "ARM";
#else
        return "Unknown Architecture";
#endif
    }

    static int GetCPUCores()
    {
#if defined(__APPLE__)
        int cores;
        size_t size = sizeof(cores);
        if (sysctlbyname("hw.ncpu", &cores, &size, NULL, 0) == 0)
        {
            return cores;
        }
        return -1; // Error
#elif defined(__linux__)
        // Linux (sysconf)
        return sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_WIN32)
        // Windows (Using GetSystemInfo)
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
#else
        return -1; // Unsupported OS
#endif
    }

    static double GetTotalMemory()
    {
#if defined(__APPLE__)
        int64_t memSize;
        size_t size = sizeof(memSize);
        if (sysctlbyname("hw.memsize", &memSize, &size, NULL, 0) == 0)
        {
            return BytesToGiga(memSize);
        }
        return -1; // Error
#elif defined(__linux__)
        // Linux (Using sysconf)
        long long memSize = sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);
        return BytesToGiga(memSize);
#elif defined(_WIN32)
        // Windows (Using GlobalMemoryStatusEx)
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo))
        {
            return BytesToGiga(memInfo.ullTotalPhys);
        }
        return -1; // Error
#else
        return -1; // Unsupported OS
#endif
    }

    static double GetFreeMemory()
    {
#if defined(__APPLE__)
        // macOS (Intel & Apple Silicon)
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics_data_t vm_stats;
        kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &count);

        if (kr != KERN_SUCCESS)
        {
            return -1.0; // Error
        }

        // Free memory = free pages + inactive pages * page size
        int64_t free_memory = (vm_stats.free_count + vm_stats.inactive_count) * sysconf(_SC_PAGESIZE);
        return BytesToGiga(free_memory);

#elif defined(__linux__)
        // Linux (Read from /proc/meminfo)
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        long long available_kb = 0;

        while (std::getline(meminfo, line))
        {
            if (line.find("MemAvailable") != std::string::npos)
            {
                sscanf(line.c_str(), "MemAvailable: %lld kB", &available_kb);
                return KiloToGiga(available_kb);
            }
        }
        return -1.0; // Error

#elif defined(_WIN32)
        // Windows (Using GlobalMemoryStatusEx)
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo))
        {
            return BytesToGiga(memInfo.ullAvailPhys);
        }
        return -1.0; // Error

#else
        return -1.0; // Unsupported OS
#endif
    }

    static double GetUsedMemory()
    {
        return GetTotalMemory() - GetFreeMemory();
    }

    static std::string GetGPUModel()
    {
        std::string gpuModel = reinterpret_cast<char const *>(glGetString(GL_RENDERER));
        return gpuModel;
    }

    static std::string GetGPUVendor()
    {
        std::string gpuVendor = reinterpret_cast<char const *>(glGetString(GL_VENDOR));
        return gpuVendor;
    }

    // Function to estimate GPU memory for Intel & Apple M-series
    static double GetEstimateGPUMemory()
    {
        int maxTextureSize = 0;

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

        // Estimate VRAM based on texture size (rough approximation)
        double estimatedVRAM = (maxTextureSize / 1024.0) * 4.0;  // GB Estimate
        return estimatedVRAM;
    }

    static std::string GetGLVersion()
    {
        std::string glVersion = reinterpret_cast<char const *>(glGetString(GL_VERSION));
        return glVersion;
    }

    static std::string GetGLSLVersion()
    {
        std::string glslVersion = reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        return glslVersion;
    }

    static double KiloToGiga(unsigned long long kiloBytes)
    {
        return kiloBytes / (1024 * 1024);
    }

    static double BytesToGiga(unsigned long long bytes)
    {
        return bytes / (1024 * 1024 * 1024);
    }
};

std::string get_sysctl_value(const char *name)
{
    char buffer[128];
    size_t size = sizeof(buffer);
    if (sysctlbyname(name, buffer, &size, NULL, 0) == 0)
    {
        return std::string(buffer, size);
    }
    return "Unknown";
}

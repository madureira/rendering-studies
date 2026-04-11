#pragma once

#include <RenderingStudies/Types.h>

// Keep this include because GraphicsInfo queries use OpenGL calls.
// IMPORTANT: QueryGraphicsInfo() requires a current valid GL context.
#include <RenderingStudies/GL.h>
// Some GL loaders (e.g. GLAD generated for OpenGL 4.1) don't ship newer enums.
// We define them here so the code compiles; runtime usage is still guarded.
#ifndef GL_CONTEXT_FLAG_DEBUG_BIT
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#endif

#ifndef GL_MAX_COMPUTE_WORK_GROUP_COUNT
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT 0x91BE
#endif

#ifndef GL_MAX_COMPUTE_WORK_GROUP_SIZE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE 0x91BF
#endif

#ifndef GL_MAX_TESS_GEN_LEVEL
#define GL_MAX_TESS_GEN_LEVEL 0x8E7F
#endif
#ifndef GL_MAX_PATCH_VERTICES
#define GL_MAX_PATCH_VERTICES 0x8E7D
#endif

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifndef __EMSCRIPTEN__
#if defined(__APPLE__)
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <unistd.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#include <unistd.h>
#elif defined(_WIN32)
#define NOMINMAX
#include <intrin.h>
#include <psapi.h>
#include <windows.h>
#endif
#endif // __EMSCRIPTEN__

#if defined(__has_include)
#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#define RS_HAS_GLFW 1
#else
#define RS_HAS_GLFW 0
#endif
#else
#define RS_HAS_GLFW 0
#endif

// ==========================
//  Data structures
// ==========================

struct SystemInfo
{
    std::string osName;
    std::string osVersion;

    std::string cpuModel;
    std::string cpuArch;
    int32 cpuLogicalCores = -1;

    float64 totalMemoryGiB = -1.0; // physical RAM
};

struct MemoryStats
{
    float64 freeMemoryGiB = -1.0; // "available" when possible
    float64 usedMemoryGiB = -1.0;
    float64 processRssMiB = -1.0; // resident set size
};

struct CpuFeatures
{
    bool sse2 = false;
    bool sse3 = false;
    bool ssse3 = false;
    bool sse41 = false;
    bool sse42 = false;
    bool avx = false;
    bool avx2 = false;

    bool neon = false; // ARM
};

struct GraphicsInfo
{
    std::string vendor;
    std::string renderer;
    std::string glVersion;
    std::string glslVersion;

    // VRAM (best-effort; may be unknown on many platforms, especially integrated GPUs)
    int32 dedicatedVramMiB; // e.g. NVX
    int32 totalAvailableVramMiB;

    // Limits / capabilities
    int32 maxTextureSize = 0;
    int32 max3DTextureSize = 0;
    int32 maxArrayTextureLayers = 0;
    int32 maxDrawBuffers = 0;
    int32 maxSamples = 0;

    int32 maxVertexAttribs = 0;
    int32 maxCombinedTextureUnits = 0;
    int32 maxUniformBlockSizeBytes = 0;
    int32 maxUniformBufferBindings = 0;

    bool supportsAnisotropy = false;
    float32 maxAnisotropy = 0.0f;

    bool supportsCompute = false;
    int32 maxComputeWorkGroupCount[3] = { 0, 0, 0 };
    int32 maxComputeWorkGroupSize[3] = { 0, 0, 0 };

    // Tessellation (OpenGL 4.0+)
    int32 maxTessGenLevel = 0;
    int32 maxPatchVertices = 0;

    bool debugContext = false;
};

#if RS_HAS_GLFW
struct DisplayInfo
{
    int32 monitorWidth = 0;
    int32 monitorHeight = 0;
    int32 refreshRate = 0;

    float32 contentScaleX = 1.0f;
    float32 contentScaleY = 1.0f;
};
#endif

// ==========================
//  HardwareUtil
// ==========================

class HardwareUtil final
{
public:
    // ---- Snapshot queries ----
    static SystemInfo QuerySystemInfo();
    static MemoryStats QueryMemoryStats();
    static CpuFeatures QueryCpuFeatures();

    // Requires a current GL context
    static GraphicsInfo QueryGraphicsInfo();

#if RS_HAS_GLFW
    // Requires GLFW initialized; window can be nullptr (fallback to primary monitor if possible)
    static DisplayInfo QueryDisplayInfo(GLFWwindow* window = nullptr);
#endif

    // ---- Legacy-style getters (kept for compatibility) ----
    static std::string GetOS()
    {
        return QuerySystemInfo().osName;
    }

    static std::string GetOSVersion()
    {
        return QuerySystemInfo().osVersion;
    }

    static std::string GetCPUModel()
    {
        return QuerySystemInfo().cpuModel;
    }

    static std::string GetCPUArchitecture()
    {
        return QuerySystemInfo().cpuArch;
    }

    static int32 GetCPUCores()
    {
        return QuerySystemInfo().cpuLogicalCores;
    }

    static double GetTotalMemory()
    {
        return QuerySystemInfo().totalMemoryGiB;
    }

    static double GetFreeMemory()
    {
        return QueryMemoryStats().freeMemoryGiB;
    }

    static double GetUsedMemory()
    {
        return QueryMemoryStats().usedMemoryGiB;
    }

    static double GetProcessRssMiB()
    {
        return QueryMemoryStats().processRssMiB;
    }

    static std::string GetGPUModel()
    {
        return QueryGraphicsInfo().renderer;
    }

    static std::string GetGPUVendor()
    {
        return QueryGraphicsInfo().vendor;
    }

    static std::string GetGLVersion()
    {
        return QueryGraphicsInfo().glVersion;
    }

    static std::string GetGLSLVersion()
    {
        return QueryGraphicsInfo().glslVersion;
    }

    static double GetEstimateGPUMemory()
    {
        // Requires GL context to be current (same as QueryGraphicsInfo)
        const GraphicsInfo g = QueryGraphicsInfo();

        if (g.dedicatedVramMiB != 0)
        {
            return double(g.dedicatedVramMiB) / 1024.0; // MiB -> GiB
        }

        if (g.totalAvailableVramMiB != 0)
        {
            return double(g.totalAvailableVramMiB) / 1024.0; // MiB -> GiB
        }

        // Fallback heuristic
        if (g.maxTextureSize > 0)
        {
            return (g.maxTextureSize / 1024.0) * 4.0; // rough estimate in GiB
        }

        return -1.0;
    }

    // Units helpers
    static float64 BytesToGiB(uint64 bytes)
    {
        return (float64)bytes / (1024.0 * 1024.0 * 1024.0);
    }

    static float64 KiBToGiB(uint64 kib)
    {
        return (float64)kib / (1024.0 * 1024.0);
    }

    static float64 BytesToMiB(uint64 bytes)
    {
        return (float64)bytes / (1024.0 * 1024.0);
    }

private:
    // ==========================
    //  Detail helpers
    // ==========================
    static inline std::string Trim(std::string s)
    {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
        return s;
    }

    static inline bool StartsWith(const std::string& s, const char* prefix)
    {
        return s.rfind(prefix, 0) == 0;
    }

#if defined(__APPLE__)
    static inline std::string SysctlString(const char* name)
    {
        size_t size = 0;
        if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0)
        {
            return "Unknown";
        }

        std::vector<char> buf(size);
        if (sysctlbyname(name, buf.data(), &size, nullptr, 0) != 0)
        {
            return "Unknown";
        }

        std::string out = std::string(buf.data());

        // sysctl strings often include '\0'
        if (!out.empty() && out.back() == '\0')
        {
            out.pop_back();
        }
        return out;
    }

    static inline int64 SysctlInt64(const char* name, int64 fallback = -1)
    {
        int64 value = 0;
        size_t size = sizeof(value);
        if (sysctlbyname(name, &value, &size, nullptr, 0) == 0)
        {
            return value;
        }
        return fallback;
    }
#endif

#if defined(__linux__)
    static inline std::string ReadFirstMatchFromFile(const char* path, const std::vector<std::string>& prefixes)
    {
        std::ifstream f(path);
        if (!f.is_open())
        {
            return "Unknown";
        }

        std::string line;
        while (std::getline(f, line))
        {
            for (const auto& p : prefixes)
            {
                if (StartsWith(line, p.c_str()))
                {
                    auto colon = line.find(':');
                    if (colon != std::string::npos)
                    {
                        return Trim(line.substr(colon + 1));
                    }
                }
            }
        }
        return "Unknown";
    }

    static inline std::string ReadOsReleasePrettyName()
    {
        std::ifstream f("/etc/os-release");
        if (!f.is_open())
        {
            return "Unknown";
        }

        std::string line;
        while (std::getline(f, line))
        {
            // PRETTY_NAME="Ubuntu 22.04.3 LTS"
            if (StartsWith(line, "PRETTY_NAME="))
            {
                auto v = line.substr(std::strlen("PRETTY_NAME="));
                v = Trim(v);
                if (!v.empty() && (v.front() == '"' || v.front() == '\''))
                {
                    v.erase(v.begin());
                }

                if (!v.empty() && (v.back() == '"' || v.back() == '\''))
                {
                    v.pop_back();
                }

                return v.empty() ? "Linux" : v;
            }
        }
        return "Linux";
    }
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    static inline void Cpuid(int32 out[4], int32 leaf, int32 subleaf)
    {
#if defined(_WIN32)
        __cpuidex(out, leaf, subleaf);
#else
// GCC/Clang
#if defined(__GNUC__) || defined(__clang__)
        __asm__ __volatile__(
            "cpuid"
            : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3])
            : "a"(leaf), "c"(subleaf));
#else
        out[0] = out[1] = out[2] = out[3] = 0;
#endif
#endif
    }

    static inline uint64 Xgetbv(uint32 xcr)
    {
#if defined(_WIN32)
        return _xgetbv(xcr);
#else
        uint32 eax = 0, edx = 0;
#if defined(__GNUC__) || defined(__clang__)
        __asm__ __volatile__(
            ".byte 0x0f, 0x01, 0xd0" // xgetbv
            : "=a"(eax), "=d"(edx)
            : "c"(xcr));
#endif
        return (uint64(edx) << 32) | uint64(eax);
#endif
    }
#endif

    static inline std::string SafeGlString(GLenum name)
    {
        const GLubyte* s = GLR(glGetString(name));
        return s ? reinterpret_cast<const char*>(s) : "Unknown";
    }

    static inline bool HasGlExtension(const char* ext)
    {
        if (!ext || !*ext)
        {
            return false;
        }

        GLint n = 0;
        GL(glGetIntegerv(GL_NUM_EXTENSIONS, &n));
        for (GLint i = 0; i < n; ++i)
        {
            const char* e = reinterpret_cast<const char*>(GLR(glGetStringi(GL_EXTENSIONS, (GLuint)i)));
            if (e && std::strcmp(e, ext) == 0)
            {
                return true;
            }
        }
        return false;
    }

    static inline int32 TryQueryVramNVX(GLenum token)
    {
        // Values are reported in KB
        GLint kb = 0;
        GL(glGetIntegerv(token, &kb));
        if (kb > 0)
        {
            return int32(kb / 1024); // MiB
        }
        return 0;
    }

    static inline int32 TryQueryVramATI()
    {
        // GL_ATI_meminfo returns KB values in arrays, but it’s "free memory" rather than total dedicated.
        // Exposes it as "totalAvailableVramMiB" if present (best-effort).
        GLint v[4] = { 0, 0, 0, 0 };
        GL(glGetIntegerv(0x87FC /*GL_TEXTURE_FREE_MEMORY_ATI*/, v));
        if (v[0] > 0)
        {
            return int32(v[0] / 1024); // MiB
        }
        return 0;
    }
};

// ==========================
//  Implementations
// ==========================

inline SystemInfo HardwareUtil::QuerySystemInfo()
{
    SystemInfo info;

#ifdef __EMSCRIPTEN__
    info.osName = "Browser";
    info.osVersion = "WebAssembly";
    info.cpuArch = "wasm32";
    info.cpuModel = "N/A";
    info.cpuLogicalCores = 1;
    info.totalMemoryGiB = -1.0;
    return info;
#endif

    // OS name
#if defined(_WIN32)
    info.osName = "Windows";
#elif defined(__APPLE__)
    info.osName = "macOS";
#elif defined(__linux__)
    info.osName = "Linux";
#else
    info.osName = "Unknown OS";
#endif

    // OS version (best-effort)
#if defined(__APPLE__)
    // macOS: "kern.osproductversion" exists on modern versions; fallback to osrelease.
    info.osVersion = SysctlString("kern.osproductversion");
    if (info.osVersion == "Unknown")
    {
        info.osVersion = SysctlString("kern.osrelease");
    }
#elif defined(__linux__)
    info.osVersion = ReadOsReleasePrettyName();
#elif defined(_WIN32)
    info.osVersion = "Unknown";
#else
    info.osVersion = "Unknown";
#endif

    // CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
    info.cpuArch = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    info.cpuArch = "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
    info.cpuArch = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
    info.cpuArch = "arm";
#else
    info.cpuArch = "Unknown";
#endif

    // CPU model
#if defined(__APPLE__)
    info.cpuModel = SysctlString("machdep.cpu.brand_string");
#elif defined(__linux__)
    // x86: "model name", ARM boards: "Hardware" or "Processor"
    info.cpuModel = ReadFirstMatchFromFile(
        "/proc/cpuinfo",
        {
            "model name", // x86/x86_64
            "Hardware",   // some ARM
            "Processor",  // some ARM
            "CPU model"   // some distros
        });
#elif defined(_WIN32)
    // Prefer CPUID brand string on x86, fallback to registry.
    info.cpuModel = "Unknown CPU";
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    int32 regs[4] = { 0, 0, 0, 0 };
    Cpuid(regs, 0x80000000, 0);
    const uint32 maxExt = (uint32)regs[0];
    if (maxExt >= 0x80000004)
    {
        char brand[49] = {};
        int32* brandInts = reinterpret_cast<int*>(brand);
        Cpuid(brandInts + 0, 0x80000002, 0);
        Cpuid(brandInts + 4, 0x80000003, 0);
        Cpuid(brandInts + 8, 0x80000004, 0);
        info.cpuModel = Trim(std::string(brand));
    }
#endif
    if (info.cpuModel == "Unknown CPU" || info.cpuModel.empty())
    {
        char buffer[128] = {};
        DWORD size = sizeof(buffer);
        if (RegGetValueA(
                HKEY_LOCAL_MACHINE,
                "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                "ProcessorNameString",
                RRF_RT_REG_SZ,
                NULL,
                buffer,
                &size) == ERROR_SUCCESS)
        {
            info.cpuModel = std::string(buffer);
        }
    }
#else
    info.cpuModel = "Unknown CPU";
#endif

    // Logical cores
#if defined(__APPLE__)
    info.cpuLogicalCores = (int32)SysctlInt64("hw.ncpu", -1);
#elif defined(__linux__)
    info.cpuLogicalCores = (int32)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    info.cpuLogicalCores = (int32)si.dwNumberOfProcessors;
#else
    info.cpuLogicalCores = -1;
#endif

    // Total memory (GiB)
#if defined(__APPLE__)
    const int64 memBytes = SysctlInt64("hw.memsize", -1);
    info.totalMemoryGiB = (memBytes > 0) ? BytesToGiB((uint64)memBytes) : -1.0;
#elif defined(__linux__)
    struct sysinfo si;
    if (sysinfo(&si) == 0)
    {
        const uint64 totalBytes = (uint64)si.totalram * (uint64)si.mem_unit;
        info.totalMemoryGiB = BytesToGiB(totalBytes);
    }
    else
    {
        info.totalMemoryGiB = -1.0;
    }
#elif defined(_WIN32)
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
    {
        info.totalMemoryGiB = BytesToGiB((uint64)ms.ullTotalPhys);
    }
    else
    {
        info.totalMemoryGiB = -1.0;
    }
#else
    info.totalMemoryGiB = -1.0;
#endif

    return info;
}

inline MemoryStats HardwareUtil::QueryMemoryStats()
{
    MemoryStats s;

#ifdef __EMSCRIPTEN__
    return s;
#endif

    // Free / available memory (GiB)
#if defined(__APPLE__)
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vm;
    const kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm, &count);
    if (kr == KERN_SUCCESS)
    {
        const uint64 pageSize = (uint64)sysconf(_SC_PAGESIZE);
        const uint64 freeBytes = (uint64)(vm.free_count + vm.inactive_count) * pageSize;
        s.freeMemoryGiB = BytesToGiB(freeBytes);
    }
#elif defined(__linux__)
    // Prefer MemAvailable from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long long availableKiB = -1;
    while (std::getline(meminfo, line))
    {
        if (line.find("MemAvailable:") != std::string::npos)
        {
            std::sscanf(line.c_str(), "MemAvailable: %lld kB", &availableKiB);
            break;
        }
    }

    if (availableKiB > 0)
    {
        s.freeMemoryGiB = KiBToGiB((uint64)availableKiB);
    }

#elif defined(_WIN32)
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
    {
        s.freeMemoryGiB = BytesToGiB((uint64)ms.ullAvailPhys);
    }
#endif

    // Used memory = total - free (best-effort, guard failures)
    const SystemInfo sys = QuerySystemInfo();
    if (sys.totalMemoryGiB > 0.0 && s.freeMemoryGiB > 0.0)
    {
        s.usedMemoryGiB = std::max(0.0, sys.totalMemoryGiB - s.freeMemoryGiB);
    }

    // Process RSS (MiB)
#if defined(__APPLE__)
    mach_task_basic_info_data_t info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS)
    {
        s.processRssMiB = BytesToMiB((uint64)info.resident_size);
    }

#elif defined(__linux__)
    // /proc/self/statm: size resident shared text lib data dt
    std::ifstream statm("/proc/self/statm");
    long long size = 0, resident = 0;
    if (statm >> size >> resident)
    {
        const uint64 pageSize = (uint64)sysconf(_SC_PAGESIZE);
        const uint64 rssBytes = (uint64)resident * pageSize;
        s.processRssMiB = BytesToMiB(rssBytes);
    }

#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        s.processRssMiB = BytesToMiB((uint64)pmc.WorkingSetSize);
    }
#endif

    return s;
}

inline CpuFeatures HardwareUtil::QueryCpuFeatures()
{
    CpuFeatures f;

#ifdef __EMSCRIPTEN__
    return f;
#endif

    // ARM NEON (compile-time best effort)
#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    f.neon = true;
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    int32 regs[4] = { 0, 0, 0, 0 };

    // Leaf 1: SSE3, SSSE3, SSE4.1, SSE4.2, AVX, OSXSAVE
    Cpuid(regs, 1, 0);
    const int32 ecx = regs[2];
    const int32 edx = regs[3];

    f.sse2 = (edx & (1 << 26)) != 0;
    f.sse3 = (ecx & (1 << 0)) != 0;
    f.ssse3 = (ecx & (1 << 9)) != 0;
    f.sse41 = (ecx & (1 << 19)) != 0;
    f.sse42 = (ecx & (1 << 20)) != 0;

    const bool avxCpu = (ecx & (1 << 28)) != 0;
    const bool osxsave = (ecx & (1 << 27)) != 0;

    // Need XCR0 check to ensure OS supports AVX state
    if (avxCpu && osxsave)
    {
        const uint64 xcr0 = Xgetbv(0);
        const bool xmm = (xcr0 & 0x2) != 0;
        const bool ymm = (xcr0 & 0x4) != 0;
        f.avx = xmm && ymm;
    }

    // Leaf 7 subleaf 0: AVX2 in EBX bit 5
    Cpuid(regs, 7, 0);
    const int32 ebx = regs[1];
    const bool avx2Cpu = (ebx & (1 << 5)) != 0;
    f.avx2 = f.avx && avx2Cpu;
#endif

    return f;
}

inline GraphicsInfo HardwareUtil::QueryGraphicsInfo()
{
    GraphicsInfo g;

    g.vendor = SafeGlString(GL_VENDOR);
    g.renderer = SafeGlString(GL_RENDERER);
    g.glVersion = SafeGlString(GL_VERSION);
    g.glslVersion = SafeGlString(GL_SHADING_LANGUAGE_VERSION);

    // Limits
    GL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &g.maxTextureSize));
    GL(glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &g.max3DTextureSize));
    GL(glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &g.maxArrayTextureLayers));
    GL(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &g.maxDrawBuffers));
    GL(glGetIntegerv(GL_MAX_SAMPLES, &g.maxSamples));

    GL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &g.maxVertexAttribs));
    GL(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &g.maxCombinedTextureUnits));
    GL(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &g.maxUniformBlockSizeBytes));
    GL(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &g.maxUniformBufferBindings));

    // Debug context flag (GL_CONTEXT_FLAGS not available in GLES3)
#ifndef __EMSCRIPTEN__
    GLint flags = 0;
    GL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
    g.debugContext = (flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0;
#else
    g.debugContext = false;
#endif

    // Anisotropy
    if (HasGlExtension("GL_EXT_texture_filter_anisotropic"))
    {
        g.supportsAnisotropy = true;
        GLfloat v = 0.0f;
        GL(glGetFloatv(0x84FF /*GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT*/, &v));
        g.maxAnisotropy = (float32)v;
    }

    // Compute shaders support (GL 4.3+ or ARB extension)
    GLint major = 0, minor = 0;
    GL(glGetIntegerv(GL_MAJOR_VERSION, &major));
    GL(glGetIntegerv(GL_MINOR_VERSION, &minor));

#ifndef __EMSCRIPTEN__
    g.supportsCompute = (major > 4) || (major == 4 && minor >= 3) || HasGlExtension("GL_ARB_compute_shader");
    if (g.supportsCompute)
    {
        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &g.maxComputeWorkGroupCount[0]));
        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &g.maxComputeWorkGroupCount[1]));
        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &g.maxComputeWorkGroupCount[2]));

        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &g.maxComputeWorkGroupSize[0]));
        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &g.maxComputeWorkGroupSize[1]));
        GL(glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &g.maxComputeWorkGroupSize[2]));
    }

    // Tessellation (OpenGL 4.0+)
    if (major > 4 || (major == 4 && minor >= 0))
    {
        GL(glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &g.maxTessGenLevel));
        GL(glGetIntegerv(GL_MAX_PATCH_VERTICES, &g.maxPatchVertices));
    }
#endif // __EMSCRIPTEN__

    // VRAM best-effort (extensions)
    // NVX (NVIDIA): returns KB
#ifndef GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX
#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#endif

    if (HasGlExtension("GL_NVX_gpu_memory_info"))
    {
        g.dedicatedVramMiB = TryQueryVramNVX(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX);
        g.totalAvailableVramMiB = TryQueryVramNVX(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX);
    }
    else if (HasGlExtension("GL_ATI_meminfo"))
    {
        g.totalAvailableVramMiB = TryQueryVramATI();
    }

    return g;
}

#if RS_HAS_GLFW
inline DisplayInfo HardwareUtil::QueryDisplayInfo(GLFWwindow* window)
{
    DisplayInfo d;

    GLFWmonitor* monitor = nullptr;
    if (window)
    {
        monitor = glfwGetWindowMonitor(window);
    }

    if (!monitor)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    if (monitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (mode)
        {
            d.monitorWidth = (int32)mode->width;
            d.monitorHeight = (int32)mode->height;
            d.refreshRate = (int32)mode->refreshRate;
        }

        float xscale = 1.0f, yscale = 1.0f;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);
        d.contentScaleX = (float32)xscale;
        d.contentScaleY = (float32)yscale;
    }

    return d;
}
#endif

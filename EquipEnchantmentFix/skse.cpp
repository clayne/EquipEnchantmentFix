#include "pch.h"

namespace SKSE
{
    constexpr size_t MAX_TRAMPOLINE_BRANCH = 64;

    constexpr UInt32 MIN_MESSAGING_INTERFACE_VERSION = 2;

    PluginHandle g_pluginHandle = kPluginHandle_Invalid;

    SKSEMessagingInterface* g_messaging;
    SKSETaskInterface* g_taskInterface;

    bool Query(const SKSEInterface* a_skse, PluginInfo* a_info)
    {
        gLog.OpenRelative(CSIDL_MYDOCUMENTS, PLUGIN_LOG_PATH);
        gLog.SetLogLevel(IDebugLog::LogLevel::Debug);

        a_info->infoVersion = PluginInfo::kInfoVersion;
        a_info->name = PLUGIN_NAME;
        a_info->version = MAKE_PLUGIN_VERSION(
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_REVISION);

        if (a_skse->isEditor)
        {
            gLog.FatalError("Loaded in editor, marking as incompatible");
            return false;
        }

        if (a_skse->runtimeVersion < MIN_SKSE_VERSION)
        {
            gLog.FatalError("Unsupported runtime version %d.%d.%d.%d, expected >= %d.%d.%d.%d",
                GET_EXE_VERSION_MAJOR(a_skse->runtimeVersion),
                GET_EXE_VERSION_MINOR(a_skse->runtimeVersion),
                GET_EXE_VERSION_BUILD(a_skse->runtimeVersion),
                GET_EXE_VERSION_SUB(a_skse->runtimeVersion),
                GET_EXE_VERSION_MAJOR(MIN_SKSE_VERSION),
                GET_EXE_VERSION_MINOR(MIN_SKSE_VERSION),
                GET_EXE_VERSION_BUILD(MIN_SKSE_VERSION),
                GET_EXE_VERSION_SUB(MIN_SKSE_VERSION));

            return false;
        }

        g_pluginHandle = a_skse->GetPluginHandle();

        return true;
    }

    bool Initialize(const SKSEInterface* a_skse)
    {
        g_messaging = (SKSEMessagingInterface*) a_skse->QueryInterface(kInterface_Messaging);
        if (g_messaging == nullptr) {
            gLog.FatalError("Could not get messaging interface");
            return false;
        }

        if (g_messaging->interfaceVersion < MIN_MESSAGING_INTERFACE_VERSION) {
            gLog.FatalError("Messaging interface too old (%d expected %d)", g_messaging->interfaceVersion, MIN_MESSAGING_INTERFACE_VERSION);
            return false;
        }

        g_taskInterface = (SKSETaskInterface*)a_skse->QueryInterface(kInterface_Task);
        if (g_taskInterface == nullptr) {
            gLog.FatalError("Couldn't get task interface.");
            return false;
        }

        if (g_taskInterface->interfaceVersion < 2) {
            gLog.FatalError("Task interface too old (%d expected %d)", g_taskInterface->interfaceVersion, 2);
            return false;
        }

        auto iface = static_cast<SKSETrampolineInterface*>(a_skse->QueryInterface(kInterface_Trampoline));

        if (!Hook::InitBranchTrampoline(a_skse, iface, MAX_TRAMPOLINE_BRANCH))
        {
            gLog.FatalError("Could not create branch trampoline.");
            return false;
        }

        return true;
    }
}
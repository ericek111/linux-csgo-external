#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/uio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <X11/Xlib.h>

namespace remote {
    class Handle;

    struct MapModuleMemoryRegion {
    public:
        // Memory
        unsigned long start;
        unsigned long end;

        // Permissions
        bool readable;
        bool writable;
        bool executable;
        bool shared;

        // File data
        unsigned long offset;
        unsigned char deviceMajor;
        unsigned char deviceMinor;
        unsigned long inodeFileNumber;
        std::string pathname;
        std::string filename;

        unsigned long client_start;

        void* find(Handle handle, const char* data, const char* pattern);
    };

    class Handle {
    public:
        unsigned long addressOfClientModule;
        unsigned long addressOfGlowPointer;
        unsigned long addressOfLocalPlayer;
        unsigned long addressOfEntityList;
        unsigned long addressOfPlayerResourcesPointer;
        unsigned long addressOfOverridePostProcessingDisablePointer;
        unsigned long addressOfPlayerResources;
        unsigned long localPlayerOffset;
        unsigned long addressOfAlt1;
        unsigned long addressOfForceAttack;
        unsigned long m_oAddressOfForceJump;
        bool shouldTrigger = false;
        bool shouldGlow = true;
        bool glowOthers = true;
        bool noFlash = false;
        bool overlay = true;
        bool running = true;
        bool justglowoff = false;
        bool justglowothersoff = false;
        bool drawVelocity = true;
        XColor speedgradient[255];
        XColor blackgradient[255];
        Handle() : pid(-1) {}
        Handle(pid_t target);
        Handle(std::string target);

        std::string GetPath();
        std::string GetWorkingDirectory();
        pid_t GetPid(){ return pid; }

        void ParseMaps();

        bool IsValid();
        bool IsRunning();

        bool Write(void* address, void* buffer, size_t size);
        bool Read(void* address, void* buffer, size_t size);

        unsigned long GetCallAddress(void* address);
        unsigned long GetAbsoluteAddress(void* address, int offset, int size);

        MapModuleMemoryRegion* GetRegionOfAddress(void* address);

    private:
        std::string GetSymbolicLinkTarget(std::string target);

        pid_t pid;
        std::string pidStr;
        //FILE* memr;
        //FILE* memw;

    public:
        std::vector<MapModuleMemoryRegion> regions;
    };

    bool FindProcessByName(std::string name, Handle* out);
};
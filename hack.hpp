#pragma once
#ifndef HACK_HPP
#define HACK_HPP

#include <stddef.h>
#include "remote.hpp"
#include "log.hpp"
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <iomanip> // setprecision
#include <unistd.h>
#include "draw.hpp"
#include <sstream>
#include <string>
#include <cmath>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#define TEAM_SPECTATOR          1
#define TEAM_T                  2
#define TEAM_CT                 3

// m_lifeState values
#define LIFE_ALIVE              0 // alive
#define LIFE_DYING              1 // playing death animation or still falling off of a ledge waiting to hit ground
#define LIFE_DEAD               2 // dead. lying still.
#define LIFE_RESPAWNABLE        3
#define LIFE_DISCARDBODY        4

#define MAX_TRAIL_LENGTH        30
#define MAX_PLAYER_NAME_LENGTH  128

// CBaseEntity::m_fFlags
// PLAYER SPECIFIC FLAGS FIRST BECAUSE WE USE ONLY A FEW BITS OF NETWORK PRECISION
#define FL_ONGROUND             (1<<0)  // At rest / on the ground
#define FL_DUCKING              (1<<1)  // Player flag -- Player is fully crouched
#define FL_WATERJUMP            (1<<3)  // player jumping out of water
#define FL_ONTRAIN              (1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN               (1<<5)  // Indicates the entity is standing in rain
#define FL_FROZEN               (1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS           (1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define FL_CLIENT               (1<<8)  // Is a player
#define FL_FAKECLIENT           (1<<9)  // Fake client, simulated server side; don't send network messages to them
#define FL_INWATER              (1<<10) // In water

// NOTE if you move things up, make sure to change this value
#define PLAYER_FLAG_BITS        11

// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define FL_FLY                  (1<<11) // Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM                 (1<<12) // Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define FL_CONVEYOR             (1<<13)
#define FL_NPC                  (1<<14)
#define FL_GODMODE              (1<<15)
#define FL_NOTARGET             (1<<16)
#define FL_AIMTARGET            (1<<17) // set if the crosshair needs to aim onto the entity
#define FL_PARTIALGROUND        (1<<18) // not all corners are valid
#define FL_STATICPROP           (1<<19) // Eetsa static prop!       
#define FL_GRAPHED              (1<<20) // worldgraph has this ent listed as something that blocks a connection
#define FL_GRENADE              (1<<21)
#define FL_STEPMOVEMENT         (1<<22) // Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH            (1<<23) // Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define FL_BASEVELOCITY         (1<<24) // Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_WORLDBRUSH           (1<<25) // Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_OBJECT               (1<<26) // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define FL_KILLME               (1<<27) // This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE               (1<<28) // You know...
#define FL_DISSOLVING           (1<<29) // We're dissolving!
#define FL_TRANSRAGDOLL         (1<<30) // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1<<31) // pusher that can't be blocked by the player

namespace hack {
    template<class T> class CUtlVector {
    public:
        T*              DataPtr;                    //0000 (054612C0)
        unsigned int    Max;                        //0004 (054612C4)
        unsigned int    unk02;                      //0008 (054612C8)
        unsigned int    Count;                      //000C (054612CC)
        unsigned int    DataPtrBack;                //0010 (054612D0)
    };

    struct GlowObjectDefinition_t {
        bool ShouldDraw( int nSlot ) const {
            return m_pEntity && ( m_nSplitScreenSlot == -1 || m_nSplitScreenSlot == nSlot ) && ( m_bRenderWhenOccluded || m_bRenderWhenUnoccluded );
        }

        bool IsUnused() const {
            return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
        }

        long writeStart() {
            return (long(&(this)->m_flGlowRed) - long(this));
        }

        long writeEnd() {
            return (long(&(this)->unk2) - long(this));
        }

        void*              m_pEntity;
        float              m_flGlowRed;
        float              m_flGlowGreen;
        float              m_flGlowBlue;
        float              m_flGlowAlpha;
        uint8_t            unk0[16];
        bool               m_bRenderWhenOccluded : 8;
        bool               m_bRenderWhenUnoccluded : 8;
        bool               m_bFullBloomRender : 8;
        unsigned char      unk1;
        int                m_nFullBloomStencilTestValue;
        int                m_nSplitScreenSlot;
        int                m_nNextFreeSlot;
        unsigned char      unk2[4];


        static const int END_OF_FREE_LIST = -1;
        static const int ENTRY_IN_USE = -2;
    }; // sizeof() == 0x34

    class CGlowObjectManager
    {
    public:
        CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions; //0000
        int m_nFirstFreeSlot; //0014 (054612D4)
        unsigned int unk1; //0018 (054612D8)
        unsigned int unk2; //001C (054612DC)
        unsigned int unk3; //0020 (054612E0)
        unsigned int unk4; //0024 (054612E4)
        unsigned int unk5; //0028 (054612E8)
    };

    struct Color {
        unsigned char _color[4];
    };


    struct Vector {
        float x, z, y;
        struct Vector& operator+=(const Vector& a) { x += a.x; y += a.y; z += a.z; return *this; }
        struct Vector& operator+=(const float& k) { x += k; y += k; z += k; return *this; }
        struct Vector& operator-=(const Vector& a) { x -= a.x; y -= a.y; z -= a.z; return *this; }
        struct Vector& operator-=(const float& k) { x -= k; y -= k; z -= k; return *this; }
        struct Vector& operator*=(const Vector& a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
        struct Vector& operator*=(const float& k) { x *= k; y *= k; z *= k; return *this; }
        struct Vector& operator=(const Vector& a) { x = a.x; y = a.y; z = a.z; return *this; }
        struct Vector& operator=(const float& k) { x = k; y = k; z = k; return *this; }
    };
    inline Vector operator+(Vector a, const Vector& b) { return a += b; }
    inline Vector operator+(Vector a, const float k) { return a += k; }
    inline Vector operator-(Vector a, const Vector& b) { return a -= b; }
    inline Vector operator-(Vector a, const float k) { return a -= k; }
    inline Vector operator*(Vector a, const Vector& b) { return a *= b; }
    inline Vector operator*(Vector a, const float k) { return a *= k; }

    struct QAngle {
        float x, y, z;
    };

    struct Vector2D {
        float x, y;
    };

    struct Entity {
    char __buf_0x00[0x74]; // 0x0
    
    QAngle m_aimPunchAngle; // 0x80
    char __buf_0x80[0x8]; // 0x80

    int m_bIsAutoaimTarget; // 0x88
    char __buf_0x8C[0x8]; // 0x8C
    unsigned int m_iEntityId; // 0x94
    char __buf_0x8sC[0x8]; // 0x8C
    int m_clrRender; // 0xA0
    int m_cellbits; // 0xA4
    char __buf_0xA8[0x4]; // 0xA8
    int m_cellX; // 0xAC
    int m_cellY; // 0xB0
    int m_cellZ; // 0xB4
    char __buf_0xB8[0x14]; // 0xB8

    Vector m_vecAbsVelocity; // 0xCC
    Vector m_vecAbsOrigin; // 0xD8
    Vector m_vecOrigin; // 0xE4
    Vector m_vecAngVelocity; // 0xF0
    QAngle m_angAbsRotation; // 0xFC
    QAngle m_angRotation; // 0x108
    float m_flGravity; // 0x114
    float m_flProxyRandomValue; // 0x118
    int m_iEFlags; // 0x11C
    unsigned char m_nWaterType; // 0x120
    unsigned char m_bDormant; // 0x121
    char __buf_0x122[0x2]; // 0x122
    float m_fEffects; // 0x124
    int m_iTeamNum; // 0x128
    int m_iPendingTeamNum; // 0x12C
    char __buf_0x130[0x4]; // 0x130
    int m_iHealth; // 0x134
    int m_fFlags; // 0x138
    Vector m_vecViewOffset; // 0x13C
    Vector m_vecVelocity; // 0x148
    Vector m_vecBaseVelocity; // 0x154
    QAngle m_angNetworkAngles; // 0x160
    Vector m_vecNetworkOrigin; // 0x16C
    float m_flFriction; // 0x178
    unsigned long long m_hNetworkMoveParent; // 0x17C
    unsigned long long m_hOwnerEntity; // 0x180
    unsigned long long m_hGroundEntity; // 0x184
    char __buf_0x18C[0x100]; // 0x18C
    unsigned char m_nModelIndex; // 0x28C
    char __buf_0x28D[0x1]; // 0x28D
    unsigned char m_nRenderFX; // 0x28E
    unsigned char m_nRenderMode; // 0x28F
    char m_MoveType; // 0x290
    char m_MoveCollide; // 0x291
    unsigned char m_nWaterLevel; // 0x292
    char __buf_0x293[0x85]; // 0x293
    float m_flUseLookAtAngle; // 0x318
    char __buf_0x31C[0x54]; // 0x31C
    char m_ModelName; // 0x370
    char __buf_0x371[0x7]; // 0x371
    unsigned long long m_Collision; // 0x378
    char __buf_0x380[0x198]; // 0x380
    char m_rgflCoordinateFrame; // 0x518
    char __buf_0x519[0x9AF]; // 0x519
    unsigned char m_bEverHadPredictionErrorsForThisCommand; // 0xEC8
        char __buf_0x297[0x25DF]; // 0x297
    int m_iAmmo; // 0x34A8
    char __buf_0x34AC[0x1CC]; // 0x34AC
    int m_iCoachingTeam; // 0x3678
    char __buf_0x367C[0x64]; // 0x367C
    float m_flDuckAmount; // 0x36E0
    float m_flDuckSpeed; // 0x36E4
    char __buf_0x36E8[0x2B0]; // 0x36E8
    int m_iFOV; // 0x3998
    int m_iFOVStart; // 0x399C
    char __buf_0x39A0[0x14]; // 0x39A0
    int m_ladderSurfaceProps; // 0x39B4
    char __buf_0x39B8[0x4]; // 0x39B8
    float m_flFOVTime; // 0x39BC
    char __buf_0x39C0[0x18]; // 0x39C0
    Vector m_vecLadderNormal; // 0x39D8
    char __buf_0x39E4[0x24]; // 0x39E4
    int m_iBonusProgress; // 0x3A08
    int m_iBonusChallenge; // 0x3A0C
    float m_flMaxspeed; // 0x3A10
    unsigned long long m_hZoomOwner; // 0x3A14
    char __buf_0x3A1C[0x14]; // 0x3A1C
    int m_vphysicsCollisionState; // 0x3A30
    char __buf_0x3A34[0x94]; // 0x3A34
    int m_afPhysicsFlags; // 0x3AC8
    unsigned long long m_hVehicle; // 0x3ACC
    unsigned long long m_hViewModel; // 0x3AD4
    char __buf_0x3ADC[0x34]; // 0x3ADC
    unsigned int m_hUseEntity; // 0x3B10
    int m_iDefaultFOV; // 0x3B14
    char __buf_0x3B18[0x10]; // 0x3B18
    unsigned int m_hViewEntity; // 0x3B28
    int m_bShouldDrawPlayerWhileUsingViewEntity; // 0x3B2C
    char __buf_0x3B30[0x2C]; // 0x3B30
    int m_iDeathPostEffect; // 0x3B5C
    int m_iObserverMode; // 0x3B60
    unsigned char m_bActiveCameraMan; // 0x3B64
    char __buf_0x3B65[0x1]; // 0x3B65
    unsigned char m_bCameraManOverview; // 0x3B66
    char __buf_0x3B67[0x1]; // 0x3B67
    int m_uCameraManGraphs; // 0x3B68
    char __buf_0x3B6C[0x8]; // 0x3B6C
    unsigned long long m_hObserverTarget; // 0x3B74

        // offsets addresses DO NOT correspond to reality!
        /*char __buf_0x00[0x88]; // 0x0
        int m_bIsAutoaimTarget; // 0x88
        char __buf_0x8C[0x8]; // 0x8C
        unsigned int m_iEntityId; // 0x94
        char __buf_0x8sC[0x8]; // 0x8C
        int m_clrRender; // 0xA0
        int m_cellbits; // 0xA4
        char __buf_0xA8[0x4]; // 0xA8
        int m_cellX; // 0xAC
        int m_cellY; // 0xB0
        int m_cellZ; // 0xB4
        char __buf_0xB8[0x14]; // 0xB8
        Vector m_vecAbsVelocity; // 0xC4
        Vector m_vecAbsOrigin; // 0xD0
        Vector m_vecOrigin; // 0xDC
        Vector m_vecAngVelocity; // 0xE8
        Vector m_angAbsRotation; // 0xF4
        Vector m_angRotation; // 0x100
        float m_flGravity; // 0x10C
        float m_flProxyRandomValue; // 0x110
        int m_iEFlags; // 0x114
        unsigned char m_nWaterType; // 0x118
        unsigned char m_bDormant; // 0x119
        char __buf_0x11A[0x6]; // 0x11A
        float m_fEffects; // 0x11C
        int m_iTeamNum; // 0x120
        int m_iPendingTeamNum; // 0x124
        int m_iHealth; // 0x12C
        unsigned long m_fFlags; // 0x130
        //char __buf_0x130[0x0]; // 0x134
        Vector m_vecViewOffset; // 0x134
        Vector m_vecVelocity; // 0x140
        Vector m_vecBaseVelocity; // 0x14C
        Vector m_angNetworkAngles; // 0x158
        Vector m_vecNetworkOrigin; // 0x164
        char __buf_0x170[0x4]; // 0x170
        int moveparent; // 0x174
        unsigned long long m_hOwnerEntity; // 0x178
        char m_iName[0x104]; // 0x188
        unsigned char m_nModelIndex; // 0x28C
        char __buf_0xABgD0[0x38D4]; 
        int m_iObserverMode; // 0x3B60
        unsigned char m_bActiveCameraMan; // 0x3B64
        char __buf_0x3B65[0x1]; // 0x3B65
        unsigned char m_bCameraManOverview; // 0x3B66
        char __buf_0x3B67[0x1]; // 0x3B67
        int m_uCameraManGraphs; // 0x3B68
        char __buf_0x3B6C[0x8]; // 0x3B6C
        unsigned long long m_hObserverTarget; // 0x3B74*/

        /*char __buf_0xABD0[0xA94F]; // 0x17C
        float m_flFlashMaxAlpha; // 0xABE4
        float m_flFlashDuration; // 0xABE8*/
    };

    struct dEntity {
        unsigned char unk0[0x11D];
        unsigned char m_bDormant;           // 0x11D
        unsigned char unk01[0x2];           // 0x11E
        float         m_fEffects;           // 0x120
        int           m_iTeamNum;           // 0x124
        int           m_iPendingTeamNum;    // 0x128
        int           m_iHealth;            // 0x12C
        unsigned long m_fFlags;             // 0x130
        char          __buf_0x128[0x30];    // 0x128
        QAngle        m_angRotation;        // 0x158
        Vector        m_vecOrigin;          // 0x164
            unsigned int m_iEntityId; // 0x8C

    };

    struct CPlantedC4 {
        char __buf_0x00[0x3001]; // 0x0
        int m_bBombTicking; // 0x3001
        char __buf_0x3005[0x7]; // 0x3005
        float m_flC4Blow; // 0x300C
        float m_flTimerLength; // 0x3010
        char __buf_0x3014[0xC]; // 0x3014
        float m_flDefuseLength; // 0x3020
        float m_flDefuseCountDown; // 0x3024
        int m_bBombDefused; // 0x3028
        unsigned int m_hBombDefuser; // 0x302C
            char __buf2_0x00[0xBB4]; // 0x3030
        int m_bStartedArming; // 0x3BE4
        float m_fArmedTime; // 0x3BE8
        unsigned char m_bBombPlacedAnimation; // 0x3BEC
        char __buf_0x3BED[0x1]; // 0x3BED
        int m_bIsPlantingViaUse; // 0x3BEE
    };

    struct CSPlayerResource {

    char __1buf_0x00[0x1180]; // 0x0
    int m_iPing[64]; // 0x1180
    char __1buf_0x1184[0x100]; // 0x1184
    int m_iKills[64]; // 0x1284
    char __1buf_0x1288[0x100]; // 0x1288
    int m_iAssists[64]; // 0x1388
    char __1buf_0x138C[0x100]; // 0x138C
    int m_iDeaths[64]; // 0x148C
    char __1buf_0x1490[0x100]; // 0x1490
    bool m_bConnected[64]; // 0x1590
    char __1buf_0x1594[0x40]; // 0x1594
    int m_iTeam[64]; // 0x15D4
    char __1buf_0x15D8[0x100]; // 0x15D8
    int m_iPendingTeam[64]; // 0x16D8
    char __1buf_0x16DC[0x100]; // 0x16DC
    bool m_bAlive[64]; // 0x17DC
    char __1buf_0x17E0[0x40]; // 0x17E0
    int m_iHealth[64]; // 0x1820
    char __1buf_0x1824[0x180]; // 0x1824
    int m_iCoachingTeam[64]; // 0x19A4

    char __buf_0x00[0x310]; // 0x0
    int m_iPlayerC4; // 0x1CB4
    int m_iPlayerVIP; // 0x1CB8
    QAngle m_bombsiteCenterA; // 0x1CBC
    QAngle m_bombsiteCenterB; // 0x1CC8
    bool m_bHostageAlive[64]; // 0x1CD4
    char __buf_0x1CD8[0x8]; // 0x1CD8
    int m_isHostageFollowingSomeone[64]; // 0x1CE0
    char __buf_0x1CE4[0x8]; // 0x1CE4
    int m_iHostageEntityIDs[64]; // 0x1CEC
    char __buf_0x1CF0[0x2C]; // 0x1CF0
    int m_hostageRescueX[64]; // 0x1D1C
    char __buf_0x1D20[0xC]; // 0x1D20
    int m_hostageRescueY[64]; // 0x1D2C
    char __buf_0x1D30[0xC]; // 0x1D30
    int m_hostageRescueZ[64]; // 0x1D3C
    char __buf_0x1D40[0xC]; // 0x1D40
    int m_iMVPs[64]; // 0x1D4C
    char __buf_0x1D50[0x100]; // 0x1D50
    bool m_bHasDefuser[64]; // 0x1E50
    char __buf_0x1E54[0x3D]; // 0x1E54
    bool m_bHasHelmet[64]; // 0x1E91
    char __buf_0x1E95[0x3F]; // 0x1E95
    int m_iArmor[64]; // 0x1ED4
    char __buf_0x1ED8[0x100]; // 0x1ED8
    int m_iScore[64]; // 0x1FD8
    char __buf_0x1FDC[0x100]; // 0x1FDC
    int m_iCompetitiveRanking[64]; // 0x20DC
    char __buf_0x20E0[0x100]; // 0x20E0
    int m_iCompetitiveWins[64]; // 0x21E0
    char __buf_0x21E4[0x100]; // 0x21E4
    int m_iCompTeammateColor[64]; // 0x22E4
    char __buf_0x22E8[0x100]; // 0x22E8
    int m_bControllingBot[64]; // 0x23E8
    char __buf_0x23EC[0x40]; // 0x23EC
    int m_iControlledPlayer[64]; // 0x242C
    char __buf_0x2430[0x100]; // 0x2430
    int m_iControlledByPlayer[64]; // 0x2530
    char __buf_0x2534[0x2180]; // 0x2534
    int m_iBotDifficulty[64]; // 0x46B4
    char __buf_0x46B8[0x100]; // 0x46B8
    int m_szClan[64]; // 0x47B8
    char __buf_0x47BC[0x40C]; // 0x47BC
    int m_iTotalCashSpent[64]; // 0x4BC8
    char __buf_0x4BCC[0x100]; // 0x4BCC
    int m_iCashSpentThisRound[64]; // 0x4CCC
    char __buf_0x4CD0[0x100]; // 0x4CD0
    int m_nEndMatchNextMapVotes[64]; // 0x4DD0
    char __buf_0x4DD4[0x100]; // 0x4DD4
    int m_bEndMatchNextMapAllVoted; // 0x4ED4
    int m_nActiveCoinRank[64]; // 0x4ED8
    char __buf_0x4EDC[0x100]; // 0x4EDC
    int m_nMusicID[64]; // 0x4FDC
    char __buf_0x4FE0[0x144]; // 0x4FE0
    int m_nPersonaDataPublicLevel[64]; // 0x5124
    char __buf_0x5128[0x100]; // 0x5128
    int m_nPersonaDataPublicCommendsLeader[64]; // 0x5228
    char __buf_0x522C[0x100]; // 0x522C
    int m_nPersonaDataPublicCommendsTeacher[64]; // 0x532C
    char __buf_0x5330[0x100]; // 0x5330
    int m_nPersonaDataPublicCommendsFriendly[64]; // 0x5430

    };

    struct CSPlayerResourceSUM {
        int m_iKills;        // 0x1284
    int m_iAssists;        // 0x1388
    int m_iDeaths;        // 0x148c
    int m_iTeam;        // 0x15d4
    int m_iHealth;        // 0x1820
    int m_iMVPs;        // 0x1d4c
    int m_iArmor;       // 0x1ed4
    bool m_bHasHelmet;  // 0x1e91
    bool m_bHasDefuser; // 0x1e50
    int m_iScore;       // 0x1fd8
    int m_iCompetitiveRanking;       // 0x20dc
    int m_iCompetitiveWins;       // 0x21e0
    };

    extern void Glow(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, Draw* draw);
};

#endif // HACK_HPP
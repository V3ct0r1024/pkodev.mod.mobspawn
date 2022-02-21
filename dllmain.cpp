#include <windows.h>
#include <detours.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctime>

#include "pointer.h"
#include "structure.h"
#include "hook.h"

#include "loader.h"


namespace pkodev
{
    // Define some useful types
    typedef std::vector<unsigned int> monsters_id_list_t;                // Monsters IDs list
    typedef std::vector<pkodev::outmap_spawn> outmap_spawn_list_t;       // Monsters that are removed from a map
    typedef std::vector<pkodev::expected_spawn> expected_spawn_list_t;   // Monsters that will soon be spawned
    typedef std::map<std::string, outmap_spawn_list_t> map_list_t;       // List of maps with removed monsters

    // Global variables
    namespace global
    {
        // Base path to the mod .DLL
        std::string path = "";

        // The last tick of the COutMapCha::Run() function
        unsigned int outmapcha_tick = 0;

        // Monsters IDs list
        monsters_id_list_t monsters_id;

        // List of maps with removed monsters
        map_list_t outmap;

        // Monsters that will soon be spawned
        expected_spawn_list_t expected;
    }
}

// Load a list of monsters IDs to watch from file
void load_monsters_id(const std::string& path, pkodev::monsters_id_list_t& arr);

// Load a list of monsters that were killed from file
void load_outmap_list(const std::string& path, pkodev::outmap_spawn_list_t& arr);

// Save a list of monsters that were killed from file
void save_outmap_list(const std::string& path, const pkodev::outmap_spawn_list_t& arr);

// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get mod infromation
void GetModInformation(mod_info& info)
{
    strcpy_s(info.name,    TOSTRING(MOD_NAME));
    strcpy_s(info.version, TOSTRING(MOD_VERSION));
    strcpy_s(info.author,  TOSTRING(MOD_AUTHOR));
    info.exe_version =     MOD_EXE_VERSION;
}

// Start the mod
void Start(const char *path)
{
    // Make pretty monsters IDs string
    auto build_id_string = [](const pkodev::monsters_id_list_t& arr) -> std::string
    {
        // Max IDs in a list
        const unsigned int max_id{ 8 };

        // String with monsters IDs
        std::string str("");

        // Build the string
        for (unsigned int i = 0; i < arr.size() && i < max_id; ++i)
        {
            str += std::to_string(arr[i]) + std::string(", ");
        }

        // Remove the last comma
        str = str.substr( 0, (str.length() - 2) );

        // Make end of the string
        str += (arr.size() > max_id) ? std::string(" and " + std::to_string(arr.size() - max_id) + " more.") : std::string(".");

        return str;
    };

    // Buffer for path to .cfg file
    char buf[MAX_PATH]{ 0x00 };

    // Build path to .cfg file
    sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));

    // Load ID of monsters to watch
    load_monsters_id(buf, pkodev::global::monsters_id);

    // Write welcome message
    std::cout << "[" << TOSTRING(MOD_NAME) << "] Monsters respawn watcher ver. " << TOSTRING(MOD_VERSION) << " by " << TOSTRING(MOD_AUTHOR) << std::endl;

    // Print loaded monsters IDs
    std::cout << "[" << TOSTRING(MOD_NAME) << "] Loaded " << pkodev::global::monsters_id.size() << " monsters IDs to check: " << build_id_string(pkodev::global::monsters_id) << std::endl;

    // Save the base path
    pkodev::global::path = std::string(path);

    // Enable hooks
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pkodev::pointer::CGameConfig__Load, pkodev::hook::CGameConfig__Load);
    DetourAttach(&(PVOID&)pkodev::pointer::COutMapCha__Run, pkodev::hook::COutMapCha__Run);
    DetourAttach(&(PVOID&)pkodev::pointer::COutMapCha__ExecTimeCha, pkodev::hook::COutMapCha__ExecTimeCha);
    DetourAttach(&(PVOID&)pkodev::pointer::SubMap__ChaSpawn, pkodev::hook::SubMap__ChaSpawn);
    DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
    // Save list of monsters that were killed
    {
        // Temporary array
        char tmp[128]{ 0x00 };

        // Save respawns for each map to files
        for (auto const& map : pkodev::global::outmap)
        {
            // Check if current map has tracked dead monsters
           if (map.second.empty() == false)
           {
                // Build file name
                sprintf_s(tmp, sizeof(tmp), "%s\\data\\%s.%s.dat", pkodev::global::path.c_str(), TOSTRING(MOD_NAME), map.first.c_str());

                // Write respawn to the file
                save_outmap_list(tmp, map.second);
            }
        }
    }

    // Disable hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pkodev::pointer::CGameConfig__Load, pkodev::hook::CGameConfig__Load);
    DetourDetach(&(PVOID&)pkodev::pointer::COutMapCha__Run, pkodev::hook::COutMapCha__Run);
    DetourDetach(&(PVOID&)pkodev::pointer::COutMapCha__ExecTimeCha, pkodev::hook::COutMapCha__ExecTimeCha);
    DetourDetach(&(PVOID&)pkodev::pointer::SubMap__ChaSpawn, pkodev::hook::SubMap__ChaSpawn);
    DetourTransactionCommit();
}

// Load a list of monsters IDs to watch from file
void load_monsters_id(const std::string& path, pkodev::monsters_id_list_t& arr)
{
    // Clear old list
    arr.clear();

    // Open file
    std::ifstream fp(path, std::ifstream::in);

    // Check that file is open
    if ( fp.is_open() == true )
    {
        // Current line
        std::string line("");

        // Read the file line by line
        while ( std::getline(fp, line, '\n') )
        {
            // Try convert string to a number
            try
            {
                // Convert string to a number with ID
                unsigned int id = std::stoul(line);

                // Search ID in the list
                auto ret = std::find_if(
                    arr.begin(),
                    arr.end(),
                    [&id](const unsigned int& _id) -> bool { return (id == _id); }
                );

                // Check that ID doesn't exist in the list
                if ( ret == arr.end() )
                {
                    // Add ID to the list
                    arr.push_back(id);
                }
            }
            catch (...)
            {
                // Conversion error (std::invalid_argument, std::out_of_range)
                continue;
            }
        }

        // Close the file
        fp.close();
    }
}

// Load a list of monsters that were killed from file
void load_outmap_list(const std::string& path, pkodev::outmap_spawn_list_t& arr)
{
    // Clear old list
    arr.clear();

    // Open file
    std::ifstream fp(path, std::ifstream::in | std::ifstream::binary);

    // Check that file is open
    if ( fp.is_open() == true )
    {
        // Number of saved records
        unsigned int n = 0;

        // Read the number
        fp.read(reinterpret_cast<char*>(&n), sizeof(n));

        // Read records
        for (unsigned int i = 0; i < n; ++i)
        {
            // Temporary record
            pkodev::outmap_spawn tmp;

            // Read the record
            fp.read( reinterpret_cast<char*>(&tmp), sizeof(tmp) );

            // Add the record to the list
            arr.push_back(tmp);
        }

        // Close the file
        fp.close();
    }
}

// Save a list of monsters that were killed from file
void save_outmap_list(const std::string& path, const pkodev::outmap_spawn_list_t& arr)
{
    // Open file
    std::ofstream fp(path, std::ofstream::out | std::ofstream::binary);

    // Check that file if open
    if ( fp.is_open() == true )
    {
        // Number of records to save
        unsigned int n = arr.size();

        // Write the number
        fp.write(reinterpret_cast<const char*>(&n), sizeof(n));

        // Write records
        for (unsigned int i = 0; i < n; ++i)
        {
            // Write a record to the file
            fp.write( reinterpret_cast<const char*>(&arr[i]), sizeof(arr[i]) );
        }

        // Close the file
        fp.close();
    }
}

// bool CGameConfig::Load(char *pszFileName)
bool __fastcall pkodev::hook::CGameConfig__Load(void* This, void* NotUsed, const char* pszFileName)
{
    // Call original function CGameConfig::Load()
    bool ret = pkodev::pointer::CGameConfig__Load(This, pszFileName);

    // Check result
    if (ret == true)
    {
        // Temporary buffer
        char tmp[128]{ 0x00 };

        // Get pointer to the first map
        char* p = reinterpret_cast<char*>(
            reinterpret_cast<unsigned int>(This) + 0x01C4
        );
        
        // Loop over map list
        while (*p != 0x00)
        {
            // Build name of file with spawn records
            sprintf_s(tmp, sizeof(tmp), "%s\\data\\%s.%s.dat", pkodev::global::path.c_str(), TOSTRING(MOD_NAME), p);

            // Make new spawn records array for current map
            pkodev::outmap_spawn_list_t spawn;

            // Load the array from file
            load_outmap_list(tmp, spawn);

            // Insert the array to the list
            pkodev::global::outmap.insert( std::make_pair(p, spawn) );

            // Go to next map
            p = p + 0x20;
        }
    }
    
    return ret;
}

// void COutMapCha::Run(unsigned long ulCurTick)
void __fastcall pkodev::hook::COutMapCha__Run(void* This, void* NotUsed, unsigned int tick)
{
    // Call original function COutMapCha::Run()
    pkodev::pointer::COutMapCha__Run(This, tick);

    // Check that pending respawns list has records
    if ( pkodev::global::expected.empty() == true )
    {
        // The list is empty, nothing to do . . .
        return;
    }

    // Calculate the number of ticks since the last method call
    unsigned int tick_1s = ( tick - pkodev::global::outmapcha_tick );

    // Check that 1 second has passed
    if ( tick_1s >= 1000 )
    {
        // Save the current tick
        pkodev::global::outmapcha_tick = tick;

        // Check pending respawns
        pkodev::global::expected.erase(
            std::remove_if(
                pkodev::global::expected.begin(),
                pkodev::global::expected.end(),
                [](const pkodev::expected_spawn& respawn) -> bool
                {
                    // Check that the time of respawn has come
                    if ( ( std::time(nullptr) - respawn.die_time ) >= (respawn.revive_time / 1000 ) )
                    {
                        // Position of the respawn point
                        pkodev::point point(respawn.x, respawn.y);

                        // Monster name pointer
                        const char* name = nullptr;

                        // Check that monster has a name
                        if ( std::strlen(respawn.name) != 0 )
                        {
                            // Set name
                            name = respawn.name;
                        }

                        // Respawn the monster
                        pkodev::character* cha = pkodev::pointer::SubMap__ChaSpawn(
                            respawn.submap_pointer,
                            respawn.cha_id,
                            respawn.ctrl,
                            respawn.angle,
                            &point,
                            respawn.eyeshot,
                            name,
                            respawn.radius
                        );

                        // Check the pointer to the new monster
                        if (cha != nullptr)
                        {
                            // Set the time of respawn
                            cha->time = respawn.revive_time;
                        }

                        // Remove pending respawn from the list
                        return true;
                    }

                    // Waiting . . .
                    return false;
                }
            ),
            pkodev::global::expected.end()
        );
    }
}

// void COutMapCha::ExecTimeCha(SMgrUnit *pChaInfo)
void __fastcall pkodev::hook::COutMapCha__ExecTimeCha(void* This, void* NotUsed, void* pChaInfo)
{
    // Get monster's state
    unsigned int step = static_cast<unsigned int>(
        *reinterpret_cast<unsigned char*>(
            reinterpret_cast<unsigned int>(pChaInfo) + 0x228
        )
    );

    // We are interested in dead (1) and being respawned (2)
    if ( (step == 1) || (step == 2) )
    {
        // Get pointer to the current monster
        character* cha = reinterpret_cast<character*>(
            *reinterpret_cast<unsigned int*>(
                reinterpret_cast<unsigned int>(pChaInfo) + 0x0C
            )
        );

        // Look for the monster ID in the tracked ID list
        auto ret = std::find_if(
            pkodev::global::monsters_id.begin(),
            pkodev::global::monsters_id.end(),
            [&cha](const unsigned int& _id) -> bool
            {
                return (_id == cha->cha_id);
            }
        );

        // Check that the ID is in the list
        if ( ret != pkodev::global::monsters_id.end() )
        {
            // Get monster map name
            std::string map = pkodev::pointer::SubMap__GetName(
                reinterpret_cast<void *>(
                    *reinterpret_cast<unsigned int*>(
                        reinterpret_cast<unsigned int>(pChaInfo) + 0x14
                    )
                )
            );

            // Get reference to respawn list
            pkodev::outmap_spawn_list_t& spwans = pkodev::global::outmap[map];

            // Handle current monster state
            switch (step)
            {
                // Check that monster is dead (1)
                case 1:
                {

                    // Fill in the structure of the dead monster respawn
                    pkodev::outmap_spawn data;

                    // Fill the structure of the dead monster
                    data.id = cha->id;
                    data.cha_id = cha->cha_id;
                    data.pos_x = cha->spawn_x;
                    data.pos_y = cha->spawn_y;
                    data.revive_time = cha->time;
                    data.die_time = static_cast<unsigned int>(std::time(nullptr));

                    // Add the monster to the dead monsters list
                    spwans.push_back(data);

                } break;

                // Check that monster is being respawned (2)
                case 2:
                {

                    // Remove the monster from the dead monsters list
                    spwans.erase(
                        std::remove_if(
                            spwans.begin(),
                            spwans.end(),
                            [&cha](const pkodev::outmap_spawn& data) -> bool
                            {
                                return (data.id == cha->id);
                            }
                        ),
                        spwans.end()
                    );

                }  break;
            }
        }
    }

    // Call original function COutMapCha::ExecTimeCha()
    pkodev::pointer::COutMapCha__ExecTimeCha(This, pChaInfo);
}

// CCharacter* SubMap::ChaSpawn(Long lChaInfoID, Char chCtrlType, Short sAngle, Point *pSPos, bool bEyeshotAbility, dbc::cChar *cszChaName, const long clSearchRadius)
pkodev::character* __fastcall pkodev::hook::SubMap__ChaSpawn(void* This, void* NotUsed, unsigned int lChaInfoID,
    char chCtrlType,  short int sAngle, pkodev::point* pSPos, bool bEyeshotAbility, char* cszChaName, const int clSearchRadius)
{
    // Get current map name
    std::string map = pkodev::pointer::SubMap__GetName(This);

    // Get reference to respawn list
    pkodev::outmap_spawn_list_t& spwans = pkodev::global::outmap[map];

    // Search the monster in the dead monsters list
    auto ret = std::find_if(
        spwans.begin(),
        spwans.end(),
        [&lChaInfoID, pSPos](const pkodev::outmap_spawn& data) -> bool
        {
            return ( (data.cha_id == lChaInfoID) && (data.pos_x == pSPos->x) && (data.pos_y == pSPos->y) );
        }
    );

    // Check that monster is found
    if ( ret != spwans.end() )
    {
        // Check that the time of respawn has come
        if ( ( std::time(nullptr) - ret->die_time ) >= ( ret->revive_time / 1000 ) )
        {
            // Remove the monster from the list
            spwans.erase(ret);

            // Call original function SubMap::ChaSpawn()
            return pkodev::pointer::SubMap__ChaSpawn(This, lChaInfoID, chCtrlType, sAngle,
                pSPos, bEyeshotAbility, cszChaName, clSearchRadius);
        }
        else
        {
            // Cancel the spawning of the monster and put it on the pending list
            pkodev::expected_spawn respawn;

            // Fill the pending respawn data
            respawn.cha_id = lChaInfoID;
            respawn.ctrl = chCtrlType;
            respawn.angle = sAngle;
            respawn.x = pSPos->x;
            respawn.y = pSPos->y;
            respawn.eyeshot = bEyeshotAbility;
            respawn.radius = clSearchRadius;
            respawn.revive_time = ret->revive_time;
            respawn.die_time = ret->die_time;
            respawn.submap_pointer = This;

            // Check monster name
            if (cszChaName != nullptr)
            {
                // Save the name
                strcpy_s(respawn.name, sizeof(respawn.name), cszChaName);
            }
            else
            {
                // Make the name empty
                strcpy_s(respawn.name, sizeof(respawn.name), "");
            }

            // Remove the monster from the dead monsters list
            spwans.erase(ret);

            // Add pending respawn to the list
            pkodev::global::expected.push_back(respawn);

            // Do not spawn the monster
            return nullptr;
        }
    }

    // Call original function SubMap::ChaSpawn()
    return pkodev::pointer::SubMap__ChaSpawn(This, lChaInfoID, chCtrlType, sAngle,
        pSPos, bEyeshotAbility, cszChaName, clSearchRadius);
}
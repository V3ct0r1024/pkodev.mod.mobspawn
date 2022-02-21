#pragma once

namespace pkodev
{
	// Character structure
	struct character
	{
		// Offset 0
		char nop0[0x48];

		// Monster world ID [0x48]
		unsigned int id;

		// Offset 1
		char nop1[0x24];

		// The X coordinate of the spawn point [0x70]
		unsigned int spawn_x;

		// The Y coordinate of the spawn point [0x74]
		unsigned int spawn_y;

		// Offset 2
		char nop2[0x16C];

		// Name 0x1E4
		char name[0x20];

		// Offset 3
		char nop3[8];

		// Respawn time [0x20C]
		unsigned int time;

		// Offset 4
		char nop4[0x7C];

		// Character ID [0x28C]
		unsigned int cha_id;
	};

	// Point structure
	struct point
	{
		// Position (x, y)
		int x;
		int y;

		// Constructor
		point() :
			x(0), y(0) { }

		// Constructor		
		point(int x_, int y_) :
			x(x_), y(y_) { }
	};

	
#pragma pack(push, 1)

	// Respawn structure
	struct outmap_spawn
	{
		// Monster world ID
		unsigned int id;

		// Monster ID
		unsigned int cha_id;

		// Position X
		unsigned int pos_x;

		// Position Y
		unsigned int pos_y;

		// Time of respawn
		unsigned int revive_time;

		// Time of death
		unsigned int die_time;
	};
#pragma pack(pop)

	// Pending respawn structure
	struct expected_spawn
	{
		// Monster ID
		unsigned int cha_id;

		// Control type
		char ctrl;

		// Angle
		short int angle;

		// Position X
		unsigned int x;

		// Position Y
		unsigned int y;

		// Eyeshot type
		bool eyeshot;

		// Monster name
		char name[32];

		// Target search radius
		int radius;

		// Time of respawn
		unsigned int revive_time;

		// Time of death
		unsigned int die_time;

		// Pointer to SubMap object
		void* submap_pointer;
	};
}
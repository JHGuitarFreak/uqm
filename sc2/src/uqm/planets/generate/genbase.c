//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "genall.h"
#include "../lander.h"
#include "../planets.h"
#include "../../build.h"
#include "../../comm.h"
#include "../../globdata.h"
#include "../../nameref.h"
#include "../../setup.h"
#include "../../sounds.h"
#include "../../state.h"
#include "libs/mathlib.h"

static bool GenerateBase_generatePlanets (SOLARSYS_STATE *solarSys);
static bool GenerateBase_generateMoons (SOLARSYS_STATE *solarSys,
		PLANET_DESC *planet);
static bool GenerateBase_generateOrbital (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world);
static COUNT GenerateBase_generateEnergy (const SOLARSYS_STATE *,
		const PLANET_DESC *world, COUNT whichNode, NODE_INFO *);
static bool GenerateBase_pickupEnergy (SOLARSYS_STATE *solarSys,
		PLANET_DESC *world, COUNT whichNode);

// This struct contains all the functions necessary to generate your star system
const GenerateFunctions generateStarBaseFunctions = {
	/* .initNpcs         = */ GenerateDefault_initNpcs,
	/* .reinitNpcs       = */ GenerateDefault_reinitNpcs,
	/* .uninitNpcs       = */ GenerateDefault_uninitNpcs,
	/* .generatePlanets  = */ GenerateBase_generatePlanets,
	/* .generateMoons    = */ GenerateBase_generateMoons,
	/* .generateName     = */ GenerateDefault_generateName,
	/* .generateOrbital  = */ GenerateBase_generateOrbital,
	/* .generateMinerals = */ GenerateDefault_generateMinerals,
	/* .generateEnergy   = */ GenerateBase_generateEnergy,
	/* .generateLife     = */ GenerateDefault_generateLife,
	/* .pickupMinerals   = */ GenerateDefault_pickupMinerals,
	/* .pickupEnergy     = */ GenerateBase_pickupEnergy,
	/* .pickupLife       = */ GenerateDefault_pickupLife,
};


static bool
GenerateBase_generatePlanets (SOLARSYS_STATE *solarSys)
{
	GenerateDefault_generatePlanets (solarSys); // Procedurally generates the planets for this system based on star location and type

	solarSys->PlanetDesc[0].data_index = SHATTERED_WORLD; // Type of world for the specified planet in array
	solarSys->PlanetDesc[0].NumPlanets = MAX_MOONS; // Number of moons for specific planet in the array [MAX_MOONS is 4]

	return true;
}

static bool
GenerateBase_generateMoons (SOLARSYS_STATE *solarSys, PLANET_DESC *planet)
{
	GenerateDefault_generateMoons (solarSys, planet); 

	// matchWorld (solarSys, planet, Which_Planet, Which_Moon_of_Planet)
	// alternatively instead of Which_Moon you can just put "MATCH_PLANET"
	// to only match the planet itself and not the moons
	if (matchWorld (solarSys, planet, 0, MATCH_PLANET))
	{
		solarSys->MoonDesc[3].data_index = DESTROYED_STARBASE; // Type of world for the specified moon
	}

	return true;
}

static bool
GenerateBase_generateOrbital (SOLARSYS_STATE *solarSys, PLANET_DESC *world)
{
	// matchWorld (solarSys, planet, Which_Planet, Which_Moon_of_Planet)
	// alternatively instead of Which_Moon you can just put "MATCH_PLANET"
	// to only match the planet itself and not the moons
	if (matchWorld (solarSys, world, 0, 3))
	{	// Since we're in orbit we have to load the lander reports manually
		/* Starbase */
		LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo); // Loads the lander font
		solarSys->SysInfo.PlanetInfo.DiscoveryString =
			CaptureStringTable (LoadStringTable (STARBASE_RUINS_STRTAB)); // Loads the text file for reading

		DoDiscoveryReport (MenuSounds); // Loads the lander report UI and reads the report

		// This destroys all things lander report related that we loaded to free up memory
		DestroyStringTable (ReleaseStringTable (
			solarSys->SysInfo.PlanetInfo.DiscoveryString));
		solarSys->SysInfo.PlanetInfo.DiscoveryString = 0;
		FreeLanderFont (&solarSys->SysInfo.PlanetInfo);

		return true;
	}

	if (matchWorld(solarSys, world, 0, MATCH_PLANET))
	{
		LoadStdLanderFont (&solarSys->SysInfo.PlanetInfo); // Loads the lander font
		solarSys->PlanetSideFrame[1] =
				CaptureDrawable (LoadGraphic (RUINS_MASK_PMAP_ANIM)); // Loads the images for the planetary ruins
		solarSys->SysInfo.PlanetInfo.DiscoveryString =
				CaptureStringTable (
						LoadStringTable (SHATTERED_RUINS_STRTAB)); // Loads the text file for reading
	}

	GenerateDefault_generateOrbital (solarSys, world);

	return true;
}

static bool
GenerateBase_pickupEnergy (SOLARSYS_STATE *solarSys, PLANET_DESC *world,
	COUNT whichNode)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET))
	{	// Does what it says on the tin, generates lander report UI that displays
		// the lander report specified in GenerateBase_generateOrbital
		// when it matches the correct planet
		GenerateDefault_landerReportCycle (solarSys);

		SetLanderTakeoff ();

		return false; // do not remove the node
	}

	(void) whichNode;
	return false;
}

static COUNT
GenerateBase_generateEnergy (const SOLARSYS_STATE *solarSys,
		const PLANET_DESC *world, COUNT whichNode, NODE_INFO *info)
{
	if (matchWorld (solarSys, world, 0, MATCH_PLANET))
	{	// This generates ruins on the matched planet
		return GenerateDefault_generateRuins (solarSys, whichNode, info);
	}

	return 0;
}

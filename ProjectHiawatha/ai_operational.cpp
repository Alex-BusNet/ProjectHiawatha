//Current Priority Setup

/*

Control of overall unit placement

    Determines if it needs to be using the unit offensively (at war)
        if not at war, determine if it should be positioning
            offensively(prep for war)
            defensively(based on 'enemy'(rival player, etc, not necessarily at war) position
        if at war, determine a priority target (nearest unit or city)
            determine if a buildup of troops is needed before an attack (combat strengths)
            Probably target nearest opponent city to our borders/any units in the way/any within our borders
                pathing may play into this



*/

#include "ai_operational.h"

AI_Operational::AI_Operational()
{

}



//AI_Operational(Bool AtWar, int Citiesfounded);
        //threatScan(Vector* Highthreats, Vector* Midthreats, Vector* Lowthreats);
//theaterAtWar();
//theaterPrep();
//cityLocation(Vector* citylocations);

//****************Tactical AI Called**************
//if(AtWar)
    //Pass vector of unit targets and city
    //also passes the city founding vector and 3 vectors of threatening units
//else
    //Pass individual position vector for each unit class
    //also passes the city founding vector and 3 vectors of threatening units





//threatScan(Vector* Highthreats, Vector* Midthreats, Vector* Lowthreats);
//************Threat Vector*************
//lists all enemy units which are within 1 of teritory or within 5 of units.
    //prioritizes within teritory/within 2 of units
    //mid priority is within 4 of units/just outside territory
    //low priority is 5 away
        //priority determines if units targeting it are strong, neutral, or weak to it
    //adds each to appropriate vector pointer


//************Theater of War***************
//theaterAtWar();
//Prioritizes the nearest city (by accessibility?) of the player civ
//If at war, sends immediately to priority targets
        //1 city at a time
            //always priority for siege units unless directly threatened
        //Units which pose a threat to borders or to forces actively invading the enemy city
            //targets threatening units with whatever they are weak to first, then neutral, and only strong against as last resort


//************Theater of War(prep)***************
//theaterPrep();
//If preparing for war, sends units in the general direction of opponent
        //Lays out a set of positions which are acceptable for unit types to station
            //infantry will be in front of siege, etc
        //careful not to be too obvious - Fog of War buffer zone?


//cityLocation(Vector* citylocations);
//***********City Founding***************
//initially prioritize up to 20 locations for city founding sites, starting with nearby and working out
//re-evaluate and remove locations that become another civ's territory
    //Update vector pointer with results

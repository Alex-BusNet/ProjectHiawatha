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



//***********City Founding***************
//initially prioritize up to 20 locations for city founding sites, starting with nearby and working out
//re-evaluate and remove locations that become another civ's territory
//each time a settler appears, ready for commands, it will direct it to the highest priority site without a city founded
        //or a settler en route




//************Theater of War***************
//Prioritizes the nearest city (by accessibility?) of the player civ
//Targets enemy units which are nearby

//If preparing for war, sends units in the general direction of opponent
        //Lays out a set of positions which are acceptable for unit types to station
            //infantry will be in front of siege, etc
        //careful not to be too obvious - Fog of War buffer zone?

//If at war, sends immediately to priority targets
        //unit pathing will check the top 5 or so targets
            //1 city at a time
                //always priority for siege units unless directly threatened
            //Units which pose a threat to borders or to forces actively invading the enemy city
                //targets threatening units with whatever they are weak to first, then neutral, and only strong against as last resort


//****************Tactical AI Called**************
//Pass either vectors of positions OR vector of unit targets and city(2 seperate constructors??)
    //individual position vector for each unit class
//also passes the city founding vector




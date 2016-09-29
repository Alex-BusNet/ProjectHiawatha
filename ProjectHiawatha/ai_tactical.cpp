//Individual unit Control


#include "ai_tactical.h"



AI_Tactical::AI_Tactical()
{

}
// is the combat handler a seperate class? Will that class be responsible for outcome estimates?




/*Reads the ultimate objective (victory type)
Determines if unit is military or worker/caravan/settler type
    if caravan, sends on trade route
    if worker, prioritizes resources and orders to improve
    if settler, read from strategic and set a target tile
    else, if unit warlike:
        read from operational AI (may be merged into this class)
            move units appropriately
            attack if combat simulator suggests victory (or acceptable loss, etc)


            this class is NOT responsible for determining best path to objective

    _____________Operational AI_____________
    Control of overall unit placement

        Determines if it needs to be using the unit offensively (at war)
            if not at war, determine if it should be positioning
                offensively(prep for war)
                defensively(based on 'enemy'(rival player, etc, not necessarily at war) position
            if at war, determine a priority target (nearest unit or city)
                determine if a buildup of troops is needed before an attack (combat strengths)

                determines appropriate location for a unit to be (but not pathing?)
  __________________end________________________




  */

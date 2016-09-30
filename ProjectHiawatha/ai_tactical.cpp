//Individual unit Control


// is the combat handler a seperate class? Will that class be responsible for outcome estimates?


/*
Determines if unit is military or worker/settler type
    if worker, prioritizes resources and orders to improve
    if settler, read from strategic and set a target tile
        Probably a spiral outward from capital, with corrections for water, mountains, etc
    else, if unit warlike:
        read from operational AI (may be merged into this class)
            move units appropriately
            attack if combat simulator suggests victory (or acceptable loss, etc)


            this class is NOT responsible for determining best path to objective


  */


#include "ai_tactical.h"



AI_Tactical::AI_Tactical()
{

}



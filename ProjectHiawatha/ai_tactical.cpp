//Individual unit Control


// is the combat handler a seperate class? Will that class be responsible for outcome estimates?


/*
determines appropriate location for a unit to be (but not pathing)

Determines if unit is military or worker/settler type
    if worker, prioritizes resources and orders to improve
    if settler, read from strategic and set a target tile
        Probably a spiral outward from capital, with corrections for water, mountains, etc
    else, if unit warlike:
        read from operational AI (may be merged into this class)
            move units appropriately
            attack if combat simulator suggests victory (or acceptable loss, etc)





  */


#include "ai_tactical.h"



AI_Tactical::AI_Tactical()
{

}


//****************Worker Control***************
//Unassigned but garrisoned workers will be targeted to the tile closest to the city
    //starting to the top right and clockwise spiral out
    //and will perform the 'ideal' improvement
//the roadworker will be given a city to connect to if not already working
    //should always be in a city OR building roads (unless captured back from barbarians??)


//*****************Settler Control**************
//Sends a settler to the highest priority city site in the queue
    //need a way to identify that a settler is heading there already
//Found city when arrived
    //remove from queue of locations
    //active_settlers --
//if a settler exists after all city locations in queue have been used, searches radially for suitable site
    //adds the site to the queue for eval purposes
        //most likely scenario if last city site is taken by others after settler is already built



//*****************Military Unit control (Prep)***************
//fills in the vectors of acceptable positions with appropriate unit
    //first slot in vector = priority placement




//****************Military Unit Control (At War)******************
//Targets each unit toward the opposing city
    //UNLESS a unit directly threatens(1 tile?) that unit
    //OR an enemy unit is near-ish (3 tiles?) that is neutral to this unit
    //OR mid-near (5 tiles?) that is weak to this unit

        //The logic will consider city as weak to siege
        //and the other appropriate weaknesses (armor weak to AT, etc)



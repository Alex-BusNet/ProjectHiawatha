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
                maybe not, kind of go berserk, but use ones that are 'strong' against FIRST
  */


#include "ai_tactical.h"



AI_Tactical::AI_Tactical()
{

}

AI_Tactical::AI_Tactical(int midGoal, Civilization *civ, QVector<Tile *> CityToBeFounded, City *cityTarget, QVector<Tile *> TroopPositions, QVector<Tile *> highThreats, QVector<Tile *> midThreats, QVector<Tile *> lowThreats)
{
    highThreatProcessing(highThreats);
    midThreatProcessing(midThreats);
    lowThreatProcessing(lowThreats);

    if(3==midGoal){
        AtWar(civ, cityTarget);
    }
    else{
        Prep(civ, TroopPositions);
    }

    settlercontrol(CityToBeFounded);
    workercontrol();
}



void AI_Tactical::Prep(Civilization *civ, QVector<Tile *> TroopPositions)
{
    //Scroll through a vector of the military units,
        //check each to see if it has moves remaining
        //direct it into an appropriate tile

}



//*****************Military Unit control (Prep)***************
    //fills in the vectors of acceptable positions with appropriate unit
        //first slot in vector = priority placement
            //while units remain
            //logic to put melee units in front, etc?





void AI_Tactical::AtWar(Civilization *civ, City *cityTarget)
{
    //Scroll through a vector of the military units,
        //check each to see if it has moves remaining
            //direct it toward the target city
                //Have each unit attack from its max range - that way the melee units will be able to get in front.
                //Attack with ranged units first

}

    //****************Military Unit Control (At War)******************
    //Targets each remaining unit toward the opposing city
        //while units remain
            //The logic will consider city as weak to siege
            //and the other appropriate weaknesses (armor weak to AT, etc)




void AI_Tactical::highThreatProcessing(QVector<Tile *> highThreats){
    //Scroll through a vector of the military units,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it has moves remaining to attack in the next 3 turns
                //Check if strong against enemy
                    //if yes, attack
                    //Remove enemy from vector if killed
    //if any enemies remain in vector,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it enough moves remaining to attack this or next turn
                //Check if strong or neutral against enemy
                    //If yes, attack
                    //Remove enemy from vector if killed
    //if any enemies remain in vector,
        //check each unit see if it enough moves remaining to attack this turn
                //If yes, attack
                //Remove enemy from vector if killed
}
    //Attacks all these targets, prioritizing units that are strong against
        //keeps going until destroyed or all military units out of moves
        //each unit will target the closest remaining high threat
            //some sort of modifier for distance, but not so much that an attack on the opposite side of continent is ignored
                //maybe have a minimum quantity of responding units? 2/attacker?
            //remove enemy from vector if killed


void AI_Tactical::midThreatProcessing(QVector<Tile *> midThreats){
    //Scroll through a vector of the military units,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it has moves remaining to attack this or next turn
                //Check if strong against enemy
                    //if yes, attack
                    //Remove enemy from vector if killed
    //if any enemies remain in vector,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it enough moves remaining to attack this turn
                //Check if strong or neutral against enemy
                    //If yes, attack
                    //Remove enemy from vector if killed
}
    //Attacks all these targets, but uses only units which are strong or neutral to them
        //keeps going until destroyed or all strong/neutral military units out of moves
        //each appropriate unit will target the closest remaining mid threat (unless more than 3 turns away)
            //remove enemy from vector if killed

void AI_Tactical::lowThreatProcessing(QVector<Tile *> lowThreats){
    //Scroll through a vector of the military units,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it has moves remaining to attack this turn
                //Check if strong against enemy
                    //if yes, attack
                    //Remove enemy from vector if killed
}
    //Attacks all these targets, but uses only units which are strong to them
        //keeps going until destroyed or all strong military units out of moves
            //each appropriate unit will target the closest remaining low threat (unless more than 2 turns away)
            //remove enemy from vector if killed


void AI_Tactical::settlercontrol(QVector<Tile *> CityToBeFounded){
    //For each settler
        //Target Path to (i) city in list
        //if it is at the location with moves left, found city
            //remove city from vector
            //may need to remove from a number of active settlers calculation
        //if no cities left in list, use the search function that was originally used to find a new location
            //add site to vector

}
    //*****************Settler Control**************
    //Sends a settler to the highest priority city site in the queue
        //need a way to identify that a settler is heading there already - queue pending_city?
    //Found city when arrived
        //remove from queue of locations
        //active_settlers --
    //if a settler exists after all city locations in queue have been used, searches radially for suitable site
        //adds the site to the queue for eval purposes
            //most likely scenario if last city site is taken by others after settler is already built





void AI_Tactical::workercontrol(){

    //for each worker
        //if garrissoned and not already improving a tile
            //start to the right of city and circle out within the city's tiles
                //when an unimproved tile is found, start improving it('ideal' improvement logic?)
        //else (non-garrisonned workers should always be road workers
            //if not already building a road
                //target a city not currently connected to capitol (bool val for this??)
}
    //****************Worker Control***************
    //Unassigned but garrisoned workers will be targeted to the tile closest to the city
        //starting to the top right and clockwise spiral out
        //and will perform the 'ideal' improvement
    //the roadworker will be given a city to connect to if not already working
        //should always be in a city OR building roads









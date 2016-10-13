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

AI_Tactical::AI_Tactical(int midGoal, Civilization *civ, QVector<Tile *> CityToBeFounded, City Target, QVector<Tile *> TroopPositions, QVector<Tile *> Highthreats, QVector<Tile *> Midthreats, QVector<Tile *> Lowthreats)
{
    if(3==midGoal){
        AtWar(civ, CityToBeFounded, Target, Highthreats, Midthreats, Lowthreats);
    }
    else{
        Prep(civ,CityToBeFounded, TroopPositions, Highthreats, Midthreats, Lowthreats);
    }
}



void AI_Tactical::Prep(Civilization *civ, QVector<Tile*> CityToBeFounded, QVector<Tile *> TroopPositions, QVector<Tile *> Highthreats, QVector<Tile *> Midthreats, QVector<Tile *> Lowthreats)
{

}





//highThreats(Vector Highthreats);
//midThreats(Vector Midthreats);
//lowThreats(Vector Lowthreats);
//*****************Military Unit control (Prep)***************
    //fills in the vectors of acceptable positions with appropriate unit
        //first slot in vector = priority placement
            //while units remain
//settlercontrol(Queue CityToBeFounded);
//workercontrol();




void AI_Tactical::AtWar(Civilization *civ, QVector<Tile*> CityToBeFounded, City Target, QVector<Tile *> Highthreats, QVector<Tile *> Midthreats, QVector<Tile *> Lowthreats)
{

}
//highThreats(Vector Highthreats);
//midThreats(Vector Midthreats);
//lowThreats(Vector Lowthreats);
    //****************Military Unit Control (At War)******************
    //Targets each unit toward the opposing city
        //while units remain
            //The logic will consider city as weak to siege
            //and the other appropriate weaknesses (armor weak to AT, etc)
//settlercontrol(Queue CityToBeFounded);
//workercontrol();



//highThreats(Vector Highthreats);
    //Attacks all these targets, prioritizing units that are strong against
        //keeps going until destroyed or all military units out of moves
        //each unit will target the closest remaining high threat
            //some sort of modifier for distance, but not so much that an attack on the opposite side of continent is ignored
                //maybe have a minimum quantity of responding units? 2/attacker?
            //remove enemy from vector if killed

//midThreats(Vector Midthreats);
    //Attacks all these targets, but uses only units which are strong or neutral to them
        //keeps going until destroyed or all strong/neutral military units out of moves
        //each appropriate unit will target the closest remaining mid threat (unless more than 3 turns away)
            //remove enemy from vector if killed

//lowThreats(Vector Lowthreats);
    //Attacks all these targets, but uses only units which are strong to them
        //keeps going until destroyed or all strong military units out of moves
            //each appropriate unit will target the closest remaining low threat (unless more than 2 turns away)
            //remove enemy from vector if killed


//settlercontrol(Queue CityToBeFounded);
    //*****************Settler Control**************
    //Sends a settler to the highest priority city site in the queue
        //need a way to identify that a settler is heading there already - queue pending_city?
    //Found city when arrived
        //remove from queue of locations
        //active_settlers --
    //if a settler exists after all city locations in queue have been used, searches radially for suitable site
        //adds the site to the queue for eval purposes
            //most likely scenario if last city site is taken by others after settler is already built





//workercontrol();
    //****************Worker Control***************
    //Unassigned but garrisoned workers will be targeted to the tile closest to the city
        //starting to the top right and clockwise spiral out
        //and will perform the 'ideal' improvement
    //the roadworker will be given a city to connect to if not already working
        //should always be in a city OR building roads (unless captured back from barbarians??)
            //Thinking barbarians should just kill workers









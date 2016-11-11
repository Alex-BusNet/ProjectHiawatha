//City Control


/*
  Determine if more cities are needed
        but only if available location
        Probably founds cities in a sort of outward spiral from capital
            stops when it has one within x spaces of another civ?

  Determine if workers are needed (1 worker per city, 1 extra dedicated to roads, work boats?)

  Determine if military units are needed (at war or ultimate objective is war-based)(and which units)

  Determine if structures are needed (and which)
        - probably set a static order
        - then give, say, defensive structures priority if under invasion, etc

*/

#include "ai_strategic.h"
#include "civilization.h"
#include "city.h"
#include "yield.h"
#include "ai_controller.h"
#include <QDebug>

AI_Strategic::AI_Strategic(Civilization *civ, Civilization *player, Map *map, GameScene *scene)
{
qDebug()<<"     Strategic AI Called";

    int midGoal = midTermGoal(civ);
    //Some logic based on the different goal options
    cityProduction(midGoal, civ);
qDebug()<<"Midgoal "<<midGoal;
    AI_Operational(midGoal, civ, player, map, scene);
    //****************Operational AI called**************
    //Operational AI will control military strategy and city founding
    //Pass it whether or not civ is preparing for / at war (midgoal)
    //Pass it number of cities currently founded (can be got from civ)
}


int AI_Strategic::midTermGoal(Civilization *civ){
    int goal;
    if(civ->GetCityList().length()<10){
        goal=1;//Settle more cities
        //Tweak for settlers currently active
    }
    else if(civ->GetCityList().length()>4){
        //needs logic to compare tech, gold, and prod yields with player
        //May be made obscolete by goal 3 logic
        goal=2;//Preparing for War
    }
    else if(0){
        //Needs to be a flag set once player has provoked the AI (Probably set by threatscan)
        goal=3;//At War
    }
    else{
        goal=4;//Build resources
    }
    qDebug()<<"Goal = "<<goal;
    return goal;
}

//*****************Calculate mid-term goal************
//The mid-term goal will be: (int returned)
//1)expanding its borders (settling)
//2)Preparing for war
//3)at war
//4)Building up its resources (working, but no longer settling)(usually also happening during expanding borders)

//Should spend the early game expanding borders/resources
//Determine if more cities are needed
      //(factor settlers already around, then city? but only if available location?)(city 0 only?)
//at some point it will decide its ready to prepare for war
    //given a minimum city number, say, 4,
    //and an estimate that it has higher production and tech level than player (by enough)
//Once it has built enough units, it will launch into at war and begin maneuvering them
    //continue to build units while at war, unless its strength drops (significantly?) below the opposing player
    //then it will retreat and try to improve production etc by a margin over the player and try again



void AI_Strategic::cityProduction(int midGoal, Civilization *civ){
    qDebug()<<"City Production";
    for(int i =0;i < civ->GetCityList().length(); i++){

        //if(NULL==cityproduction){/Determine if city is currently building something
            if(1==midGoal){//Settle more cities
                if(0==i){//Only first city builds settlers - logistical parameter


                    //civ->GetCityAt(i)->setProductionName("Settler");
                    //This only sets the name, not the production. Need to determine the proper production function
                    qDebug()<<"     Settler";
                    //Set city to build settler

                    //Not currently producing units?
                }
                else{

                    //Should have Cities+1 workers
                    //logic for garrisoned and road workers
                        //If worker is needed, should be built in the city that doesn't have a garrisoned worker
                        //roadworker gets built in capitol early on
                    //Then build buildings
                    //Minimum military strength(defensive)
                }
            }
            else if(2==midGoal||3==midGoal){
                //Build military units
            }
            else{
                //Build workers, as needed, then buildings
                //Minimum military strength(defensive)
            }
        //}
    }
}
//*************City production Decision Tree***********
//prioritizes military if it is ready for war, then expansion, then having a worker, then buildings
//should cycle through each city, and then run this calculation only if nothing is being produced in that city

//something like
//while(city.hasnext){
        //if(city.production==NULL){

//if (((cities+active_settlers+pending_settlers)<10)&&((happiness-(2*active_settlers))>2))
        //city(1) build settler (city(1) should be the capital)
        //pending_settlers++
                //pending settlers tracks settlers being built, and has a -- when one is completed
                //active_settlers is settlers currently seeking a city site, -- if killed or founds city
                //2 happiness is arbitrary, should reflect the number of happiness a new city loses
                //also needs to factor active_settlers into happiness calculation for cities about to be founded
//else if(roadworker==false)||(city.garissonedworker==0)
        //build worker
                //civ should have 1 worker per city and a roadbuilder
                //worker will be garrisoned in the city and can work on any tile in the city's borders
//else if(needs military units)
                //calculations of relative military strength,
                //what units are pending
                //etc.
                //should always have some sort of minimum strength
                //builds a lot when preparing for/at war
//else if(buildings needed)
                //calculate what buildings are needed
                //Priority: 1)Food, 2)Production 3)Gold 4)Science 5)Military
                //hypothetically
                //maybe prioritize happiness until it finishes its 15 cities










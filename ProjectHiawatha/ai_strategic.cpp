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

AI_Strategic::AI_Strategic(Civilization *civ, Civilization *player, Map *map)
{
qDebug()<<"     Strategic AI Called";

    civ->clearThreats();
    invasionCheck(civ,player,map);

    int midGoal = midTermGoal(civ);
    //Some logic based on the different goal options
    cityProduction(midGoal, civ, map);
    qDebug()<<"Midgoal "<<midGoal;
    aiOp = new AI_Operational(midGoal, civ, player, map);
    delete aiOp;
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
        qDebug()<<"********Broken Goal Logic here";
        goal=2;//Preparing for War
    }
    else if(civ->getProvoked()){
        //Needs to be a flag set once player has provoked the AI (set by InvasionCheck)
        //might need moved ahead of goal 1
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



void AI_Strategic::cityProduction(int midGoal, Civilization *civ, Map* map){
    qDebug()<<"City Production";

    bool activeSettler = false;
    for(int i = 0;i< civ->GetUnitList().length();i++){
        if("Settler"==civ->GetUnitAt(i)->GetName()){
            activeSettler=true;
        }
    }//Checks if there is already a settler around


    for(int i =0;i < civ->GetCityList().length(); i++){

        //if(NULL==cityproduction){/Determine if city is currently building something
            if(1==midGoal){//Settle more cities
                if(0==i&&!activeSettler){//Only first city builds settlers - logistical parameter
                    //Logic to only build 1 settler at a time

                    ///For debugging purposes, Settler production has been set to 10. This will need to be reset to 100.
                    civ->GetCityAt(i)->setCurrentProductionCost(10);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Settler");
                    civ->GetCityAt(i)->setProductionIndex(3);

//                    qDebug()<<"ACCUMULATED PRODUCTION: "<<civ->GetCityList().at(i)->getAccumulatedProduction();
//                    qDebug()<<"PRODUCTION COST: "<<civ->GetCityList().at(i)->getCurrentProductionCost();

                    qDebug()<<"     Settler";
                    //Set city to build settler

                }
                else if(civ->getProvoked()){
                    civ->GetCityAt(i)->setCurrentProductionCost(425);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Modern Armor");
                    civ->GetCityAt(i)->setProductionIndex(31);
                    qDebug()<<"     Modern Armor";
                }
                else if(!civ->GetCityAt(i)->getHasWorker()){
                    civ->GetCityAt(i)->setCurrentProductionCost(70);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Worker");
                    civ->GetCityAt(i)->setProductionIndex(6);
                    qDebug()<<"     Worker";
                }
                else{
                    civ->GetCityAt(i)->setCurrentProductionCost(75);
                    civ->GetCityAt(i)->setIsUnit(false);
                    civ->GetCityAt(i)->setProductionName("Walls");
                    civ->GetCityAt(i)->setProductionIndex(0);
                    qDebug()<<"walls";
                    //logic for road workers
                        //roadworker gets built in capitol early on
                }
            }
            else if(2==midGoal||3==midGoal){
                civ->GetCityAt(i)->setCurrentProductionCost(425);
                civ->GetCityAt(i)->setIsUnit(true);
                civ->GetCityAt(i)->setProductionName("Stealth Bomber");
                civ->GetCityAt(i)->setProductionIndex(32);
                qDebug()<<"     Stealth Bomber";
                //Build military units
            }
            else{
                //buildings and Minimum military strength(defensive)
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

//if (cities+active_settlers+pending_settlers)<10)
        //city(0) build settler (city(0) should be the capital)
        //1 settler at a time
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
                //Priority: 1)Food, 2)Production 3)Science 4)Military 5)Gold
                //hypothetically
                //maybe prioritize happiness until it finishes its 15 cities



void AI_Strategic::invasionCheck(Civilization *civ, Civilization *player, Map *map)
{
    for(int i = 0; i<player->GetUnitList().length(); i++){

        if(map->GetTileAt(player->GetUnitAt(i)->GetTileIndex())->GetControllingCiv()==civ->getCiv()){

            //a tile queue already holds all of a civ's adjacent tiles
            //So a scan of those tiles would also be good, for tier 2 targets

            qDebug()<<"Invasion";
            civ->setProvoked(true);
            QVector<Unit*> tempVec = civ->getHighThreats();
            tempVec.push_back(player->GetUnitAt(i));
            civ->setHighThreats(tempVec);
        }
        else{

        }
    }
}

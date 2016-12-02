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
#include "tile.h"
#include "unitcontroller.h"
#include <qdebug.h>

AI_Operational::AI_Operational()
{

}



AI_Operational::AI_Operational(QVector<Tile *> CityToBeFounded, Civilization *civ, Civilization *player, Map *map)
{
    qDebug()<<"         Operational AI Called";

    threatScan(civ, player, map);

    if(!civ->isAtWar()){
        qDebug()<<"AI_Ops Midgoal 1";
        //theaterAtWar(civ, player, cityTarget, map);
        //Probably not anything for operational in this context, aside from threat detection
        //midgoal==2 is resource gathering - run theaterprep?
        theaterPrep(civ, player);
    }
    else{
        theaterAtWar(civ, player, cityTarget, map);
    }

    aiTact = new AI_Tactical(civ, player, map, CityToBeFounded, cityTarget);
}


//****************Tactical AI Called**************
    //Pass target city
    //also passes the city founding vector and 3 vectors of threatening units
    //Pass position vector for military units





void AI_Operational::threatScan(Civilization *civ, Civilization *player, Map *map)
{
    qDebug()<<"Threatscan";
    //Clear threats each time, else they remain after the unit dies

    UnitController* unitCon = new UnitController();
    Unit* unit;
    //Check tiles near cities
        for(int i = 0; i<civ->GetCityList().length();i++){
             QVector<Tile*> borderingTiles=civ->GetCityAt(i)->tileQueue;

             for(int j=0; j<borderingTiles.length();j++){

                 if(0==borderingTiles.at(j)->GetCivListIndex()){
                    qDebug()<<"Enemy Near";
                    QVector<Unit*> tempVec = civ->getMidThreats();
                    unit = unitCon->FindUnitAtTile(borderingTiles.at(j),map,player->GetUnitList());
                    tempVec.push_back(unit);
                    civ->setMidThreats(tempVec);
                    qDebug()<<"Unit: "<<unit->GetTileIndex();
                 }
            }
            qDebug()<<"Enemy in adjacent tile check";

    //Using the Map::GetNeighbors(Tile *node) algorithm
        //search radially out from controlled tiles
            //Units within 1 of territory added to midThreats
    //Use same algorithm from vector of military units
        //if enemy found within 2 of units, add to highThreats
        //else if found within 3 of units, add to midThreats
        //else if within 5 add to lowThreats
        }
}

//************Threat Vector*************
//lists all enemy units which are within 1 of teritory or within 5 of units.
    //prioritizes within teritory/within 2 of units
    //mid priority is within 4 of units/just outside territory
    //low priority is 5 away
        //priority determines if units targeting it are strong, neutral, or weak to it
    //adds each to appropriate vector pointer
//Class the target city in this as well, so that some units will almost always prioritize it?
//See Map::GetNeighbors(Tile *node) for scanning algorithm



void AI_Operational::theaterAtWar(Civilization *civ, Civilization *player, City *cityTarget, Map *map){
    qDebug()<<"Theater At War";
    int targetIndex=0, targetDistance=INT_MAX;
    UnitController *checkDist;
    for(int i =0; i< player->GetCityList().length();i++){
        if(checkDist->GetDistance(civ->GetCityAt(0)->GetCityTile(),player->GetCityAt(i)->GetCityTile())<targetDistance){
            targetIndex=i;
            targetDistance=checkDist->GetDistance(civ->GetCityAt(0)->GetCityTile(),player->GetCityAt(i)->GetCityTile());
        }
    }
    this->cityTarget=player->GetCityAt(targetIndex);
}
//************Theater of War***************
//Prioritizes the nearest city (by accessibility?) of the player civ
//If at war, sends x units immediately to priority targets
        //1 city at a time
//scan outward for nearest player city
    //Sets it as city target
//Probably just easiest to use the findpath from AI capitol to each player city
//and return the closest from there
//Apropriate location??????
//always priority for siege units unless directly threatened
        //Units which pose a threat to borders or to forces actively invading the enemy city
            //targets threatening units with whatever they are weak to first, then neutral, and only strong against as last resort



void AI_Operational::theaterPrep(Civilization *civ, Civilization *player){

    //Might be partially obsolete because of provocation mechanic

    //Scans for player's nearest city
        //Locates open tiles within civ's borders that are near the enemy city
            //Maybe does the rotational scan out from the located city?
            //adds tiles within ai's civ to vector, starting with nearest to enemy city
}
//************Theater of War(prep)***************
//If preparing for war, sends units in the general direction of opponent's nearest city
        //Lays out a set of positions which are acceptable for unit types to station

//Location???
            //infantry will be in front of siege, etc
        //careful not to be too obvious - Fog of War buffer zone?



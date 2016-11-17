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



AI_Operational::AI_Operational(int midGoal, Civilization *civ, Civilization *player, Map *map)
{
    qDebug()<<"         Operational AI Called";

    threatScan(civ, player, map);

//    if(!settlerActive)
        cityLocation(civ, map);

    if(1==midGoal){
        qDebug()<<"AI_Ops Midgoal 1";
    }
    else if(2==midGoal){
        theaterPrep(civ, player, troopLocations);
    }
    else if(3==midGoal){
       theaterAtWar(civ, player, cityTarget);
    }
    else{
        //Probably not anything for operational in this context, aside from threat detection
        //midgoal==4 is resource gathering - run theaterprep?
        theaterPrep(civ, player, troopLocations);
    }

    AI_Tactical(midGoal, civ, player, map, cityLocations, cityTarget, troopLocations);

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



void AI_Operational::theaterAtWar(Civilization *civ, Civilization *player, City *cityTarget){
    //scan outward for nearest player city
        //Sets it as city target

    //Probably just easiest to use the findpath from AI capitol to each player city
    //and return the closest from there

}
//************Theater of War***************
//Prioritizes the nearest city (by accessibility?) of the player civ
//If at war, sends x units immediately to priority targets
        //1 city at a time

//Apropriate location??????
//always priority for siege units unless directly threatened
        //Units which pose a threat to borders or to forces actively invading the enemy city
            //targets threatening units with whatever they are weak to first, then neutral, and only strong against as last resort



void AI_Operational::theaterPrep(Civilization *civ, Civilization *player, QVector<Tile *> troopLocations){

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


void AI_Operational::cityLocation(Civilization *civ, Map *map){
    qDebug()<<"City Locations";
    for(int i=0; i<(14-civ->GetCityList().length());i++)
    {
        int cityIndex = civ->GetCityAt(0)->GetCityTile()->GetTileIndex(), indexToSettle;

        if(cityIndex + (15 * (i+1)) < map->GetBoardSize())
        {
            indexToSettle = cityIndex + (15 * (i+1));
        }
        else if(cityIndex - (15 * (1+1)) > 0)
        {
            indexToSettle = cityIndex - (15 * (i+1));
        }
        else
        {
            qDebug()<<"City invalid";
            indexToSettle = 255;
        }

        if(map->GetTileAt(indexToSettle)->Walkable
                && map->GetTileAt(indexToSettle)->GetTileType()!=WATER
                && map->GetTileAt(indexToSettle)->GetControllingCiv()==NO_NATION)
        {
            qDebug()<<"Adding tile to list of potential locations"<<indexToSettle;
            cityLocations.push_back(map->GetTileAt(indexToSettle));
        }
        //Settler Test


        //Locates appropriate number of city sites - logic?
            //Plans for 10 cities, but finds an extra 5 sites to account for sites being taken
    }
}
//***********City Founding***************
//initially prioritize up to 15 locations for city founding sites, starting with nearby and working out
//re-evaluate and remove locations that become another civ's territory
    //Update vector pointer with results

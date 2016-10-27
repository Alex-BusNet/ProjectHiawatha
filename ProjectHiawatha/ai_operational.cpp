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
#include <qdebug.h>

AI_Operational::AI_Operational()
{

}



AI_Operational::AI_Operational(int midGoal, Civilization *civ, Civilization *player, Map *map, GameScene *scene)
{
    qDebug()<<"         Operational AI Called";

    threatScan(civ, player);


    if(1==midGoal){
        cityLocation(civ);
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

    AI_Tactical(midGoal, civ, player, map, scene, cityLocations, cityTarget, troopLocations);
}


//****************Tactical AI Called**************
    //Pass target city
    //also passes the city founding vector and 3 vectors of threatening units
    //Pass position vector for military units





void AI_Operational::threatScan(Civilization *civ, Civilization *player)
{
    qDebug()<<"Threatscan";
    for(int i = 0; i<player->GetUnitList().length(); i++){
        int unitTileColumn = player->GetUnitAt(i)->GetTileColumn();
        int unitTileRow = player->GetUnitAt(i)->GetTileRow();

        qDebug()<<"Column: "<<unitTileColumn<<" Row: "<<unitTileRow<<" TileID: "<<player->GetUnitAt(i)->GetTileIndex();
        //Column and Row are not changing?

        Tile *tile1 = new Tile(unitTileRow, unitTileColumn);

        if(tile1->GetControllingCiv()==civ->getCiv()){
            //compare with tiles owned by AI?
            qDebug()<<"Invasion";
        }
        else{
            qDebug()<<"Not Invasion";
        }
    }
    //Units within territory are added to highThreats
    //Using the Map::GetNeighbors(Tile *node) algorithm
        //search radially out from controlled tiles
            //Units within 1 of territory added to midThreats
    //Use same algorithm from vector of military units
        //if enemy found within 2 of units, add to highThreats
        //else if found within 3 of units, add to midThreats
        //else if within 5 add to lowThreats
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
}
//************Theater of War***************
//Prioritizes the nearest city (by accessibility?) of the player civ
//If at war, sends immediately to priority targets
        //1 city at a time

//Apropriate location??????
//always priority for siege units unless directly threatened
        //Units which pose a threat to borders or to forces actively invading the enemy city
            //targets threatening units with whatever they are weak to first, then neutral, and only strong against as last resort



void AI_Operational::theaterPrep(Civilization *civ, Civilization *player, QVector<Tile *> troopLocations){
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


void AI_Operational::cityLocation(Civilization *civ){
    for(int i=0; i<(15-civ->GetCityList().length());i++){
        //Locates appropriate number of city sites - logic?
            //Plans for 10 cities, but finds an extra 5 sites to account for sites being taken
    }
}
//***********City Founding***************
//initially prioritize up to 15 locations for city founding sites, starting with nearby and working out
//re-evaluate and remove locations that become another civ's territory
    //Update vector pointer with results

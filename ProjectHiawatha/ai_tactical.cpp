//Individual unit Control
/*determines appropriate location for a unit to be (but not pathing)
Determines if unit is military or worker/settler type
    if worker, prioritizes resources and orders to improve
    if settler, read from strategic and set a target tile
        Probably a spiral outward from capital, with corrections for water, mountains, etc
    else, if unit warlike:
        read from operational AI (may be merged into this class)
            move units appropriately
            attack if combat simulator suggests victory (or acceptable loss, etc)
                maybe not, kind of go berserk, but use ones that are 'strong' against FIRST */

#include "ai_tactical.h"
#include <QDebug>

AI_Tactical::AI_Tactical(Civilization *civ, Map *map, QVector<Tile *> CityToBeFounded, City *cityTarget)
{
    //qDebug()<<"             Tactical AI called";
    highThreatProcessing(civ, map);
    lowThreatProcessing(civ, map);
    if(civ->isAtWar()){
        AtWar(civ, map, cityTarget);
    }
    settlercontrol(civ, map, CityToBeFounded);
    workercontrol(civ, map);
}

void AI_Tactical::AtWar(Civilization *civ, Map *map, City *cityTarget)
{
    int combatUnits=0;
    for(int i = 0;i< civ->GetUnitList().length();i++){
        if(!civ->GetUnitAt(i)->isNonCombat()){
            combatUnits++;
        }
    }//Tally combat units
    QVector<Unit*> unitlist=civ->GetUnitList();
    UnitController *UnitControl= new UnitController();
    bool conquered=false;
    for(int i=0; i<unitlist.length();i++){
        Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
        if(civ->GetUnitAt(i)->isMelee&&(combatUnits>4)){
            QList<Tile*> inRange = map->GetNeighbors(unitlocation);
            bool canHit = false;
            for(int j=0; j<inRange.length();j++){
                if(inRange.at(j)->GetTileIndex()==cityTarget->GetCityTile()->GetTileIndex()){
                    canHit=true;
                    if(0>=cityTarget->GetCityHealth()){
                        unitlist.at(i)->SetUnitTargetTileIndex(cityTarget->GetCityTile()->GetTileIndex());
                        civ->setCityFounding(AIQueueData{CONQUER,unitlist.at(i)});
                        conquered=true;
                    }//Only Melee units can conquer cities
                    else{
                        //qDebug()<<"Attack City of "<<(cityTarget->GetName());
                        UnitControl->AttackCity(unitlist.at(i),cityTarget);
                        while(!unitlist.at(i)->isPathEmpty()){
                            unitlist.at(i)->UpdatePath();
                        }//Clear remaining movement
                        //qDebug()<<"test"<<unitlist.at(i)->GetTargetTileIndex();
                    }
                }//Melee Attack
                if(conquered){
                    //qDebug()<<"Conquered Break 1";
                    break;
                }
            }//Check if in range
            if(!canHit&&(civ->GetUnitAt(i)->RequiresOrders)){
                //qDebug()<<"Send to city: "<<(cityTarget->GetName());
                QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(cityTarget->GetCityTile()->GetTileIndex()),1);
                int k=0;
                while(unitlist.at(i)->isPathEmpty()){
                    if(targetNeighbor.at(k)->ContainsUnit||MOUNTAIN==targetNeighbor.at(k)->GetTileType()||WATER==targetNeighbor.at(k)->GetTileType()){
                        k++;
                        continue;
                    }//Check if tile is occupied
                    //qDebug()<<"K="<<k;
                    if(k>6){
                        //qDebug()<<"All tiles occupied";
                        break;
                    }
                    //qDebug()<<"Sent to tile: "<<targetNeighbor.at(k)->GetTileIndex();
                    UnitControl->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                    //qDebug()<<"test2"<<unitlist.at(i)->GetTargetTileIndex();
                }//Attempts to send melee units into attack range
                if(civ->GetUnitAt(i)->RequiresOrders){
                    QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(cityTarget->GetCityTile()->GetTileIndex()),3);
                    int k=0;
                    while(unitlist.at(i)->isPathEmpty()){
                        if(targetNeighbor.at(k)->ContainsUnit||MOUNTAIN==targetNeighbor.at(k)->GetTileType()||WATER==targetNeighbor.at(k)->GetTileType()){
                            k++;
                            continue;
                        }//Check if tile is occupied
                        UnitControl->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                        //qDebug()<<"Test3"<<unitlist.at(i)->GetTargetTileIndex();
                    }
                }//Places units in 'On Deck' position if attack spots are occupied
            }//Direct Melee units to an appropriate tile
        }//Melee Unit City Combat
        else if(!civ->GetUnitAt(i)->isNonCombat()&&(combatUnits>4)){
            QList<Tile*> inRange = map->GetNeighborsRange(unitlocation,2);
            bool canHit = false;
            for(int j=0; j<inRange.length();j++){
                if(inRange.at(j)->GetTileIndex()==cityTarget->GetCityTile()->GetTileIndex()){
                    canHit=true;
                    if(0<cityTarget->GetCityHealth()){
                        //qDebug()<<"Attack City of "<<(cityTarget->GetName());
                        UnitControl->AttackCity(unitlist.at(i),cityTarget);
                        while(!unitlist.at(i)->isPathEmpty()){
                            unitlist.at(i)->UpdatePath();
                        }//Clear remaining movement
                        //qDebug()<<"Ranged Attack from "<<unitlist.at(i)->GetTargetTileIndex();
                    }//Stop attacking with Ranged Units when the city hits 0hp (Only Melee units can conquer cities)
                }//Ranged Attack
            }//Check if in range
            if(!canHit&&(civ->GetUnitAt(i)->RequiresOrders)){
                //qDebug()<<"Send to city: "<<(cityTarget->GetName());
                QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(cityTarget->GetCityTile()->GetTileIndex()),2);
                int k=0;
                while(unitlist.at(i)->isPathEmpty()){
                    if(targetNeighbor.at(k)->ContainsUnit||MOUNTAIN==targetNeighbor.at(k)->GetTileType()||WATER==targetNeighbor.at(k)->GetTileType()){
                        k++;
                        continue;
                    }//Check if tile is occupied
                    UnitControl->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                    //qDebug()<<"Ranged moving to "<<unitlist.at(i)->GetTargetTileIndex()<<unitlist.at(i)->GetPath().length();
                }//Attempt to send ranged units into attack range
            }//Direct ranged units to appropriate tile
        }//Ranged Unit City Combat
        if(conquered){
            //qDebug()<<"Conquered Break 2";
            break;
        }
    }//Apply to each unit
}
    //Scroll through a vector of the military units,
        //check each to see if it has moves remaining
            //direct it toward the target city
                //Have each unit attack from its max range - that way the melee units will be able to get in front.
                //Attack with ranged units first
    //****************Military Unit Control (At War)******************
    //Targets each remaining unit toward the opposing city
        //while units remain
            //The logic will consider city as weak to siege
            //and the other appropriate weaknesses (armor weak to AT, etc)

void AI_Tactical::highThreatProcessing(Civilization *civ, Map *map){
    //qDebug()<<"             High Threat Processing Start";
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
    UnitController *UnitControl= new UnitController();
    QVector<Unit*> threatVec;
    for(int i=0;i<civ->getHighThreats().length();i++){
        threatVec.push_back(civ->getHighThreats().at(i));
    }
    for(int i = 0; i<unitlist.length();i++){
        if(!threatVec.isEmpty()&&civ->isAtWar()){
            //qDebug()<<"Not empty, at "<<threatVec.at(0)->GetTileIndex();
            //Find AI Troop location
            Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
            if(civ->GetUnitList().at(i)->GetUnitType()==WARRIOR){
                //Logic should be for all combat units
                //Will need additional logic for other unit types (ranged, etc)
                if(civ->GetUnitAt(i)->isMelee){
                    QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                    bool canHit = false;
                    for(int j=0; j<inRange.length();j++){
                        if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
                            //qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
                            canHit=true;
                            UnitControl->Attack(unitlist.at(i),threatVec.at(0),false);
                            while(!unitlist.at(i)->isPathEmpty()){
                                unitlist.at(i)->UpdatePath();
                            }
                            unitlist.at(i)->RequiresOrders=false;
                            //qDebug()<<unitlist.at(i)->GetTargetTileIndex();
                        }
                    }
                    if(!canHit){
                        //qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
                        QList<Tile*> targetNeighbor = map->GetNeighbors(map->GetTileAt(threatVec.at(0)->GetTileIndex()));
                        int k=0;
                        while(unitlist.at(i)->isPathEmpty()){
                            UnitControl->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                            k++;
                            if(k>5){break;}
                        }//Find path accounts for impassable terrain around the target unit
                    }
                }
                else{
                    //qDebug()<<"ranged";
                }
            }
            else {
                //qDebug()<<"worker/settler";
            }
        }
    }
}
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

    //Attacks all these targets, prioritizing units that are strong against
        //keeps going until destroyed or all military units out of moves
        //each unit will target the closest remaining high threat
            //some sort of modifier for distance, but not so much that an attack on the opposite side of continent is ignored
                //maybe have a minimum quantity of responding units? 2/attacker?
            //remove enemy from vector if killed

void AI_Tactical::lowThreatProcessing(Civilization *civ, Map *map){
    //qDebug()<<"             Low Threat Processing Start";
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
    UnitController *UnitControl= new UnitController();
    QVector<Unit*> threatVec;
    for(int i=0;i<civ->getLowThreats().length();i++){
        threatVec.push_back(civ->getLowThreats().at(i));
    }
    for(int i = 0; i<unitlist.length();i++){
        if(!threatVec.isEmpty()&&civ->isAtWar()){
                //qDebug()<<"Not empty, at "<<threatVec.at(0)->GetTileIndex();
            //Find AI Troop location
            Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
            if(civ->GetUnitList().at(i)->GetUnitType()==WARRIOR){
                //Logic should be for all combat units
                //Will need additional logic for other unit types (ranged, etc)
                if(civ->GetUnitAt(i)->isMelee){
                    QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                    bool canHit = false;
                    for(int j=0; j<inRange.length();j++){
                        if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
                            //qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
                            canHit=true;
                            UnitControl->Attack(unitlist.at(i),threatVec.at(0),false);
                            while(!unitlist.at(i)->isPathEmpty()){
                                unitlist.at(i)->UpdatePath();
                            }
                            //qDebug()<<unitlist.at(i)->GetTargetTileIndex();
                        }
                    }
                    if(!canHit){
                        //qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
                        QList<Tile*> targetNeighbor = map->GetNeighbors(map->GetTileAt(threatVec.at(0)->GetTileIndex()));
                        int k=0;
                        while(unitlist.at(i)->isPathEmpty()){
                            UnitControl->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                            k++;
                            if(k>5){break;}
                        }//Find path accounts for impassable terrain around the target unit
                    }
                }
                else{
                    //qDebug()<<"ranged";
                }
            }
            else {
                //qDebug()<<"worker/settler";
            }
        }
    }
}
    //Scroll through a vector of the military units,
        //Check first enemy for weaknesses and strengths
            //check each unit see if it has moves remaining to attack this turn
                //Check if strong against enemy
                    //if yes, attack
                    //Remove enemy from vector if killed
    //Attacks all these targets, but uses only units which are strong to them
        //keeps going until destroyed or all strong military units out of moves
            //each appropriate unit will target the closest remaining low threat (unless more than 2 turns away)
            //remove enemy from vector if killed

void AI_Tactical::settlercontrol(Civilization *civ, Map *map, QVector<Tile *> CityToBeFounded){
    //qDebug()<<"Settler Control";
    QVector<Unit*> unitlist=civ->GetUnitList();
    UnitController *UnitControl= new UnitController();
    for(int i = 0; i<unitlist.length();i++){
        //Find worker location
        Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
        //qDebug() << "unitType:" << unitlist.at(i)->GetName();
        if(civ->GetUnitList().at(i)->GetName()=="Settler"){
            //qDebug()<<"Settler selected";
            if(map->GetTileAt(unitlist.at(i)->GetTileIndex())==CityToBeFounded.at(0)){
                //qDebug() << "------AI" << civ->getCivIndex() << "writing to foundCity queue";
                civ->setCityFounding(AIQueueData{AI_FOUND_CITY,unitlist.at(i)});
                //This will get set regardless if a city is actually settled or not.
                // If the city is not founded, the AI <<SHOULD>> try to move the settler
                // to the next valid location until a city is actually settled.
                civ->cityFounded = true;
                break;
            }
            else
            {
                //qDebug()<<"going to"<<civ->GetUnitAt(i)->GetTargetTileIndex();
                if(civ->GetUnitAt(i)->isPathEmpty())
                {
                    //qDebug()<<"Traveling Settler";
                    //qDebug() << "Setting path to" << CityToBeFounded.at(0)->GetTileIDString();
                    UnitControl->FindPath(unitlocation,CityToBeFounded.at(0),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                }
                //qDebug()<<"Path not empty";
            }
        }
    }
}
    //For each settler
        //Target Path to (i) city in list
        //if it is at the location with moves left, found city
            //remove city from vector
            //may need to remove from a number of active settlers calculation
        //if no cities left in list, use the search function that was originally used to find a new location
            //add site to vector
    //*****************Settler Control**************
    //Sends a settler to the highest priority city site in the queue
        //need a way to identify that a settler is heading there already - queue pending_city?
    //Found city when arrived
        //remove from queue of locations
        //active_settlers --
    //if a settler exists after all city locations in queue have been used, searches radially for suitable site
        //adds the site to the queue for eval purposes
            //most likely scenario if last city site is taken by others after settler is already built



void AI_Tactical::workercontrol(Civilization *civ, Map *map){
    //qDebug()<<"             Worker Control Start";
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
    UnitController *UnitControl= new UnitController();
    //Make sure a roadworker exists
    bool roadWorkerExists=false;
    for(int i = 0; i<unitlist.length();i++){
        if(civ->GetUnitList().at(i)->GetUnitType()==WORKER&&(civ->GetUnitAt(i)->RequiresOrders)){
            //Find worker location
            Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
            if(civ->GetUnitList().at(i)->isFortified){
                //Improve Tiles *******************
            }
            else if (civ->GetUnitList().at(i)->isRoadWorker){
                roadWorkerExists=true;
                //Build Roads ********************
            }
            else{
                //Check each city for garrisoned worker
                for(int j = 0; j<civ->GetCityList().length();j++){
                    //qDebug()<<civ->GetCityAt(j)->getHasWorker();
                    if(!civ->GetCityAt(j)->getHasWorker()&&!unitlist.at(i)->isGarrisoned){
                        if(unitlocation->GetTileIndex()==civ->GetCityAt(j)->GetCityTile()->GetTileIndex()){
                            //Garrison Worker
                            //qDebug()<<"garrison worker";
                            //civ->GetUnitList().at(i)->isFortified=true;
                            civ->GetCityAt(j)->GarrisonWorker(civ->GetUnitList().at(i));
                        }
                        else {
                            //qDebug()<<"City at "<<civ->GetCityAt(j)->GetCityTile()->GetTileIndex();
                            //Send the unused worker to city
                            unitlist.at(i)->SetUnitTargetTile(civ->GetCityAt(j)->GetCityTile()->GetTileID().column, civ->GetCityAt(j)->GetCityTile()->GetTileID().row);
                            unitlist.at(i)->SetUnitTargetTileIndex(civ->GetCityAt(j)->GetCityTile()->GetTileIndex());
                            UnitControl->FindPath(unitlocation,civ->GetCityAt(j)->GetCityTile(),map,unitlist.at(i), WarData{civ->GetCivListIndexAtWar(), civ->GetNationAtWar()});
                        }
                    }
                    else if(false==roadWorkerExists){
                        civ->GetUnitList().at(i)->isRoadWorker=true;
                        roadWorkerExists=true;
                    }
                }
                //Find workers that aren't garrisoned or roadbuilding
                //Match them with cities that don't have workers
                    //Should actually be built in the correct city, so just immediately garisson if not roadworker
                //or make them a roadworker if needed roadWorkerExists==false
            }
        }
    }
}
    //for each worker
        //if garrissoned and not already improving a tile
            //start to the right of city and circle out within the city's tiles
                //when an unimproved tile is found, start improving it('ideal' improvement logic?)
        //else (non-garrisonned workers should always be road workers
            //if not already building a road
                //target a city not currently connected to capitol (bool val for this??)
    //****************Worker Control***************
    //Unassigned but garrisoned workers will be targeted to the tile closest to the city
        //starting to the top right and clockwise spiral out
        //and will perform the 'ideal' improvement
    //the roadworker will be given a city to connect to if not already working
        //should always be in a city OR building roads

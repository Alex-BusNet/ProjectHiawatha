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
//#define DEBUG

AI_Tactical::AI_Tactical(Civilization *civ, Civilization *player, Map *map, QVector<Tile *> CityToBeFounded, City *cityTarget)
{
#ifdef DEBUG
     qDebug()<<"             Tactical AI called";
#endif
    uc = new UnitController();
    highThreatProcessing(civ, map);
    lowThreatProcessing(civ, map);
    if(civ->isAtWar()){
        AtWar(civ, player, map, cityTarget);
    }
    settlercontrol(civ, map, CityToBeFounded);
    workercontrol(civ, map);
}

void AI_Tactical::AtWar(Civilization *civ, Civilization *player, Map *map, City *cityTarget)
{
#ifdef DEBUG
    qDebug()<<"AtWar";
#endif
    int combatUnits=0;
    for(int i = 0;i< civ->GetUnitList().length();i++){
        if(!civ->GetUnitAt(i)->isNonCombat()){
            combatUnits++;
        }
    }//Tally combat units
    QVector<Unit*> unitlist=civ->GetUnitList();
    bool conquered=false;

    Unit* unit;
    QVector<Unit*> targetunits;

    QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(cityTarget->GetCityTile()->GetTileIndex()),2);
    QList<Tile*> meleeTarget = map->GetNeighborsRange(map->GetTileAt(cityTarget->GetCityTile()->GetTileIndex()),1);

    for(int i=(targetNeighbor.length()-1);i>=0;i--){
        for(int j=(meleeTarget.length()-1);j>=0;j--){
            if(targetNeighbor.at(i)->GetTileIndex()==meleeTarget.at(j)->GetTileIndex()){
                targetNeighbor.removeAt(i);
            }
            if(cityTarget->GetCityTile()->GetTileIndex()==meleeTarget.at(j)->GetTileIndex()){
                meleeTarget.removeAt(j);
            }
            if(cityTarget->GetCityTile()->GetTileIndex()==targetNeighbor.at(i)->GetTileIndex()){
                targetNeighbor.removeAt(i);
            }
        }
    }

    for(int i=(targetNeighbor.length()-1);i>=0;i--){
        OccupantData od = map->GetODFromTileAt(targetNeighbor.at(i)->GetTileIndex());
        if((od.OccupantNation != NO_NATION) || MOUNTAIN==targetNeighbor.at(i)->GetTileType()){
#ifdef DEBUG
            qDebug()<<"Removing "<<targetNeighbor.at(i)->GetTileIndex();
#endif
            if((od.OccupantNation != NO_NATION) && (0==od.civIndex)){
                unit = uc->FindUnitAtTile(targetNeighbor.at(i),player->GetUnitList());
                targetunits.push_back(unit);
            }
            targetNeighbor.removeAt(i);
        }
    }
    for(int i=(meleeTarget.length()-1);i>=0;i--){
        OccupantData od = map->GetODFromTileAt(targetNeighbor.at(i)->GetTileIndex());
        if((od.OccupantNation != NO_NATION) || MOUNTAIN==meleeTarget.at(i)->GetTileType()){
#ifdef DEBUG
            qDebug()<<"Removing "<<meleeTarget.at(i)->GetTileIndex();
#endif
            if((od.OccupantNation != NO_NATION) && (0==od.civIndex)){
                unit = uc->FindUnitAtTile(meleeTarget.at(i),player->GetUnitList());
                targetunits.push_back(unit);
            }
            meleeTarget.removeAt(i);
        }
    }

    //Attack Units Around City
    for(int i=0; i<unitlist.length();i++){
        if(unitlist.at(i)->isPathEmpty()){
            if(targetunits.length()>0){
                Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
                if(!civ->GetUnitList().at(i)->isNonCombat()){
                    if(civ->GetUnitAt(i)->isMelee){
                        QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                        bool canHit = false;
                        for(int j=0; j<inRange.length();j++){
                            if(inRange.at(j)->GetTileIndex()==targetunits.at(0)->GetTileIndex()){
                                canHit=true;
                                uc->Attack(unitlist.at(i),targetunits.at(0),false);
                                while(!unitlist.at(i)->isPathEmpty()){
                                    unitlist.at(i)->UpdatePath(map);
                                }
                            }
                        }
                        if(!canHit){
                            QList<Tile*> targetNeighbor = map->GetNeighbors(map->GetTileAt(targetunits.at(0)->GetTileIndex()));
                            int k=0;
                            while(unitlist.at(i)->isPathEmpty()){
                                uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                                k++;
                                if(k>6){break;}
                            }//Find path accounts for impassable terrain around the target unit
                        }
                    }
                    else{
                         QList<Tile*> inRange = map->GetNeighborsRange(unitlocation,2);
                         bool canHit = false;
                         for(int j=0; j<inRange.length();j++){
                             if(inRange.at(j)->GetTileIndex()==targetunits.at(0)->GetTileIndex()){

                                 canHit=true;
                                 uc->Attack(unitlist.at(i),targetunits.at(0),false);
                                 while(!unitlist.at(i)->isPathEmpty()){
                                     unitlist.at(i)->UpdatePath(map);
                                 }
                             }
                         }
                         if(!canHit){
                             QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(targetunits.at(0)->GetTileIndex()),2);
                             int k=0;
                             while(unitlist.at(i)->isPathEmpty()){
                                 uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                                 k++;
                                 if(k>19){break;}
                             }//Find path accounts for impassable terrain around the target unit
                         }
                    }
                }
            }//Check for target units
        }//Only units that havent moved this turn
    }//Apply to each unit





    //Attack City
    for(int i=0; i<unitlist.length();i++){
        if(unitlist.at(i)->isPathEmpty()){
            Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
            if(civ->GetUnitAt(i)->isMelee&&(combatUnits>4)){
                QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                bool canHit = false;
                for(int j=0; j<inRange.length();j++){
                    if(inRange.at(j)->GetTileIndex()==cityTarget->GetCityTile()->GetTileIndex()){
                        canHit=true;
                        if(0>=cityTarget->GetCityHealth()){
                            unitlist.at(i)->SetUnitTargetTileIndex(cityTarget->GetCityTile()->GetTileIndex());
                            civ->setCityFounding(AIQueueData{CONQUER,unitlist.at(i), NULL});
                            conquered=true;
                        }//Only Melee units can conquer cities
                        else{
#ifdef DEBUG
                            qDebug()<<"Attack City of "<<(cityTarget->GetName());
#endif
                            uc->AttackCity(unitlist.at(i),cityTarget);
                            while(!unitlist.at(i)->isPathEmpty()){
                                unitlist.at(i)->UpdatePath(map);
                            }//Clear remaining movement
#ifdef DEBUG
                            qDebug()<<"test"<<unitlist.at(i)->GetTargetTileIndex();
#endif
                        }
                    }//Melee Attack
                    if(conquered){
#ifdef DEBUG
                        qDebug()<<"Conquered Break 1";
#endif
                        break;
                    }
                }//Check if in range
                if(!canHit&&(civ->GetUnitAt(i)->RequiresOrders)){
#ifdef DEBUG
                    qDebug()<<"Send to city: "<<(cityTarget->GetName());
#endif
                    for(int k=0;k<meleeTarget.length();k++){
                        if(unitlist.at(i)->isPathEmpty()){
                            uc->FindPath(unitlocation,meleeTarget.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                            meleeTarget.removeAt(k);
#ifdef DEBUG
                            qDebug()<<"Sent to tile: "<<targetNeighbor.at(k)->GetTileIndex();
#endif
                        }
                    }//Attempts to send melee units into attack range
                }//Direct Melee units to an appropriate tile
            }//Melee Unit City Combat


            //Start Ranged
            else if(!civ->GetUnitAt(i)->isNonCombat()&&(combatUnits>4)){
                QList<Tile*> inRange = map->GetNeighborsRange(unitlocation,2);
                bool canHit = false;
                for(int j=0; j<inRange.length();j++){
                    if(inRange.at(j)->GetTileIndex()==cityTarget->GetCityTile()->GetTileIndex()){
                        canHit=true;
                        if(0<cityTarget->GetCityHealth()){
#ifdef DEBUG
                            qDebug()<<"Attack City of "<<(cityTarget->GetName());
#endif
                            uc->AttackCity(unitlist.at(i),cityTarget);
                            while(!unitlist.at(i)->isPathEmpty()){
                                unitlist.at(i)->UpdatePath(map);
                            }//Clear remaining movement
#ifdef DEBUG
                            qDebug()<<"Ranged Attack from "<<unitlist.at(i)->GetTargetTileIndex();
#endif
                        }//Stop attacking with Ranged Units when the city hits 0hp (Only Melee units can conquer cities)
                    }//Ranged Attack
                }//Check if in range
                if(!canHit&&(civ->GetUnitAt(i)->RequiresOrders)){
#ifdef DEBUG
                    qDebug()<<"Send to city: "<<(cityTarget->GetName());
#endif
                    for(int k=0;k<targetNeighbor.length();k++){
                        if(unitlist.at(i)->isPathEmpty()){
                            uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                            targetNeighbor.removeAt(k);
#ifdef DEBUG
                            qDebug()<<"Ranged: K is: "<<k<<" Ranged moving to "<<unitlist.at(i)->GetTargetTileIndex()<<unitlist.at(i)->GetPath().length();
#endif
                        }
                    }//Attempt to send ranged units into attack range
                }//Direct ranged units to appropriate tile
            }//Ranged Unit City Combat
            if(conquered){
#ifdef DEBUG
                qDebug()<<"Conquered Break 2";
#endif
                break;
            }
        }//Only units that havent moved this turn
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
#ifdef DEBUG
     qDebug()<<"             High Threat Processing Start";
#endif
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
    QVector<Unit*> threatVec;
    for(int i=0;i<civ->getHighThreats().length();i++){
        threatVec.push_back(civ->getHighThreats().at(i));
    }
    for(int i = 0; i<unitlist.length();i++){
        if(!threatVec.isEmpty()&&civ->isAtWar()){
#ifdef DEBUG
     qDebug()<<"Not empty, at "<<threatVec.at(0)->GetTileIndex();
#endif
            //Find AI Troop location
             Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
             if(!civ->GetUnitList().at(i)->isNonCombat()){
                 if(civ->GetUnitAt(i)->isMelee){
                     QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                     bool canHit = false;
                     for(int j=0; j<inRange.length();j++){
                         if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
#ifdef DEBUG
    qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                             canHit=true;
                             uc->Attack(unitlist.at(i),threatVec.at(0),false);
                             while(!unitlist.at(i)->isPathEmpty()){
                                 unitlist.at(i)->UpdatePath(map);
                             }
#ifdef DEBUG
    qDebug()<<unitlist.at(i)->GetTargetTileIndex();
#endif
                         }
                     }
                     if(!canHit){
#ifdef DEBUG
    qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                         QList<Tile*> targetNeighbor = map->GetNeighbors(map->GetTileAt(threatVec.at(0)->GetTileIndex()));
                         int k=0;
                         while(unitlist.at(i)->isPathEmpty()){
                             uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                             k++;
                             if(k>6){break;}
                         }//Find path accounts for impassable terrain around the target unit
                     }
                 }
                 else{
#ifdef DEBUG
    qDebug()<<"ranged";
#endif
                      QList<Tile*> inRange = map->GetNeighborsRange(unitlocation,2);
                      bool canHit = false;
                      for(int j=0; j<inRange.length();j++){
                          if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
#ifdef DEBUG
    qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                              canHit=true;
                              uc->Attack(unitlist.at(i),threatVec.at(0),false);
                              while(!unitlist.at(i)->isPathEmpty()){
                                  unitlist.at(i)->UpdatePath(map);
                              }
#ifdef DEBUG
    qDebug()<<unitlist.at(i)->GetTargetTileIndex();
#endif
                          }
                      }
                      if(!canHit){
#ifdef DEBUG
    qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                          QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(threatVec.at(0)->GetTileIndex()),2);
                          int k=0;
                          while(unitlist.at(i)->isPathEmpty()){
                              uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                              k++;
                              if(k>19){break;}
                          }//Find path accounts for impassable terrain around the target unit
                      }
                 }
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
#ifdef DEBUG
     qDebug()<<"             Low Threat Processing Start";
#endif
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
    QVector<Unit*> threatVec;
    for(int i=0;i<civ->getLowThreats().length();i++){
        threatVec.push_back(civ->getLowThreats().at(i));
    }
    for(int i = 0; i<unitlist.length();i++){
        if(!threatVec.isEmpty()&&civ->isAtWar()){
#ifdef DEBUG
     qDebug()<<"Not empty, at "<<threatVec.at(0)->GetTileIndex();
#endif
            //Find AI Troop location
            Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
            if(!civ->GetUnitList().at(i)->isNonCombat()){
                if(civ->GetUnitAt(i)->isMelee){
                    QList<Tile*> inRange = map->GetNeighbors(unitlocation);
                    bool canHit = false;
                    for(int j=0; j<inRange.length();j++){
                        if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
#ifdef DEBUG
     qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                            canHit=true;
//                            civ->setCityFounding(AIQueueData{ATTACK_RANGE, unitlist.at(i), threatVec.at(0)});
                            uc->Attack(unitlist.at(i),threatVec.at(0),false);
                            while(!unitlist.at(i)->isPathEmpty()){
                                unitlist.at(i)->UpdatePath(map);
                            }
#ifdef DEBUG
     qDebug()<<unitlist.at(i)->GetTargetTileIndex();
#endif
                        }
                    }
                    if(!canHit){
#ifdef DEBUG
     qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                        QList<Tile*> targetNeighbor = map->GetNeighbors(map->GetTileAt(threatVec.at(0)->GetTileIndex()));
                        int k=0;
                        while(unitlist.at(i)->isPathEmpty()){
                            uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                            k++;
                            if(k>6){break;}
                        }//Find path accounts for impassable terrain around the target unit
                    }
                }
                else{
#ifdef DEBUG
     qDebug()<<"ranged";
#endif
                     QList<Tile*> inRange = map->GetNeighborsRange(unitlocation,2);
                     bool canHit = false;
                     for(int j=0; j<inRange.length();j++){
                         if(inRange.at(j)->GetTileIndex()==threatVec.at(0)->GetTileIndex()){
#ifdef DEBUG
    qDebug()<<"Attack to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                             canHit=true;
//                             civ->setCityFounding(AIQueueData{ATTACK_RANGE, unitlist.at(i), threatVec.at(0)});
                             uc->Attack(unitlist.at(i),threatVec.at(0),false);
                             while(!unitlist.at(i)->isPathEmpty()){
                                 unitlist.at(i)->UpdatePath(map);
                             }
#ifdef DEBUG
    qDebug()<<unitlist.at(i)->GetTargetTileIndex();
#endif
                         }
                     }
                     if(!canHit){
#ifdef DEBUG
    qDebug()<<"Send to target at "<<(threatVec.at(0)->GetTileIndex());
#endif
                         QList<Tile*> targetNeighbor = map->GetNeighborsRange(map->GetTileAt(threatVec.at(0)->GetTileIndex()),2);
                         int k=0;
                         while(unitlist.at(i)->isPathEmpty()){
                             uc->FindPath(unitlocation,targetNeighbor.at(k),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                             k++;
                             if(k>19){break;}
                         }//Find path accounts for impassable terrain around the target unit
                     }
                }
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
    #ifdef DEBUG
     qDebug()<<"Settler Control";
#endif
    QVector<Unit*> unitlist=civ->GetUnitList();
    for(int i = 0; i<unitlist.length();i++){
        //Find worker location
        Tile *unitlocation = map->GetTileAt(unitlist.at(i)->GetTileIndex());
#ifdef DEBUG
     qDebug() << "unitType:" << unitlist.at(i)->GetName();
#endif
        if(civ->GetUnitList().at(i)->GetName()=="Settler"){
#ifdef DEBUG
     qDebug()<<"Settler selected";
#endif
            if(map->GetTileAt(unitlist.at(i)->GetTileIndex())==CityToBeFounded.at(0)){
#ifdef DEBUG
     qDebug() << "------AI" << civ->getCivIndex() << "writing to foundCity queue";
#endif
                civ->setCityFounding(AIQueueData{AI_FOUND_CITY, unitlist.at(i), NULL});
                //This will get set regardless if a city is actually settled or not.
                // If the city is not founded, the AI <<SHOULD>> try to move the settler
                // to the next valid location until a city is actually settled.
                civ->cityFounded = true;
                break;
            }
            else
            {
#ifdef DEBUG
     qDebug()<<"going to"<<civ->GetUnitAt(i)->GetTargetTileIndex();
#endif
                if(civ->GetUnitAt(i)->isPathEmpty())
                {
#ifdef DEBUG
     qDebug()<<"Traveling Settler";
#endif
#ifdef DEBUG
     qDebug() << "Setting path to" << CityToBeFounded.at(0)->GetTileIDString();
#endif
                    uc->FindPath(unitlocation,CityToBeFounded.at(0),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
                }
#ifdef DEBUG
     qDebug()<<"Path not empty";
#endif
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
#ifdef DEBUG
     qDebug()<<"             Worker Control Start";
#endif
    //Get list of units and make a controller
    QVector<Unit*> unitlist=civ->GetUnitList();
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
#ifdef DEBUG
     qDebug()<<civ->GetCityAt(j)->getHasWorker();
#endif
                    if(!civ->GetCityAt(j)->getHasWorker()&&!unitlist.at(i)->isGarrisoned){
                        if(unitlocation->GetTileIndex()==civ->GetCityAt(j)->GetCityTile()->GetTileIndex()){
                            //Garrison Worker
#ifdef DEBUG
     qDebug()<<"garrison worker";
#endif
                            //civ->GetUnitList().at(i)->isFortified=true;
                            civ->GetCityAt(j)->GarrisonWorker(civ->GetUnitList().at(i));
                        }
                        else {
#ifdef DEBUG
     qDebug()<<"City at "<<civ->GetCityAt(j)->GetCityTile()->GetTileIndex();
#endif
                            //Send the unused worker to city
                            unitlist.at(i)->SetUnitTargetTile(civ->GetCityAt(j)->GetCityTile()->GetTileID().column, civ->GetCityAt(j)->GetCityTile()->GetTileID().row);
                            unitlist.at(i)->SetUnitTargetTileIndex(civ->GetCityAt(j)->GetCityTile()->GetTileIndex());
                            uc->FindPath(unitlocation,civ->GetCityAt(j)->GetCityTile(),map,unitlist.at(i), WarData{civ->isAtWar(), civ->GetCivListIndexAtWar()});
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

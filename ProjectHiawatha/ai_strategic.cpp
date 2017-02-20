//City Control
/*Determine if more cities are needed
        but only if available location
        Probably founds cities in a sort of outward spiral from capital
            stops when it has one within x spaces of another civ?
  Determine if workers are needed (1 worker per city, 1 extra dedicated to roads, work boats?)
  Determine if military units are needed (at war or ultimate objective is war-based)(and which units)
  Determine if structures are needed (and which)
        - probably set a static order
        - then give, say, defensive structures priority if under invasion, etc*/

#include "ai_strategic.h"
#include "civilization.h"
#include "city.h"
#include "yield.h"
#include "ai_controller.h"
#include <QDebug>
//#define DEBUG

AI_Strategic::AI_Strategic(Civilization *civ, Civilization *player, Map *map){
#ifdef DEBUG
     qDebug()<<"     Strategic AI Called";
#endif
    cityLocation(civ, map);
    civ->clearThreats();
    invasionCheck(civ,player,map);
    cityProduction(civ, player);
    aiOp = new AI_Operational(cityLocations, civ, player, map);
    //****************Operational AI called**************
    //Operational AI will control military strategy and city founding
    //Pass it whether or not civ is preparing for / at war (midgoal)
    //Pass it number of cities currently founded (can be got from civ)
    if(civ->cityFounded){
#ifdef DEBUG
     qDebug() << "New city founded. Removing location from list.";
#endif
        civ->cityFounded = false;
        cityLocations.removeFirst();
    }
#ifdef DEBUG
     qDebug()<<"                 AI Turn Complete for "<<civ->getCiv();
#endif
}

void AI_Strategic::cityProduction(Civilization *civ,Civilization *player){
#ifdef DEBUG
     qDebug()<<"City Production";
#endif
     int civStrength = civ->GetMilitaryStrength();
     int playerStrength=player->GetMilitaryStrength();
     int techLevel = (civ->getTechIndex()-1);
#ifdef DEBUG
     qDebug()<<"Tech Level"<<techLevel;
     qDebug()<<"Civ Strength: "<<civStrength<<" Player Strength: "<<playerStrength;
#endif


    QVector<Unit*> oldMelee;
    QVector<Unit*> oldRanged;
    QVector<Unit*> oldSiege;
    QVector<Unit*> oldMounted;
    QVector<Unit*> oldAntiMounted;
    bool activeSettler = false;
    int workers=0, combatUnits=0, meleeUnits=0, rangedUnits=0, siegeUnits=0, mountedUnits=0, antiMoutedUnits=0;
    for(int i = 0;i< civ->GetUnitList().length();i++){
        QString unitname=civ->GetUnitAt(i)->GetName();
        if("Settler"==unitname){
            activeSettler=true;
        }
        else if("Worker"==unitname){
            workers++;
        }
        else if("Warrior"==unitname||"Swordsman"==unitname||"Musketman"==unitname||"Rifleman"==unitname||"Infantry"==unitname||"Mechanized Infantry"==unitname){
            meleeUnits++;
            combatUnits++;
            if(((techLevel>3)&&("Warrior"==unitname))||((techLevel>6)&&("Musketman"==unitname))||((techLevel>10)&&("Rifleman"==unitname))||((techLevel>12)&&("Infantry"==unitname))){
                oldMelee.push_back(civ->GetUnitAt(i));
            }
        }
        else if("Archer"==unitname||"Crossbowman"==unitname){
            rangedUnits++;
            combatUnits++;
            if(((techLevel>5)&&("Archer"==unitname))||((techLevel>12)&&("Crossbowman"==unitname))){
                oldRanged.push_back(civ->GetUnitAt(i));
            }
        }
        else if("Catapult"==unitname||"Cannon"==unitname||"Artillery"==unitname){
            siegeUnits++;
            combatUnits++;
            if(((techLevel>7)&&("Catapult"==unitname))||((techLevel>10)&&("Cannon"==unitname))){
                oldSiege.push_back(civ->GetUnitAt(i));
            }
        }
        else if("Knight"==unitname||"Cavalry"==unitname||"Tank"==unitname||"Modern Armor"==unitname){
            mountedUnits++;
            combatUnits++;
            if(((techLevel>8)&&("Knight"==unitname))||((techLevel>11)&&("Cavalry"==unitname))||((techLevel>13)&&("Tank"==unitname))){
                oldMounted.push_back(civ->GetUnitAt(i));
            }
        }
        else if("Spearman"==unitname||"Pikeman"==unitname||"Helicopter Gunship"==unitname){
            antiMoutedUnits++;
            combatUnits++;
            if(((techLevel>4)&&("Spearman"==unitname))||((techLevel>10)&&("Pikeman"==unitname))){
                oldAntiMounted.push_back(civ->GetUnitAt(i));
            }
        }
    }//Tallies various unit types
    if(!civ->isAtWar()){
        if(((6<=combatUnits)&&(civStrength>(playerStrength*2)))||(12<=combatUnits)){
         qDebug()<<combatUnits<<"AI Declare War!";
            //Needs to use the official logic, or else the war is a surprise!
            civ->setCityFounding(AIQueueData{AI_DECLARE_WAR,civ->GetUnitAt(0)});
            civ->SetAtWar(0);
            //Should declare war!
            //Mostly just gets triggered if the AI has completed all buildings and has amassed a large army
            //Also makes it difficult to make peace with powerful civs once you've declared war
        }
    }

    for(int i =0;i < civ->GetCityList().length(); i++){
        if("No Current Production"==civ->GetCityAt(i)->getProductionName()){//Determine if city is currently building something
            if(((!civ->isAtWar())||(civ->GetCityList().length()<2)||(15<combatUnits))&&(22>civ->GetCityAt(i)->getNumberOfBuildings())){//Settle more cities, builds workers and buildings
                if((0==i)&&(!activeSettler)&&(11>civ->GetCityList().length()&&(1<=cityLocations.length()))){//Only first city builds settlers - logistical parameter
                    //Logic to only build 1 settler at a time
                    civ->GetCityAt(i)->setCurrentProductionCost(100);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Settler");
                    civ->GetCityAt(i)->setProductionIndex(3);
                    //Set city to build settler
                }
                else if((!civ->GetCityAt(i)->getHasWorker())&&(1<civ->GetCityList().length())&&(workers<civ->GetCityList().length())){
                    civ->GetCityAt(i)->setCurrentProductionCost(70);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Worker");
                    civ->GetCityAt(i)->setProductionIndex(6);
                }
                else{
                    int numBuildings=civ->GetCityAt(i)->getNumberOfBuildings();
                    //Buildings are based on tech progress
                    if((0==numBuildings)&&(0<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(75);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Walls");
                        civ->GetCityAt(i)->setProductionIndex(0);
                    }
                    else if((1==numBuildings)&&(0<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(50);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Granary");
                        civ->GetCityAt(i)->setProductionIndex(5);
                    }
                    else if((2==numBuildings)&&(0<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(70);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("LightHouse");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if((3==numBuildings)&&(0<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Barracks");
                        civ->GetCityAt(i)->setProductionIndex(3);
                    }
                    else if((4==numBuildings)&&(2<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Library");
                        civ->GetCityAt(i)->setProductionIndex(1);
                    }
                    else if((5==numBuildings)&&(3<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Colosseum");
                        civ->GetCityAt(i)->setProductionIndex(7);
                    }
                    else if((6==numBuildings)&&(3<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(110);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Temple");
                        civ->GetCityAt(i)->setProductionIndex(12);
                    }
                    else if((7==numBuildings)&&(3<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(120);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Courthouse");
                        civ->GetCityAt(i)->setProductionIndex(8);
                    }
                    else if((8==numBuildings)&&(4<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Forge");
                        civ->GetCityAt(i)->setProductionIndex(16);
                    }
                    else if((9==numBuildings)&&(4<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(115);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Aqueduct");
                        civ->GetCityAt(i)->setProductionIndex(13);
                    }
                    else if((10==numBuildings)&&(4<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Monastery");
                        civ->GetCityAt(i)->setProductionIndex(20);
                    }
                    else if((11==numBuildings)&&(4<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(150);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Market");
                        civ->GetCityAt(i)->setProductionIndex(19);
                    }
                    else if((12==numBuildings)&&(5<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(175);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Castle");
                        civ->GetCityAt(i)->setProductionIndex(15);
                    }
                    else if((13==numBuildings)&&(5<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(180);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("University");
                        civ->GetCityAt(i)->setProductionIndex(21);
                    }
                    else if((14==numBuildings)&&(6<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(220);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Observatory");
                        civ->GetCityAt(i)->setProductionIndex(2);
                    }
                    else if((15==numBuildings)&&(6<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(200);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Theatre");
                        civ->GetCityAt(i)->setProductionIndex(6);
                    }
                    else if((16==numBuildings)&&(6<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(300);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Bank");
                        civ->GetCityAt(i)->setProductionIndex(4);
                    }
                    else if((17==numBuildings)&&(9<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Factory");
                        civ->GetCityAt(i)->setProductionIndex(10);
                    }
                    else if((18==numBuildings)&&(9<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hospital");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if((19==numBuildings)&&(13<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Harbor");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else if((20==numBuildings)&&(13<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hydro Plant");
                        civ->GetCityAt(i)->setProductionIndex(14);
                    }
                    else if((21==numBuildings)&&(13<=techLevel)){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Nuclear Plant");
                        civ->GetCityAt(i)->setProductionIndex(17);
                    }
                }
            }
            if("No Current Production"==civ->GetCityAt(i)->getProductionName()){
                if(15>=combatUnits){
                    //Unit capabilities are based on Tech Level (linear progression)
                    if((2>techLevel)){
                        //Tech level 1 (Arch-warrior)
                        if(meleeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(45);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Warrior");
                            civ->GetCityAt(i)->setProductionIndex(5);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                        }
                    }
                    else if((3>techLevel)){
                        //Tech Level 2 (Spear)
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(45);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Warrior");
                            civ->GetCityAt(i)->setProductionIndex(5);
                        }
                        else if(rangedUnits<6){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(60);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Spearman");
                            civ->GetCityAt(i)->setProductionIndex(4);
                        }
                    }
                    else if((4>techLevel)){
                        //Tech 3 cata/sword
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Swordsman");
                            civ->GetCityAt(i)->setProductionIndex(8);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(rangedUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                        }
                    }
                    else if((5>techLevel)){
                        //Tech 4 pike
                        if(meleeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Swordsman");
                            civ->GetCityAt(i)->setProductionIndex(8);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(rangedUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                        }
                    }
                    else if((6>techLevel)){
                        //Tech 5 cross/knight
                        if(meleeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Swordsman");
                            civ->GetCityAt(i)->setProductionIndex(8);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                        }
                    }
                    else if((7>techLevel)){
                        //Tech 6 musket
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(150);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Musketman");
                            civ->GetCityAt(i)->setProductionIndex(16);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else if(antiMoutedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                        }
                    }
                    else if((8>techLevel)){
                        //tech 7 cann
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(150);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Musketman");
                            civ->GetCityAt(i)->setProductionIndex(16);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else if(antiMoutedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cannon");
                            civ->GetCityAt(i)->setProductionIndex(12);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                        }
                    }
                    else if((10>techLevel)){
                        //tech 8 cav/rifl (9 is naval only)
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Rifleman");
                            civ->GetCityAt(i)->setProductionIndex(17);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                            else if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cavalry");
                            civ->GetCityAt(i)->setProductionIndex(14);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cannon");
                            civ->GetCityAt(i)->setProductionIndex(12);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                        }
                    }
                    else if((11>techLevel)){
                        //tech 10 art/infan
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(320);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Infantry");
                            civ->GetCityAt(i)->setProductionIndex(23);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                            else if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(rangedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cavalry");
                            civ->GetCityAt(i)->setProductionIndex(14);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                        }
                    }
                    else if((12>techLevel)){
                        //tech 11 tank
                        if(meleeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(320);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Infantry");
                            civ->GetCityAt(i)->setProductionIndex(23);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                            else if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                        else if(siegeUnits<6){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                            else if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                    }
                    else if((13>techLevel)){
                        //tech 12 mech inf
                        if(meleeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                            civ->GetCityAt(i)->setProductionIndex(30);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                            else if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                        else if(siegeUnits<6){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                            else if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                    }
                    else{
                        //tech 13 heli/modarmor
                        if(meleeUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                            civ->GetCityAt(i)->setProductionIndex(30);
                            if(oldMelee.length()>0){
                                oldMelee.at(0)->SetHealth(-1);
                                oldMelee.removeAt(0);
                            }
                        }
                        else if(antiMoutedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Helicopter Gunship");
                            civ->GetCityAt(i)->setProductionIndex(28);
                            if(oldAntiMounted.length()>0){
                                oldAntiMounted.at(0)->SetHealth(-1);
                                oldAntiMounted.removeAt(0);
                            }
                        }
                        else if(mountedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Modern Armor");
                            civ->GetCityAt(i)->setProductionIndex(31);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                        else if(siegeUnits<6){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            if(oldSiege.length()>0){
                                oldSiege.at(0)->SetHealth(-1);
                                oldSiege.removeAt(0);
                            }
                            else if(oldRanged.length()>0){
                                oldRanged.at(0)->SetHealth(-1);
                                oldRanged.removeAt(0);
                            }
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Modern Armor");
                            civ->GetCityAt(i)->setProductionIndex(31);
                            if(oldMounted.length()>0){
                                oldMounted.at(0)->SetHealth(-1);
                                oldMounted.removeAt(0);
                            }
                        }
                    }
                }
            }
#ifdef DEBUG
            qDebug()<<civ->GetCityAt(i)->GetName()<<" now producing "<<civ->GetCityAt(i)->getProductionName();
#endif
        }
        else{
#ifdef DEBUG
     qDebug()<<civ->GetCityAt(i)->GetName()<<" already producing "<<civ->GetCityAt(i)->getProductionName();
#endif
        }
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

void AI_Strategic::invasionCheck(Civilization *civ, Civilization *player, Map *map){
    for(int i = 0; i<player->GetUnitList().length(); i++){
        if(map->GetTileAt(player->GetUnitAt(i)->GetTileIndex())->GetControllingCiv()==civ->getCiv()){
#ifdef DEBUG
     qDebug()<<"Invasion";
#endif
            QVector<Unit*> tempVec = civ->getHighThreats();
            tempVec.push_back(player->GetUnitAt(i));
            civ->setHighThreats(tempVec);
        }
    }
}

void AI_Strategic::cityLocation(Civilization *civ, Map *map){
#ifdef DEBUG
     qDebug()<<"City Locations";
#endif
        for(int i=0; i<(24-civ->GetCityList().length());i++){
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
#ifdef DEBUG
     qDebug()<<"City invalid";
#endif
                indexToSettle = 255;
            }
            if(map->GetTileAt(indexToSettle)->Walkable
                    && map->GetTileAt(indexToSettle)->GetTileType()!=WATER
                    && map->GetTileAt(indexToSettle)->GetControllingCiv()==NO_NATION)
            {
                bool goodTile=true;
                QList<Tile*> inRange = map->GetNeighborsRange(map->GetTileAt(indexToSettle), 3);
                for(int j = 0; j<inRange.length();j++){
                    if(NO_NATION!=inRange.at(j)->GetControllingCiv()){
                        goodTile=false;
                    }
                }
                if(goodTile){
#ifdef DEBUG
     qDebug()<<"Adding tile to list of potential locations"<<indexToSettle;
#endif
                    cityLocations.push_back(map->GetTileAt(indexToSettle));
                }
                else{
#ifdef DEBUG
     qDebug()<<map->GetTileAt(indexToSettle)->GetTileIndex()<<" is bad tile";
#endif
                }
            }
            //Locates appropriate number of city sites
                //Plans for 10 cities, but finds an extra 5 sites to account for sites being taken
        }
    //***********City Founding***************
    //initially prioritize up to 15 locations for city founding sites, starting with nearby and working out
    //re-evaluate and remove locations that become another civ's territory
        //Update vector pointer with results
}

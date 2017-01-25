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
    cityProduction(civ);
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

void AI_Strategic::cityProduction(Civilization *civ){
    #ifdef DEBUG
     qDebug()<<"City Production";
#endif
    bool activeSettler = false;
    int workers=0, combatUnits=0, meleeUnits=0, rangedUnits=0, siegeUnits=0, mountedUnits=0, antiMoutedUnits=0, fighters=0, bombers=0;
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
        }
        else if("Archer"==unitname||"Crossbowman"==unitname){
            rangedUnits++;
            combatUnits++;
        }
        else if("Catapult"==unitname||"Cannon"==unitname||"Artillery"==unitname){
            siegeUnits++;
            combatUnits++;
        }
        else if("Knight"==unitname||"Cavalry"==unitname||"Tank"==unitname||"Modern Armor"==unitname){
            mountedUnits++;
            combatUnits++;
        }
        else if("Spearman"==unitname||"Pikeman"==unitname||"Helicopter Gunship"==unitname){
            antiMoutedUnits++;
            combatUnits++;
        }
        else if("Fighter"==unitname||"Jet Fighter"==unitname){
            fighters++;
            combatUnits++;
        }
        else if("Bomber"==unitname||"Stealth Bomber"==unitname){
            bombers++;
            combatUnits++;
        }
    }//Tallies various unit types
    for(int i =0;i < civ->GetCityList().length(); i++){
        if("No Current Production"==civ->GetCityAt(i)->getProductionName()){//Determine if city is currently building something
            if(!civ->isAtWar()||civ->GetCityList().length()<2){//Settle more cities, builds workers and buildings
#ifdef DEBUG
     qDebug()<<"produce stuff";
#endif
                if((0==i)&&(!activeSettler)&&(11>civ->GetCityList().length()&&(1<=cityLocations.length()))){//Only first city builds settlers - logistical parameter
                    //Logic to only build 1 settler at a time
                    ///For debugging purposes, Settler production has been set to 10. This will need to be reset to 100.
                    civ->GetCityAt(i)->setCurrentProductionCost(100);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Settler");
                    civ->GetCityAt(i)->setProductionIndex(3);
                    //Set city to build settler
                }
                else if((!civ->GetCityAt(i)->getHasWorker())&&(1<civ->GetCityList().length())&&(workers<civ->GetCityList().length())){
#ifdef DEBUG
     qDebug()<<"City: "<<civ->GetCityAt(i)->GetName();
#endif
                    civ->GetCityAt(i)->setCurrentProductionCost(70);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Worker");
                    civ->GetCityAt(i)->setProductionIndex(6);
                }
                else{
#ifdef DEBUG
     qDebug()<<" Buildings";
#endif
                    int numBuildings=civ->GetCityAt(i)->getNumberOfBuildings();
                    //Buildings are a linear progression, which simulates tech progress
                    if(0==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(75);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Walls");
                        civ->GetCityAt(i)->setProductionIndex(0);
                    }
                    else if(1==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(50);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Granary");
                        civ->GetCityAt(i)->setProductionIndex(5);
                    }
                    else if(2==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(70);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("LightHouse");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(3==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Barracks");
                        civ->GetCityAt(i)->setProductionIndex(3);
                    }
                    else if(4==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Library");
                        civ->GetCityAt(i)->setProductionIndex(1);
                    }
                    else if(5==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Colosseum");
                        civ->GetCityAt(i)->setProductionIndex(7);
                    }
                    else if(6==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(110);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Temple");
                        civ->GetCityAt(i)->setProductionIndex(12);
                    }
                    else if(7==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(120);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Courthouse");
                        civ->GetCityAt(i)->setProductionIndex(8);
                    }
                    else if(8==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Forge");
                        civ->GetCityAt(i)->setProductionIndex(16);
                    }
                    else if(9==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(115);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Aqueduct");
                        civ->GetCityAt(i)->setProductionIndex(13);
                    }
                    else if(10==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Monastery");
                        civ->GetCityAt(i)->setProductionIndex(20);
                    }
                    else if(11==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(150);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Market");
                        civ->GetCityAt(i)->setProductionIndex(19);
                    }
                    else if(12==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(175);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Castle");
                        civ->GetCityAt(i)->setProductionIndex(15);
                    }
                    else if(13==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(180);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("University");
                        civ->GetCityAt(i)->setProductionIndex(21);
                    }
                    else if(14==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(220);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Observatory");
                        civ->GetCityAt(i)->setProductionIndex(2);
                    }
                    else if(15==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(200);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Theatre");
                        civ->GetCityAt(i)->setProductionIndex(6);
                    }
                    else if(16==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(300);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Bank");
                        civ->GetCityAt(i)->setProductionIndex(4);
                    }
                    else if(17==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Factory");
                        civ->GetCityAt(i)->setProductionIndex(10);
                    }
                    else if(18==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hospital");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(19==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Harbor");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else if(20==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hydro Plant");
                        civ->GetCityAt(i)->setProductionIndex(14);
                    }
                    else if(21==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Nuclear Plant");
                        civ->GetCityAt(i)->setProductionIndex(17);
                    }
                }
            }
            else {
                int numBuildings=civ->GetCityAt(i)->getNumberOfBuildings();
                //Unit capabilities are based on what buildings exist (linear progression)
#ifdef DEBUG
 qDebug()<<"Provoked Construction at tech level "<<numBuildings;
#endif
                if(4>=numBuildings){
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
                else if(7>=numBuildings){
                    //Tech Level 2 (Spear)
                    if(meleeUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(45);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Warrior");
                        civ->GetCityAt(i)->setProductionIndex(5);
                    }
                    else if(rangedUnits<9){
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
                else if(11>=numBuildings){
                    //Tech 3 cata/sword
                    //Tech 4 pike
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(80);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Swordsman");
                        civ->GetCityAt(i)->setProductionIndex(8);
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
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(80);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Catapult");
                        civ->GetCityAt(i)->setProductionIndex(7);
                    }
                }
                else if(12>=numBuildings){
                    //Tech 5 cross/knight
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(80);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Swordsman");
                        civ->GetCityAt(i)->setProductionIndex(8);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(mountedUnits<1){
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
                else if(13>=numBuildings){
                    //Tech 6 carav/musket
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(150);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Musketman");
                        civ->GetCityAt(i)->setProductionIndex(16);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(mountedUnits<1){
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
                else if(14>=numBuildings){
                    //tech 7 cann/frig
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(150);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Musketman");
                        civ->GetCityAt(i)->setProductionIndex(16);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
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
                    }
                }
                else if(15>=numBuildings){
                    //tech 8 cav/rifl
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(225);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Rifleman");
                        civ->GetCityAt(i)->setProductionIndex(17);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(mountedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(225);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Cavalry");
                        civ->GetCityAt(i)->setProductionIndex(14);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(190);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Cannon");
                        civ->GetCityAt(i)->setProductionIndex(12);
                    }
                }
                else if(16>=numBuildings){
                    //tech 9 iron
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(225);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Rifleman");
                        civ->GetCityAt(i)->setProductionIndex(17);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(mountedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(225);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Cavalry");
                        civ->GetCityAt(i)->setProductionIndex(14);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(190);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Cannon");
                        civ->GetCityAt(i)->setProductionIndex(12);
                    }
                }
                else if(17>=numBuildings){
                    //tech 10 art/dest/infan
                    if(meleeUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(320);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Infantry");
                        civ->GetCityAt(i)->setProductionIndex(23);
                    }
                    else if(rangedUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(90);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Pikeman");
                        civ->GetCityAt(i)->setProductionIndex(11);
                    }
                    else if(mountedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(225);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Cavalry");
                        civ->GetCityAt(i)->setProductionIndex(14);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(330);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Artillery");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                }
                else if(18>=numBuildings){
                    //tech 11 battl/carr/fight/sub/tank
                    if(meleeUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(320);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Infantry");
                        civ->GetCityAt(i)->setProductionIndex(23);
                    }
                    else if(rangedUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Crossbowman");
                        civ->GetCityAt(i)->setProductionIndex(9);
                    }
                    else if(mountedUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(375);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Tank");
                        civ->GetCityAt(i)->setProductionIndex(26);
                    }
                    else if(siegeUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(330);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Artillery");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(380);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Fighter");
                        civ->GetCityAt(i)->setProductionIndex(22);
                    }
                }
                else if(19>=numBuildings){
                    //tech 12 bomb/mech inf
                    if(meleeUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(380);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                        civ->GetCityAt(i)->setProductionIndex(30);
                    }
                    else if(mountedUnits<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(375);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Tank");
                        civ->GetCityAt(i)->setProductionIndex(26);
                    }
                    else if(siegeUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(330);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Artillery");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else if(fighters<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(380);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Fighter");
                        civ->GetCityAt(i)->setProductionIndex(22);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(375);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Bomber");
                        civ->GetCityAt(i)->setProductionIndex(27);
                    }
                }
                else if(20>=numBuildings){
                    //tech 13 heli/jetfig/modarmor
                    if(meleeUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(380);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                        civ->GetCityAt(i)->setProductionIndex(30);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Helicopter Gunship");
                        civ->GetCityAt(i)->setProductionIndex(28);
                    }
                    else if(mountedUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Modern Armor");
                        civ->GetCityAt(i)->setProductionIndex(31);
                    }
                    else if(siegeUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(330);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Artillery");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else if(fighters<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Jet Fighter");
                        civ->GetCityAt(i)->setProductionIndex(29);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(375);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Bomber");
                        civ->GetCityAt(i)->setProductionIndex(27);
                    }
                }
                else if(21>=numBuildings){
                    //tech 14 stealthbomb
                    if(meleeUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(380);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                        civ->GetCityAt(i)->setProductionIndex(30);
                    }
                    else if(antiMoutedUnits<1){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Helicopter Gunship");
                        civ->GetCityAt(i)->setProductionIndex(28);
                    }
                    else if(mountedUnits<2){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Modern Armor");
                        civ->GetCityAt(i)->setProductionIndex(31);
                    }
                    else if(siegeUnits<5){
                        civ->GetCityAt(i)->setCurrentProductionCost(330);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Artillery");
                        civ->GetCityAt(i)->setProductionIndex(18);
                    }
                    else if(fighters<3){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Jet Fighter");
                        civ->GetCityAt(i)->setProductionIndex(29);
                    }
                    else{
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(true);
                        civ->GetCityAt(i)->setProductionName("Stealth Bomber");
                        civ->GetCityAt(i)->setProductionIndex(32);
                    }
                }
            }
            qDebug()<<"City now producing "<<civ->GetCityAt(i)->getProductionName();
        }
        else{
#ifdef DEBUG
     qDebug()<<"City already producing "<<civ->GetCityAt(i)->getProductionName();
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
        for(int i=0; i<(14-civ->GetCityList().length());i++){
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

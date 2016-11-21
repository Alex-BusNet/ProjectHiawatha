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
    int combatUnits=0, meleeUnits=0, rangedUnits=0, siegeUnits=0, mountedUnits=0, antiMoutedUnits=0, attackShips=0, submarines=0, fighters=0, bombers=0, carriers=0;
    for(int i = 0;i< civ->GetUnitList().length();i++){
        QString unitname=civ->GetUnitAt(i)->GetName();
        if("Settler"==unitname){
            activeSettler=true;
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
        else if("Galley"==unitname||"Caravel"==unitname||"Frigate"==unitname||"Ironclad"==unitname||"Destroyer"==unitname||"Battleship"==unitname){
            attackShips++;
            combatUnits++;
        }
        else if("Submarine"==unitname){
            submarines++;
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
        else if("Carrier"==unitname){
            carriers++;
            combatUnits++;
        }
    }//Tallies various unit types

    //No tally for scout or worker?

    for(int i =0;i < civ->GetCityList().length(); i++){
        if("No Current Production"==civ->GetCityAt(i)->getProductionName()){//Determine if city is currently building something
            if(1==midGoal){//Settle more cities
                qDebug()<<"produce stuff";
                if((0==i)&&(!activeSettler)&&(11>civ->GetCityList().length())){//Only first city builds settlers - logistical parameter
                    //Logic to only build 1 settler at a time
                    ///For debugging purposes, Settler production has been set to 10. This will need to be reset to 100.
                    civ->GetCityAt(i)->setCurrentProductionCost(10);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Settler");
                    civ->GetCityAt(i)->setProductionIndex(3);
                    qDebug()<<"     Settler";
                    //Set city to build settler
                }
                else if((!civ->GetCityAt(i)->getHasWorker())&&(1<civ->GetCityList().length())){
                    civ->GetCityAt(i)->setCurrentProductionCost(70);
                    civ->GetCityAt(i)->setIsUnit(true);
                    civ->GetCityAt(i)->setProductionName("Worker");
                    civ->GetCityAt(i)->setProductionIndex(6);
                    qDebug()<<"     Worker";
                }
                else if(civ->getProvoked()&&combatUnits<20){
                    int numBuildings=civ->GetCityAt(i)->getNumberOfBuildings();
                    //Unit capabilities are based on what buildings exist (linear progression)
                    qDebug()<<"Provoked Construction at tech level "<<numBuildings;
                    if(4>=numBuildings){
                        //Tech level 1 (Arch-warrior)
                        if(meleeUnits<10){
                            civ->GetCityAt(i)->setCurrentProductionCost(45);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Warrior");
                            civ->GetCityAt(i)->setProductionIndex(5);
                            qDebug()<<"Warrior";
                        }
                        else if(rangedUnits<8){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                            qDebug()<<"Archer";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Galley");
                            civ->GetCityAt(i)->setProductionIndex(1);
                            qDebug()<<"Galley";
                        }
                    }
                    else if(7>=numBuildings){
                        //Tech Level 2 (Spear)
                        if(meleeUnits<8){
                            civ->GetCityAt(i)->setCurrentProductionCost(45);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Warrior");
                            civ->GetCityAt(i)->setProductionIndex(5);
                            qDebug()<<"Warrior";
                        }
                        else if(rangedUnits<7){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                            qDebug()<<"Archer";
                        }
                        else if(antiMoutedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(60);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Spearman");
                            civ->GetCityAt(i)->setProductionIndex(4);
                            qDebug()<<"Spearman";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Galley");
                            civ->GetCityAt(i)->setProductionIndex(1);
                            qDebug()<<"Galley";
                        }
                    }
                    else if(11>=numBuildings){
                        //Tech 3 cata/sword
                        //Tech 4 pike
                        if(meleeUnits<7){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Swordsman");
                            civ->GetCityAt(i)->setProductionIndex(8);
                            qDebug()<<"Swordsman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Archer");
                            civ->GetCityAt(i)->setProductionIndex(0);
                            qDebug()<<"Archer";
                        }
                        else if(antiMoutedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                            qDebug()<<"Catapult";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Galley");
                            civ->GetCityAt(i)->setProductionIndex(1);
                            qDebug()<<"Galley";
                        }
                    }
                    else if(12>=numBuildings){
                        //Tech 5 cross/knight
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Swordsman");
                            civ->GetCityAt(i)->setProductionIndex(8);
                            qDebug()<<"Swordsman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                            qDebug()<<"Knight";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                            qDebug()<<"Catapult";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(50);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Galley");
                            civ->GetCityAt(i)->setProductionIndex(1);
                            qDebug()<<"Galley";
                        }
                    }
                    else if(13>=numBuildings){
                        //Tech 6 carav/musket
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(150);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Musketman");
                            civ->GetCityAt(i)->setProductionIndex(16);
                            qDebug()<<"Musketman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                            qDebug()<<"Knight";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(80);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Catapult");
                            civ->GetCityAt(i)->setProductionIndex(7);
                            qDebug()<<"Catapult";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(120);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Caravel");
                            civ->GetCityAt(i)->setProductionIndex(13);
                            qDebug()<<"Caravel";
                        }
                    }
                    else if(14>=numBuildings){
                        //tech 7 cann/frig
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(150);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Musketman");
                            civ->GetCityAt(i)->setProductionIndex(16);
                            qDebug()<<"Musketman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Knight");
                            civ->GetCityAt(i)->setProductionIndex(10);
                            qDebug()<<"Knight";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cannon");
                            civ->GetCityAt(i)->setProductionIndex(12);
                            qDebug()<<"Cannon";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Frigate");
                            civ->GetCityAt(i)->setProductionIndex(15);
                            qDebug()<<"Frigate";
                        }
                    }
                    else if(15>=numBuildings){
                        //tech 8 cav/rifl
                        if(meleeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Rifleman");
                            civ->GetCityAt(i)->setProductionIndex(17);
                            qDebug()<<"Rifleman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cavalry");
                            civ->GetCityAt(i)->setProductionIndex(14);
                            qDebug()<<"Cavalry";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cannon");
                            civ->GetCityAt(i)->setProductionIndex(12);
                            qDebug()<<"Cannon";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Frigate");
                            civ->GetCityAt(i)->setProductionIndex(15);
                            qDebug()<<"Frigate";
                        }
                    }
                    else if(16>=numBuildings){
                        //tech 9 iron
                        if(meleeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Rifleman");
                            civ->GetCityAt(i)->setProductionIndex(17);
                            qDebug()<<"Rifleman";
                        }
                        else if(rangedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cavalry");
                            civ->GetCityAt(i)->setProductionIndex(14);
                            qDebug()<<"Cavalry";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(190);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cannon");
                            civ->GetCityAt(i)->setProductionIndex(12);
                            qDebug()<<"Cannon";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(275);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Ironclad");
                            civ->GetCityAt(i)->setProductionIndex(24);
                            qDebug()<<"Ironclad";
                        }
                    }
                    else if(17>=numBuildings){
                        //tech 10 art/dest/infan
                        if(meleeUnits<6){
                            civ->GetCityAt(i)->setCurrentProductionCost(320);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Infantry");
                            civ->GetCityAt(i)->setProductionIndex(23);
                            qDebug()<<"Infantry";
                        }
                        else if(rangedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(antiMoutedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(90);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Pikeman");
                            civ->GetCityAt(i)->setProductionIndex(11);
                            qDebug()<<"Pikeman";
                        }
                        else if(mountedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(225);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Cavalry");
                            civ->GetCityAt(i)->setProductionIndex(14);
                            qDebug()<<"Cavalry";
                        }
                        else if(siegeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            qDebug()<<"Artillery";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Destroyer");
                            civ->GetCityAt(i)->setProductionIndex(22);
                            qDebug()<<"Destroyer";
                        }
                    }
                    else if(18>=numBuildings){
                        //tech 11 battl/carr/fight/sub/tank
                        if(meleeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(320);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Infantry");
                            civ->GetCityAt(i)->setProductionIndex(23);
                            qDebug()<<"Infantry";
                        }
                        else if(rangedUnits<1){
                            civ->GetCityAt(i)->setCurrentProductionCost(125);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Crossbowman");
                            civ->GetCityAt(i)->setProductionIndex(9);
                            qDebug()<<"Crossbowman";
                        }
                        else if(mountedUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            qDebug()<<"Tank";
                        }
                        else if(siegeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            qDebug()<<"Artillery";
                        }
                        else if(fighters<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Fighter");
                            civ->GetCityAt(i)->setProductionIndex(22);
                            qDebug()<<"Fighter";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Battleship");
                            civ->GetCityAt(i)->setProductionIndex(19);
                            qDebug()<<"Battleship";
                        }

                        //Carrier and Sub not used right now
                    }
                    else if(19>=numBuildings){
                        //tech 12 bomb/mech inf
                        if(meleeUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                            civ->GetCityAt(i)->setProductionIndex(30);
                            qDebug()<<"Mechanized Infantry";
                        }
                        else if(mountedUnits<5){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Tank");
                            civ->GetCityAt(i)->setProductionIndex(26);
                            qDebug()<<"Tank";
                        }
                        else if(siegeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            qDebug()<<"Artillery";
                        }
                        else if(fighters<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Fighter");
                            civ->GetCityAt(i)->setProductionIndex(22);
                            qDebug()<<"Fighter";
                        }
                        else if(bombers<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Bomber");
                            civ->GetCityAt(i)->setProductionIndex(27);
                            qDebug()<<"Bomber";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Battleship");
                            civ->GetCityAt(i)->setProductionIndex(19);
                            qDebug()<<"Battleship";
                        }
                    }
                    else if(20>=numBuildings){
                        //tech 13 heli/jetfig/modarmor
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                            civ->GetCityAt(i)->setProductionIndex(30);
                            qDebug()<<"Mechanized Infantry";
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Helicopter Gunship");
                            civ->GetCityAt(i)->setProductionIndex(28);
                            qDebug()<<"Helicopter Gunship";
                        }
                        else if(mountedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Modern Armor");
                            civ->GetCityAt(i)->setProductionIndex(31);
                            qDebug()<<"Modern Armor";
                        }
                        else if(siegeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            qDebug()<<"Artillery";
                        }
                        else if(fighters<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Jet Fighter");
                            civ->GetCityAt(i)->setProductionIndex(29);
                            qDebug()<<"Jet Fighter";
                        }
                        else if(bombers<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(375);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Bomber");
                            civ->GetCityAt(i)->setProductionIndex(27);
                            qDebug()<<"Bomber";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Battleship");
                            civ->GetCityAt(i)->setProductionIndex(19);
                            qDebug()<<"Battleship";
                        }
                    }
                    else if(21>=numBuildings){
                        //tech 14 stealthbomb
                        if(meleeUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Mechanized Infantry");
                            civ->GetCityAt(i)->setProductionIndex(30);
                            qDebug()<<"Mechanized Infantry";
                        }
                        else if(antiMoutedUnits<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Helicopter Gunship");
                            civ->GetCityAt(i)->setProductionIndex(28);
                            qDebug()<<"Helicopter Gunship";
                        }
                        else if(mountedUnits<4){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Modern Armor");
                            civ->GetCityAt(i)->setProductionIndex(31);
                            qDebug()<<"Modern Armor";
                        }
                        else if(siegeUnits<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(330);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Artillery");
                            civ->GetCityAt(i)->setProductionIndex(18);
                            qDebug()<<"Artillery";
                        }
                        else if(fighters<3){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Jet Fighter");
                            civ->GetCityAt(i)->setProductionIndex(29);
                            qDebug()<<"Jet Fighter";
                        }
                        else if(bombers<2){
                            civ->GetCityAt(i)->setCurrentProductionCost(425);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Stealth Bomber");
                            civ->GetCityAt(i)->setProductionIndex(32);
                            qDebug()<<"Stealth Bomber";
                        }
                        else{
                            civ->GetCityAt(i)->setCurrentProductionCost(380);
                            civ->GetCityAt(i)->setIsUnit(true);
                            civ->GetCityAt(i)->setProductionName("Battleship");
                            civ->GetCityAt(i)->setProductionIndex(19);
                            qDebug()<<"Battleship";
                        }
                    }
                }

                else{
                    qDebug()<<" Buildings";
                    int numBuildings=civ->GetCityAt(i)->getNumberOfBuildings();
                    //Buildings are a linear progression, which simulates tech progress
                    if(0==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(75);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Walls");
                        civ->GetCityAt(i)->setProductionIndex(0);
                        qDebug()<<"walls";
                    }
                    else if(1==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(50);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Granary");
                        civ->GetCityAt(i)->setProductionIndex(5);
                        qDebug()<<"Granary";
                    }
                    else if(2==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(70);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("LightHouse");
                        civ->GetCityAt(i)->setProductionIndex(9);
                        qDebug()<<"Lighthouse";
                    }
                    else if(3==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(125);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Barracks");
                        civ->GetCityAt(i)->setProductionIndex(3);
                        qDebug()<<"Barracks";
                    }
                    else if(4==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Library");
                        civ->GetCityAt(i)->setProductionIndex(1);
                        qDebug()<<"Library";
                    }
                    else if(5==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(100);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Colosseum");
                        civ->GetCityAt(i)->setProductionIndex(7);
                        qDebug()<<"Colosseum";
                    }
                    else if(6==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(110);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Temple");
                        civ->GetCityAt(i)->setProductionIndex(12);
                        qDebug()<<"Temple";
                    }
                    else if(7==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(120);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Courthouse");
                        civ->GetCityAt(i)->setProductionIndex(8);
                        qDebug()<<"Courthouse";
                    }
                    else if(8==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Forge");
                        civ->GetCityAt(i)->setProductionIndex(16);
                        qDebug()<<"Forge";
                    }
                    else if(9==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(115);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Aqueduct");
                        civ->GetCityAt(i)->setProductionIndex(13);
                        qDebug()<<"Aqueduct";
                    }
                    else if(10==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(130);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Monastery");
                        civ->GetCityAt(i)->setProductionIndex(20);
                        qDebug()<<"Monastery";
                    }
                    else if(11==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(150);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Market");
                        civ->GetCityAt(i)->setProductionIndex(19);
                        qDebug()<<"Market";
                    }
                    else if(12==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(175);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Castle");
                        civ->GetCityAt(i)->setProductionIndex(15);
                        qDebug()<<"Castle";
                    }
                    else if(13==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(180);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("University");
                        civ->GetCityAt(i)->setProductionIndex(21);
                        qDebug()<<"University";
                    }
                    else if(14==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(220);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Observatory");
                        civ->GetCityAt(i)->setProductionIndex(2);
                        qDebug()<<"Observatory";
                    }
                    else if(15==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(200);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Theatre");
                        civ->GetCityAt(i)->setProductionIndex(6);
                        qDebug()<<"Theatre";
                    }
                    else if(16==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(300);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Bank");
                        civ->GetCityAt(i)->setProductionIndex(4);
                        qDebug()<<"Bank";
                    }
                    else if(17==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Factory");
                        civ->GetCityAt(i)->setProductionIndex(10);
                        qDebug()<<"Factory";
                    }
                    else if(18==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(365);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hospital");
                        civ->GetCityAt(i)->setProductionIndex(11);
                        qDebug()<<"Hospital";
                    }
                    else if(19==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Harbor");
                        civ->GetCityAt(i)->setProductionIndex(18);
                        qDebug()<<"Harbor";
                    }
                    else if(20==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(425);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Hydro Plant");
                        civ->GetCityAt(i)->setProductionIndex(14);
                        qDebug()<<"Hydro Plant";
                    }
                    else if(21==numBuildings){
                        civ->GetCityAt(i)->setCurrentProductionCost(500);
                        civ->GetCityAt(i)->setIsUnit(false);
                        civ->GetCityAt(i)->setProductionName("Nuclear Plant");
                        civ->GetCityAt(i)->setProductionIndex(17);
                        qDebug()<<"Nuclear Plant";
                    }
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
               qDebug()<<"Production Logic Fail"; //buildings and Minimum military strength(defensive)
            }
        }
        else{
            qDebug()<<"City already producing "<<civ->GetCityAt(i)->getProductionName();
        }
    }

    //logic for road workers
        //roadworker gets built in capitol early on
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

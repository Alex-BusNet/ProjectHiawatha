//City Control


/*
  Determine if more cities are needed (Thinking it will calculate this based on happiness)
        (If happiness >2, then city? but only if available location?)
        Probably founds cities in a sort of outward spiral from capital
            stops when it has one within x spaces of another civ?

  Determine if workers are needed (1 worker per city, 1 extra dedicated to roads, work boats?)

  Determine if military units are needed (at war or ultimate objective is war-based)(and which units)

  Determine if structures are needed (and which)
        - probably set a static order
        - then give, say, defensive structures priority if under invasion, etc



Probably called 1x per turn, and calls the operational AI, then tactical AI, from within it.
        possible tactical is called by operational?
*/

#include "ai_strategic.h"

AI_Strategic::AI_Strategic()
{

}


//*****************Calculate mid-term goal************
//The mid-term goal will be:
//1)expanding its borders
//2)Building up its resources (usually also happening during expanding borders)
//3)Preparing for war
//4)at war

//Should spend the early game expanding borders/resources
//at some point it will decide its ready to prepare for war
    //given a minimum city number, say, 5,
    //and an estimate that it has higher production and tech level than player (by enough)
//Once it has built enough units, it will launch into at war and begin maneuvering them
    //continue to build units while at war, unless its strength drops below the opposing player
    //then it will retreat and try to improve production etc by a margin over the player and try again



//*************City production Decision Tree***********
//prioritizes expansion, then having a worker, then military if it is ready for war, then buildings
//should cycle through each city, and then run this calculation only if nothing is being produced in that city

//something like
//while(city.hasnext){
        //if(city.production==NULL){

//if (((cities+active_settlers+pending_settlers)<15)&&((happiness-(2*active_settlers))>2))
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


//****************Operational AI called**************
//Operational AI will control military strategy and city founding
//Pass it whether or not civ is preparing for / at war
//Pass it number of cities currently founded







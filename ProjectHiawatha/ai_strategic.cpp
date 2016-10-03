//City Control


/*
  Determine if more cities are needed (Thinking it will calculate this based on happiness)
        (If happiness >2, then city? but only if available location?)
  Determine if workers are needed (1 worker per city, work boats?)
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



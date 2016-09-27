//City Control

#include "ai_strategic.h"

AI_Strategic::AI_Strategic()
{

}

/*
  Read ultimate objective
  Determine if more cities are needed (Thinking it will calculate this based on happiness and map resources)
  Determine if workers/caravans are needed (1 worker per city, work boats?, caravans?)
  Determine if military units are needed (at war or ultimate objective is war-based)(and which)
  Determine if structures are needed (and which)
        - probably set a static order based on victory type,
        - then give, say, defensive structures priority if under invasion, etc




 ________________Grand Strategic________________
 depends if AI will attempt anything other than conquest victories
 __________________end____________________________


*/

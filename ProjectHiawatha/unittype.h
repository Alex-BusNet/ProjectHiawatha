#ifndef UNITTYPE_H
#define UNITTYPE_H
#include <QString>

enum UnitType { WORKER, SETTLER, WARRIOR, ARCHER, SPEARMAN, PIKEMAN, HORSEMAN, SCOUT, HORSE_ARCHER, CATAPULT,
                TREBUCHET, TRIREME, GALLEY, KNIGHT, CROSSBOWMAN, LANDSNACK, LONGSWORDSMAN, SWORDSMAN, MUSKETMAN,
                RIFLEMAN, CARAVEL, CANNON, LANCER, CAVALRY, INFANTRY, FRIGATE, IRONCLAD, ARTILLERY, TANK,
                PARATROOPER, ANTI_TANK_GUN, AA, BATTLESHIP, DESTROYER, CARRIER, SUBMARINE, NUKE, MOBILE_SAM,
                HELICOPTER_GUNSHIP,MECHANIZED_INFANTRY, MODERN_ARMOR, FIGHTER, BOMBER, JET_FIGHTER, STEALTH_BOMBER, GDR, GANDHI_DR
              };

const unsigned int ALL_BUTTONS_VISIBLE = 0x3FFFF;
const unsigned int WORKER_BUTTONS_VISIBLE = 0x3FFC3;
const unsigned int MELEE_BUTTONS_VISIBLE = 0x30013;
const unsigned int RANGE_BUTTONS_VISIBLE = 0x30007;
const unsigned int SETTLER_BUTTONS_VISIBLE = 0x30023;
const unsigned int NO_BUTTONS_VISIBLE = 0x30000;
const unsigned int BASIC_BUTTONS_VISIBLE = 0x30003;

/*
 * b19 = <Empty>
 * b18 = <Empty>
 * b17 = Technology Tree
 * b16 = Diplomacy
 *
 * b15 = Build Farm
 * b14 = Build Mine
 * b13 = Build Trade Post
 * b12 = Build Plantantion
 *
 * b11 = Build Road
 * b10 = Build Quarry
 * b09 = Build Fish Boat
 * b08 = Build Camp
 *
 * b07 = Build Oil Well
 * b06 = Build Pasture
 * b05 = Found City
 * b04 = Attack Unit
 *
 * b03 = Attack City        - Is never visible by default
 * b02 = Range Attack
 * b01 = Fortify Unit
 * b00 = Move Unit
 */

#endif // UNITTYPE_H

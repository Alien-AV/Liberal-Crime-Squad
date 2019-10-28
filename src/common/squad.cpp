#include "includes.h"
#include <algorithm>

squadst::squadst()
{
    for(int p=0;p<6;p++) squad[p]=NULL;
    strcpy(name,"");
    activity.type=ACTIVITY_NONE,id=-1,stance=SQUADSTANCE_STANDARD;
}
squadst::~squadst() { delete_and_clear(loot); }


bool squadst::remove_from_squad(Creature* squaddie) {    //assumes single copy of a squaddie in a squad
    auto new_end_iter = std::remove(std::begin(squad), std::end(squad), squaddie);
    auto removed_any = new_end_iter != std::end(squad);
    if(removed_any) *new_end_iter = nullptr;
    return removed_any;
}
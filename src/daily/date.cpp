/*
 * Copyright (c) 2002,2003,2004 by Tarn Adams
 * Copyright 2017 Stephen M. Webb  <stephen.webb@bregmasoft.ca>
 *
 * This file is part of Liberal Crime Squad.
 *
 * Liberal Crime Squad is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/*
 * This file was created by Chris Johnson (grundee@users.sourceforge.net)
 * by copying code from game.cpp into monthly/endgame.cpp.
 */
#include "daily/date.h"
#include "externs.h"
#include "locations/world.h"
#include <algorithm>

enum DateResults
{
   DATERESULT_MEETTOMORROW,
   DATERESULT_BREAKUP,
   DATERESULT_JOINED,
   DATERESULT_ARRESTED
};

// Handles the result of a date or vacation
static int dateresult(int aroll,int troll,datest &d,int e,int p,int y)
{
   music.play(MUSIC_DATING);
   std::string s = "";
   if(aroll>troll)
   {
      set_color(COLOR_CYAN,COLOR_BLACK,1);
      move(y,0);y++;
      addstr(d.date[e]->name, gamelog);
      addstr(" is quite taken with ", gamelog);
      addstr(pool[p]->name, gamelog);
      addstr("'s unique life philosophy...", gamelog);
      gamelog.newline();

      getkey();

      if(loveslavesleft(*pool[p]) <= 0)
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,1);

         move(y++,0);
         addstr("But ", gamelog);
         addstr(pool[p]->name, gamelog);
         addstr(" is already dating ", gamelog);
         int num_relationships=loveslaves(*pool[p]);
         if(pool[p]->flag&CREATUREFLAG_LOVESLAVE) num_relationships++;
         if(num_relationships==1) addstr("someone!", gamelog);
         else addstr(std::to_string(num_relationships)+" people!", gamelog);
         gamelog.newline();
         move(y++,0);
         addstr(pool[p]->name, gamelog);
         addstr(" isn't seductive enough to juggle ", gamelog);
         if(num_relationships==1) addstr("another", gamelog);
         else addstr("yet another", gamelog);
         addstr(" relationship.", gamelog);
         gamelog.newline();

         getkey();
         set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(y++,0);
         addstr("It was fun though. They agree to part ways amicably.", gamelog);
         gamelog.nextMessage();

         getkey();

         delete_and_remove(d.date,e);

         return DATERESULT_BREAKUP;
      }

      if(LCSrandom((aroll-troll)/2)>d.date[e]->get_attribute(ATTRIBUTE_WISDOM,true))
      {
         set_color(COLOR_GREEN,COLOR_BLACK,1);
         move(y,0);y++;
         addstr("In fact, ", gamelog);
         addstr(d.date[e]->name, gamelog);
         addstr(" is ", gamelog);
         addstr(pool[p]->name, gamelog);
         addstr("'s totally unconditional love-slave!", gamelog);
         gamelog.nextMessage();
         //Get map of their workplace
         location[d.date[e]->worklocation]->mapped=1;
         location[d.date[e]->worklocation]->hidden=0;

         getkey();

         d.date[e]->flag|=CREATUREFLAG_LOVESLAVE;
         d.date[e]->hireid=pool[p]->id;

         erase();

         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(0,0);
         addstr("The Self-Nullifying Infatuation of ");
         addstr(d.date[e]->propername);

         move(2,0);
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         addstr("What name will you use for this ");
         addstr(d.date[e]->get_type_name());
         addstr(" in its presence?");
         move(3,0);
         addstr("If you do not enter anything, their real name will be used.");

         enter_name(4,0,d.date[e]->name,CREATURE_NAMELEN,d.date[e]->propername);

         sleeperize_prompt(*d.date[e],*pool[p],8);

         pool.push_back(d.date[e]);
         stat_recruits++;
         d.date.erase(d.date.begin() + e);

         return DATERESULT_JOINED;
      }
      else
      {
         if(d.date[e]->align == Alignment::CONSERVATIVE && d.date[e]->get_attribute(ATTRIBUTE_WISDOM,false)>3)
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            y++;
            move(y++,0);
            addstr(s+pool[p]->name+" is slowly warming "+d.date[e]->name+"'s frozen Conservative heart.", gamelog);
            gamelog.newline();
            move(y++,0);
            d.date[e]->adjust_attribute(ATTRIBUTE_WISDOM,-1);
            d.date[e]->adjust_attribute(ATTRIBUTE_HEART,+1);
         }
         else if(d.date[e]->get_attribute(ATTRIBUTE_WISDOM,false)>3)
         {
            d.date[e]->adjust_attribute(ATTRIBUTE_WISDOM,-1);
         }
         //Possibly date reveals map of location
         else if(location[d.date[e]->worklocation]->mapped==0 && !LCSrandom(d.date[e]->get_attribute(ATTRIBUTE_WISDOM,false)))
         {
            y++;
            move(y++,0);
            addstr(s+d.date[e]->name+" turns the topic of discussion to the "
               +location[d.date[e]->worklocation]->name+".", gamelog);
            gamelog.newline();
            move(y++,0);
            if(!(location[d.date[e]->worklocation]->type<=SITE_RESIDENTIAL_SHELTER))
            {
               addstr(pool[p]->name, gamelog);
               addstr(" was able to create a map of the site with this information.", gamelog);
               gamelog.newline();
               y++;
            }
            else
            {
               addstr(pool[p]->name, gamelog);
               addstr(" knows all about that already.", gamelog);
               gamelog.newline();
               y++;
            }
            location[d.date[e]->worklocation]->mapped=1;
            location[d.date[e]->worklocation]->hidden=0;
         }

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y++,0);
         addstr("They'll meet again tomorrow.", gamelog);
         gamelog.nextMessage();

         getkey();

         return DATERESULT_MEETTOMORROW;
      }
   }
   else if(aroll==troll)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(y++,0);
      addstr(d.date[e]->name,gamelog);
      addstr(" seemed to have fun, but left early",gamelog);
      move(y++,0);
      switch(LCSrandom(4))
      {
      case 0: addstr(s+" to wash "+d.date[e]->hisher()+" hair.",gamelog); break;
      case 1: addstr(" due to an allergy attack.",gamelog); break;
      case 2: addstr(" due to an early meeting tomorrow.",gamelog); break;
      case 3: addstr(s+" to catch "+d.date[e]->hisher()+" favourite TV show.", gamelog); break;
      case 4:
         addstr(s+" to take care of "+d.date[e]->hisher()+" pet",gamelog);
         switch (LCSrandom(3 + (law[LAW_ANIMALRESEARCH]==Alignment::ARCH_CONSERVATIVE)))
         {
         case 0: addstr(" cat.",gamelog); break;
         case 1: addstr(" dog.",gamelog); break;
         case 2: addstr(" fish.",gamelog); break;
         case 3: addstr(" six-legged pig.",gamelog); break;
         }
         break;
      case 5: addstr(" to go to a birthday party.",gamelog); break;
      case 6: addstr(s+" to recharge "+d.date[e]->hisher()+" cell phone.",gamelog); break;
      }
      move(y++,0);
      addstr("They'll meet again tomorrow.",gamelog);
      gamelog.nextMessage();

      getkey();

      return DATERESULT_MEETTOMORROW;
   }
   else
   {
      //WISDOM POSSIBLE INCREASE
      if (d.date[e]->align == Alignment::CONSERVATIVE && aroll < troll/2)
      {
         set_color(COLOR_RED,COLOR_BLACK,1);
         move(y++,0);

         addstr("Talking with ", gamelog);
         addstr(d.date[e]->name, gamelog);
         addstr(" actually curses ", gamelog);
         addstr(pool[p]->name, gamelog);
         addstr("'s mind with wisdom!!!", gamelog);
         gamelog.newline();
         pool[p]->adjust_attribute(ATTRIBUTE_WISDOM,+1);

         if(d.date[e]->get_skill(SKILL_RELIGION)>pool[p]->get_skill(SKILL_RELIGION))
            pool[p]->train(SKILL_RELIGION,20*(d.date[e]->get_skill(SKILL_RELIGION)-pool[p]->get_skill(SKILL_RELIGION)));
         if(d.date[e]->get_skill(SKILL_SCIENCE)>pool[p]->get_skill(SKILL_SCIENCE))
            pool[p]->train(SKILL_SCIENCE,20*(d.date[e]->get_skill(SKILL_SCIENCE)-pool[p]->get_skill(SKILL_SCIENCE)));
         if(d.date[e]->get_skill(SKILL_BUSINESS)>pool[p]->get_skill(SKILL_BUSINESS))
            pool[p]->train(SKILL_BUSINESS,20*(d.date[e]->get_skill(SKILL_BUSINESS)-pool[p]->get_skill(SKILL_BUSINESS)));

         getkey();
      }

      //BREAK UP

      // If your squad member is wanted by the police, a conservative who breaks up with
      // them has a 1 in 50 chance of ratting them out, unless the person being dated is law
      // enforcement, prison guard, or agent, in which case there is a 1 in 4 chance. -Fox
      if((iscriminal(*pool[p])) &&
         (!LCSrandom(50) ||(LCSrandom(2) && (d.date[e]->kidnap_resistant()))))
      {
         move(y++,0);
         set_color(COLOR_RED,COLOR_BLACK,1);
         addstr(d.date[e]->name, gamelog);
         addstr(" was leaking information to the police the whole time!", gamelog);

         getkey();

         move(y++,0);
         // 3/4 chance of being arrested if less than 50 juice,
         // 1/2 chance of being arrested if more than 50 juice
         if((pool[p]->juice<50 && LCSrandom(2)) || LCSrandom(2))
         {
            // Find the police station
            long ps=find_police_station(*pool[p]);

            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr(pool[p]->name, gamelog);
            addstr(" has been arrested.", gamelog);
            gamelog.nextMessage();

            removesquadinfo(*pool[p]);
            pool[p]->carid=-1;
            pool[p]->location=ps;
            pool[p]->drop_weapons_and_clips(NULL);
            pool[p]->activity.type=ACTIVITY_NONE;

            getkey();

            delete_and_remove(d.date,e);

            return DATERESULT_ARRESTED;
         }
         else
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("But ", gamelog);
            addstr(pool[p]->name, gamelog);
            addstr(" escapes the police ambush!", gamelog);
            gamelog.nextMessage();
         }
      }
      else
      {
         int ls = loveslaves(*pool[p]);
         if (ls > 0 && LCSrandom(2))
         {
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            move(y++,0);
            addstr(s+"The date starts well, but goes horribly wrong when "+d.date[e]->name, gamelog);
            move(y++,0);
            addstr(s+"notices "+pool[p]->name+"'s ", gamelog);
            switch (ls)
            {
               case 5:
                  addstr("awe-inspiring ", gamelog);
                  break;
               case 4:
                  addstr("intricate ", gamelog);
                  break;
               case 3:
                  addstr("complicated ", gamelog);
                  break;
               case 2:
                  addstr("detailed ", gamelog);
                  break;
               case 1:
                  break;
               default:
                  addstr("mind-bending ", gamelog);
            }
            addstr(s+"schedule for keeping "+d.date[e]->himher(),gamelog);
            addstr(s+" from meeting ",gamelog);
            move(y++,0);
            int lsfound = 0;
            for (int q=0; q<(int)pool.size(); q++)
            {
               if(pool[q]->hireid == pool[p]->id && pool[q]->alive && pool[q]->flag & CREATUREFLAG_LOVESLAVE)
               {
                  lsfound++;
                  if (lsfound == 1)
                  {
                     addstr(pool[q]->name, gamelog);
                  }
                  else if (lsfound < ls)
                  {
                     addstr(s+", "+pool[q]->name, gamelog);
                  }else
                  {
                     addstr(s+" and "+ pool[q]->name, gamelog);
                  }
               }
            }
            addstr(".",gamelog);
            gamelog.newline();
            move(y++,0);
         }
         else
         {
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            move(y++,0);
            addstr(d.date[e]->name, gamelog);
            addstr(" can sense that things just aren't working out.", gamelog);
            gamelog.newline();
            move(y++,0);
         }
         addstr("This relationship is over.", gamelog);
         gamelog.nextMessage();
      }

      getkey();

      delete_and_remove(d.date,e);

      return DATERESULT_BREAKUP;
   }
}

/* daily - date - dater p gets back from vacation */
bool
completevacation(datest& d, int p, char& clearformess)
{
   music.play(MUSIC_DATING);
   int e=0;

   clearformess=1;

   erase();
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(0,0);
   addstr(pool[p]->name, gamelog);
   addstr(" is back from vacation.", gamelog);
   gamelog.nextMessage();

   // Temporarily make the date Conservative so that high-juice liberals aren't trivial to seduce
   Alignment datealignment = d.date[e]->align;
   d.date[e]->align = Alignment::CONSERVATIVE;

   short aroll=pool[p]->skill_roll(SKILL_SEDUCTION)*2;
   short troll=d.date[e]->attribute_roll(ATTRIBUTE_WISDOM);

   // Attribute roll over; reset date's alignment to what it should be
   d.date[e]->align = datealignment;

   pool[p]->train(SKILL_SEDUCTION,LCSrandom(11)+15);

   int thingsincommon=0;
   for(int s=0;s<SKILLNUM;s++)
      if(d.date[e]->get_skill(s)>=1 && pool[p]->get_skill(s)>=1)
         //Has a skill that is at least half the same skill of the other person on the date.
         if (d.date[e]->get_skill(s)<=pool[p]->get_skill(s)*2)
            thingsincommon++;
   aroll += thingsincommon*3;

   pool[p]->train(SKILL_SCIENCE,
      max(d.date[e]->get_skill(SKILL_SCIENCE)-pool[p]->get_skill(SKILL_SCIENCE),0));
   pool[p]->train(SKILL_RELIGION,
      max(d.date[e]->get_skill(SKILL_RELIGION)-pool[p]->get_skill(SKILL_RELIGION),0));
   pool[p]->train(SKILL_BUSINESS,
      max(d.date[e]->get_skill(SKILL_BUSINESS)-pool[p]->get_skill(SKILL_BUSINESS),0));

   if(d.date[e]->skill_roll(SKILL_BUSINESS))
   {
      troll+=d.date[e]->skill_roll(SKILL_BUSINESS);
      aroll+=pool[p]->skill_roll(SKILL_BUSINESS);
   }
   if(d.date[e]->skill_roll(SKILL_RELIGION))
   {
      troll+=d.date[e]->skill_roll(SKILL_RELIGION);
      aroll+=pool[p]->skill_roll(SKILL_RELIGION);
   }
   if(d.date[e]->skill_roll(SKILL_SCIENCE))
   {
      troll+=d.date[e]->skill_roll(SKILL_SCIENCE);
      aroll+=pool[p]->skill_roll(SKILL_SCIENCE);
   }


   switch(dateresult(aroll,troll,d,e,p,2))
   {
   default:
   case DATERESULT_MEETTOMORROW:return 0;
   case DATERESULT_BREAKUP:     return 1;
   case DATERESULT_JOINED:      return 1;
   case DATERESULT_ARRESTED:    return 1;
   }
}



/* daily - date - dater p goes on some dates */
bool
completedate(datest& d, int p, char& clearformess)
{
   music.play(MUSIC_DATING);
   int e;
   clearformess=1;

   erase();
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(0,0);
   addstr(pool[p]->name, gamelog);
   addstr(" has ", gamelog);
   if(len(d.date)==1)
   {
      if(pool[p]->clinic) addstr("a \"hot\" date with ", gamelog);
      else addstr("a hot date with ", gamelog);
   }
   else addstr("dates to manage with ", gamelog);
   for(e=0;e<len(d.date);e++)
   {
      addstr(d.date[e]->name, gamelog);

      if(e<=len(d.date)-3) addstr(", ", gamelog);
      else if(e==len(d.date)-2) addstr(" and ", gamelog);
      else
      {
         if(pool[p]->clinic>0)
         {
            addstr(" at ", gamelog);
            addstr(location[pool[p]->location]->name, gamelog);
         }
         addstr(".", gamelog);
      }
   }

   gamelog.newline();

   getkey();

   if(len(d.date)>1&&!LCSrandom(len(d.date)>2?4:6))
   {
      switch(LCSrandom(3))
      {
         case 0:
            move(2,0);
            if(len(d.date)>2) addstr("Unfortunately, they all know each other and had been discussing",gamelog);
            else addstr("Unfortunately, they know each other and had been discussing",gamelog);
            move(3,0);
            addstr(pool[p]->name,gamelog);
            addstr(".  An ambush was set for the lying dog...",gamelog);
            gamelog.newline();

            getkey();

            break;
         case 1:
            move(2,0);
            if(len(d.date)>2) addstr("Unfortunately, they all turn up at the same time.",gamelog);
            else addstr("Unfortunately, they turn up at the same time.",gamelog);
            gamelog.newline();
            move(3,0);
            addstr("Ruh roh...",gamelog);
            gamelog.newline();

            getkey();

            break;
         default:
            move(2,0);
            addstr(pool[p]->name,gamelog);
            if(len(d.date)>2)
               addstr_fl(gamelog," realizes %s has committed to eating %d meals at once.",pool[p]->heshe(),len(d.date));
            else
            {
               addstr(" mixes up the names of ",gamelog);
               addstr(d.date[0]->name,gamelog);
               addstr(" and ",gamelog);
               addstr(d.date[1]->name,gamelog);
               gamelog.newline();
            }
            move(3,0);
            addstr("Things go downhill fast.",gamelog);
            gamelog.newline();

            getkey();

            break;
      }

	   static const char *date_fail[] =
	   {
	      " is publicly humiliated."
	      " runs away.",
	      " escapes through the bathroom window.",
	      " spends the night getting drunk alone.",
	      " gets chased out by an angry mob.",
	      " gets stuck washing dishes all night.",
	      " is rescued by a passing Elite Liberal.",
	      " makes like a tree and leaves."
	   };
      move(5,0);
      addstr(pool[p]->name,gamelog);
      addstr(pickrandom(date_fail),gamelog);
      addjuice(*pool[p],-5,-50);
      gamelog.nextMessage();

      getkey();

      return 1;
   }

   for(e=len(d.date)-1;e>=0;e--)
   {
      erase();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("Seeing ");
      addstr(d.date[e]->name, gamelog);
      addstr(", ", gamelog);
      addstr(d.date[e]->get_type_name(),gamelog);
      addstr(", ", gamelog);
      addstr(location[d.date[e]->worklocation]->getname(false,true), gamelog);
      gamelog.newline();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      printfunds();

      //Others come to dates unarmed and wearing normal
      //clothing
      vector<Item*> temp;
      d.date[e]->drop_weapons_and_clips(&temp);
      Armor atmp(*armortype[getarmortype("ARMOR_CLOTHES")]);
      d.date[e]->give_armor(atmp,&temp);

      printcreatureinfo(d.date[e]);
      makedelimiter();

      while(len(temp))
      {
         if(temp.back()->is_weapon())
            d.date[e]->give_weapon(*(static_cast<Weapon*>(temp.back())),NULL); //casts -XML
         else if(temp.back()->is_armor())
            d.date[e]->give_armor(*(static_cast<Armor*>(temp.back())),NULL);
         else if(temp.back()->is_clip())
            d.date[e]->take_clips(*(static_cast<Clip*>(temp.back())),temp.back()->get_number());
         delete_and_remove(temp,len(temp)-1);
      }

      move(10,0);
      addstr("How should ");
      addstr(pool[p]->name);
      addstr(" approach the situation?");

      if(ledger.get_funds()>=100&&!pool[p]->clinic)set_color(COLOR_WHITE,COLOR_BLACK,0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(11,0);
      addstr("A - Spend a hundred bucks tonight to get the ball rolling.");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(12,0);
      addstr("B - Try to get through the evening without spending a penny.");
      if(!pool[p]->clinic&&pool[p]->blood==100)set_color(COLOR_WHITE,COLOR_BLACK,0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(13,0);
      if(pool[p]->blood==100) addstr("C - Spend a week on a cheap vacation (stands up any other dates).");
      else addstr("C - Spend a week on a cheap vacation (must be uninjured).");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(14,0);
      addstr("D - Break it off.");
      if (d.date[e]->align == Alignment::CONSERVATIVE && !pool[p]->clinic)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(15,0);
         addstr("E - Just kidnap the Conservative bitch.");
      }

      int thingsincommon = 0;
      for(int s=0;s<SKILLNUM;s++)
         if(d.date[e]->get_skill(s)>=1 && pool[p]->get_skill(s)>=1)
            //Has a skill that is at least half the same skill of the other person on the date.
            if (d.date[e]->get_skill(s)<=pool[p]->get_skill(s)*2)
               thingsincommon++;
      while(true)
      {
         int c=getkey();

         short aroll=pool[p]->skill_roll(SKILL_SEDUCTION);
         short troll=d.date[e]->attribute_roll(ATTRIBUTE_WISDOM);
         if(d.date[e]->align==Alignment::CONSERVATIVE)
            troll+=troll*(d.date[e]->juice/100);
         // Even liberals and moderates shouldn't be TOO easy to seduce! -- SlatersQuest
         else if(d.date[e]->align==Alignment::MODERATE)
            troll+=troll*(d.date[e]->juice/150);
         else troll+=troll*(d.date[e]->juice/200);

         char test=0;
		   aroll += thingsincommon * 3;
         if(c=='a'&&ledger.get_funds()>=100&&!pool[p]->clinic)
         {
            ledger.subtract_funds(100,EXPENSE_DATING);
            aroll+=LCSrandom(10);
            test=true;
         }
         else if(c=='b') test=true;

         if(test)
         {
            pool[p]->train(SKILL_SEDUCTION,LCSrandom(4)+5);
            pool[p]->train(SKILL_SCIENCE,
               max(d.date[e]->get_skill(SKILL_SCIENCE)-pool[p]->get_skill(SKILL_SCIENCE),0));
            pool[p]->train(SKILL_RELIGION,
               max(d.date[e]->get_skill(SKILL_RELIGION)-pool[p]->get_skill(SKILL_RELIGION),0));
            pool[p]->train(SKILL_BUSINESS,
               max(d.date[e]->get_skill(SKILL_BUSINESS)-pool[p]->get_skill(SKILL_BUSINESS),0));

            if(d.date[e]->get_skill(SKILL_BUSINESS))
            {
               troll+=d.date[e]->skill_roll(SKILL_BUSINESS);
               aroll+=pool[p]->skill_roll(SKILL_BUSINESS);
            }
            if(d.date[e]->get_skill(SKILL_RELIGION))
            {
               troll+=d.date[e]->skill_roll(SKILL_RELIGION);
               aroll+=pool[p]->skill_roll(SKILL_RELIGION);
            }
            if(d.date[e]->get_skill(SKILL_SCIENCE))
            {
               troll+=d.date[e]->skill_roll(SKILL_SCIENCE);
               aroll+=pool[p]->skill_roll(SKILL_SCIENCE);
            }

            int y=17;
            if(dateresult(aroll,troll,d,e,p,y)==DATERESULT_ARRESTED) return 1;
            break;
         }

         if(c=='c'&&!pool[p]->clinic&&pool[p]->blood==100)
         {
            for(int e2=len(d.date)-1;e2>=0;e2--)
            {
               if(e2==e) continue;
               delete_and_remove(d.date,e2);
               e=0;
            }
            d.timeleft=7;
            pool[p]->train(SKILL_SEDUCTION,LCSrandom(40)+15);
            pool[p]->train(SKILL_SCIENCE,
               max((d.date[e]->get_skill(SKILL_SCIENCE)-pool[p]->get_skill(SKILL_SCIENCE))*4,0));
            pool[p]->train(SKILL_RELIGION,
               max((d.date[e]->get_skill(SKILL_RELIGION)-pool[p]->get_skill(SKILL_RELIGION))*4,0));
            pool[p]->train(SKILL_BUSINESS,
               max((d.date[e]->get_skill(SKILL_BUSINESS)-pool[p]->get_skill(SKILL_BUSINESS))*4,0));
            return 0;
         }
         if(c=='d')
         {
            delete_and_remove(d.date,e);
            break;
         }
         if (c=='e' && d.date[e]->align == Alignment::CONSERVATIVE && !pool[p]->clinic)
         {
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            int bonus=0;
            move(17,0);
            addstr(pool[p]->name, gamelog);

            if(pool[p]->get_weapon().is_ranged())
            {
               addstr(" comes back from the bathroom toting the ", gamelog);
               addstr(pool[p]->get_weapon().get_name(1), gamelog);
               move(18,0);
               addstr("and threatens to blow the Conservative's brains out!", gamelog);
               gamelog.newline();
               bonus=5;
            }
            else if(pool[p]->is_armed())
            {
               addstr(" grabs the Conservative from behind, holding the ", gamelog);
               addstr(pool[p]->get_weapon().get_name(1), gamelog);
               move(18,0);
               addstr("to the corporate slave's throat!", gamelog);
               gamelog.newline();
               if(pool[p]->get_weapon().can_take_hostages())
                  bonus=5;
               else bonus=-1; // Conservative emboldened by the fact that you're trying
            }                 // to kidnap them with a gavel or some shit like that
            else
            {
               addstr(" seizes the Conservative swine from behind and warns it", gamelog);
               move(18,0);
               if (law[LAW_FREESPEECH] != Alignment::ARCH_CONSERVATIVE)
                  addstr("not to fuck around!", gamelog);
               else
                  addstr("not to [resist]!", gamelog);
               gamelog.newline();

               bonus+=pool[p]->get_skill(SKILL_HANDTOHAND)-1;
            }

            getkey();

            // Kidnap probably succeeds if the conservative isn't very dangerous,
            // but fails 15 times as often if the conservative is tough stuff.
            if((!d.date[e]->kidnap_resistant()&&
                LCSrandom(15))||
                LCSrandom(2+bonus))
            {

               set_color(COLOR_GREEN,COLOR_BLACK,1);
               move(20,0);
               addstr(d.date[e]->name, gamelog);
               if(bonus)
               {
                  addstr(" doesn't resist.", gamelog);
                  gamelog.newline();
               }
               else
               {
                  addstr(" struggles and yells for help, but nobody comes.", gamelog);
                  gamelog.newline();
               }

               getkey();

               move(22,0);
               addstr(pool[p]->name, gamelog);
               addstr(" kidnaps the Conservative!", gamelog);
               gamelog.nextMessage();

               getkey();

               d.date[e]->namecreature();
               strcpy(d.date[e]->propername,d.date[e]->name);

               d.date[e]->location=pool[p]->location;
               d.date[e]->base=pool[p]->base;
               d.date[e]->flag|=CREATUREFLAG_MISSING;

               //Kidnapped wearing normal clothes and no weapon
               d.date[e]->drop_weapons_and_clips(NULL);
               Armor clothes(*armortype[getarmortype("ARMOR_CLOTHES")]);
               d.date[e]->give_armor(clothes,NULL);

               //Create interrogation data
               d.date[e]->activity.intr()=new interrogation;

               erase();

               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(0,0);
               addstr("The Education of ");
               addstr(d.date[e]->propername);

               move(2,0);
               set_color(COLOR_WHITE,COLOR_BLACK,0);
               addstr("What name will you use for this ");
               addstr(d.date[e]->get_type_name());
               addstr(" in its presence?");

               move(3,0);
               addstr("If you do not enter anything, their real name will be used.");

               enter_name(4,0,d.date[e]->name,CREATURE_NAMELEN,d.date[e]->propername);

               pool.push_back(d.date[e]);
               stat_kidnappings++;
               d.date.erase(d.date.begin()+e);
               break;
            }
            else
            {
               int y=20;
               if(LCSrandom(2))
               {
                  set_color(COLOR_MAGENTA,COLOR_BLACK,1);
                  move(y++,0);
                  addstr(d.date[e]->name, gamelog);
                  addstr(" manages to get away on the way back to the safehouse!", gamelog);
                  gamelog.newline();

                  getkey();

                  move((++y)++,0);
                  addstr(pool[p]->name, gamelog);
                  addstr(" has failed to kidnap the Conservative.", gamelog);
                  gamelog.nextMessage();

                  // Charge with kidnapping
                  criminalize(*pool[p],LAWFLAG_KIDNAPPING);

                  getkey();

                  delete_and_remove(d.date,e);
                  break;
               }
               else
               {
                  set_color(COLOR_RED,COLOR_BLACK,1);
                  move(y++,0);
                  addstr(d.date[e]->name, gamelog);
                  addstr("'s fist is the last thing ", gamelog);
                  addstr(pool[p]->name, gamelog);
                  addstr(" remembers seeing!", gamelog);
                  gamelog.newline();

                  getkey();

                  move((++y)++,0);
                  addstr("The Liberal wakes up in the police station...", gamelog);
                  gamelog.nextMessage();

                  // Find the police station
                  int ps=find_police_station(*pool[p]);

                  // Arrest the Liberal
                  removesquadinfo(*pool[p]);
                  pool[p]->carid=-1;
                  pool[p]->location=ps;
                  pool[p]->drop_weapons_and_clips(NULL);
                  pool[p]->activity.type=ACTIVITY_NONE;

                  // Charge with kidnapping
                  criminalize(*pool[p],LAWFLAG_KIDNAPPING);

                  getkey();

                  delete_and_remove(d.date,e);
                  return 1;
               }
            }
         }
      }
   }

   if(len(d.date))
   {
      d.timeleft=0;
      return 0;
   }
   else return 1;
}

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

// Note: this file is encoded in the PC-8 / Code Page 437 / OEM-US character set
// (The same character set used by Liberal Crime Squad when it is running)
// Certain special characters won't display correctly unless your text editor is
// set to use that character set, such as this e with an accent: ‚

// In Windows Notepad with the Terminal font, OEM/DOS encoding it should work fine.
// You can set this in Notepad by going to Format->Font and choosing the Terminal font,
// then choosing OEM/DOS in the Script dropdown box.

// In Notepad++ go to the Encoding menu, Character sets, Western European, OEM-US... easy!

// In Code::Blocks's editor go to Settings->Editor->the Other Settings tab and
// then pick WINDOWS-437 from the dropdown box and then choose the radio button
// to make this the default encoding and disable auto-detection of the encoding.
// Then close the file and reopen it (since Code::Blocks detects the encoding
// when it opens the file and it can't be changed after that; what we changed was
// how it detects encoding for files it opens in the future, not files already open).

// In Microsoft Visual C++, right-click the file in the Solution Explorer,
// select "Open With...", choose "C++ Source Code Editor (with encoding)",
// then choose "OEM United States - Codepage 437".

// In MS-DOS Editor (included with Windows as EDIT.COM in your system32 directory),
// the codepage will be correct already since it's running in a console window just
// like Liberal Crime Squad. Well OK, the encoding might be wrong, but then it's wrong
// in Liberal Crime Squad TOO, and to fix it, go to Control Panel, Regional and Language Settings,
// Advanced tab, and choose English (United States) from the dropdown box as the encoding
// for non-Unicode applications, then press OK.

// If you have a Linux or other UNIX-based system you are obviously smart enough
// to figure out for yourself how to open a file in OEM-US PC-8 codepage 437 in
// your favorite text editor. If you're on Mac OS X, well that's UNIX-based, figure
// it out for yourself.

#include <externs.h>
#include "daily/siege.h"

// Sets the text color to the thematic color for the given alignment
// extended_range forces colors to be set on a 5 point scale instead
// of just basic liberal-moderate-conservative
void set_alignment_color(Alignment alignment, bool extended_range)
{
   switch(alignment)
   {
   case Alignment::ARCH_CONSERVATIVE:
      set_color(COLOR_RED,COLOR_BLACK,1);
      break;
   case Alignment::CONSERVATIVE:
      if(extended_range)
         set_color(COLOR_MAGENTA,COLOR_BLACK,1);
      else set_color(COLOR_RED,COLOR_BLACK,1);
      break;
   case Alignment::MODERATE:
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      break;
   case Alignment::LIBERAL:
      if(extended_range)
         set_color(COLOR_CYAN,COLOR_BLACK,1);
      else set_color(COLOR_GREEN,COLOR_BLACK,1);
      break;
   case Alignment::ELITE_LIBERAL:
      set_color(COLOR_GREEN,COLOR_BLACK,1);
      break;
   case Alignment::STALINIST:
      set_color(COLOR_RED,COLOR_BLACK,1);
      break;
   default: // This should not happen! Set a strange color to indicate an error!
      set_color(COLOR_YELLOW,COLOR_RED,1,1); // blinking yellow on red background
      break;
   }
}


// Sets the text color per activity type
void set_activity_color(long activity_type)
{
   switch(activity_type)
   {
      // Liberal actvism
   case ACTIVITY_SLEEPER_LIBERAL:
   case ACTIVITY_TROUBLE:
   case ACTIVITY_GRAFFITI:
   case ACTIVITY_DOS_ATTACKS:
   case ACTIVITY_HACKING:
   case ACTIVITY_WRITE_LETTERS:
   case ACTIVITY_WRITE_GUARDIAN:
      set_color(COLOR_GREEN,COLOR_BLACK,1);
      break;

      // Recruitment
   case ACTIVITY_RECRUITING:
      set_color(COLOR_GREEN,COLOR_BLACK,0);
      break;

      // Less exciting liberal activities
   case ACTIVITY_SLEEPER_SPY:
   case ACTIVITY_COMMUNITYSERVICE:
   case ACTIVITY_POLLS:
      set_color(COLOR_BLUE,COLOR_BLACK,1);
      break;

      // Stealing things
   case ACTIVITY_SLEEPER_STEAL:
   case ACTIVITY_WHEELCHAIR:
   case ACTIVITY_STEALCARS:
      set_color(COLOR_CYAN,COLOR_BLACK,0);
      break;

      // Illegal fundraising
   case ACTIVITY_SLEEPER_EMBEZZLE:
   case ACTIVITY_SELL_DRUGS:
   case ACTIVITY_PROSTITUTION:
   case ACTIVITY_CCFRAUD:
   case ACTIVITY_DOS_RACKET:
      set_color(COLOR_RED,COLOR_BLACK,1);
      break;

      // Legal fundraising
   case ACTIVITY_DONATIONS:
   case ACTIVITY_SELL_TSHIRTS:
   case ACTIVITY_SELL_ART:
   case ACTIVITY_SELL_MUSIC:
      set_color(COLOR_CYAN,COLOR_BLACK,1);
      break;

      // Clothing/garment stuff
   case ACTIVITY_REPAIR_ARMOR:
   case ACTIVITY_MAKE_ARMOR:
      set_color(COLOR_CYAN,COLOR_BLACK,0);
      break;

      // Teaching
   case ACTIVITY_TEACH_POLITICS:
   case ACTIVITY_TEACH_FIGHTING:
   case ACTIVITY_TEACH_COVERT:
   //and studying
   case ACTIVITY_STUDY_DEBATING:
   case ACTIVITY_STUDY_MARTIAL_ARTS:
   case ACTIVITY_STUDY_DRIVING:
   case ACTIVITY_STUDY_PSYCHOLOGY:
   case ACTIVITY_STUDY_FIRST_AID:
   case ACTIVITY_STUDY_LAW:
   case ACTIVITY_STUDY_DISGUISE:
   case ACTIVITY_STUDY_SCIENCE:
   case ACTIVITY_STUDY_BUSINESS:
   //case ACTIVITY_STUDY_COOKING:
   case ACTIVITY_STUDY_GYMNASTICS:
   case ACTIVITY_STUDY_ART:
   case ACTIVITY_STUDY_MUSIC:
   case ACTIVITY_STUDY_TEACHING:
   case ACTIVITY_STUDY_WRITING:
   case ACTIVITY_STUDY_LOCKSMITHING:
   case ACTIVITY_STUDY_COMPUTERS:
      set_color(COLOR_MAGENTA,COLOR_BLACK,1);
      break;

      // Interrogating
   case ACTIVITY_HOSTAGETENDING:
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      break;

      // Dealing with your injuries
   case ACTIVITY_CLINIC:
      set_color(COLOR_RED,COLOR_BLACK,0);
      break;

      // Doing something Conservative
   case ACTIVITY_SLEEPER_CONSERVATIVE:
      set_color(COLOR_RED,COLOR_BLACK,1);
      break;

      // Dealing with the dead
   case ACTIVITY_BURY:
      set_color(COLOR_BLACK,COLOR_BLACK,1);
      break;

      // Nothing terribly important
   case ACTIVITY_HEAL: // Identical to none in practice
   case ACTIVITY_NONE:
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      break;

      // Going somewhere
   case ACTIVITY_VISIT:
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      break;

      // Quitting being a sleeper to join the LCS
   case ACTIVITY_SLEEPER_JOINLCS:
      set_color(COLOR_RED,COLOR_BLACK,1);
      break;
   case ACTIVITY_AUGMENT:
      set_color(COLOR_BLUE,COLOR_BLACK,1);
      break;

   default: // This should not happen! Set a strange color to indicate an error!
      set_color(COLOR_YELLOW,COLOR_RED,1); // yellow on red background
      break;
   }
}

/* location and squad header */
void locheader()
{
   if(activesquad!=NULL && activesquad->squad[0]->location!=-1)
   {
      if(location[activesquad->squad[0]->location]->siege.siege)
      {
         if(location[activesquad->squad[0]->location]->siege.underattack)set_color(COLOR_RED,COLOR_BLACK,1);
         else set_color(COLOR_YELLOW,COLOR_BLACK,1);
      }
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
   }
   else if(selectedsiege!=-1)
   {
      if(location[selectedsiege]->siege.siege)
      {
         if(location[selectedsiege]->siege.underattack)set_color(COLOR_RED,COLOR_BLACK,1);
         else set_color(COLOR_YELLOW,COLOR_BLACK,1);
      }
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
   }
   else set_color(COLOR_WHITE,COLOR_BLACK,0);
   move(0,0);

   if(activesquad!=NULL && activesquad->squad[0]->location!=-1)
   {
      addstr(location[activesquad->squad[0]->location]->getname(false, true));
      addstr(", ");
   }
   else
   {
      if(selectedsiege==-1)
      {
         addstr("No Squad Selected");
         addstr(", ");
      }
      else
      {
         addstr(location[selectedsiege]->getname(false, true));
         addstr(", ");
      }
   }

   addstr(getmonth(month,true));
   addstr(" ");
   addstr(day);
   addstr(", ");
   addstr(year);

   if(activesquad==NULL && selectedsiege==-1)
   {
      set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(3,6);
      addstr("To form a new squad:");
      move(4,6);
      addstr("1) R - Review Assets and Form Squads");
      move(5,6);
      addstr("2) Press Z to Assemble a New Squad");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
   }

   printfunds();

   if(activesquad!=NULL)
   {
      std::string str=getactivity(activesquad->activity);
      set_activity_color(activesquad->activity.type);
      if(activesquad->activity.type==ACTIVITY_NONE)
      {
         bool haveact=false,multipleact=false;
         for(int p=0;p<6;p++)
         {
            if(activesquad->squad[p]==NULL) continue;
            const std::string str2=getactivity(activesquad->squad[p]->activity);
            set_activity_color(activesquad->squad[p]->activity.type);
            if(haveact&&str!=str2) multipleact=true;
            str=str2,haveact=true;
         }
         if(multipleact)
         {
            str="Acting Individually";
            set_color(COLOR_WHITE,COLOR_BLACK,1);
         }
      }
      mvaddstr(0,41,str);
   }
}

/* party info at top of screen */
void printparty()
{
   Creature *party[6]={NULL,NULL,NULL,NULL,NULL,NULL};
   if(activesquad!=NULL)
      for(int p=0;p<6;p++)party[p]=activesquad->squad[p];

   set_color(COLOR_WHITE,COLOR_BLACK,0);

   for(int i=2;i<8;i++)
   {
      move(i,0);
      addstr("                                                                                "); // 80 spaces
   }

   if(party_status!=-1&&party[party_status]==NULL)party_status=-1;

   if(party_status!=-1)
   {
      printcreatureinfo(party[party_status]);

      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(1,0);
      addstr(party_status+1);
   }
   else
   {
      char str[200];

      move(1,0);
      addstr("#ÄCODE NAMEÄÄÄÄÄÄÄÄÄÄÄÄSKILLÄÄÄWEAPONÄÄÄÄÄÄÄÄÄARMORÄÄÄÄÄÄÄÄÄÄHEALTHÄÄÄTRANSPORTÄ"); // 80 characters

      for(int p=0;p<6;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(p+2,0);
         addstr("                                                                                "); // 80 spaces
         mvaddchar(p+2,0,'1'+p);

         if(party[p]!=NULL)
         {
            if(party[p]->prisoner!=NULL)set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(p+2,2);
            addstr(party[p]->name);
            if(party[p]->prisoner!=NULL)addstr("+H");

            int skill=0;
            char bright=0;
            for(int sk=0;sk<SKILLNUM;sk++)
            {
               skill+=(int)party[p]->get_skill(sk);
               if(party[p]->get_skill_ip(sk)>=100+(10*party[p]->get_skill(sk))&&
                  party[p]->get_skill(sk)<party[p]->skill_cap(sk,true))bright=1;
            }

            set_color(COLOR_WHITE,COLOR_BLACK,bright);
            move(p+2,23);
            addstr(skill);
            addstr("/");
            addstr(party[p]->get_weapon_skill());

            move(p+2,31);
            if(mode!=GAMEMODE_SITE)set_color(COLOR_WHITE,COLOR_BLACK,0);
            else switch(weaponcheck(*party[p]))
            {
               case -1:
               case 0:set_color(COLOR_GREEN,COLOR_BLACK,1);break;
               case 1:set_color(COLOR_YELLOW,COLOR_BLACK,1);break;
               case 2:set_color(COLOR_RED,COLOR_BLACK,1);break;
            }
            if(party[p]->has_thrown_weapon && len(party[p]->extra_throwing_weapons))
               addstr(party[p]->extra_throwing_weapons[0]->get_shortname(0));
            else addstr(party[p]->get_weapon().get_shortname(0));
            //set_color(COLOR_WHITE,COLOR_BLACK,0);
            if(party[p]->get_weapon().get_ammoamount()>0)
            {
               //set_color(COLOR_WHITE,COLOR_BLACK,0);
               addstr(" (");
               addstr(party[p]->get_weapon().get_ammoamount());
               addstr(")");
            }
            else if(party[p]->get_weapon().uses_ammo())
            {
               set_color(COLOR_BLACK,COLOR_BLACK,1);
               if(len(party[p]->clips))
               {
                  addstr(" (");
                  addstr(party[p]->count_clips());
                  addstr(")");
               }
               else
                  addstr(" (XX)");
            }
            else if(party[p]->get_weapon().is_throwable() && !party[p]->has_thrown_weapon)
               addstr(" (1)");
            else if(party[p]->has_thrown_weapon && len(party[p]->extra_throwing_weapons))
            {
               set_color(COLOR_BLACK,COLOR_BLACK,1);
               addstr(" (");
               addstr(party[p]->count_weapons()-party[p]->is_armed());
               addstr(")");
            }

            if(mode!=GAMEMODE_SITE)
            {
               int fg = COLOR_WHITE;
               int bg = COLOR_BLACK;
               int in = 1;
               if(party[p]->get_armor().is_bloody())
                  {bg = COLOR_RED;  in = 1;}
               if(party[p]->get_armor().get_quality() > party[p]->get_armor().get_quality_levels())
                  {fg = COLOR_BLACK;}
               else if(party[p]->get_armor().is_damaged())
                  {fg = COLOR_YELLOW; in = 1;}
               else if(party[p]->get_armor().get_quality() >1)
                  {fg = COLOR_YELLOW; in = (bg==COLOR_RED);}
               if (fg == COLOR_WHITE && bg == COLOR_BLACK)
                  in = 0;
               set_color(fg,bg,in);
            }
            else
            {
               switch(hasdisguise(*party[p]))
               {
               case 1:set_color(COLOR_GREEN,COLOR_BLACK,1);break;
               case 2:set_color(COLOR_YELLOW,COLOR_BLACK,1);break;
               default:
                  if(party[p]->get_armor().get_stealth_value() > 1)
                     set_color(COLOR_BLACK,COLOR_BLACK,1);
                  else set_color(COLOR_RED,COLOR_BLACK,1);
                  break;
               }
               if(sitealarmtimer>=0 || sitealarm==1)
                  if(party[p]->get_armor().get_stealth_value() > 1)
                     set_color(COLOR_BLACK,COLOR_BLACK,1);
            }
            mvaddstr(p+2,46,party[p]->get_armor().get_shortname());

            printhealthstat(*party[p],p+2,61,TRUE);

            set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(p+2,70);

            long v=-1;
            if(showcarprefs==1)v=id_getcar(party[p]->pref_carid);
            else v=id_getcar(party[p]->carid);
            if(v!=-1&&showcarprefs!=-1)
            {
               strcpy(str,vehicle[v]->shortname());
               if(showcarprefs==1?party[p]->pref_is_driver:party[p]->is_driver)strcat(str,"-D");
            }
            else
            {
               int legok=2;
               if((party[p]->wound[BODYPART_LEG_RIGHT] & WOUND_NASTYOFF)||
                  (party[p]->wound[BODYPART_LEG_RIGHT] & WOUND_CLEANOFF))legok--;
               if((party[p]->wound[BODYPART_LEG_LEFT] & WOUND_NASTYOFF)||
                  (party[p]->wound[BODYPART_LEG_LEFT] & WOUND_CLEANOFF))legok--;
               if(party[p]->flag & CREATUREFLAG_WHEELCHAIR)strcpy(str,"Wheelchair");
               else if(legok>=1)strcpy(str,"On Foot");
               else strcpy(str,"On \"Foot\"");
            }
            addstr(str);
         }
      }
   }

   makedelimiter();
}


/* location info at top of screen */
void printlocation(long loc)
{
   if(location[loc]->siege.siege)
   {
      if(!location[loc]->siege.underattack)
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,1);
         mvaddstr(2,1,"The police have surrounded this location.");
      }
      else
      {
         set_color(COLOR_RED,COLOR_BLACK,1);
         switch(location[loc]->siege.siegetype)
         {
         case SIEGE_POLICE:
            mvaddstr(2,1,"The police are raiding this location!");break;
         case SIEGE_CIA:
            mvaddstr(2,1,"The CIA is raiding this location!");break;
         case SIEGE_HICKS:
            mvaddstr(2,1,"The masses are storming this location!");break;
         case SIEGE_CORPORATE:
            mvaddstr(2,1,"The Corporations are raiding this location!");break;
         case SIEGE_CCS:
            mvaddstr(2,1,"The CCS is raiding this location!");break;
         case SIEGE_FIREMEN:
            mvaddstr(2,1,"Firemen are raiding this location!");break;
         }
      }
   }
   else
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      mvaddstr(2,1,"You are not under siege...  yet.");
   }

   if(location[loc]->can_be_upgraded())
   {
      if(numbereating(loc)>0)
      {
         if(fooddaysleft(loc))
         {
            if(fooddaysleft(loc)<4)
            {
               if(!location[loc]->siege.siege)set_color(COLOR_WHITE,COLOR_BLACK,0);
               else set_color(COLOR_YELLOW,COLOR_BLACK,1);
               mvaddstr(3,1,"This location has food for only a few days.");
            }
         }
         else
         {
            if(!location[loc]->siege.siege)set_color(COLOR_WHITE,COLOR_BLACK,0);
            else set_color(COLOR_RED,COLOR_BLACK,1);
            mvaddstr(3,1,"This location has insufficient food stores.");
         }
      }

      if(location[loc]->compound_walls & COMPOUND_BASIC)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         mvaddstr(4,1,"FORTIFIED COMPOUND");
      }

      if(location[loc]->compound_walls & COMPOUND_PRINTINGPRESS)
      {
         set_color(COLOR_BLUE,COLOR_BLACK,1);
         mvaddstr(4,31,"PRINTING PRESS");
      }

      if(location[loc]->front_business!=-1)
      {
         set_color(COLOR_MAGENTA,COLOR_BLACK,1);
         mvaddstr(4,54,"BUSINESS FRONT");
      }

      if(location[loc]->compound_walls & COMPOUND_CAMERAS)
      {
         if(location[loc]->siege.siege&&location[loc]->siege.cameras_off)
         {
            set_color(COLOR_RED,COLOR_BLACK,0);
            mvaddstr(5,1,"CAMERAS OFF");
         }
         else
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            mvaddstr(5,1,"CAMERAS ON");
         }
      }

      if(location[loc]->compound_walls & COMPOUND_TRAPS)
      {
         set_color(COLOR_RED,COLOR_BLACK,1);
         mvaddstr(5,16,"BOOBY TRAPS");
      }

      if(location[loc]->compound_walls & COMPOUND_AAGUN)
      {
         set_color(COLOR_CYAN,COLOR_BLACK,1);
         mvaddstr(5,33,"AA GUN");
      }

      if(location[loc]->compound_walls & COMPOUND_TANKTRAPS)
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,1);
         mvaddstr(5,46,"TANK TRAPS");
      }

      if(location[loc]->siege.siege&&location[loc]->siege.lights_off)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(5,60,"LIGHTS OUT");
      }
      else if(location[loc]->compound_walls & COMPOUND_GENERATOR)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         mvaddstr(5,61,"GENERATOR");
      }

      int eaters=numbereating(loc),days=fooddaysleft(loc);

      if(eaters>0)
      {
         if(days>=1)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            mvaddstr(6,50,days);
            addstr(" Day");
            if(days!=1)addchar('s');
            addstr(" of Food Left");
         }
         else if(days==0)
         {
            set_color(COLOR_RED,COLOR_BLACK,0);
            mvaddstr(6,50,"Not Enough Food");
         }
      }
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      mvaddstr(6,1,location[loc]->compound_stores);
      addstr(" Daily Ration");
      if(location[loc]->compound_stores!=1)addstr("s");

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      mvaddstr(6,30,eaters);
      addstr(" Eating");
   }
}


/* character info at top of screen */
void printcreatureinfo(Creature *cr, unsigned char knowledge)
{
   char str[200];

   makedelimiter(1);

   set_color(COLOR_WHITE,COLOR_BLACK,0);
   mvaddstr(1,2,cr->name);
   addstr(", ");
   addstr(gettitle(*cr));

   if(cr->prisoner!=NULL)
   {
      addstr(", holding ");
      switch(cr->prisoner->type)
      {
      case CREATURE_POLITICIAN:addstr("a cursing Politician");break;
      case CREATURE_CORPORATE_CEO:addstr("a squirming CEO");break;
      case CREATURE_RADIOPERSONALITY:addstr("a crying Radio Personality");break;
      case CREATURE_NEWSANCHOR:addstr("a smarmy News Anchor");break;
      case CREATURE_SCIENTIST_EMINENT:addstr("a frightened Eminent Scientist");break;
      case CREATURE_JUDGE_CONSERVATIVE:addstr("an angry Hangin' Judge");break;
      default:
         addstr(cr->prisoner->name);
         break;
      }
   }

   mvaddstr(2,0,"Hrt:    ");
   if(knowledge>0)
      addstr(cr->get_attribute(ATTRIBUTE_HEART,true));
   else addstr("?");
   mvaddstr(3,0,"Int:    ");
   if(knowledge>0)
      addstr(cr->get_attribute(ATTRIBUTE_INTELLIGENCE,true));
   else addstr("?");
   mvaddstr(4,0,"Wis:    ");
   if(knowledge>0)
      addstr(cr->get_attribute(ATTRIBUTE_WISDOM,true));
   else addstr("?");
   mvaddstr(5,0,"Hlth:   ");
   if(knowledge>1)
      addstr(cr->get_attribute(ATTRIBUTE_HEALTH,true));
   else addstr("?");
   mvaddstr(2,11,"Agi:    ");
   if(knowledge>1)
      addstr(cr->get_attribute(ATTRIBUTE_AGILITY,true));
   else addstr("?");
   mvaddstr(3,11,"Str:    ");
   if(knowledge>1)
      addstr(cr->get_attribute(ATTRIBUTE_STRENGTH,true));
   else addstr("?");
   mvaddstr(4,11,"Char:   ");
   if(knowledge>0)
      addstr(cr->get_attribute(ATTRIBUTE_CHARISMA,true));
   else addstr("?");
   mvaddstr(5,11,"Trans: ");
   long v=-1;
   if(showcarprefs==1)v=id_getcar(cr->pref_carid);
   else v=id_getcar(cr->carid);
   if(v!=-1&&showcarprefs!=-1)
   {
      strcpy(str,vehicle[v]->shortname());
      char d;
      if(showcarprefs==1)d=cr->pref_is_driver;
      else d=cr->is_driver;
      if(d)strcat(str,"-D");
   }
   else
   {
      int legok=2;
      if((cr->wound[BODYPART_LEG_RIGHT] & WOUND_NASTYOFF)||
         (cr->wound[BODYPART_LEG_RIGHT] & WOUND_CLEANOFF))legok--;
      if((cr->wound[BODYPART_LEG_LEFT] & WOUND_NASTYOFF)||
         (cr->wound[BODYPART_LEG_LEFT] & WOUND_CLEANOFF))legok--;
      if(cr->flag & CREATUREFLAG_WHEELCHAIR)strcpy(str,"Wheelchair");
      else if(legok>=1)strcpy(str,"On Foot");
      else strcpy(str,"On \"Foot\"");
   }
   addstr(str);
   move(6,0);
   if(mode!=GAMEMODE_SITE) set_color(COLOR_WHITE,COLOR_BLACK,0);
   else switch(weaponcheck(*cr))
   {
      case -1:
      case 0:set_color(COLOR_GREEN,COLOR_BLACK,1);break;
      case 1:set_color(COLOR_YELLOW,COLOR_BLACK,1);break;
      case 2:set_color(COLOR_RED,COLOR_BLACK,1);break;
   }
   addstr("Weapon: ");
   addstr(cr->get_weapon_string(1));


   move(7,0);
   if(mode!=GAMEMODE_SITE)
   {
      int fg = COLOR_WHITE;
      int bg = COLOR_BLACK;
      int in = 1;
      if(cr->get_armor().is_bloody())
         {bg = COLOR_RED;  in = 0;}
      if(cr->get_armor().get_quality() > cr->get_armor().get_quality_levels())
         {fg = COLOR_BLACK;}
      else if(cr->get_armor().is_damaged())
         {fg = COLOR_YELLOW; in = 1;}
      else if(cr->get_armor().get_quality() >1)
         {fg = COLOR_YELLOW; in = (bg==COLOR_RED);}
      if (fg == COLOR_WHITE && bg == COLOR_BLACK)
         in = 0;
      set_color(fg,bg,in);
   }
   else
   {
      switch(hasdisguise(*cr))
      {
      case 1:set_color(COLOR_GREEN,COLOR_BLACK,1);break;
      case 2:set_color(COLOR_YELLOW,COLOR_BLACK,1);break;
      default:
         if(cr->get_armor().get_stealth_value() > 1)
            set_color(COLOR_BLACK,COLOR_BLACK,1);
         else set_color(COLOR_RED,COLOR_BLACK,1);
      }
      if(sitealarmtimer>=0 || sitealarm==1)
         if(cr->get_armor().get_stealth_value() > 1)
            set_color(COLOR_BLACK,COLOR_BLACK,1);
   }
   addstr("Clothes: ");
   addstr(cr->get_armor_string(false));

   set_color(COLOR_WHITE,COLOR_BLACK,0);

   char used[SKILLNUM];
   memset(used,0,sizeof(char)*SKILLNUM);

   int snum=5;
   char printed=1;

   while(snum>0&&printed)
   {
      printed=0;

      int max=0;
      long maxs=-1;
      for(int s=0;s<SKILLNUM;s++)
      {
         if((cr->get_skill(s)*10000+cr->get_skill_ip(s))>max && !used[s])
         {
            max=(cr->get_skill(s)*10000+cr->get_skill_ip(s));
            maxs=s;
         }
      }

      if(maxs!=-1)
      {
         used[maxs]=1;
         printed=1;

         // Maxed skills are cyan
         if(cr->skill_cap(maxs,true)!=0 && cr->get_skill(maxs)>=cr->skill_cap(maxs,true))set_color(COLOR_CYAN,COLOR_BLACK,1);
         // About to level up skills are white
         else if(cr->get_skill_ip(maxs)>=100+(10*cr->get_skill(maxs))&&
            cr->get_skill(maxs)<cr->skill_cap(maxs,true))set_color(COLOR_WHITE,COLOR_BLACK,1);
         // <1 skills are dark gray
         else if(cr->get_skill(maxs)<1)set_color(COLOR_BLACK,COLOR_BLACK,1);
         // >=1 skills are light gray
         else set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(3+5-snum,31);
         if(knowledge>5-snum)
            addstr(Skill::get_name(maxs));
         else addstr("???????");
         addstr(": ");
         if(knowledge>7-snum)
            addstr(cr->get_skill(maxs));
         else addchar('?');

         if(snum==5&&printed)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(2,31);
            addstr("Top Skills:");
         }
      }

      snum--;
   }

   int woundsum=0;
   for(int w=0;w<BODYPARTNUM;w++)
      if(cr->wound[w]!=0)woundsum++;

   printhealthstat(*cr,1,49,FALSE);

   if(woundsum>0)
   {
      for(int w=0;w<BODYPARTNUM;w++)
      {
         if(cr->wound[w] & WOUND_BLEEDING)set_color(COLOR_RED,COLOR_BLACK,1);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(2+w,49);
         switch(w)
         {
         case BODYPART_HEAD:addstr("Head:");break;
         case BODYPART_BODY:addstr("Body:");break;
         case BODYPART_ARM_RIGHT:addstr("Right Arm:");break;
         case BODYPART_ARM_LEFT:addstr("Left Arm:");break;
         case BODYPART_LEG_RIGHT:addstr("Right Leg:");break;
         case BODYPART_LEG_LEFT:addstr("Left Leg:");break;
         }

         move(2+w,61);
         if(cr->wound[w] & WOUND_NASTYOFF)addstr("Ripped off");
         else if(cr->wound[w] & WOUND_CLEANOFF)addstr("Clean sever");
         else
         {
            int sum=0;

            if(cr->wound[w] & WOUND_SHOT)sum++;
            if(cr->wound[w] & WOUND_CUT)sum++;
            if(cr->wound[w] & WOUND_BRUISED)sum++;
            if(cr->wound[w] & WOUND_BURNED)sum++;
            if(cr->wound[w] & WOUND_TORN)sum++;

            if(sum==0)
            {
               set_color(COLOR_GREEN,COLOR_BLACK,1);
               if(cr->animalgloss==ANIMALGLOSS_ANIMAL)
                  addstr("Animal");
               else addstr("Liberal");
            }

            if(cr->wound[w] & WOUND_SHOT){addstr("Sht");sum--;if(sum>0)addstr(",");}
            if(cr->wound[w] & WOUND_BRUISED){addstr("Brs");sum--;if(sum>0)addstr(",");}
            if(cr->wound[w] & WOUND_CUT){addstr("Cut");sum--;if(sum>0)addstr(",");}
            if(cr->wound[w] & WOUND_TORN){addstr("Trn");sum--;if(sum>0)addstr(",");}
            if(cr->wound[w] & WOUND_BURNED){addstr("Brn");sum--;if(sum>0)addstr(",");}
         }
      }
      set_color(COLOR_WHITE,COLOR_BLACK,0);
   }
}


/* full character sheet (with surrounding interface) */
void fullstatus(int p)
{
   if(activesquad==NULL)return;

   const int pagenum=3;
   int page=0;

   while(true)
   {
      erase();

      set_color(COLOR_GREEN,COLOR_BLACK,1);
      move(0,0);
      addstr("Profile of a Liberal");

      if(page==0) printliberalstats(*activesquad->squad[p]);
      else if(page==1) printliberalskills(*activesquad->squad[p]);
      else if(page==2) printliberalcrimes(*activesquad->squad[p]);

      move(23,0);
      addstr("N - Change Code Name      G - Fix Gender Label");
      if(activesquad->squad[1]!=NULL)
         addstr("    LEFT/RIGHT - Other Liberals");
      move(24,0);
      addstr("Press any other key to continue the Struggle");
      addstr("    UP/DOWN  - More Info");

      int c=getkey();

      if(activesquad->squad[1]!=NULL&&((c==KEY_LEFT)||(c==KEY_RIGHT)))
      {
         int sx=(c==KEY_LEFT)?-1:1;
         do p=(p+6+sx)%6; while(activesquad->squad[p]==NULL);
      }
      else if(c==KEY_DOWN)
      {
         page++;
         page%=pagenum;
      }
      else if(c==KEY_UP)
      {
         page--;
         if(page<0) page=pagenum-1;
         page%=pagenum;
      }
      else if(c=='n')
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(23,0,"What is the new code name?                                                      "); // 80 characters
         mvaddstr(24,0,"                                                                                "); // 80 spaces

         enter_name(24,0,activesquad->squad[p]->name,CREATURE_NAMELEN,activesquad->squad[p]->propername);
      }
      else if(c=='g')
      {
         activesquad->squad[p]->gender_liberal++;
         if(activesquad->squad[p]->gender_liberal>2)
            activesquad->squad[p]->gender_liberal=0;
      }
      else break;
   }
}


/* Full screen character sheet, skills only edition */
void printliberalskills(Creature &cr)
{
   // Add name
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   move(2,0);
   if(strcmp(cr.propername,cr.name)!=0)
      addstr("Code name: ");
   else addstr("Name: ");
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   addstr(cr.name);
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   addstr(", ");
   addstr(gettitle(cr));
   addstr(" (");
   addstr(cr.get_type_name());
   addstr(")");

   // Add all skills
   for(int s=0;s<SKILLNUM;s++)
   {
      if(s%3==0 && s<9)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(4,27*(s/3));
         addstr("SKILL");
         move(4,15+27*(s/3));
         addstr("NOW   MAX");
      }

      // Maxed skills are cyan
      if(cr.skill_cap(s,true)!=0 && cr.get_skill(s)>=cr.skill_cap(s,true))set_color(COLOR_CYAN,COLOR_BLACK,1);
      // About to level up skills are white
      else if(cr.get_skill_ip(s)>=100+(10*cr.get_skill(s))&&
         cr.get_skill(s)<cr.skill_cap(s,true))set_color(COLOR_WHITE,COLOR_BLACK,1);
      // <1 skills are dark gray
      else if(cr.get_skill(s)<1)set_color(COLOR_BLACK,COLOR_BLACK,1);
      // >=1 skills are light gray
      else set_color(COLOR_WHITE,COLOR_BLACK,0);

      move(5+s/3,27*(s%3));
      addstr(Skill::get_name(s));
      addstr(": ");
      move(5+s/3,14+27*(s%3));
      addstr_f("%2d.",cr.get_skill(s));
      if(cr.get_skill_ip(s)<100+(10*cr.get_skill(s)))
      {
         if((cr.get_skill_ip(s)*100)/(100+(10*cr.get_skill(s)))!=0)
         {
            if((cr.get_skill_ip(s)*100)/(100+(10*cr.get_skill(s)))<10)
               addstr("0");
            addstr((cr.get_skill_ip(s)*100)/(100+(10*cr.get_skill(s))));
         }
         else addstr("00");
      }
      else addstr("99+");

      if(cr.skill_cap(s,true)==0 || cr.get_skill(s)<cr.skill_cap(s,true))
         set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(5+s/3,20+27*(s%3));
      addstr_f("%2d.00",cr.skill_cap(s,true));
   }
   set_color(COLOR_WHITE,COLOR_BLACK,0);
}


/* full screen character sheet */
void printliberalstats(Creature &cr)
{
   set_color(COLOR_WHITE,COLOR_BLACK,0);

   // Add name
   move(2,0);
   addstr("Name: ");
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   addstr(cr.name);
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   if(strcmp(cr.propername,cr.name)!=0)
   {
      //The names do not match, print real name as well
      addstr(" (");
      addstr(cr.propername);
      addstr(")");
   }
   addstr(", ");
   addstr(gettitle(cr));
   addstr(" (");
   addstr(cr.get_type_name());
   addstr(")");
   move(3,0);

   // Add birthdate
   addstr("Born ");
   addstr(getmonth(cr.birthday_month));
   addstr(" ");
   addstr(cr.birthday_day);
   addstr(", ");
   if(cr.birthday_month < month ||
      (cr.birthday_month == month && cr.birthday_day <= day))
      addstr(year-cr.age);
   else addstr(year-1-cr.age);
   // Add age
   addstr(" (Age ");
   addstr(cr.age);
   // Assess their gender in an Elite Liberal way
   if(cr.gender_liberal == GENDER_MALE)
      addstr(", Male");
   else if(cr.gender_liberal == GENDER_FEMALE)
      addstr(", Female");
   else addstr(", Genderqueer");
   // DON'T Note if there's some conflict with Conservative society's perceptions
   //if(cr.gender_liberal != cr.gender_conservative && cr.gender_liberal != GENDER_NEUTRAL)
   //   addstr("*");
   addstr(")");

   move(3,46);
   statebrokenlaws(cr);
   set_color(COLOR_WHITE,COLOR_BLACK,0);

   // Add juice
   move(10,16);
   addstr("Juice: ");
   addstr(cr.juice);
   if(cr.juice<1000)
   {
      move(11,16);addstr("Next:  ");
      if(cr.juice<0)addstr("0");
      else if(cr.juice<10)addstr("10");
      else if(cr.juice<50)addstr("50");
      else if(cr.juice<100)addstr("100");
      else if(cr.juice<200)addstr("200");
      else if(cr.juice<500)addstr("500");
      else addstr("1000");
   }
   // Add attributes
   move(5,0);addstr("Heart: ");
   addstr(cr.get_attribute(ATTRIBUTE_HEART,true));
   move(6,0);addstr("Intelligence: ");
   addstr(cr.get_attribute(ATTRIBUTE_INTELLIGENCE,true));
   move(7,0);addstr("Wisdom: ");
   addstr(cr.get_attribute(ATTRIBUTE_WISDOM,true));
   move(8,0);addstr("Health: ");
   addstr(cr.get_attribute(ATTRIBUTE_HEALTH,true));
   move(9,0);addstr("Agility: ");
   addstr(cr.get_attribute(ATTRIBUTE_AGILITY,true));
   move(10,0);addstr("Strength: ");
   addstr(cr.get_attribute(ATTRIBUTE_STRENGTH,true));
   move(11,0);addstr("Charisma: ");
   addstr(cr.get_attribute(ATTRIBUTE_CHARISMA,true));

   // Add highest skills
   char used[SKILLNUM];
   memset(used,0,sizeof(char)*SKILLNUM);

   int skills_max=16;
   char printed=1;

   move(5,28);
   addstr("SKILL");
   move(5,43);
   addstr("NOW   MAX");
   for(int skills_shown=0;skills_shown<skills_max&&printed;skills_shown++)
   {
      printed=0;

      int max=0;
      long maxs=-1;
      for(int s=0;s<SKILLNUM;s++)
      {
         if((cr.get_skill(s)*10000+cr.get_skill_ip(s))>max && !used[s])
         {
            max=(cr.get_skill(s)*10000+cr.get_skill_ip(s));
            maxs=s;
         }
      }

      if(maxs!=-1)
      {
         used[maxs]=1;
         printed=1;

         // Maxed skills are cyan
         if(cr.skill_cap(maxs,true)!=0 && cr.get_skill(maxs)>=cr.skill_cap(maxs,true))set_color(COLOR_CYAN,COLOR_BLACK,1);
         // About to level up skills are white
         else if(cr.get_skill_ip(maxs)>=100+(10*cr.get_skill(maxs))&&
            cr.get_skill(maxs)<cr.skill_cap(maxs,true))set_color(COLOR_WHITE,COLOR_BLACK,1);
         // <1 skills are dark gray
         else if(cr.get_skill(maxs)<1)set_color(COLOR_BLACK,COLOR_BLACK,1);
         // >=1 skills are light gray
         else set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(6+skills_shown,28);
         addstr(Skill::get_name(maxs));
         addstr(": ");
         move(6+skills_shown,42);
         addstr_f("%2d.",cr.get_skill(maxs));
         if(cr.get_skill_ip(maxs)<100+(10*cr.get_skill(maxs)))
         {
            if((cr.get_skill_ip(maxs)*100)/(100+(10*cr.get_skill(maxs)))!=0)
            {
               if((cr.get_skill_ip(maxs)*100)/(100+(10*cr.get_skill(maxs)))<10)
                  addchar('0');
               addstr((cr.get_skill_ip(maxs)*100)/(100+(10*cr.get_skill(maxs))));
            }
            else addstr("00");
         }
         else addstr("99+");

         if(cr.skill_cap(maxs,true)==0 || cr.get_skill(maxs)<cr.skill_cap(maxs,true))
            set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(6+skills_shown,48);
         addstr_f("%2d.00",cr.skill_cap(maxs,true));
      }
   }

   set_color(COLOR_WHITE,COLOR_BLACK,0);

   // Add weapon
   move(13,0);
   addstr("Weapon: ");
   addstr(cr.get_weapon_string(0));

   // Add clothing
   move(14,0);
   addstr("Clothes: ");
   addstr(cr.get_armor_string(true));


   // Add vehicle
   move(15,0);
   addstr("Car: ");
   long v=-1;
   if(showcarprefs==1)v=id_getcar(cr.pref_carid);
   else v=id_getcar(cr.carid);
   if(v!=-1&&showcarprefs!=-1)
   {
      addstr(vehicle[v]->shortname());
      char d;
      if(showcarprefs==1)d=cr.pref_is_driver;
      else d=cr.is_driver;
      if(d)addstr("-D");
   }
   else
   {
      int legok=2;
      if((cr.wound[BODYPART_LEG_RIGHT] & WOUND_NASTYOFF)||
         (cr.wound[BODYPART_LEG_RIGHT] & WOUND_CLEANOFF))legok--;
      if((cr.wound[BODYPART_LEG_LEFT] & WOUND_NASTYOFF)||
         (cr.wound[BODYPART_LEG_LEFT] & WOUND_CLEANOFF))legok--;
      if(cr.flag & CREATUREFLAG_WHEELCHAIR)addstr("Wheelchair");
      else if(legok>=1)addstr("On Foot");
      else addstr("On \"Foot\"");
   }

    for(int i=0;i<len(pool);i++) {
        if (pool[i]->id == cr.hireid) {
            move(18,55);
            addstr("Contact: ");
            addstr(std::string(pool[i]->name).substr(0,15));
            break;
        }
    }
   // Add recruit stats
   if(cr.flag!=CREATUREFLAG_BRAINWASHED)
   {
      move(19,55);
      addstr(maxsubordinates(cr)-subordinatesleft(cr));
      addstr(" Recruits / ");
      addstr(maxsubordinates(cr));
      addstr(" Max");
   }
   else
   {
      move(19,55);
      addstr("Enlightened ");
      addstr("Can't Recruit");
   }
   // Any meetings with potential recruits scheduled?
   if(scheduledmeetings(cr))
   {
      move(21,55);
      addstr("Scheduled Meetings: ");
      addstr(scheduledmeetings(cr));
   }
   // Add seduction stats
   move(20,55);
   int lovers = loveslaves(cr);
   if(cr.flag & CREATUREFLAG_LOVESLAVE)
      lovers++;
   if(lovers)
   {
      addstr(lovers);
      addstr(" Romantic Interest");
      if(lovers>1)addstr("s");
   }
   // Any dates with potential love interests scheduled?
   if(scheduleddates(cr))
   {
      move(22,55);
      addstr("Scheduled Dates:    ");
      addstr(scheduleddates(cr));
   }

	printwoundstat(cr,5,55);

   //SPECIAL WOUNDS
   set_color(COLOR_RED,COLOR_BLACK,0);

   int y=17;
   int x=0;
   if(cr.special[SPECIALWOUND_HEART]!=1)
   {
      move(y++,x);
      addstr("Heart Punctured");
   }
   if(cr.special[SPECIALWOUND_RIGHTLUNG]!=1)
   {
      move(y++,x);
      addstr("R. Lung Collapsed");
   }
   if(cr.special[SPECIALWOUND_LEFTLUNG]!=1)
   {
      move(y++,x);
      addstr("L. Lung Collapsed");
   }
   if(cr.special[SPECIALWOUND_NECK]!=1)
   {
      move(y++,x);
      addstr("Broken Neck");
   }
   if(cr.special[SPECIALWOUND_UPPERSPINE]!=1)
   {
      move(y++,x);
      addstr("Broken Up Spine");
   }
   if(cr.special[SPECIALWOUND_LOWERSPINE]!=1)
   {
      move(y++,x);
      addstr("Broken Lw Spine");
   }
   if(cr.special[SPECIALWOUND_RIGHTEYE]!=1)
   {
      move(y++,x);
      addstr("No Right Eye");
   }
   if(cr.special[SPECIALWOUND_LEFTEYE]!=1)
   {
      move(y++,x);
      addstr("No Left Eye");
   }
   if(cr.special[SPECIALWOUND_NOSE]!=1)
   {
      move(y++,x);
      addstr("No Nose");
   }
   if(cr.special[SPECIALWOUND_TONGUE]!=1)
   {
      move(y++,x);
      addstr("No Tongue");
   }
   if(cr.special[SPECIALWOUND_TEETH]!=TOOTHNUM)
   {
      move(y++,x);
      if(cr.special[SPECIALWOUND_TEETH]==0)addstr("No Teeth");
      else if(cr.special[SPECIALWOUND_TEETH]==TOOTHNUM-1)addstr("Missing a Tooth");
      else if(cr.special[SPECIALWOUND_TEETH]<TOOTHNUM)addstr("Missing Teeth");
   }
   if(cr.special[SPECIALWOUND_LIVER]!=1)
   {
      move(y++,x);
      addstr("Liver Damaged");
   }
   if(cr.special[SPECIALWOUND_RIGHTKIDNEY]!=1)
   {
      move(y++,x);
      addstr("R. Kidney Damaged");
   }
   if(cr.special[SPECIALWOUND_LEFTKIDNEY]!=1)
   {
      move(y++,x);
      addstr("L. Kidney Damaged");
   }
   if(cr.special[SPECIALWOUND_STOMACH]!=1)
   {
      move(y++,x);
      addstr("Stomach Injured");
   }
   if(cr.special[SPECIALWOUND_SPLEEN]!=1)
   {
      move(y++,x);
      addstr("Busted Spleen");
   }
   if(cr.special[SPECIALWOUND_RIBS]!=RIBNUM)
   {
      move(y++,x);
      if(cr.special[SPECIALWOUND_RIBS]==0)addstr("All Ribs Broken");
      else if(cr.special[SPECIALWOUND_RIBS]==RIBNUM-1)addstr("Broken Rib");
      else if(cr.special[SPECIALWOUND_RIBS]<RIBNUM)addstr("Broken Ribs");
   }

   set_color(COLOR_WHITE,COLOR_BLACK,0);

   for(int i=0,y=12;i<AUGMENTATIONNUM;i++,y++)
   {
      if(cr.get_augmentation(i).type==-1) continue;
      mvaddstr(y,55,Augmentation::get_name(i));
      addstr(": ");
      addstr(cr.get_augmentation(i).name);
   }
}

/* Full screen character sheet, crime sheet */
void printliberalcrimes(Creature &cr)
{
   // Add name
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   if(strcmp(cr.propername,cr.name)!=0)
      mvaddstr(2,0,"Code name: ");
   else mvaddstr(2,0,"Name: ");
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   addstr(cr.name);
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   addstr(", ");
   addstr(gettitle(cr));
   addstr(" (");
   addstr(cr.get_type_name());
   addstr(")");
   // Show outstanding convictions in addition to untried crimes
   if(cr.deathpenalty)
   {
      set_color(COLOR_RED,COLOR_BLACK,1);
      if(location[cr.location]->type==SITE_GOVERNMENT_PRISON)
         mvaddstr(3,0,"On DEATH ROW");
      else mvaddstr(3,0,"Sentenced to DEATH");
   }
   else if(cr.sentence<0)
   {
      set_color(COLOR_RED,COLOR_BLACK,1);
      if(location[cr.location]->type==SITE_GOVERNMENT_PRISON)
         mvaddstr(3,0,"Serving life in prison");
      else mvaddstr(3,0,"Sentenced to life in prison");
   }
   else if(cr.sentence>0)
   {
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      if(location[cr.location]->type==SITE_GOVERNMENT_PRISON)
         mvaddstr(3,0,"Serving ");
      else mvaddstr(3,0,"Sentenced to ");
      addstr(cr.sentence);
      addstr(" months in prison.");
   }

   // Add all crimes
   for(int i = 0;i < LAWFLAGNUM; i++)
   {
      if(i%2==0 && i<4)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(4,40*(i/2),"CRIME");
         mvaddstr(4,30+40*(i/2),"NUM");
      }

      // Commited crimes are yellow
      if(cr.crimes_suspected[i] > 0)
         set_color(COLOR_YELLOW,COLOR_BLACK,1);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);

      mvaddstr(5+i/2,40*(i%2),getlawflag(i)+": ");
      mvaddstr_f(5+i/2,30+40*(i%2),"%2d",cr.crimes_suspected[i]);
   }

   set_color(COLOR_WHITE,COLOR_BLACK,0);
}

/* draws a horizontal line across the screen */
void makedelimiter(int y,int x)
{
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   if(mode==GAMEMODE_SITE&&y==8&&x==0&&mapshowing) // special case: there is a map on the right in site mode
      mvaddstr(y,x,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂ"); // 80 characters
   else // normal delimiter
      mvaddstr(y,x,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ"); // 80 characters
}

void printwoundstat(Creature &cr,int y,int x)
{
	// Add wound status
   for(int w=0;w<BODYPARTNUM;w++)
   {
      if(cr.wound[w] & WOUND_BLEEDING)set_color(COLOR_RED,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);

      move(y+w,x);
      switch(w)
      {
      case BODYPART_HEAD:addstr("Head:");break;
      case BODYPART_BODY:addstr("Body:");break;
      case BODYPART_ARM_RIGHT:addstr("Right Arm:");break;
      case BODYPART_ARM_LEFT:addstr("Left Arm:");break;
      case BODYPART_LEG_RIGHT:addstr("Right Leg:");break;
      case BODYPART_LEG_LEFT:addstr("Left Leg:");break;
      }

      move(y+w,x+11);
      if(cr.wound[w] & WOUND_NASTYOFF)addstr("Ripped off");
      else if(cr.wound[w] & WOUND_CLEANOFF)addstr("Severed");
      else
      {
         int sum=0;

         if(cr.wound[w] & WOUND_SHOT)sum++;
         if(cr.wound[w] & WOUND_CUT)sum++;
         if(cr.wound[w] & WOUND_BRUISED)sum++;
         if(cr.wound[w] & WOUND_BURNED)sum++;
         if(cr.wound[w] & WOUND_TORN)sum++;

         if(sum==0)
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            if(cr.animalgloss==ANIMALGLOSS_ANIMAL)
               addstr("Animal");
            else addstr("Liberal");
         }

         if(cr.wound[w] & WOUND_SHOT){addstr("Shot");sum--;if(sum>0)addstr(",");}
         if(cr.wound[w] & WOUND_BRUISED){addstr("Bruised");sum--;if(sum>0)addstr(",");}
         if(cr.wound[w] & WOUND_CUT){addstr("Cut");sum--;if(sum>0)addstr(",");}
         if(cr.wound[w] & WOUND_TORN){addstr("Torn");sum--;if(sum>0)addstr(",");}
         if(cr.wound[w] & WOUND_BURNED){addstr("Burned");sum--;if(sum>0)addstr(",");}
      }
   }
}

/* prints a character's health description (One Leg, Liberal, NearDETH...) */
void printhealthstat(Creature &g,int y,int x,char smll)
{
   short woundsum=0;
   bool bleeding=false;
   for(int w=0;w<BODYPARTNUM;w++)
   {
      if(g.wound[w]!=0)woundsum++;
      if(g.wound[w] & WOUND_BLEEDING)bleeding=true;
   }

   int armok=2,legok=2;
   if((g.wound[BODYPART_ARM_RIGHT] & WOUND_NASTYOFF)||
      (g.wound[BODYPART_ARM_RIGHT] & WOUND_CLEANOFF))armok--;
   if((g.wound[BODYPART_ARM_LEFT] & WOUND_NASTYOFF)||
      (g.wound[BODYPART_ARM_LEFT] & WOUND_CLEANOFF))armok--;
   if((g.wound[BODYPART_LEG_RIGHT] & WOUND_NASTYOFF)||
      (g.wound[BODYPART_LEG_RIGHT] & WOUND_CLEANOFF))legok--;
   if((g.wound[BODYPART_LEG_LEFT] & WOUND_NASTYOFF)||
      (g.wound[BODYPART_LEG_LEFT] & WOUND_CLEANOFF))legok--;

   move(y,x);
   if(bleeding)set_color(COLOR_RED,COLOR_BLACK,1);
   else set_color(COLOR_WHITE,COLOR_BLACK,1);
   if(!g.alive)
   {
      set_color(COLOR_BLACK,COLOR_BLACK,1);
      addstr("Deceased");
   }
   else if(g.blood<=20)
   {
      if(smll)addstr("NearDETH");
      else addstr("Near Death");
   }
   else if(g.blood<=50)
   {
      if(smll)addstr("BadWound");
      else addstr("Badly Wounded");
   }
   else if(g.blood<=75)
   {
      addstr("Wounded");
   }
   else if(g.blood<100)
   {
      if(smll)addstr("LtWound");
      else addstr("Lightly Wounded");
   }
   else if(g.special[SPECIALWOUND_NECK]==2)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("NckBroke");
      else addstr("Neck Broken");
   }
   else if(g.special[SPECIALWOUND_UPPERSPINE]==2)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("Quadpleg");
      else addstr("Quadraplegic");
   }
   else if(g.special[SPECIALWOUND_LOWERSPINE]==2)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("Parapleg");
      else addstr("Paraplegic");
   }
   else if(g.special[SPECIALWOUND_RIGHTEYE]==0&&
      g.special[SPECIALWOUND_LEFTEYE]==0&&
      g.special[SPECIALWOUND_NOSE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("FaceGone");
      else addstr("Face Gone");
   }
   else if(legok==0&&armok==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("No Limbs");
   }
   else if((legok==1&&armok==0)||(armok==1&&legok==0))
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("One Limb");
   }
   else if(legok==2&&armok==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("No Arms");
   }
   else if(legok==0&&armok==2)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("No Legs");
   }
   else if(legok==1&&armok==1)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("1Arm1Leg");
      else addstr("One Arm, One Leg");
   }
   else if(armok==1)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("One Arm");
   }
   else if(legok==1)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("One Leg");
   }
   else if(g.special[SPECIALWOUND_RIGHTEYE]==0&&
      g.special[SPECIALWOUND_LEFTEYE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("Blind");
   }
   else if((g.special[SPECIALWOUND_RIGHTEYE]==0||
      g.special[SPECIALWOUND_LEFTEYE]==0)&&
      g.special[SPECIALWOUND_NOSE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("FaceMutl");
      else addstr("Face Mutilated");
   }
   else if(g.special[SPECIALWOUND_NOSE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("NoseGone");
      else addstr("Missing Nose");
   }
   else if(g.special[SPECIALWOUND_RIGHTEYE]==0||
      g.special[SPECIALWOUND_LEFTEYE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("One Eye");
      else addstr("Missing Eye");
   }
   else if(g.special[SPECIALWOUND_TONGUE]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("NoTongue");
      else addstr("No Tongue");
   }
   else if(g.special[SPECIALWOUND_TEETH]==0)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      addstr("No Teeth");
   }
   else if(g.special[SPECIALWOUND_TEETH]<TOOTHNUM)
   {
      if(!bleeding)set_color(COLOR_GREEN,COLOR_BLACK,0);
      if(smll)addstr("MisTeeth");
      else addstr("Missing Teeth");
   }
   else
   {
      if (g.align == Alignment::CONSERVATIVE)
      {
         set_color(COLOR_RED,COLOR_BLACK,1);
         if(smll)addstr("Consrvtv");
         else addstr("Conservative");
      }
      else if (g.align == Alignment::MODERATE)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         addstr("Moderate");
      }
      else
      {
         set_color(COLOR_GREEN,COLOR_BLACK,1);
         if(g.animalgloss==ANIMALGLOSS_ANIMAL)
            addstr("Animal");
         else addstr("Liberal");
      }
   }
}

/*
This function prints the cash the player has with optional prefix as
well as screen coordinates.

Please note that offsetx is the offset from the right of the screen, y is
the offset from the top as always.
*/
void printfunds(int y, int offsetx, const char* prefix)
{
   char moneystr[50],prefixbuffer[50];

   if(prefix==NULL)
      strncpy(prefixbuffer,"",50);
   else strncpy(prefixbuffer,prefix,50);

   strcpy(moneystr,ledger.get_funds());

   //Save screen coordinates for later.
   int begy,begx;
   getyx(stdscr,begy,begx);

   //Save color and brightness information for later.
   short colorpair,front,back;

   attr_t attrs;
   attr_get(&attrs,&colorpair,NULL);
   bool dim=(attrs & WA_DIM)!=0;
   pair_content(colorpair,&front,&back);

   //Move, set color, and write.
   move(y,80-len(moneystr)-len(prefixbuffer)-offsetx);
   addstr(prefixbuffer);
   addstr(moneystr);

   //Recover old settings
   move(begy,begx);
   set_color(front,back,dim);
}

/* prints a short blurb showing how to page forward */
void addnextpagestr()
{
   if(interface_pgup=='[')
      addstr("] - Next");
   else if(interface_pgup=='.')
      addstr(": - Next");
   else addstr("PGDN - Next");
}

/* prints a short blurb showing how to page back */
void addprevpagestr()
{
   if(interface_pgup=='[')
      addstr("[ - Previous");
   else if(interface_pgup=='.')
      addstr("; - Previous");
   else addstr("PGUP - Previous");
}

/* prints a long blurb showing how to page forward and back */
void addpagestr()
{
   if(interface_pgup=='[')
      addstr("[] to view other Liberal pages.");
   else if(interface_pgup=='.')
      addstr("; and : to view other Liberal pages.");
   else addstr("PGUP/PGDN to view other Liberal pages.");
}

static char sbuf[81]; // used by addstr_f(), mvaddstr_f(), addstr_fl(), and mvaddstr_fl()
#define FORMAT_OUTPUT  va_list args; \
                       va_start(args,format); \
                       vsnprintf(sbuf,81,format,args); \
                       va_end(args);

/*	addstr with formatted output	*/
int addstr_f(const char * format, ...)
{
   FORMAT_OUTPUT
   return addstr(sbuf);
}

/*	mvaddstr with formatted output	*/
int mvaddstr_f(int y, int x, const char * format, ...)
{
   FORMAT_OUTPUT
   return mvaddstr(y,x,sbuf);
}


/*	addstr with formatted output and logging	*/
int addstr_fl(Log &log, const char * format, ...)
{
   FORMAT_OUTPUT
   return addstr(sbuf,log);
}

/*	mvaddstr with formatted output and logging	*/
int mvaddstr_fl(int y, int x, Log &log, const char * format, ...)
{
   FORMAT_OUTPUT
   return mvaddstr(y,x,sbuf,log);
}

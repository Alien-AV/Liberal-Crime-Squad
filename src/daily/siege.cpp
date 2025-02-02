/*
 * Copyright (c) 2002,2003,2004 by Tarn Adams
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
// set to use that character set, such as this e with an accent: �

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

#include "daily/siege.h"

#include "daily/daily.h"
#include "externs.h"
#include "locations/locations.h"
#include "locations/world.h"
#include "politics/politics.h"


/* TODO
make it less likely to be raided based on:
        - the number of sleepers.
        - how effective said sleepers are.
        - what action each sleeper is doing (promoting liberalism decreases chance while promoting conservatism increases chance)
        - what the sleeper does for a living (police officers are more influential than janitors, for instance)

make it more likely to be raided:
        - when a liberal hacks or steals credit cards.
        - dead bodies in the base. [DONE]
*/


/* siege - gives up on sieges with empty locations */
/* Work in progress. It works, but needs to be called in more places. */
/* Currently, it only works when you confront a siege and then fail. */
static void
resolvesafehouses()
{
   for(int l=0;l<len(location);l++)
   {
      if(location[l]->renting>=0&&location[l]->siege.siege)
      {
         cleangonesquads();
         selectedsiege = l; // hack for calling giveup()
         giveup();
      }
   }
}


/* siege - "you are wanted for _______ and other crimes..." */
static void
statebrokenlaws(int loc)
{
   music.play(MUSIC_SIEGE);
   short breakercount[LAWFLAGNUM]={0};
   int typenum=0,criminalcount=0,kidnapped=0;
   char kname[100];

   for(int p=0;p<len(pool);p++)
   {
      if(!pool[p]->alive||pool[p]->location!=loc) continue;

      if(pool[p]->flag&CREATUREFLAG_KIDNAPPED)
      {
         strcpy(kname,pool[p]->propername);
         kidnapped++;
      }

      if(iscriminal(*pool[p])) criminalcount++;

      for(int i=0;i<LAWFLAGNUM;i++) if(pool[p]->crimes_suspected[i]) breakercount[i]++;
   }
   for(int i=0;i<LAWFLAGNUM;i++) if(breakercount[i]) typenum++;

   erase();

   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(1,1);
   if(location[loc]->siege.underattack) addstr("You hear shouts:", gamelog);
   else addstr("You hear a blaring voice on a loudspeaker:", gamelog);
   gamelog.newline();

   move(3,1);
   if(location[loc]->siege.escalationstate>=2&&publicmood(-1)<20)
      addstr("In the name of God, your campaign of terror ends here!", gamelog);
   else addstr("Surrender yourselves!", gamelog);
   gamelog.newline();

   move(4,1);

   //KIDNAP VICTIM
   if(kidnapped)
   {
      addstr("Release ", gamelog);
      addstr(kname, gamelog);
      if(kidnapped>1) addstr(" and the others", gamelog);
      addstr(" unharmed!", gamelog);
   }
   //TREASON
   else if(breakercount[LAWFLAG_TREASON])
      addstr("You are wanted for treason", gamelog);
   //TERRORISM
   else if(breakercount[LAWFLAG_TERRORISM])
      addstr("You are wanted for terrorism", gamelog);
   //MURDERER
   else if(breakercount[LAWFLAG_MURDER])
      addstr("You are wanted for first degree murder", gamelog);
   //KIDNAPPER
   else if(breakercount[LAWFLAG_KIDNAPPING])
      addstr("You are wanted for kidnapping", gamelog);
   //BANK ROBBER
   else if(breakercount[LAWFLAG_BANKROBBERY])
      addstr("You are wanted for bank robbery", gamelog);
   //ARSONIST
   else if(breakercount[LAWFLAG_ARSON])
      addstr("You are wanted for arson", gamelog);
   //BURN FLAG
   else if(breakercount[LAWFLAG_BURNFLAG])
   {
      if(law[LAW_FLAGBURNING] == Alignment::ARCH_CONSERVATIVE)addstr("You are wanted for Flag Murder", gamelog);
      else if(law[LAW_FLAGBURNING] == Alignment::CONSERVATIVE)addstr("You are wanted for felony flag burning", gamelog);
      else addstr("You are wanted for flag burning", gamelog);
   }
   //SPEECH
   else if(breakercount[LAWFLAG_SPEECH])
      addstr("You are wanted for sedition", gamelog);
   //BROWNIES
   else if(breakercount[LAWFLAG_BROWNIES])
      addstr("You are wanted for sale and distribution of a controlled substance", gamelog);
   //ESCAPED
   else if(breakercount[LAWFLAG_ESCAPED])
      addstr("You are wanted for escaping prison", gamelog);
   //HELP ESCAPED
   else if(breakercount[LAWFLAG_HELPESCAPE])
      addstr("You are wanted for aiding a prison escape", gamelog);
   //JURY
   else if(breakercount[LAWFLAG_JURY])
      addstr("You are wanted for jury tampering", gamelog);
   //RACKETEERING
   else if(breakercount[LAWFLAG_RACKETEERING])
      addstr("You are wanted for racketeering", gamelog);
   //EXTORTION
   else if(breakercount[LAWFLAG_EXTORTION])
      addstr("You are wanted for extortion", gamelog);
   //ASSAULT
   else if(breakercount[LAWFLAG_ARMEDASSAULT])
      addstr("You are wanted for assault with a deadly weapon", gamelog);
   //ASSAULT
   else if(breakercount[LAWFLAG_ASSAULT])
      addstr("You are wanted for misdemeanor assault", gamelog);
   //CAR THEFT
   else if(breakercount[LAWFLAG_CARTHEFT])
      addstr("You are wanted for grand theft auto", gamelog);
   //CC FRAUD
   else if(breakercount[LAWFLAG_CCFRAUD])
      addstr("You are wanted for credit card fraud", gamelog);
   //THIEF
   else if(breakercount[LAWFLAG_THEFT])
      addstr("You are wanted for petty larceny", gamelog);
   //PROSTITUTION
   else if(breakercount[LAWFLAG_PROSTITUTION])
      addstr("You are wanted for prostitution", gamelog);
   //HIRE ILLEGAL
   else if(breakercount[LAWFLAG_HIREILLEGAL])
      addstr((to_right_of(law[LAW_IMMIGRATION], Alignment::LIBERAL)?"You are wanted for hiring an illegal alien":"You are wanted for hiring an undocumented worker"), gamelog);
   //GUN USE
   /*else if(breakercount[LAWFLAG_GUNUSE])
      addstr("You are wanted for firing an illegal weapon", gamelog);
   //GUN CARRY
   else if(breakercount[LAWFLAG_GUNCARRY])
      addstr("You are wanted for possession of an illegal weapon", gamelog);*/
   //COMMERCE
   else if(breakercount[LAWFLAG_COMMERCE])
      addstr("You are wanted for interference with interstate commerce", gamelog);
   //INFORMATION
   else if(breakercount[LAWFLAG_INFORMATION])
      addstr("You are wanted for unlawful access of an information system", gamelog);
   //UNLAWFUL BURIAL
   else if(breakercount[LAWFLAG_BURIAL])
      addstr("You are wanted for unlawful burial", gamelog);
   //BREAKING
   else if(breakercount[LAWFLAG_BREAKING])
      addstr("You are wanted for breaking and entering", gamelog);
   //VANDALISM
   else if(breakercount[LAWFLAG_VANDALISM])
      addstr("You are wanted for vandalism", gamelog);
   //RESIST
   else if(breakercount[LAWFLAG_RESIST])
      addstr("You are wanted for resisting arrest", gamelog);
   //DISTURBANCE
   else if(breakercount[LAWFLAG_DISTURBANCE])
      addstr("You are wanted for disturbing the peace", gamelog);
   //PUBLIC NUDITY
   else if(breakercount[LAWFLAG_PUBLICNUDITY])
      addstr("You are wanted for indecent exposure", gamelog);
   //LOITERING
   else if(breakercount[LAWFLAG_LOITERING])
      addstr("You are wanted for loitering", gamelog);
   //THEY WERE LOOKING FOR SOMEONE ELSE
   else addstr("You are wanted for harboring a fugitive from justice", gamelog);

   if(!kidnapped)
   {
      if(typenum>1) addstr(" and other crimes", gamelog);
      addstr(".", gamelog);
   }
   gamelog.nextMessage();

   getkey();
}


void
statebrokenlaws(Creature const& cr)
{
   bool kidnapped=(cr.flag&CREATUREFLAG_KIDNAPPED),criminal=false,breakercount[LAWFLAGNUM];

   for(int i=0;i<LAWFLAGNUM;i++)
      if(cr.crimes_suspected[i]) breakercount[i]=true,criminal=true;
      else breakercount[i]=false;

   if(!criminal&&!kidnapped) return;

   set_color(COLOR_YELLOW,COLOR_BLACK,1);
   addstr("WANTED FOR ");

   //KIDNAP VICTIM
   if(kidnapped)
      addstr("REHABILITATION");
   //TREASON
   else if(breakercount[LAWFLAG_TREASON])
      addstr("TREASON");
   //TERRORISM
   else if(breakercount[LAWFLAG_TERRORISM])
      addstr("TERRORISM");
   //MURDERER
   else if(breakercount[LAWFLAG_MURDER])
      addstr("MURDER");
   //KIDNAPPER
   else if(breakercount[LAWFLAG_KIDNAPPING])
      addstr("KIDNAPPING");
   //BANK ROBBER
   else if(breakercount[LAWFLAG_BANKROBBERY])
      addstr("BANK ROBBERY");
   //ARSONIST
   else if(breakercount[LAWFLAG_BANKROBBERY])
      addstr("ARSON");
   //BURN FLAG
   else if(breakercount[LAWFLAG_BURNFLAG])
      addstr(law[LAW_FLAGBURNING] == Alignment::ARCH_CONSERVATIVE?"FLAG MURDER":"FLAG BURNING");
   //SPEECH
   else if(breakercount[LAWFLAG_SPEECH])
      addstr("HARMFUL SPEECH");
   //BROWNIES
   else if(breakercount[LAWFLAG_BROWNIES])
      addstr("DRUG DEALING");
   //ESCAPED
   else if(breakercount[LAWFLAG_ESCAPED])
      addstr("ESCAPING PRISON");
   //HELP ESCAPED
   else if(breakercount[LAWFLAG_HELPESCAPE])
      addstr("RELEASING PRISONERS");
   //JURY
   else if(breakercount[LAWFLAG_JURY])
      addstr("JURY TAMPERING");
   //RACKETEERING
   else if(breakercount[LAWFLAG_RACKETEERING])
      addstr("RACKETEERING");
   //EXTORTION
   else if(breakercount[LAWFLAG_EXTORTION])
      addstr("EXTORTION");
   //ASSAULT
   else if(breakercount[LAWFLAG_ARMEDASSAULT])
      addstr("ARMED ASSAULT");
   //ASSAULT
   else if(breakercount[LAWFLAG_ASSAULT])
      addstr("ASSAULT");
   //CAR THEFT
   else if(breakercount[LAWFLAG_CARTHEFT])
      addstr("GRAND THEFT AUTO");
   //CC FRAUD
   else if(breakercount[LAWFLAG_CCFRAUD])
      addstr("CREDIT CARD FRAUD");
   //THIEF
   else if(breakercount[LAWFLAG_THEFT])
      addstr("THEFT");
   //PROSTITUTION
   else if(breakercount[LAWFLAG_PROSTITUTION])
      addstr("PROSTITUTION");
   //HIRE ILLEGAL
   else if(breakercount[LAWFLAG_HIREILLEGAL])
      addstr(to_right_of(law[LAW_IMMIGRATION], Alignment::LIBERAL)?"HIRING ILLEGAL ALIENS":"HIRING UNDOCUMENTED WORKERS");
   //GUN USE
   /*else if(breakercount[LAWFLAG_GUNUSE])
      addstr("FIRING ILLEGAL WEAPONS");
   //GUN CARRY
   else if(breakercount[LAWFLAG_GUNCARRY])
      addstr("CARRYING ILLEGAL WEAPONS");*/
   //COMMERCE
   else if(breakercount[LAWFLAG_COMMERCE])
      addstr("ELECTRONIC SABOTAGE");
   //INFORMATION
   else if(breakercount[LAWFLAG_INFORMATION])
      addstr("HACKING");
   //UNLAWFUL BURIAL
   else if(breakercount[LAWFLAG_BURIAL])
      addstr("UNLAWFUL BURIAL");
   //BREAKING
   else if(breakercount[LAWFLAG_BREAKING])
      addstr("BREAKING AND ENTERING");
   //VANDALISM
   else if(breakercount[LAWFLAG_VANDALISM])
      addstr("VANDALISM");
   //RESIST
   else if(breakercount[LAWFLAG_RESIST])
      addstr("RESISTING ARREST");
   //DISTURBANCE
   else if(breakercount[LAWFLAG_DISTURBANCE])
      addstr("DISTURBING THE PEACE");
   //PUBLIC NUDITY
   else if(breakercount[LAWFLAG_PUBLICNUDITY])
      addstr("PUBLIC NUDITY");
   //LOITERING
   else if(breakercount[LAWFLAG_LOITERING])
      addstr("LOITERING");
}

/* siege - updates upcoming sieges */
void siegecheck(bool canseethings)
{
   if(disbanding)return;

// Upkeep - even base-less should be considered.
// XXX - candidate to create nice function?
  // Cleanse record on things that aren't illegal right now
  for(int p=0;p<len(pool);p++)
  {
    if (to_left_of(law[LAW_FLAGBURNING], Alignment::MODERATE))
       pool[p]->crimes_suspected[LAWFLAG_BURNFLAG] = 0;
    if (to_left_of(law[LAW_DRUGS], Alignment::MODERATE))
      pool[p]->crimes_suspected[LAWFLAG_BROWNIES]=0;
    if (to_left_of(law[LAW_IMMIGRATION], Alignment::MODERATE))
      pool[p]->flag &= ~CREATUREFLAG_ILLEGALALIEN;
    if (to_left_of(law[LAW_FREESPEECH], Alignment::ARCH_CONSERVATIVE))
      pool[p]->crimes_suspected[LAWFLAG_SPEECH]=0;
  }
  if (to_left_of(law[LAW_FREESPEECH], Alignment::ARCH_CONSERVATIVE))
    offended_firemen = 0;

   //FIRST, THE COPS
   int numpres;
   for(int l=0;l<len(location);l++)
   {
      if(location[find_police_station(l)]->closed)
      {
         location[l]->heat = static_cast<int>(location[l]->heat * 0.95);
      }
      if(location[l]->siege.siege)continue;
      if(location[l]->renting==RENTING_NOCONTROL)continue;
      numpres=0;

      if(location[l]->siege.timeuntillocated == -2)
      {
         //IF JUST SIEGED, BUY SOME TIME
         location[l]->siege.timeuntillocated=-1;
      }
      else
      {
         //HUNTING
         if(location[l]->siege.timeuntillocated>0)
         {
            if(location[l]->front_business == -1 || LCSrandom(2))
            {
               location[l]->siege.timeuntillocated--;
               // Hunt faster if location is extremely hot
               if(location[l]->heat > 100)
               {
                  int hunt_speed;
                  hunt_speed = location[l]->heat / 50;
                  while(hunt_speed&&location[l]->siege.timeuntillocated>1)
                  {
                     location[l]->siege.timeuntillocated--;
                     hunt_speed--;
                  }
               }
            }
         }

         //CHECK FOR CRIMINALS AT THIS BASE
         int crimes=0;
         //int heatprotection=0;
         for(int p=0;p<len(pool);p++)
         {
            // Sleepers and people not at this base don't count
            if(pool[p]->location!=l || pool[p]->flag & CREATUREFLAG_SLEEPER)continue;

            if(!pool[p]->alive) // Corpses attract attention
            {
               crimes += 5;
               continue;
            }
            if (pool[p]->flag & CREATUREFLAG_KIDNAPPED && pool[p]->align != Alignment::LIBERAL)
            {
               crimes += 5*pool[p]->joindays; // Kidnapped persons increase heat
               continue;
            }
            if (pool[p]->align != Alignment::LIBERAL)
              continue; // Non-liberals don't count other than that
            numpres++;

            // Accumulate heat from liberals who have it, but let them bleed it off in the process
            if(pool[p]->heat > 0)
            {
               crimes += pool[p]->heat/(pool[p]->activity.type==ACTIVITY_NONE?60:10) + 1;
               pool[p]->heat -= MIN(pool[p]->activity.type==ACTIVITY_NONE?pool[p]->heat/10:5,pool[p]->heat);
            }
         }

         // Determine how effective your current safehouse
         // is at keeping the police confused
         location[l]->update_heat_protection();

         // Let the place cool off if not accumulating heat
         if(crimes < location[l]->heat)
         {
            location[l]->heat -= 1;

            if(location[l]->heat < 0)
               location[l]->heat = 0;
         }
         else
         {
            // Update location heat
            if(crimes > location[l]->heat) location[l]->heat += (crimes-location[l]->heat)/10 + 1;

            // Begin planning siege if high heat on location
            if(location[l]->heat > location[l]->heat_protection &&
               LCSrandom(500) < location[l]->heat &&
               !(location[l]->siege.timeuntillocated>=0)) //Do not re-plan siege.
            {
               // Set time until siege is carried out
               location[l]->siege.timeuntillocated += 2 + (short)LCSrandom(6);
            }
         }

         // Sleepers at the police department may give a warning just before police raids
         if(location[l]->siege.timeuntillocated==1)
         {
            int policesleeperwarning=0;
            for(int pl=0;pl<len(pool);pl++)
            {
               if(pool[pl]->flag & CREATUREFLAG_SLEEPER &&
                  pool[pl]->location!=-1 &&
                  location[pool[pl]->location]->type==SITE_GOVERNMENT_POLICESTATION &&
                  location[pool[pl]->location]->city==location[l]->city)
               {
                  //if(pool[pl]->infiltration*100>LCSrandom(50))
                  {
                     policesleeperwarning=1;
                     break;
                  }
               }
            }
            if(policesleeperwarning)
            {
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("You have received advance warning from your sleepers regarding ", gamelog);
               move(9,1);
               addstr("an imminent police raid on ", gamelog);
               addstr(location[l]->getname());
               addstr(".", gamelog);
               gamelog.newline();

               if(location[l]->siege.escalationstate>=1)
               {
                  move(11,1);
                  addstr("The fighting force will be composed of national guard troops.", gamelog);
               }
               if(location[l]->siege.escalationstate>=2)
               {
                  move(12,1);
                  addstr("A tank will cover the entrance to the compound.", gamelog);
               }
               if(location[l]->siege.escalationstate>=3)
               {
                  move(13,1);
                  addstr("Planes will bomb the compound during the siege, and the final ", gamelog);
                  move(14,1);
                  addstr("attack will be carried out by SEAL Team 6.", gamelog);
               }
               gamelog.nextMessage(); //Write out buffer to prepare for next message.

               move (15,1);
               addstr("Press Esc to ponder the situation...");
               int c;
               do c=getkey(); while(c!='x'&&c!=ESC);
            }
         }

         //COPS RAID THIS LOCATION
         if(!location[l]->siege.timeuntillocated)
         {
            location[l]->siege.timeuntillocated=-2;
            location[l]->heat=0;

            if(numpres>0)
            {
               music.play(MUSIC_SIEGE);
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);

               move(8,1);
               addstr("The police have surrounded the ", gamelog);
               addstr(location[l]->getname(), gamelog);
               addstr("!", gamelog);
               gamelog.newline();
               location[l]->siege.underattack=0;

               getkey();

               //MENTION ESCALATION STATE
               if(location[l]->siege.escalationstate>=1)
               {
                  move(9,1);
                  addstr("National Guard troops are replacing normal SWAT units.", gamelog);
                  gamelog.nextMessage();

                  getkey();
               }
               if(location[l]->siege.escalationstate>=2)
               {
                  move(10,1);
                  if(location[l]->compound_walls & COMPOUND_TANKTRAPS)
                     addstr("An M1 Abrams Tank is stopped by the tank traps.", gamelog);
                  else addstr("An M1 Abrams Tank takes up position outside the compound.", gamelog);
                  gamelog.nextMessage();

                  getkey();
               }
               if(location[l]->siege.escalationstate>=3)
               {
                  move(11,1);
                  addstr("You hear jet bombers streak overhead.", gamelog);
                  gamelog.nextMessage();

                  getkey();
               }

               // "You are wanted for blahblah and other crimes."
               statebrokenlaws(l);

               location[l]->siege.siege=1;
               location[l]->siege.siegetype=SIEGE_POLICE;
               location[l]->siege.lights_off=0;
               location[l]->siege.cameras_off=0;
            }
            else
            {
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);

               move(8,1);
               addstr("The cops have raided the ", gamelog);
               addstr(location[l]->getname(), gamelog);
               addstr(", an unoccupied safehouse.", gamelog);
               gamelog.newline();

               getkey();

               int y=9;

               for(int p=len(pool)-1;p>=0;p--)
               {
                  if(pool[p]->location!=l) continue;
                  if(!pool[p]->alive)
                  {
                     move(y,1);y++;
                     addstr(pool[p]->name, gamelog);
                     addstr("'s corpse has been recovered.", gamelog);
                     gamelog.newline();

                     getkey();

                     delete_and_remove(pool,p);
                     continue;
                  }
                  if (pool[p]->align != Alignment::LIBERAL)
                  {
                     move(y,1);y++;
                     addstr(pool[p]->name, gamelog);
                     addstr(" has been rescued.", gamelog);
                     gamelog.newline();

                     getkey();

                     delete_and_remove(pool,p);
                     continue;
                  }
               }

               gamelog.newline();

               delete_and_clear(location[l]->loot);

               for(int v=len(vehicle)-1;v>=0;v--)
                  if(vehicle[v]->get_location()==l)
                     delete_and_remove(vehicle,v);
            }
         }

         //OTHER OFFENDABLE ENTITIES
            //CORPS
         if (location[l]->heat&&location[l]->siege.timeuntilcorps == -1
             && !location[l]->siege.siege
             && offended_corps
             && !LCSrandom(600)
             && numpres>0)
         {
            location[l]->siege.timeuntilcorps = (short) LCSrandom(3) + 1;
            // *JDS* CEO sleepers may give a warning before corp raids
            int ceosleepercount=0;
            for(int pl=0;pl<len(pool);pl++)
            {
               if(pool[pl]->flag & CREATUREFLAG_SLEEPER&&
                  pool[pl]->type==CREATURE_CORPORATE_CEO)
               {
                  ceosleepercount=1;
               }
            }
            if(ceosleepercount||!LCSrandom(5))
            {
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("You have received ", gamelog);
               if(ceosleepercount)addstr("your sleeper CEO's warning", gamelog);
               else addstr("an anonymous tip", gamelog);
               addstr(" that the Corporations ", gamelog);
               move(9,1);
               addstr("are hiring mercenaries to attack ", gamelog);
               if(ceosleepercount)addstr(location[l]->getname(), gamelog);
               else addstr("the LCS", gamelog);
               addstr(".", gamelog);
               gamelog.nextMessage();

               getkey();
            }
         }
         else if(location[l]->siege.timeuntilcorps>0)location[l]->siege.timeuntilcorps--; // Corp raid countdown!
         else if(location[l]->siege.timeuntilcorps==0&&!location[l]->siege.siege&&offended_corps&&numpres>0)
         {
            music.play(MUSIC_SIEGE);
            location[l]->siege.timeuntilcorps=-1;
            // Corps raid!
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Corporate mercenaries are raiding the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr("!", gamelog);
            gamelog.nextMessage();

            getkey();

            location[l]->siege.siege=1;
            location[l]->siege.siegetype=SIEGE_CORPORATE;
            location[l]->siege.underattack=1;
            location[l]->siege.lights_off=0;
            location[l]->siege.cameras_off=0;
            offended_corps=0;
         }
         else if(location[l]->siege.timeuntilcorps==0)location[l]->siege.timeuntilcorps=-1; // Silently call off foiled corp raids

            //CONSERVATIVE CRIME SQUAD
         bool ccs_active = endgamestate>=ENDGAME_CCS_APPEARANCE && endgamestate<ENDGAME_CCS_DEFEATED;
         bool target_interesting = endgamestate>=ENDGAME_CCS_SIEGES || location[l]->compound_walls & COMPOUND_PRINTINGPRESS;
         if(ccs_active && target_interesting)
         {
            if(location[l]->heat&&location[l]->siege.timeuntilccs==-1&&!location[l]->siege.siege&&!LCSrandom(60)&&numpres>0)
            {
               location[l]->siege.timeuntilccs = (short) LCSrandom(3) + 1;
               // CCS sleepers may give a warning before raids
               int ccssleepercount=0;
               for(int pl=0;pl<len(pool);pl++)
               {
                  if(pool[pl]->flag & CREATUREFLAG_SLEEPER&&
                     (pool[pl]->type==CREATURE_CCS_VIGILANTE || pool[pl]->type==CREATURE_CCS_ARCHCONSERVATIVE ||
                      pool[pl]->type==CREATURE_CCS_MOLOTOV || pool[pl]->type==CREATURE_CCS_SNIPER))
                  {
                     ccssleepercount=1;
                     break;
                  }
               }
               if(ccssleepercount>0)
               {
                  erase();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr("You have received a sleeper warning that the CCS ", gamelog);
                  move(9,1);
                  addstr(" is gearing up to attack ", gamelog);
                  addstr(location[l]->name, gamelog);
                  addstr(".", gamelog);
                  gamelog.nextMessage();

                  getkey();
               }
            }
            else if(location[l]->siege.timeuntilccs>0)location[l]->siege.timeuntilccs--; // CCS raid countdown!
            else if(location[l]->siege.timeuntilccs==0&&!location[l]->siege.siege&&numpres>0)
            {
               music.play(MUSIC_SIEGE);
               location[l]->siege.timeuntilccs=-1;
               // CCS raid!
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);

               move(8,1);
               addstr("A screeching truck pulls up to ", gamelog);
               addstr(location[l]->getname(), gamelog);
               addstr("!", gamelog);
               gamelog.newline();

               getkey();

               if(!(location[l]->compound_walls & COMPOUND_TANKTRAPS) &&
                  !LCSrandom(5))
               {
                  // CCS Carbombs safehouse!!
                  erase();
                  set_color(COLOR_RED,COLOR_BLACK,1);

                  move(8,1);
                  addstr("The truck plows into the building and explodes!", gamelog);
                  gamelog.nextMessage();

                  getkey();

                  erase();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);

                  move(0,1);
                  addstr("CCS CAR BOMBING CASUALTY REPORT");

                  move(2,1);
                  addstr("KILLED: ");
                  int killed_y = 2;
                  int killed_x = 9;

                  move(6,1);
                  addstr("INJURED: ");
                  int injured_y = 6;
                  int injured_x = 10;

                  for(int i=0;i<len(pool);i++)
                  {
                     if(pool[i]->location==l)
                     {
                        if(LCSrandom(2))
                        {
                           int namelength=len(pool[i]->name);
                           pool[i]->blood -= (short) LCSrandom(101 - pool[i]->juice / 10) + 10;
                           if(pool[i]->blood<0)
                           {
                              if(killed_x+namelength>78)
                              {
                                 killed_y++;
                                 killed_x=1;
                                 //Add limit for killed_y.
                              }
                              move(killed_y,killed_x);
                              pool[i]->die();
                              set_alignment_color(pool[i]->align,false);
                              addstr(pool[i]->name);
                              addstr(", ");
                              killed_x+=namelength+2;
                           }
                           else
                           {
                              if(injured_x+namelength>78)
                              {
                                 injured_y++;
                                 injured_x=1;
                                 //Add limit for injured_y.
                              }
                              move(injured_y,injured_x);
                              set_alignment_color(pool[i]->align,false);
                              addstr(pool[i]->name);
                              addstr(", ");
                              injured_x+=namelength+2;
                           }
                           //set_alignment_color(pool[i]->align,false);
                           //addstr(pool[i]->name);
                        }
                     }
                  }

                  getkey();
               }
               else
               {
                  // CCS Raids safehouse
                  erase();
                  set_color(COLOR_RED,COLOR_BLACK,1);

                  move(8,1);
                  addstr("CCS members pour out of the truck and shoot in the front doors!", gamelog);
                  gamelog.nextMessage();

                  getkey();

                  location[l]->siege.siege=1;
                  location[l]->siege.siegetype=SIEGE_CCS;
                  location[l]->siege.underattack=1;
                  location[l]->siege.lights_off=0;
                  location[l]->siege.cameras_off=0;
               }
            }
            else if(location[l]->siege.timeuntilccs==0)location[l]->siege.timeuntilccs=-1; // Silently call off foiled ccs raids
         }


            //CIA
         if(location[l]->heat&&location[l]->siege.timeuntilcia==-1&&!location[l]->siege.siege&&offended_cia&&!LCSrandom(300)&&numpres>0)
         {
            location[l]->siege.timeuntilcia = (short) LCSrandom(3) + 1;
            // *JDS* agent sleepers may give a warning before cia raids
            int agentsleepercount=0;
            for(int pl=0;pl<len(pool);pl++)
            {
               if(pool[pl]->flag & CREATUREFLAG_SLEEPER&&
                  pool[pl]->type==CREATURE_AGENT)
               {
                  //if(pool[pl]->infiltration*100>LCSrandom(100))
                  {
                     agentsleepercount=1;
                     break;
                  }
               }
            }
            if(agentsleepercount)
            {
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("A sleeper agent has reported that the CIA is planning ", gamelog);
               move(9,1);
               addstr("to launch an attack on ", gamelog);
               addstr(location[l]->getname(), gamelog);
               addstr(".", gamelog);
               gamelog.nextMessage();

               getkey();
            }
         }
         else if(location[l]->siege.timeuntilcia>0)location[l]->siege.timeuntilcia--; // CIA raid countdown!
         else if(location[l]->siege.timeuntilcia==0&&!location[l]->siege.siege&&offended_cia&&numpres>0)
         {
            music.play(MUSIC_SIEGE);
            location[l]->siege.timeuntilcia=-1;
            // CIA raids!
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Unmarked black vans are surrounding the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr("!", gamelog);
            gamelog.newline();

            if(location[l]->compound_walls & COMPOUND_CAMERAS)
            {
               move(9,1);
               addstr("Through some form of high technology, they've managed ", gamelog);
               move(10,1);
               addstr("to shut off the lights and the cameras!", gamelog);
               gamelog.nextMessage();
            }
            else if(location[l]->compound_walls & COMPOUND_GENERATOR)
            {
               move(9,1);
               addstr("Through some form of high technology, they've managed ", gamelog);
               move(10,1);
               addstr("to shut off the lights!", gamelog);
               gamelog.nextMessage();
            }
            else
            {
               move(9,1);
               addstr("They've shut off the lights!", gamelog);
               gamelog.nextMessage();
            }

            getkey();

            location[l]->siege.siege=1;
            location[l]->siege.siegetype=SIEGE_CIA;
            location[l]->siege.underattack=1;
            location[l]->siege.lights_off=1;
            location[l]->siege.cameras_off=1;
         }
         else if(location[l]->siege.timeuntilcia==0)location[l]->siege.timeuntilcia=-1; // Silently call off foiled cia raids

            //HICKS
         if(!location[l]->siege.siege&&offended_amradio&&attitude[VIEW_AMRADIO]<=35&&!LCSrandom(600)&&numpres>0)
         {
            music.play(MUSIC_SIEGE);
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Masses dissatisfied with your lack of respect for AM Radio ", gamelog);
            move(9,1);
            addstr("are storming the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr("!", gamelog);
            gamelog.nextMessage();

            getkey();

            location[l]->siege.siege=1;
            location[l]->siege.siegetype=SIEGE_HICKS;
            location[l]->siege.underattack=1;
            location[l]->siege.lights_off=0;
            location[l]->siege.cameras_off=0;
            offended_amradio=0;
         }
         if(!location[l]->siege.siege&&offended_cablenews&&attitude[VIEW_CABLENEWS]<=35&&!LCSrandom(600)&&numpres>0)
         {
            music.play(MUSIC_SIEGE);
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Masses dissatisfied with your lack of respect for Cable News ", gamelog);
            move(9,1);
            addstr("are storming the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr("!", gamelog);
            gamelog.nextMessage();

            getkey();

            location[l]->siege.siege=1;
            location[l]->siege.siegetype=SIEGE_HICKS;
            location[l]->siege.underattack=1;
            location[l]->siege.lights_off=0;
            location[l]->siege.cameras_off=0;
            offended_cablenews=0;
         }
         //Firemen
         if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE && location[l]->siege.timeuntilfiremen==-1 && !location[l]->siege.siege &&
            offended_firemen && numpres>0 && location[l]->compound_walls & COMPOUND_PRINTINGPRESS && !LCSrandom(90))
         {
            location[l]->siege.timeuntilfiremen=(short)LCSrandom(3)+1;

            // Sleeper Firemen can warn you of an impending raid
            int firemensleepercount=0;
            for(int pl=0;pl<len(pool);pl++)
               if(pool[pl]->flag & CREATUREFLAG_SLEEPER &&
                  pool[pl]->type==CREATURE_FIREFIGHTER &&
                  location[pool[pl]->location]->city==location[l]->city)
                  firemensleepercount++;
            if(LCSrandom(firemensleepercount+1)>0||!LCSrandom(10))
            {
               erase();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               if(firemensleepercount) addstr("A sleeper Fireman has informed you that ", gamelog);
               else addstr("Word in the underground is that ", gamelog);
               move(9,1);
               addstr("the Firemen are planning to burn ", gamelog);
               addstr(location[l]->name, gamelog);
               addstr(".", gamelog);
               gamelog.nextMessage();

               getkey();
            }

         } else if(location[l]->siege.timeuntilfiremen>0) location[l]->siege.timeuntilfiremen--;
         else if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE && location[l]->siege.timeuntilfiremen==0 && !location[l]->siege.siege&&numpres>0)
         {
            music.play(MUSIC_SIEGE);
            location[l]->siege.timeuntilfiremen=-1;
            // Firemen raid!
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Screaming fire engines pull up to the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr("!", gamelog);
            gamelog.newline();

            move(9,1);
            addstr("Armored firemen swarm out, pilot lights burning.", gamelog);
            gamelog.newline();

            getkey();

            erase();

            set_color(COLOR_WHITE,COLOR_BLACK,1);
            move(1,1);
            addstr("You hear a screeching voice over the sound of fire engine sirens:", gamelog);
            gamelog.newline();

            move(3,1);
            addstr("Surrender yourselves!", gamelog);
            gamelog.newline();

            move(4,1);
            addstr("Unacceptable Speech has occurred at this location.", gamelog);
            gamelog.newline();

            move(6,1);
            addstr("Come quietly and you will not be harmed.", gamelog);
            gamelog.nextMessage();

            getkey();

            location[l]->siege.siege=1;
            location[l]->siege.siegetype=SIEGE_FIREMEN;
            location[l]->siege.underattack=1;
            location[l]->siege.lights_off=0;
            location[l]->siege.cameras_off=0;
            offended_firemen=0;
         }
         else if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE && location[l]->siege.timeuntilfiremen==0)
         {
            location[l]->siege.timeuntilfiremen=-1;
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("The Firemen have raided the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr(", an unoccupied safehouse.", gamelog);
            gamelog.newline();

            getkey();

            int y=9;

            for(int p=len(pool)-1;p>=0;p--)
            {
               if(pool[p]->location!=l)continue;
               if(!pool[p]->alive)
               {
                  move(y++,1);
                  addstr(pool[p]->name, gamelog);
                  addstr("'s corpse has been recovered.", gamelog);
                  gamelog.newline();

                  getkey();

                  delete_and_remove(pool,p);
                  continue;
               }
               if (pool[p]->align != Alignment::LIBERAL)
               {
                  move(y++,1);
                  addstr(pool[p]->name, gamelog);
                  addstr(" has been rescued.", gamelog);
                  gamelog.newline();

                  getkey();

                  delete_and_remove(pool,p);
                  continue;
               }
            }
            delete_and_clear(location[l]->loot);

            if(location[l]->compound_walls & COMPOUND_PRINTINGPRESS)
            {
               move(10,1);
               addstr("The printing press is dismantled and burned.", gamelog);
               gamelog.newline();
               location[l]->compound_walls &= ~COMPOUND_PRINTINGPRESS;
               offended_firemen=0;
            }

            if(location[l]->front_business!=-1)
            {
               move(12,1);
               addstr("Materials relating to the business front have been destroyed.", gamelog);
               gamelog.newline();
               location[l]->front_business=-1;
            }
            gamelog.newline();
         }
         else if (to_right_of(law[LAW_FREESPEECH], Alignment::MODERATE) && location[l]->siege.timeuntilfiremen==0)
            location[l]->siege.timeuntilfiremen=-1,offended_firemen=0;
      }
   }
}



/* siege - updates sieges in progress */
void siegeturn(bool clearformess)
{
   if(disbanding)return;

   // Count people at each location
   int l;
   //int hs=-1;
   std::vector<int> liberalcount(len(location), 0);

   for(int p=0;p<len(pool);p++)
   {
      if(!pool[p]->alive)continue; // Dead people don't count
      if(pool[p]->align!=Alignment::LIBERAL)continue; // Non-liberals don't count
      if(pool[p]->location==-1)continue; // Vacationers don't count
      liberalcount[pool[p]->location]++;
   }

   for(l=0;l<len(location);l++) if(location[l]->siege.siege)
      {
         //resolve sieges with no people
         if(liberalcount[l]==0)
         {
            erase();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(8,1);
            addstr("Conservatives have raided the ", gamelog);
            addstr(location[l]->getname(), gamelog);
            addstr(", an unoccupied safehouse.", gamelog);
            gamelog.newline();

            if(location[l]->siege.siegetype==SIEGE_CCS&&location[l]->type==SITE_INDUSTRY_WAREHOUSE)
               location[l]->renting=RENTING_CCS; // CCS Captures warehouse

            getkey();

            int y=9;

            for(int p=len(pool)-1;p>=0;p--)
            {
               if(pool[p]->location!=l) continue;
               if(!pool[p]->alive)
               {
                  move(y++,1);
                  addstr(pool[p]->name);
                  addstr("'s corpse has been recovered.", gamelog);
                  gamelog.newline();

                  getkey();

                  delete_and_remove(pool,p);
                  continue;
               }
               if(pool[p]->align!=Alignment::LIBERAL)
               {
                  move(y++,1);
                  addstr(pool[p]->name);
                  addstr(" has been rescued.", gamelog);
                  gamelog.newline();

                  getkey();

                  delete_and_remove(pool,p);
                  continue;
               }
            }
            delete_and_clear(location[l]->loot);

            for(int v=len(vehicle)-1;v>=0;v--) if(vehicle[v]->get_location()==l) delete_and_remove(vehicle,v);

            gamelog.newline();

            location[l]->siege.siege=0;
         }

         if(!location[l]->siege.underattack)
         {
            // Seperate logging message.
            gamelog.record("A day passes while under siege...");
            gamelog.newline();

            //EAT
            bool starving=false;
            int eaters=numbereating(l);
            if(location[l]->compound_stores==0&&eaters>0)
            {
               starving=true;
               if(clearformess) erase();
               else makedelimiter();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("Your Liberals are starving!", gamelog);
               gamelog.newline();

               getkey();
            }
            if(location[l]->compound_stores>=eaters) location[l]->compound_stores-=eaters;
            else location[l]->compound_stores=0;

            //ATTACK!
            char attack=0;

            for(int p=0;p<len(pool);p++)
            {
               if(!pool[p]->alive||pool[p]->location!=l) continue;

               if(starving) pool[p]->blood-=(short)LCSrandom(8)+4;

               // Check if liberal starved to death.
               if(pool[p]->blood<=0)
               {
                  pool[p]->die();

                  if(clearformess) erase();
                  else makedelimiter();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr(pool[p]->name , gamelog);
                  addstr(" has starved to death.", gamelog);
                  gamelog.newline();

                  getkey();
               }
            }

            if(!LCSrandom(12))attack=1;

            if(attack)
            {
               if(clearformess) erase();
               else makedelimiter();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("The cops are coming!", gamelog);
               gamelog.newline();

               getkey();

               location[l]->siege.underattack=1;
            }
            else
            {
               char no_bad=1;

               //CUT LIGHTS
               if(!location[l]->siege.lights_off &&
                  !(location[l]->compound_walls & COMPOUND_GENERATOR) && !LCSrandom(10))
               {
                  no_bad=0;

                  if(clearformess) erase();
                  else makedelimiter();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr("The police have cut the lights!", gamelog);
                  gamelog.newline();

                  getkey();

                  location[l]->siege.lights_off=1;
               }

               //SNIPER
               if(!(location[l]->compound_walls & COMPOUND_BASIC) && !LCSrandom(5))
               {
                  no_bad=0;

                  vector<int> pol;
                  for(int p=0;p<len(pool);p++) if(pool[p]->alive&&pool[p]->location==l) pol.push_back(p);

                  if(len(pol))
                  {
                     if(clearformess) erase();
                     else makedelimiter();
                     set_color(COLOR_WHITE,COLOR_BLACK,1);
                     move(8,1);
                     int targ=pickrandom(pol);
                     if((int)LCSrandom(50)>pool[targ]->juice)
                     {
                        addstr("A sniper takes out ", gamelog);
                        addstr(pool[targ]->name, gamelog);
                        addstr("!", gamelog);
                        gamelog.newline();

                        if(pool[targ]->align == Alignment::LIBERAL) stat_dead++,liberalcount[l]--;

                        removesquadinfo(*pool[targ]);

                        pool[targ]->die();
                     }
                     else
                     {
                        addstr("A sniper nearly hits ", gamelog);
                        addstr(pool[targ]->name, gamelog);
                        addstr("!", gamelog);
                        gamelog.newline();
                     }

                     getkey();
                  }
               }

               if(location[l]->siege.escalationstate>=3 && !LCSrandom(3))
               {
                  no_bad=0;

                  //AIR STRIKE!
                  bool hit=true;

                  if(clearformess) erase();
                  else makedelimiter();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr("You hear planes streak overhead!", gamelog);
                  gamelog.newline();

                  getkey();

                  bool hasAAGun = location[l]->compound_walls & COMPOUND_AAGUN;
                  bool hasGenerator = location[l]->compound_walls & COMPOUND_GENERATOR;

                  if(hasAAGun)
                  {
                     if(clearformess) erase();
                     else makedelimiter();
                     move(8,1);
                     addstr("The thunder of the anti-aircraft gun shakes the compound!", gamelog);
                     gamelog.newline();

                     getkey();

                     if(clearformess) erase();
                     else makedelimiter();
                     move(8,1);
                     if(LCSrandom(5))
                     {
                        hit=false;
                        if(LCSrandom(2)) addstr("You didn't shoot any down, but you've made them think twice!", gamelog);
                        else
                        {
                           addstr("Hit! One of the bombers slams into to the ground.", gamelog);
                           gamelog.newline();

                           getkey();

                           if(clearformess) erase();
                           else makedelimiter();
                           move(8,1);
                           addstr("It's all over the TV. Everyone in the Liberal Crime Squad gains 20 juice!", gamelog);
                           for(int p=0;p<len(pool);p++) addjuice(*pool[p],20,1000);
                        }
                        gamelog.newline();

                        getkey();
                     }
                     else
                     {
                        addstr("A skilled pilot gets through!", gamelog);
                        gamelog.newline();

                        getkey();
                     }
                  }

                  if(hit)
                  {
                     if(clearformess) erase();
                     else makedelimiter();
                     move(8,1);
                     addstr("Explosions rock the compound!", gamelog);
                     gamelog.newline();

                     getkey();

                     if(hasAAGun && !LCSrandom(3))
                     {
                        if(clearformess) erase();
                        else makedelimiter();
                        move(8,1);
                        addstr("The anti-aircraft gun takes a direct hit!", gamelog);
                        gamelog.newline();

                        getkey();

                        if(clearformess) erase();
                        else makedelimiter();
                        move(8,1);
                        addstr("There's nothing left but smoking wreckage...", gamelog);
                        gamelog.newline();

                        getkey();

                        location[l]->compound_walls&=~COMPOUND_AAGUN;
                     }
                     else if(hasGenerator && !LCSrandom(3))
                     {
                        if(clearformess) erase();
                        else makedelimiter();
                        move(8,1);
                        addstr("The generator has been destroyed!", gamelog);
                        gamelog.newline();

                        getkey();

                        if(clearformess) erase();
                        else makedelimiter();
                        move(8,1);
                        addstr("The lights fade and all is dark.", gamelog);
                        gamelog.newline();

                        getkey();

                        location[l]->compound_walls&=~COMPOUND_GENERATOR;
                        location[l]->siege.lights_off=1;
                     }
                     if(!LCSrandom(2))
                     {
                        vector<int> pol;
                        for(int p=0;p<len(pool);p++) if(pool[p]->alive&&pool[p]->location==l) pol.push_back(p);

                        if(len(pol))
                        {
                           if(clearformess) erase();
                           else makedelimiter();
                           set_color(COLOR_WHITE,COLOR_BLACK,1);
                           move(8,1);
                           int targ=pickrandom(pol);
                           if((int)LCSrandom(100)>pool[targ]->juice)
                           {
                              addstr(pool[targ]->name, gamelog);
                              addstr(" was killed in the bombing!", gamelog);
                              gamelog.newline();

                              if(pool[targ]->align == Alignment::LIBERAL) stat_dead++,liberalcount[l]--;

                              removesquadinfo(*pool[targ]);

                              pool[targ]->die();
                           }
                           else
                           {
                              addstr(pool[targ]->name, gamelog);
                              addstr(" narrowly avoided death!", gamelog);
                              gamelog.newline();
                           }

                           getkey();
                        }
                     }
                     else
                     {
                        if(clearformess) erase();
                        else makedelimiter();
                        move(8,1);
                        addstr("Fortunately, no one is hurt.", gamelog);
                        gamelog.newline();

                        getkey();
                     }
                  }
               }

               if((location[l]->compound_walls & COMPOUND_TANKTRAPS) &&
                  location[l]->siege.escalationstate>=3 && !LCSrandom(15))
               {
                  no_bad=0;

                  //ENGINEERS
                  if(clearformess) erase();
                  else makedelimiter();
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr("Army engineers have removed your tank traps.", gamelog);
                  gamelog.newline();

                  getkey();

                  if(clearformess) move(9,1);
                  else
                  {
                     makedelimiter();
                     move(8,1);
                  }
                  addstr("The tank moves forward to your compound entrance.", gamelog);
                  gamelog.newline();

                  getkey();

                  location[l]->compound_walls&=~COMPOUND_TANKTRAPS;
               }

               //NEED GOOD THINGS TO BALANCE THE BAD

               // ELITE REPORTER SNEAKS IN
               if(!LCSrandom(20)&&no_bad&&liberalcount[l]>0)
               {
                  char repname[200];
                  generate_name(repname);

                  set_color(COLOR_WHITE,COLOR_BLACK,0);

                  erase();
                  move(1,1);
                  addstr("Elitist ", gamelog);
                  addstr(repname, gamelog);
                  addstr(" from the ", gamelog);
                  switch(LCSrandom(5))
                  {
                     case 0:addstr("news program", gamelog);break;
                     case 1:addstr("news magazine", gamelog);break;
                     case 2:addstr("website", gamelog);break;
                     case 3:addstr("scandal rag", gamelog);break;
                     case 4:addstr("newspaper", gamelog);break;
                  }
                  addstr(" ");
                  switch(LCSrandom(12))
                  {
                     case 0:addstr("Daily", gamelog);break;
                     case 1:addstr("Nightly", gamelog);break;
                     case 2:addstr("Current", gamelog);break;
                     case 3:addstr("Pressing", gamelog);break;
                     case 4:addstr("Socialist", gamelog);break;
                     case 5:addstr("American", gamelog);break;
                     case 6:addstr("National", gamelog);break;
                     case 7:addstr("Union", gamelog);break;
                     case 8:addstr("Foreign", gamelog);break;
                     case 9:addstr("Associated", gamelog);break;
                     case 10:addstr("International", gamelog);break;
                     case 11:addstr("County", gamelog);break;
                  }
                  addstr(" ", gamelog);
                  switch(LCSrandom(11))
                  {
                     case 0:addstr("Reporter", gamelog);break;
                     case 1:addstr("Issue", gamelog);break;
                     case 2:addstr("Take", gamelog);break;
                     case 3:addstr("Constitution", gamelog);break;
                     case 4:addstr("Times", gamelog);break;
                     case 5:addstr("Post", gamelog);break;
                     case 6:addstr("News", gamelog);break;
                     case 7:addstr("Affair", gamelog);break;
                     case 8:addstr("Statesman", gamelog);break;
                     case 9:addstr("Star", gamelog);break;
                     case 10:addstr("Inquirer", gamelog);break;
                  }
                  move(2,1);
                  addstr("got into the compound somehow!", gamelog);
                  gamelog.newline();

                  getkey();

                  int best=0;
                  for(int p=0,bestvalue=-1000;p<len(pool);p++)
                  {
                     if (!pool[p]->alive || pool[p]->align != Alignment::LIBERAL || pool[p]->location != l) continue;

                     int sum=pool[p]->get_attribute(ATTRIBUTE_INTELLIGENCE, true)
                            +pool[p]->get_attribute(ATTRIBUTE_HEART, true)
                            +pool[p]->get_skill(SKILL_PERSUASION)
                            +pool[p]->juice;

                     if(sum>bestvalue) best=p,bestvalue=sum;
                  }

                  move(4,1);
                  addstr(pool[best]->name, gamelog);
                  addstr(" decides to give an interview.", gamelog);
                  gamelog.newline();

                  getkey();

                  move(6,1);
                  addstr("The interview is wide-ranging, covering a variety of topics.", gamelog);
                  gamelog.newline();

                  getkey();

                  int segmentpower=pool[best]->attribute_roll(ATTRIBUTE_INTELLIGENCE)
                                  +pool[best]->attribute_roll(ATTRIBUTE_HEART)
                                  +pool[best]->skill_roll(SKILL_PERSUASION)
                                  +pool[best]->skill_roll(SKILL_PERSUASION)
                                  +pool[best]->skill_roll(SKILL_PERSUASION);

                  move(8,1);
                  if(segmentpower<15)
                  {
                     addstr(repname, gamelog);
                     addstr(" canceled the interview halfway through", gamelog);
                     move(9,1);
                     addstr("and later used the material for a Broadway play called", gamelog);
                     move(10,1);
                     switch(LCSrandom(11))
                     {
                        case 0: addstr("Flaming", gamelog); break;
                        case 1: if (law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)
                                  addstr("Dumb", gamelog);
                                else
                                  addstr("Retarded", gamelog); break;
                        case 2: addstr("Insane", gamelog); break;
                        case 3: addstr("Crazy", gamelog); break;
                        case 4: addstr("Loopy", gamelog); break;
                        case 5: addstr("Idiot", gamelog); break;
                        case 6: addstr("Empty-Headed", gamelog); break;
                        case 7: addstr("Nutty", gamelog); break;
                        case 8: addstr("Half-Baked", gamelog); break;
                        case 9: addstr("Pot-Smoking", gamelog); break;
                        case 10: addstr("Stoner", gamelog); break;
                     }
                     addstr(" ", gamelog);
                     switch(LCSrandom(10))
                     {
                        case 0: addstr("Liberal", gamelog); break;
                        case 1: addstr("Socialist", gamelog); break;
                        case 2: addstr("Anarchist", gamelog); break;
                        case 3: addstr("Communist", gamelog); break;
                        case 4: addstr("Marxist", gamelog); break;
                        case 5: addstr("Green", gamelog); break;
                        case 6: addstr("Elite", gamelog); break;
                        case 7: addstr("Guerrilla", gamelog); break;
                        case 8: addstr("Commando", gamelog); break;
                        case 9: addstr("Soldier", gamelog); break;
                     }
                     addstr(".", gamelog);
                     gamelog.newline();
                  }
                  else if(segmentpower<20)
                  {
                     addstr("But the interview is so boring that ", gamelog);
                     addstr(repname, gamelog);
                     addstr(" falls asleep.", gamelog);
                     gamelog.newline();
                  }
                  else if(segmentpower<25)
                  {
                     addstr("But ", gamelog);
                     addstr(pool[best]->name, gamelog);
                     addstr(" stutters nervously the whole time.", gamelog);
                     gamelog.newline();
                  }
                  else if(segmentpower<30)
                  {
                     addstr(pool[best]->name, gamelog);
                     addstr("'s verbal finesse leaves something to be desired.", gamelog);
                     gamelog.newline();
                  }
                  else if(segmentpower<45)
                  {
                     addstr(pool[best]->name, gamelog);
                     addstr(" represents the LCS well.", gamelog);
                     gamelog.newline();
                  }
                  else if(segmentpower<60)
                  {
                     addstr("The discussion was exciting and dynamic.", gamelog);
                     move(9,1);
                     addstr("Even the Cable News and AM Radio spend days talking about it.", gamelog);
                     gamelog.newline();
                  }
                  else
                  {
                     addstr(repname);
                     addstr(" later went on to win a Pulitzer for it.", gamelog);
                     move(9,1);
                     addstr("Virtually everyone in America was moved by ", gamelog);
                     addstr(pool[best]->name, gamelog);
                     addstr("'s words.", gamelog);
                     gamelog.newline();
                  }

                  getkey();

                  //CHECK PUBLIC OPINION
                  change_public_opinion(VIEW_LIBERALCRIMESQUAD,20);
                  change_public_opinion(VIEW_LIBERALCRIMESQUADPOS,(segmentpower-25)/2,segmentpower+50);
                  for(int v=0;v<5;v++) change_public_opinion(LCSrandom(VIEWNUM-3),(segmentpower-25)/2);
               }
            }
            gamelog.newline(); // single blank line after every siege day
         } // end if(!location[l]->siege.underattack)
      } // end for(l=0;l<len(location);l++) if(location[l]->siege.siege)
}



/* siege - handles giving up */
void giveup()
{
   int loc=-1;
   if(selectedsiege!=-1)loc=selectedsiege;
   if(activesquad!=NULL)loc=activesquad->squad[0]->location;
   if(loc==-1)return;

   if(location[loc]->renting>1)location[loc]->renting=RENTING_NOCONTROL;

   //IF POLICE, END SIEGE
   if(location[loc]->siege.siegetype==SIEGE_POLICE ||
      location[loc]->siege.siegetype==SIEGE_FIREMEN)
   {
      music.play(MUSIC_SIEGE);
      int polsta=find_police_station(loc);

      //END SIEGE
      erase();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(1,1);
      if(location[loc]->siege.siegetype==SIEGE_POLICE && location[loc]->siege.escalationstate == 0)
         addstr("The police", gamelog);
      else if(location[loc]->siege.siegetype==SIEGE_POLICE && location[loc]->siege.escalationstate >= 1)
         addstr("The soldiers", gamelog);
      else addstr("The firemen", gamelog);
      addstr(" confiscate everything, including Squad weapons.", gamelog);
      gamelog.newline();

      int kcount=0,pcount=0,icount=0,p;
      char kname[100],pname[100],pcname[100];
      for(p=len(pool)-1;p>=0;p--)
      {
         if(pool[p]->location!=loc||!pool[p]->alive) continue;

         if(pool[p]->flag&CREATUREFLAG_ILLEGALALIEN) icount++;

         if(pool[p]->flag&CREATUREFLAG_MISSING&&pool[p]->align == Alignment::CONSERVATIVE)
         {
            kcount++;
            strcpy(kname,pool[p]->propername);
            if(pool[p]->type==CREATURE_RADIOPERSONALITY) offended_amradio=1;
            if(pool[p]->type==CREATURE_NEWSANCHOR) offended_cablenews=1;
            //clear interrogation data if deleted
            delete pool[p]->activity.intr();
         }
      }

      //CRIMINALIZE POOL IF FOUND WITH KIDNAP VICTIM OR ALIEN
      if(kcount) criminalizepool(LAWFLAG_KIDNAPPING,-1,loc);
      if(icount) criminalizepool(LAWFLAG_HIREILLEGAL,-1,loc);

      if(location[loc]->siege.siegetype==SIEGE_FIREMEN&&location[loc]->compound_walls&COMPOUND_PRINTINGPRESS)
         criminalizepool(LAWFLAG_SPEECH,-1,loc); // Criminalize pool for unacceptable speech

      //LOOK FOR PRISONERS (MUST BE AFTER CRIMINALIZATION ABOVE)
      for(p=len(pool)-1;p>=0;p--)
      {
         if(pool[p]->location!=loc||!pool[p]->alive) continue;

         if(iscriminal(*pool[p])&&!(pool[p]->flag&CREATUREFLAG_MISSING&&pool[p]->align == Alignment::CONSERVATIVE))
         {
            pcount++;
            strcpy(pname,pool[p]->propername);
            strcpy(pcname,pool[p]->name);
         }
      }

      if(kcount==1)
      {
         move(3,1);
         addstr(kname);
         addstr(" is rehabilitated and freed.", gamelog);
         gamelog.newline();
      }
      if(kcount>1)
      {
         move(3,1);
         addstr("The kidnap victims are rehabilitated and freed.", gamelog);
         gamelog.newline();
      }
      if(pcount==1)
      {
         move(5,1);
         addstr(pname, gamelog);
         if(strcmp(pname,pcname))
         {
            addstr(", aka ", gamelog);
            addstr(pcname, gamelog);
            addstr(",", gamelog);
         }
         move(6,1);
         addstr("is taken to the police station.", gamelog);
         gamelog.newline();
      }
      if(pcount>1)
      {
         move(5,1);
         addstr(pcount, gamelog);
         addstr(" Liberals are taken to the police station.", gamelog);
         gamelog.newline();
      }
      if(ledger.get_funds()>0)
      {
         if(ledger.get_funds()<=2000 || location[loc]->siege.siegetype==SIEGE_FIREMEN)
         {
            move(8,1);
            addstr("Fortunately, your funds remain intact.", gamelog);
            gamelog.newline();
         }
         else
         {
            move(8,1);
            int confiscated = LCSrandom(LCSrandom(ledger.get_funds()-2000)+1)+1000;
            if(ledger.get_funds()-confiscated > 50000)
               confiscated += ledger.get_funds() - 30000 - LCSrandom(20000) - confiscated;
            addstr_fl(gamelog,"Law enforcement has confiscated $%d in LCS funds.",confiscated);
            gamelog.newline();
            ledger.subtract_funds(confiscated,EXPENSE_CONFISCATED);
         }
      }
      if(location[loc]->siege.siegetype==SIEGE_FIREMEN)
      {
         if(location[loc]->compound_walls & COMPOUND_PRINTINGPRESS)
         {
            move(10,1);
            addstr("The printing press is dismantled and burned.", gamelog);
            gamelog.newline();
            location[loc]->compound_walls &= ~COMPOUND_PRINTINGPRESS;
         }
      }
      else
      {
         if(location[loc]->compound_walls)
         {
            move(10,1);
            addstr("The compound is dismantled.", gamelog);
            gamelog.newline();
            location[loc]->compound_walls=0;
         }
      }
      if(location[loc]->front_business!=-1)
      {
         move(12,1);
         addstr("Materials relating to the business front have been taken.", gamelog);
         gamelog.newline();
         location[loc]->front_business=-1;
      }

      getkey();

      if(location[loc]->siege.siegetype==SIEGE_FIREMEN)
         offended_firemen=0; // Firemen do not hold grudges

      for(p=len(pool)-1;p>=0;p--)
      {
         if(pool[p]->location!=loc) continue;

         //ALL KIDNAP VICTIMS FREED REGARDLESS OF CRIMES
         if((pool[p]->flag & CREATUREFLAG_MISSING)||
            !pool[p]->alive)
         {
            // Clear actions for anybody who was tending to this person
            for(int i=0;i<len(pool);i++)
               if(pool[i]->alive&&pool[i]->activity.type==ACTIVITY_HOSTAGETENDING&&pool[i]->activity.arg==pool[p]->id)
                  pool[i]->activity.type=ACTIVITY_NONE;

            removesquadinfo(*pool[p]);
            delete_and_remove(pool,p);
            continue;
         }

         //TAKE SQUAD EQUIPMENT
         if(pool[p]->squadid!=-1)
         {
            int sq=getsquad(pool[p]->squadid);
            if(sq!=-1)delete_and_clear(squad[sq]->loot);
         }

         pool[p]->drop_weapons_and_clips(NULL);

         if(iscriminal(*pool[p]))
         {
            removesquadinfo(*pool[p]);
            pool[p]->location=polsta;
            pool[p]->activity.type=ACTIVITY_NONE;
         }
      }

      location[loc]->siege.siege=0;
   }
   else
   {
      //OTHERWISE IT IS SUICIDE
      int killnumber=0;
      for(int p=len(pool)-1;p>=0;p--)
      {
         if(pool[p]->location!=loc) continue;

         if(pool[p]->alive&&pool[p]->align == Alignment::LIBERAL) stat_dead++;

         killnumber++;
         removesquadinfo(*pool[p]);
         pool[p]->die();
         pool[p]->location=-1;
      }

      if(location[loc]->siege.siegetype==SIEGE_CCS&&location[loc]->type==SITE_INDUSTRY_WAREHOUSE)
         location[loc]->renting=RENTING_CCS; // CCS Captures warehouse

      erase();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(1,1);
      addstr("Everyone in the ", gamelog);
      addstr(location[loc]->getname(), gamelog);
      addstr(" is slain.", gamelog);
      gamelog.newline();
      if(!endcheck(-2)) music.play(MUSIC_SIEGE); // play correct music for if we lost the game or didn't lose it

      getkey();

      newsstoryst *ns=new newsstoryst;
      ns->type=NEWSSTORY_MASSACRE;
      ns->loc=loc;
      ns->crime.push_back(location[loc]->siege.siegetype);
      ns->crime.push_back(killnumber);
      newsstory.push_back(ns);

      //MUST SET cursite TO SATISFY endcheck() CODE
      int tmp=cursite;
      cursite=loc;
      endcheck();
      cursite=tmp;

      location[loc]->siege.siege=0;
   }

   //CONFISCATE MATERIAL
   delete_and_clear(location[loc]->loot);
   for(int v=len(vehicle)-1;v>=0;v--)
      if(vehicle[v]->get_location()==loc)
         delete_and_remove(vehicle,v);

   gamelog.newline();

}



/* siege - checks how many days of food left at the site */
int fooddaysleft(int loc)
{
   int eaters=numbereating(loc);
   if(eaters==0) return -1;
   return location[loc]->compound_stores/eaters+((location[loc]->compound_stores%eaters)>eaters/2);
}



/* siege - checks how many people are eating at the site */
int numbereating(int loc)
{
   int eaters=0;
   for(int p=0;p<len(pool);p++) //Must be here, alive, Liberal, and not a sleeper, to count as an eater
      if(pool[p]->location==loc&&pool[p]->alive&&pool[p]->align == Alignment::LIBERAL&&!(pool[p]->flag&CREATUREFLAG_SLEEPER)) eaters++;
   return eaters;
}

// Siege -- Mass combat outside safehouse
char sally_forth_aux(int loc)
{
   int p;
   reloadparty();
   siegest siege = location[loc]->siege;
   cursite = loc;

   for(int e=0;e<ENCMAX;e++) encounter[e].exists=0;

   switch(siege.siegetype)
   {
   case SIEGE_CIA:
   case SIEGE_HICKS:
   case SIEGE_CORPORATE:
   case SIEGE_CCS:
   case SIEGE_FIREMEN:
   default:
       //todo: decide here regarding the comment below
      // So yeah, WTF is the reason there isn't a break statement here???
      // If everything ends up in the SIEGE_POLICE case we shouldn't even have a switch statement here at all.
   case SIEGE_POLICE: // Currently only police sieges should allow this
      // SWAT teams
      if(siege.escalationstate==0)
         for(int e=0;e<ENCMAX-9;e++)
            makecreature(encounter[e],CREATURE_SWAT);
      // Military
      else if(siege.escalationstate>=1)
         for(int e=0;e<ENCMAX-9;e++)
            makecreature(encounter[e],CREATURE_SOLDIER);
      // M1 Tank
      if(siege.escalationstate>=2 && !(location[loc]->compound_walls & COMPOUND_TANKTRAPS))
         makecreature(encounter[ENCMAX-9],CREATURE_TANK);
      break;
   }

   mode=GAMEMODE_CHASEFOOT;
   music.play(MUSIC_DEFENSE);
   bool ranaway = false;

   while(true)
   {
      // Count heroes
      int partysize=0,partyalive=0;
      for(p=0;p<len(pool);p++) if(pool[p]->align == Alignment::LIBERAL&&pool[p]->location==cursite&&!(pool[p]->flag&CREATUREFLAG_SLEEPER))
      {
         partysize++;
         if(pool[p]->alive) partyalive++;
      }

      // Count bad guys
      int encsize=0;
      for(int e=0;e<ENCMAX;e++) if(encounter[e].exists) encsize++;

      // Let's roll
       auto_promote_available_liberals_into_squad(loc);
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(0,0);
      addstr(location[loc]->getname());

      // Player's party
      if(partyalive==0) party_status=-1;
      printparty();

      if(partyalive>0)
      {
         // Options
         if(partysize>1)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(9,40);
         addstr("O - Change the squad's Liberal order");
         if (partysize > 0 && (party_status == -1 || partysize > 1))
           set_color(COLOR_WHITE,COLOR_BLACK,0);
         else
           set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(10,40);
         addstr("# - Check the status of a squad Liberal");
         if(party_status!=-1)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(11,40);
         addstr("0 - Show the squad's Liberal status");

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(9,1);
         addstr("D - Escape");
         move(10,1);
         addstr("E - Equip");
         move(11,1);
         addstr("F - Fight!");
         move(12,1);
         addstr("G - Surrender");
      }
      else
      {
         endcheck(-2); // play the right music in case we're dead
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(9,1);
         addstr("C - Reflect on your Conservative judgment.");
         cleanup_squads_saved_for_restoration_after_siege();
      }

      // Enemies
      printencounter();

      // check if we fought the previous loop; if so, add a blank gamelog line
      if(foughtthisround) gamelog.newline();
      foughtthisround=0;

      int c=getkey();

      // Reflecting on your poor judgment
      if(partyalive==0&&c=='c'&&!endcheck())
      {
         mode=GAMEMODE_BASE;
         return 0;
      }

      // Providing orders
      if(partyalive>0)
      {
         // Reorder
         if(c=='o'&&partysize>1) orderparty();

         // View status
         if(c=='0') party_status=-1;

         // Character info
         if(c>='1'&&c<='6') if(activesquad->squad[c-'1']!=NULL)
         {
            if(party_status==c-'1')fullstatus(party_status);
            else party_status=c-'1';
         }

         // Surrender
         if(c=='g') giveup();

         // Run away
         if(c=='d')
         {
            if(encounter[0].exists&&encounter[0].type==CREATURE_COP)
            {
               sitestory->crime.push_back(CRIME_FOOTCHASE);
               criminalizeparty(LAWFLAG_RESIST);
            }
            evasiverun();
            enemyattack();
            creatureadvance();
            ranaway = true;
         }

         if(c=='f')
         {
            youattack();
            enemyattack();
            creatureadvance();
         }

         if(c=='e') equip(location[loc]->loot,-1);

         // Check for victory
         partysize=0,partyalive=0;
         for(p=0;p<len(pool);p++) if(pool[p]->align == Alignment::LIBERAL&&pool[p]->location==cursite&&!(pool[p]->flag&CREATUREFLAG_SLEEPER))
         {
            partysize++;
            if(pool[p]->alive) partyalive++;
         }

         int baddiecount=0;
         for(int e=0;e<ENCMAX;e++) if(encounter[e].enemy()&&encounter[e].alive&&encounter[e].exists) baddiecount++;

         if(partyalive&&!baddiecount)
         {
            for(int p=0;p<len(pool);p++) for(int w=0;w<BODYPARTNUM;w++) pool[p]->wound[w]&=~WOUND_BLEEDING;
            mode=GAMEMODE_BASE;
            if(ranaway)
            {
               music.play(MUSIC_CONQUER);
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               clearmessagearea();
               move(16,1);
               addstr("You're free!", gamelog);
               gamelog.nextMessage();

               getkey();

               escapesiege(false);
               return 1;
            }
            else
            {
               music.play(MUSIC_CONQUER);
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               clearmessagearea();
               move(16,1);
               addstr("The siege is broken!", gamelog);
               gamelog.nextMessage();

               getkey();

               conquertext();
               escapesiege(true);
               return 2;
            }
         }
      }
   }
   mode=GAMEMODE_BASE;
   return 1;
}

/* siege - prepares for exiting the siege to fight the attackers head on */
void sally_forth()
{  //GIVE INFO SCREEN
   music.play(MUSIC_DEFENSE);
   erase();
   set_color(COLOR_RED,COLOR_BLACK,1);
   move(1,26);
   addstr("UNDER SIEGE: ESCAPE OR ENGAGE");

   set_color(COLOR_WHITE,COLOR_BLACK,0);
   move(3,16);
   addstr("You are about to exit the compound to lift the Conservative");
   move(4,11);
   addstr("siege on your safehouse.  The enemy is ready for you, and");
   move(5,11);
   addstr("you will have to defeat them all or run away to survive this");
   move(6,11);
   addstr("encounter.");
   move(8,11);
   addstr("Your Squad has filled out to six members if any were ");
   move(9,11);
   addstr("available.  If you have a larger pool of Liberals, they");
   move(10,11);
   addstr("will provide cover fire from the compound until needed.");

   int loc=-1;
   if(selectedsiege!=-1)loc=selectedsiege;
   if(activesquad!=NULL)loc=activesquad->squad[0]->location;
   if(loc == -1)return;

   set_color(COLOR_RED,COLOR_BLACK,1);
   move(23,11);
   addstr("Press any key to Confront the Conservative Aggressors");

   // Seperate logging text
   gamelog.log("Your Liberals sally forth to confront the siege.");

   getkey();

   if(location[loc]->siege.siegetype==SIEGE_CCS&&location[loc]->type==SITE_INDUSTRY_WAREHOUSE)
      location[loc]->renting=RENTING_CCS; // CCS Captures warehouse -- this will be reversed if you fight them off

   //CRIMINALIZE
   if(location[loc]->siege.siegetype==SIEGE_POLICE) criminalizepool(LAWFLAG_RESIST,-1,loc);

   //DELETE ALL SQUADS IN THIS AREA UNLESS THEY ARE THE activesquad
   for(int sq=len(squad)-1;sq>=0;sq--)
      if(squad[sq]!=activesquad&&squad[sq]->squad[0])
         if(squad[sq]->squad[0]->location==loc)
         {
            if(activesquad)
            {
               for(int p=0;p<6;p++)
               {
                  if(!squad[sq]->squad[p]) continue;
                  squad[sq]->squad[p]->squadid=-1;
               }
               delete_and_remove(squad,sq);
            }
            else activesquad=squad[sq];
         }

   // No squads at the location? Form a new one.
   if(!activesquad)
   {
      squad.push_back(new squadst);
      squad.back()->id=cursquadid++;
      strcpy(squad.back()->name,location[selectedsiege]->getname(true));
      strcat(squad.back()->name," Defense");
      int i=0;
      for(int p=0;p<len(pool);p++)
         if(pool[p]->location==selectedsiege&&pool[p]->alive&&pool[p]->align == Alignment::LIBERAL)
         {
            squad.back()->squad[i]=pool[p];
            pool[p]->squadid=squad.back()->id;
            if(++i>=6) break;
         }
      activesquad = squad.back();
   }

   //MAKE SURE PARTY IS ORGANIZED
    auto_promote_available_liberals_into_squad(loc);

   //START FIGHTING
   newsstoryst *ns=new newsstoryst;
   ns->type=NEWSSTORY_SQUAD_ESCAPED;
   ns->positive=1;
   ns->loc=loc;
   ns->siegetype=location[loc]->siege.siegetype;
   newsstory.push_back(ns);
   sitestory = ns;
   char result = sally_forth_aux(loc);
   if(result==2) ns->type=NEWSSTORY_SQUAD_BROKESIEGE;
   // If you fail, make sure the safehouse isn't under siege anymore by
   // forcing you to "give up".
   if(result==0)
   {
      gamelog.log("You have been defeated.");
      resolvesafehouses();
   }
}

void show_siege_info_screen(int loc){
    erase();
    set_color(COLOR_RED,COLOR_BLACK,1);
    move(1,26);
    addstr("UNDER ATTACK: ESCAPE OR ENGAGE");

    set_color(COLOR_WHITE,COLOR_BLACK,0);
    move(3,16);
    addstr("You are about to engage Conservative forces in battle.");
    move(4,11);
    addstr("You will find yourself in the Liberal safehouse, and it will");
    move(5,11);
    addstr("be swarming with Conservative units.  The Liberal Crime");
    move(6,11);
    addstr("Squad will be located far from the entrance to the safehouse.");
    move(7,11);
    addstr("It is your task to bring your squad out to safety, or fight");
    move(8,11);
    addstr("off the Conservatives within the perimeter.  Either way you");
    move(9,11);
    addstr("choose, any equipment from the safehouse which isn't held by a");
    move(10,11);
    addstr("Liberal will be scattered about the compound.  Save what");
    move(11,11);
    addstr("you can.  You might notice your Squad has filled out to");
    move(12,11);
    addstr("six members if any were available.  If you have a larger pool");
    move(13,11);
    addstr("of Liberals, they will be traveling behind the Squad.");
    move(14,11);
    addstr("There is a new button, (R)eorganize, which reflects this.");
    move(15,11);
    addstr("Squad members in the back with firearms can provide cover");
    move(16,11);
    addstr("fire.  If you have at least six people total, then six must");
    move(17,11);
    addstr("be in the Squad.  If less than six, then they all must.");

    if(location[loc]->compound_walls&COMPOUND_CAMERAS)
    {
        move(18,16);
        addstr("Your security cameras let you see units on the (M)ap.");
    }
    if(location[loc]->compound_walls&COMPOUND_TRAPS)
    {
        move(19,16);
        addstr("Your traps will harass the enemy, but not the Squad.");
    }

    set_color(COLOR_RED,COLOR_BLACK,1);
    move(23,11);
    addstr("Press any key to Confront the Conservative Aggressors");

    // Seperate logging text
    gamelog.log("Your Liberals confront the Conservatives within the safehouse.");

    getkey();
}

vector<squadst *> squads_saved_for_restoration_after_siege;

void cleanup_squads_saved_for_restoration_after_siege() {
    for(auto saved_squad : squads_saved_for_restoration_after_siege){
        delete saved_squad;
    }
    squads_saved_for_restoration_after_siege.clear();
}

/* siege - prepares for entering site mode to fight the siege */
void escape_engage()
{
   music.play(MUSIC_DEFENSE);

    int loc=-1;
    if(selectedsiege!=-1) loc=selectedsiege;
    if(activesquad!=NULL) loc=activesquad->squad[0]->location;
    if(loc == -1) return;

   show_siege_info_screen(loc);

   if(location[loc]->siege.siegetype==SIEGE_CCS&&location[loc]->type==SITE_INDUSTRY_WAREHOUSE)
      location[loc]->renting=RENTING_CCS; // CCS Captures warehouse -- this will be reversed if you fight them off

   //CRIMINALIZE
   if(location[loc]->siege.siegetype==SIEGE_POLICE) criminalizepool(LAWFLAG_RESIST,-1,loc);

    //DELETE ALL SQUADS IN THIS AREA UNLESS THEY ARE THE activesquad
    squads_saved_for_restoration_after_siege.clear();
    for (int squad_index = len(squad) - 1; squad_index >= 0; squad_index--) {
        if (squad[squad_index] != activesquad &&
            squad[squad_index]->squad[0] &&
            squad[squad_index]->squad[0]->location == loc) {
            if (activesquad) {
                for (int squaddie_index = 0; squaddie_index < 6; squaddie_index++)
                {
                    if (squad[squad_index]->squad[squaddie_index] != nullptr) {
                        squad[squad_index]->squad[squaddie_index]->squadid = -1;
                    }
                }
                squads_saved_for_restoration_after_siege.insert(squads_saved_for_restoration_after_siege.begin(), squad[squad_index]);
                squad.erase(squad.begin() + squad_index);
            } else activesquad = squad[squad_index];
        }
    }

   // No squads at the location? Form a new one.
   if(!activesquad)
   {
      squad.push_back(new squadst);
      squad.back()->id=cursquadid++;
      strcpy(squad.back()->name,location[selectedsiege]->getname(true));
      strcat(squad.back()->name," Defense");
      int i=0;
      for(int p=0;p<len(pool);p++) {
          if (pool[p]->location == selectedsiege &&
                pool[p]->alive &&
                pool[p]->align == Alignment::LIBERAL)
          {
              squad.back()->squad[i] = pool[p];
              pool[p]->squadid = squad.back()->id;
              if (++i >= 6) break;
          }
      }
      activesquad = squad.back();
   }

   //MAKE SURE PARTY IS ORGANIZED
    auto_promote_available_liberals_into_squad(loc);

   //START FIGHTING
   newsstoryst *ns=new newsstoryst;
   if(location[loc]->siege.underattack) ns->type=NEWSSTORY_SQUAD_FLEDATTACK;
   else ns->type=NEWSSTORY_SQUAD_ESCAPED;
   ns->positive=1;
   ns->loc=loc;
   ns->siegetype=location[loc]->siege.siegetype;
   newsstory.push_back(ns);
   mode_site(loc);
}

void restore_squads_after_siege() {
    for(auto saved_squad : squads_saved_for_restoration_after_siege){
        auto at_least_one_alive = false;
        for(auto squaddie : saved_squad->squad){
            if(squaddie && squaddie->alive){
                if(squaddie->squadid == activesquad->id){
                    activesquad->remove_from_squad(squaddie);
                }
                squaddie->squadid = saved_squad->id;
                at_least_one_alive = true;
            }
        }
        if (at_least_one_alive) {
            squad.push_back(saved_squad);
        }
        else
        {
            delete saved_squad;
        }
    }
    squads_saved_for_restoration_after_siege.clear();
}

/* siege - what happens when you escaped the siege */
void escapesiege(char won)
{
   //TEXT IF DIDN'T WIN
   if(!won)
   {
      music.play(MUSIC_CONQUER);
      //GIVE INFO SCREEN
      erase();
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(1,32);
      addstr("You have escaped!", gamelog);
      gamelog.nextMessage();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(3,16);
      addstr("The Conservatives thought that the Liberal Crime Squad was");
      move(4,11);
      addstr("finished, but once again, Conservative Thinking has proven");
      move(5,11);
      addstr("itself to be based on Unsound Notions.");
      move(6,16);
      addstr("However, all is not well.  In your haste to escape you have");
      move(7,11);
      addstr("lost everything that you've left behind.  You'll have");
      move(8,11);
      addstr("to start from scratch in a new safe house.  Your");
      move(9,11);
      addstr("funds remain under your control, fortunately.  Your flight has");
      move(10,11);
      addstr("given you some time to regroup, but the Conservatives will");
      move(11,11);
      addstr("doubtless be preparing another assault.");

      int homes=-1;
      if(activesquad)
         if(activesquad->squad[0]!=NULL)
            homes=find_homeless_shelter(*activesquad->squad[0]);

      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(13,11);
      addstr("Press any key to split up and lay low for a few days");

      // Seperate logging text
      gamelog.log("Your Liberals split up and lay low for a few days.");

      getkey();

      //dump retrieved loot in homeless shelter - is there anywhere better to put it?
      if(activesquad&&homes!=-1) location[homes]->getloot(activesquad->loot);

      activesquad=NULL; //active squad cannot be disbanded in removesquadinfo,
                        //but we need to disband current squad as the people are going to be 'away'.

      //GET RID OF DEAD, etc.
      if(location[cursite]->renting>1)location[cursite]->renting=RENTING_NOCONTROL;

      for(int p=len(pool)-1;p>=0;p--)
      {
         if(pool[p]->location!=cursite) continue;
         if(!pool[p]->alive)
         {
            delete_and_remove(pool,p);
            continue;
         }

         //BASE EVERYONE LEFT AT HOMELESS SHELTER
         removesquadinfo(*pool[p]);
         pool[p]->hiding=(short)LCSrandom(3)+2;
         if(pool[p]->align == Alignment::LIBERAL) // not a hostage
            pool[p]->location=-1;
         else // hostages don't go into hiding, just shove em into the homeless shelter
            pool[p]->location=homes;
         pool[p]->base=homes;
      }
      delete_and_clear(location[cursite]->loot);

      for(int v=len(vehicle)-1;v>=0;v--)
         if(vehicle[v]->get_location()==cursite)
            delete_and_remove(vehicle,v);

      location[cursite]->compound_walls=0;
      location[cursite]->compound_stores=0;
      location[cursite]->front_business=-1;
      initlocation(*location[cursite]);
   }
   else {
       restore_squads_after_siege();
   }

    cleanup_squads_saved_for_restoration_after_siege();

   //SET UP NEW SIEGE CHARACTERISTICS, INCLUDING TIMING
   location[cursite]->siege.siege=0;
   if(won&&location[cursite]->siege.siegetype==SIEGE_POLICE)
   {
      location[cursite]->siege.timeuntillocated=(short)LCSrandom(4)+4;
      location[cursite]->siege.escalationstate++;
      if(police_heat<4) police_heat++;
   }
}



/* siege - flavor text when you fought off the raid */
void conquertext()
{
   //GIVE INFO SCREEN
   music.play(MUSIC_CONQUER);
   erase();
   set_color(COLOR_GREEN,COLOR_BLACK,1);
   move(1,26);
   addstr("* * * * *   VICTORY   * * * * *", gamelog);
   gamelog.newline();

   if(location[cursite]->siege.siegetype==SIEGE_POLICE)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(3,16);
      addstr("The Conservative automatons have been driven back �� for ", gamelog);
      move(4,11);
      addstr("the time being.  While they are regrouping, you might consider ", gamelog);
      move(5,11);
      addstr("abandoning this safe house for a safer location.", gamelog);
   }
   else
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(3,16);
      addstr("The Conservative automatons have been driven back.  ", gamelog);
      move(4,11);
      addstr("Unfortunately, you will never truly be safe from ", gamelog);
      move(5,11);
      addstr("this filth until the Liberal Agenda is realized.", gamelog);
   }
   gamelog.nextMessage();

   move(7,19);
   addstr("Press C to Continue Liberally.");

   while(getkey()!='c');
}



/* siege - flavor text when you crush a CCS safe house */
void conquertextccs()
{
   music.play(MUSIC_CONQUER);
   //GIVE INFO SCREEN
   erase();
   set_color(COLOR_GREEN,COLOR_BLACK,1);
   move(1,26);
   addstr("* * * * *   VICTORY   * * * * *", gamelog);
   gamelog.newline();

   if(ccs_kills<3)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(3,16);
      if(ccs_siege_kills>10)
      {
         addstr("Gunfire still ringing in their ears, the squad revels in ", gamelog);
         move(4,11);
         addstr("their victory.  ", gamelog);
      }
      else
      {
         addstr("The CCS Lieutenant lying dead at their feet, the squad ", gamelog);
         move(4,11);
         addstr("slips away.  ");
      }
      addstr("The CCS Founder wasn't here, but for now, their ", gamelog);
      move(5,11);
      addstr("power has been severely weakened.  Once the safehouse cools off, ", gamelog);
      move(6,11);
      addstr("this will make a fine base for future Liberal operations.", gamelog);
   }
   else
   {
      move(3,16);
      if(ccs_siege_kills>10)
      {
         addstr("Gunfire still ringing in their ears, the squad revels in ", gamelog);
         move(4,11);
         addstr("their final victory.  ", gamelog);

         move(6,16);
         addstr("As your Liberals pick through the remains of the safehouse, ", gamelog);
         move(7,11);
         addstr("it is increasingly clear that this was the CCS's last safehouse.", gamelog);
      }
      else
      {
         addstr("The CCS Founder lying dead at their feet, the squad ", gamelog);
         move(4,11);
         addstr("slips away.  ", gamelog);

         move(6,16);
         addstr("With its Founder killed in the heart of their own base, ", gamelog);
         move(7,11);
         addstr("the last of the enemy's morale and confidence is shattered.", gamelog);
      }
      gamelog.newline();

      move(9,16);
      addstr("The CCS has been completely destroyed.  Now wasn't there a ", gamelog);
      move(10,16);
      addstr("revolution to attend to?", gamelog);
      gamelog.newline();

      move(12,5);
      addstr("+200 JUICE TO EVERYONE FOR ERADICATING THE CONSERVATIVE CRIME SQUAD", gamelog);

      for(int p=0;p<len(pool);p++) addjuice(*pool[p],200,1000);
   }
   gamelog.nextMessage();

   move(15,19);
   addstr("Press C to Continue Liberally.");

   while(getkey()!='c');
}



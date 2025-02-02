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
#include "externs.h"

#include "items/money.h"
#include "locations/world.h"


bool goodguyattack = false;

/* attack handling for each side as a whole */
void youattack()
{
   foughtthisround=1;

   short wasalarm=sitealarm;

   goodguyattack = true;

   for(int p=0;p<6;p++)
   {
      if(activesquad->squad[p]==NULL) continue;
      if(!activesquad->squad[p]->alive) continue;

      vector<int> super_enemies;
      vector<int> dangerous_enemies;
      vector<int> enemies;
      vector<int> non_enemies;

      for(int e=0;e<ENCMAX;e++)
      {
         if(encounter[e].alive&&encounter[e].exists)
         {
            if(encounter[e].enemy())
            {
               if(encounter[e].animalgloss==ANIMALGLOSS_TANK&&
                  encounter[e].stunned==0)
                  super_enemies.push_back(e);
               else if((encounter[e].is_armed()||
                       (encounter[e].type==CREATURE_COP&&encounter[e].align==Alignment::MODERATE)||
                        encounter[e].type==CREATURE_SCIENTIST_EMINENT||
                        encounter[e].type==CREATURE_JUDGE_LIBERAL||
                        encounter[e].type==CREATURE_JUDGE_CONSERVATIVE||
                        encounter[e].type==CREATURE_CORPORATE_CEO||
                        encounter[e].type==CREATURE_POLITICIAN||
                        encounter[e].type==CREATURE_RADIOPERSONALITY||
                        encounter[e].type==CREATURE_NEWSANCHOR||
                        encounter[e].type==CREATURE_MILITARYOFFICER||
                        encounter[e].specialattack!=-1)&&
                        encounter[e].blood>=40&&
                        encounter[e].stunned==0)
                  dangerous_enemies.push_back(e);
               else enemies.push_back(e);
            }
            else non_enemies.push_back(e);
         }
      }

      if(!(len(super_enemies)+len(dangerous_enemies)+len(enemies))) return;

      int target;
      // If there are "super enemies", shoot at one of them unless we're using a persuasion-based attack
      if(len(super_enemies)&&
       ((activesquad->squad[p]->type!=CREATURE_SCIENTIST_EMINENT&&
         activesquad->squad[p]->type!=CREATURE_JUDGE_LIBERAL&&
         activesquad->squad[p]->type!=CREATURE_JUDGE_CONSERVATIVE&&
         activesquad->squad[p]->type!=CREATURE_CORPORATE_CEO&&
         activesquad->squad[p]->type!=CREATURE_POLITICIAN&&
         activesquad->squad[p]->type!=CREATURE_RADIOPERSONALITY&&
         activesquad->squad[p]->type!=CREATURE_NEWSANCHOR&&
         activesquad->squad[p]->type!=CREATURE_MILITARYOFFICER&&
        !activesquad->squad[p]->get_weapon().has_musical_attack())||
       (!activesquad->squad[p]->get_weapon().has_musical_attack()&&
         activesquad->squad[p]->is_armed())))
         target=pickrandom(super_enemies);
      // Else, if there are "dangerous enemies", shoot at one of them
      else if(len(dangerous_enemies))
         target=pickrandom(dangerous_enemies);
      // Else, if there are regular enemies, shoot at one of them
      else if(len(enemies)) target=pickrandom(enemies);
      // Else, we skipped a "super enemy" because it we're using a persuasion-based attack but it's the only enemy left so we have to pick it
      else target=pickrandom(super_enemies);

      char mistake=0;
      // Changed from 1% chance to being based on weapon skill
      if(len(non_enemies)&&activesquad->squad[p]->get_weapon_skill()<8&&
         !LCSrandom(10+10*activesquad->squad[p]->get_weapon_skill()))
      {
         target=pickrandom(non_enemies);
         mistake=1;
      }

      char actual;
      short beforeblood=encounter[target].blood;
      if(encounter[target].align == Alignment::LIBERAL) mistake=1;
      attack(*activesquad->squad[p],encounter[target],mistake,actual);

      if(actual)
      {
         if(mistake)
         {
            alienationcheck(mistake);
            sitestory->crime.push_back(CRIME_ATTACKED_MISTAKE);
            sitecrime+=10;
         }
         else
         {
            sitecrime+=3;
            addjuice(*(activesquad->squad[p]),1,200);
         }
         sitestory->crime.push_back(CRIME_ATTACKED);
         // Charge with assault if first strike
         if(sitealarm && (!wasalarm||(beforeblood>encounter[target].blood && beforeblood==100)))
         {
            if(!activesquad->squad[p]->is_armed())
               criminalize(*activesquad->squad[p],LAWFLAG_ASSAULT);
            else criminalize(*activesquad->squad[p],LAWFLAG_ARMEDASSAULT);
         }
      }

      if(!encounter[target].alive)
      {
         delenc(target,1);
         if(!mistake)
         {
            for(int p=0;p<6;p++)
            {
               if(activesquad->squad[p]==NULL) continue;
               if(!activesquad->squad[p]->alive) continue;

               addjuice(*(activesquad->squad[p]),5,500);
            }
         }
      }
   }

   for(int e=0;e<ENCMAX;e++)
   {
      if(encounter[e].exists && encounter[e].alive && encounter[e].enemy())
      {
         sitealarm=1;
         break;
      }
   }

   //COVER FIRE
   if(location[cursite]->siege.siege)
   {
      for(int p=0;p<len(pool);p++)
      {
         if (!pool[p]->alive) continue;
         if (pool[p]->align != Alignment::LIBERAL) continue;
         if (pool[p]->squadid != -1) continue;
         if (pool[p]->location != cursite) continue;

         // Juice check to engage in cover fire
         // 10% chance for every 10 juice, starting at
         // 10% chance for 0 juice -- caps out at 100%
         // chance to fire at 90 juice
         //if(LCSrandom(10)-pool[p]->juice/10>0)continue;

         if(pool[p]->get_weapon().get_attack(true,false,false))
         {
            char conf=0;
            if(pool[p]->get_weapon().get_ammoamount()>0) conf=1;
            if(pool[p]->get_weapon().get_attack(true,false,false)->uses_ammo)
               if(pool[p]->can_reload()) conf=1;

            if(conf)
            {
               vector<int> goodtarg,badtarg;

               for(int e=0;e<ENCMAX;e++)
               {
                  if(encounter[e].alive&&encounter[e].exists)
                  {
                     if(encounter[e].enemy())goodtarg.push_back(e);
                     else badtarg.push_back(e);
                  }
               }

               if(!len(goodtarg)) return;

               int target=pickrandom(goodtarg);

               char mistake=0;

               if(len(badtarg)&&!LCSrandom(10))
               {
                  target=pickrandom(badtarg);
                  mistake=1;
               }

               char actual;
               attack(*pool[p],encounter[target],mistake,actual);

               if(actual)
               {
                  if(mistake)
                  {
                     alienationcheck(mistake);
                     sitestory->crime.push_back(CRIME_ATTACKED_MISTAKE);
                     sitecrime+=10;
                  }

                  if(!pool[p]->is_armed())
                     criminalize(*pool[p],LAWFLAG_ASSAULT);
                  else criminalize(*pool[p],LAWFLAG_ARMEDASSAULT);
               }

               if(!encounter[target].alive) delenc(target,1);
            }
         }
      }
   }
}



void enemyattack()
{
   static const char *escape_crawling[] =
   {
      " crawls off moaning...",
      " crawls off whimpering...",
      " crawls off trailing blood...",
      " crawls off screaming...",
      " crawls off crying...",
      " crawls off sobbing...",
      " crawls off whispering...",
      " crawls off praying...",
      " crawls off cursing..."
   };

   static const char *escape_running[] =
   {
      " makes a break for it!",
      " escapes crying!",
      " runs away!",
      " gets out of there!",
      " runs hollering!",
      " bolts out of there!",
      " runs away screaming!",
   };


   foughtthisround=1;

   goodguyattack = false;

   bool armed=false;
   for(int i=0;i<6;i++)
   {
      if(activesquad->squad[i]==NULL) break;
      if(activesquad->squad[i]->is_armed()) armed=true;
   }

#ifdef NOENEMYATTACK
   return;
#endif

   int e2, e;
   char printed;
   for(e=0;e<ENCMAX;e++)
   {
      if(!encounter[e].exists) continue;
      if(!encounter[e].alive) continue;

      if(sitealarm==1&&encounter[e].type==CREATURE_BOUNCER&&encounter[e].align!=Alignment::LIBERAL)
         conservatise(encounter[e]);
      if(encounter[e].enemy()) encounter[e].cantbluff=2;

      if(mode!=GAMEMODE_CHASECAR)
      {
         // Encountered creature will flee if:
         // (a) Non-Conservative, and not recently converted via music or some other mechanism
         // (b) Conservative, no juice, unarmed, non-tank/animal, enemy is armed, and fails a morale check based in part on injury level
         // (c) Conservative, and lost more than 55% blood
         // (d) There's a fire, they are not firefighters, and they fail a random check
         // Encountered creatures will never flee if they are tanks, animals, or so hurt they can't move
         char fire=0;
         if(mode==GAMEMODE_SITE)
         {
            if(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_START ||
               levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_END)
               fire=1;
            else if(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_PEAK)
               fire=2;
         }

         if(((!encounter[e].enemy()||
            (encounter[e].juice==0&&!encounter[e].is_armed()&&armed&&encounter[e].blood<signed(70+LCSrandom(61))))
            &&!(encounter[e].flag & CREATUREFLAG_CONVERTED))||(encounter[e].blood<45&&encounter[e].juice<200)
            ||((fire*LCSrandom(5)>=3)&&!(encounter[e].type==CREATURE_FIREFIGHTER)))
         {
            if(encounter[e].animalgloss==ANIMALGLOSS_NONE)
            {
               if(!incapacitated(encounter[e],0,printed))
               {
                  if(printed)
                  {
                     printparty();
                     if(mode==GAMEMODE_CHASECAR||
                        mode==GAMEMODE_CHASEFOOT) printchaseencounter();
                     else printencounter();

                     getkey();
                  }

                  clearmessagearea();

                  move(16,1);
                  addstr(encounter[e].name, gamelog);
                  if((encounter[e].wound[BODYPART_LEG_RIGHT] & WOUND_NASTYOFF)||
                     (encounter[e].wound[BODYPART_LEG_RIGHT] & WOUND_CLEANOFF)||
                     (encounter[e].wound[BODYPART_LEG_LEFT] & WOUND_NASTYOFF)||
                     (encounter[e].wound[BODYPART_LEG_LEFT] & WOUND_CLEANOFF)||
                     (encounter[e].blood<45))
                     addstr(pickrandom(escape_crawling), gamelog);
                  else addstr(pickrandom(escape_running), gamelog);

                  gamelog.newline();

                  delenc(e,0);
                  e--;

                  printparty();
                  if(mode==GAMEMODE_CHASECAR||
                     mode==GAMEMODE_CHASEFOOT) printchaseencounter();
                  else printencounter();

                  getkey();
               }

               continue;
            }
         }
      }

      vector<int> goodtarg,badtarg;

      if(encounter[e].enemy())
      {
         for(int p=0;p<6;p++) if(activesquad->squad[p]!=NULL)
            if(activesquad->squad[p]->alive) goodtarg.push_back(p);
      }
      else
      {
         for(e2=0;e2<ENCMAX;e2++)
         {
            if (!encounter[e2].exists) continue;
            if (!encounter[e2].alive) continue;
            if (encounter[e2].align != Alignment::CONSERVATIVE) continue;

            goodtarg.push_back(e2);
         }
      }

      for(e2=0;e2<ENCMAX;e2++)
      {
         if(!encounter[e2].exists) continue;
         if(!encounter[e2].alive) continue;
         if(encounter[e2].enemy()) continue;

         badtarg.push_back(e2);
      }

      if(!len(goodtarg)) return;

      int target=pickrandom(goodtarg);

      char canmistake=1;

      int encnum=0;
      for(e2=0;e2<ENCMAX;e2++) if(encounter[e2].exists) encnum++;

      if((encounter[e].type==CREATURE_SCIENTIST_EMINENT||
          encounter[e].type==CREATURE_JUDGE_LIBERAL||
          encounter[e].type==CREATURE_JUDGE_CONSERVATIVE||
          encounter[e].type==CREATURE_CORPORATE_CEO||
          encounter[e].type==CREATURE_POLITICIAN||
          encounter[e].type==CREATURE_RADIOPERSONALITY||
          encounter[e].type==CREATURE_NEWSANCHOR||
          encounter[e].type==CREATURE_MILITARYOFFICER)&&encnum<ENCMAX) canmistake=0;

      char actual;
      if(canmistake)
      {
         if(encounter[e].enemy())
         {
            if(activesquad->squad[target]->prisoner!=NULL && !LCSrandom(2))
            {
               attack(encounter[e],*activesquad->squad[target]->prisoner,1,actual);
               if(!activesquad->squad[target]->prisoner->alive)
               {
                  if(activesquad->squad[target]->prisoner->squadid==-1)
                  {
                     clearmessagearea();
                     set_color(COLOR_WHITE,COLOR_BLACK,1);
                     move(16,1);
                     addstr(activesquad->squad[target]->name, gamelog);
                     addstr(" drops ", gamelog);
                     addstr(activesquad->squad[target]->prisoner->name, gamelog);
                     addstr("'s body.", gamelog);
                     gamelog.newline();

                     int prisonerType = activesquad->squad[target]->prisoner->type;

                     if(prisonerType==CREATURE_CORPORATE_CEO||
                        prisonerType==CREATURE_POLITICIAN||
                        prisonerType==CREATURE_RADIOPERSONALITY||
                        prisonerType==CREATURE_NEWSANCHOR||
                        prisonerType==CREATURE_SCIENTIST_EMINENT||
                        prisonerType==CREATURE_JUDGE_CONSERVATIVE||
                        prisonerType==CREATURE_MILITARYOFFICER) sitecrime+=30;

                     makeloot(*activesquad->squad[target]->prisoner,groundloot);

                     getkey();

                     delete_and_nullify(activesquad->squad[target]->prisoner);
                  }
               }
               continue;
            }
         }

         if(!LCSrandom(10)&&len(badtarg))
         {
            target=pickrandom(badtarg);
            if(encounter[target].flag & CREATUREFLAG_CONVERTED)
               attack(encounter[e],encounter[target],0,actual);
            else attack(encounter[e],encounter[target],1,actual);
            if(!encounter[target].alive) delenc(target,1);
            continue;
         }
      }

      if(encounter[e].enemy())
         attack(encounter[e],*activesquad->squad[target],0,actual);
      else attack(encounter[e],encounter[target],0,actual);
   }
}



/* attack handling for an individual creature and its target */
void attack(Creature &a,Creature &t,char mistake,char &actual,bool force_melee)
{
   actual=0;

   char str[200];

   clearmessagearea(true);  // erase the whole length and redraw map if applicable, since previous combat messages can be wider than 53 chars.
   if(goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
   else set_color(COLOR_RED,COLOR_BLACK,1);

   //INCAPACITATED
   char incaprint;
   a.forceinc=0;
   if(incapacitated(a,0,incaprint))
   {
      if(incaprint)
      {
         printparty();
         if(mode==GAMEMODE_CHASECAR||
            mode==GAMEMODE_CHASEFOOT) printchaseencounter();
         else printencounter();

         getkey();
      }

      a.forceinc=1;

      return;
   }

   //SPECIAL ATTACK!
   int encnum=0;
   for(int e=0;e<ENCMAX;e++) if(encounter[e].exists) encnum++;

   if(!force_melee &&
    (((a.type==CREATURE_COP&&a.align==Alignment::MODERATE&&a.enemy())||
       a.type==CREATURE_SCIENTIST_EMINENT||
       a.type==CREATURE_JUDGE_LIBERAL||
       a.type==CREATURE_JUDGE_CONSERVATIVE||
      (a.type==CREATURE_CORPORATE_CEO&&LCSrandom(2))||
       a.type==CREATURE_POLITICIAN||
       a.type==CREATURE_RADIOPERSONALITY||
       a.type==CREATURE_NEWSANCHOR||
       a.type==CREATURE_MILITARYOFFICER||
       a.get_weapon().has_musical_attack()) &&
      (a.get_weapon().has_musical_attack() || !a.is_armed() || a.align != Alignment::LIBERAL)))
   {
      if(a.align == Alignment::LIBERAL||encnum<ENCMAX)
      {
         specialattack(a, t, actual);
         return;
      }
   }

   //RELOAD
   if((a.will_reload(mode==GAMEMODE_CHASECAR,force_melee)
    ||(a.has_thrown_weapon && len(a.extra_throwing_weapons)))
    &&!force_melee)
   {
      if(a.will_reload(mode==GAMEMODE_CHASECAR,force_melee))
      {
         a.reload(false);
         strcpy(str,a.name);
         strcat(str," reloads.");
      }
      else if(a.has_thrown_weapon && len(a.extra_throwing_weapons))
      {
         a.ready_another_throwing_weapon();
         strcpy(str,a.name);
         strcat(str," readies another ");
         strcat(str,a.get_weapon().get_name());
         strcat(str,".");
      }
      move(16,1);
      addstr(str, gamelog);
      gamelog.newline();

      printparty();
      if(mode==GAMEMODE_CHASECAR
       ||mode==GAMEMODE_CHASEFOOT) printchaseencounter();
      else printencounter();

      getkey();

      return;
   }
   else if(a.has_thrown_weapon) a.has_thrown_weapon = false;

   const attackst* attack_used = NULL;
   attack_used = a.get_weapon().get_attack(mode==GAMEMODE_CHASECAR,          //Force ranged if in a car.
                                           force_melee,
                                          (force_melee || !a.can_reload())); //No reload if force melee or unable to reload.

   if(attack_used==NULL) //No attack possible.
      return;            //Then bail, they can't fight

   bool melee=true;
   if(attack_used->ranged) melee=false;
   bool sneak_attack=false;

   strcpy(str,a.name);
   strcat(str," ");
   if(mistake) strcat(str,"MISTAKENLY ");
   if(!a.is_armed())
   {
      if(!a.animalgloss) //Move into WEAPON_NONE -XML
      {
         if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)+1))
            strcat(str,"punches");
         else if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)))
            strcat(str,"swings at");
         else if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)-1))
            strcat(str,"grapples with");
         else if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)-2))
            strcat(str,"kicks");
         else if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)-3))
            strcat(str,"strikes at");
         else if(!LCSrandom(a.get_skill(SKILL_HANDTOHAND)-4))
            strcat(str,"jump kicks");
         else strcat(str,"gracefully strikes at");
      }
      else
      {
         if(a.specialattack==ATTACK_CANNON)
         {
            strcat(str,"fires a 120mm shell at");
            melee=false;
         }
         else if(a.specialattack==ATTACK_FLAME) strcat(str,"breathes fire at");
         else if(a.specialattack==ATTACK_SUCK) strcat(str,"stabs");
         else strcat(str,"bites");
      }
   }
   else
   {
      if(attack_used->can_backstab && a.align==Alignment::LIBERAL && !mistake)
      {
         if(t.cantbluff<1 && sitealarm<1)
         {
            sneak_attack = true;
            strcat(str,"sneaks up on");
            if(sitealarmtimer>10 || sitealarmtimer<0) sitealarmtimer=10;
            t.cantbluff = 2;
         }
      }

      if(!sneak_attack)
      {
         strcat(str,attack_used->attack_description);
         sitealarm=1;
      }
   }

   strcat(str," ");
   strcat(str,t.name);
   move(16,1);
   addstr(str, gamelog);

   strcpy(str,"");

   if(a.is_armed() && !attack_used->thrown)
   {
      strcat(str," with a ");
      strcat(str,a.get_weapon().get_name(1));
   }
   strcat(str,"!");
   addstr(str, gamelog);
   gamelog.newline();

   getkey();

   if(goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
   else set_color(COLOR_RED,COLOR_BLACK,1);
   strcpy(str, a.heshe(true)); // capitalize=true. Shorten the string so it doesn't spill over as much; we already said attacker's name on the previous line anyways.

   int bonus=0; // Accuracy bonus or penalty that does NOT affect damage or counterattack chance

   //SKILL EFFECTS
   int wsk=attack_used->skill;

   Creature* driver = getChaseDriver(t);
   Vehicle* vehicle = getChaseVehicle(t);
   Creature* adriver = getChaseDriver(a);
   Vehicle* avehicle = getChaseVehicle(a);
   
   // Basic roll
   int aroll=a.skill_roll(wsk);
   // In a car chase, the driver provides the defence roll instead of the victim.
   int droll= 0;
   if (mode!=GAMEMODE_CHASECAR)
   {
      droll = t.skill_roll(SKILL_DODGE)/2;
   }
   else
   {
      if (driver!=NULL && vehicle!=NULL)
      {  // without a vehicle or driver, you get a zero roll.
         droll = driver->skill_roll(PSEUDOSKILL_DODGEDRIVE);
      }
      if (adriver!=NULL && avehicle!=NULL)
      {
         bonus += avehicle->attackbonus(adriver->id == a.id);  // Attack bonus depends on attacker's car and whether attacker is distracted by driving.
      }
      else // shouldn't happen
      {
         bonus -= 10; // You're on the wrong side of a drive-by shooting?!
      }
   }
   
   if(sneak_attack)
   {
      droll=t.attribute_roll(ATTRIBUTE_WISDOM)/2;
      aroll+=a.skill_roll(SKILL_STEALTH);
      a.train(wsk,10);
   }
   else
   {
      if (driver!=NULL)
         driver->train(SKILL_DRIVING, aroll/2);
      else
         t.train(SKILL_DODGE,aroll*2);
      a.train(wsk,droll*2+5);
   }

   // Hostages interfere with attack
   if(t.prisoner!=NULL) bonus-=LCSrandom(10);
   if(a.prisoner!=NULL) aroll-=LCSrandom(10);

   //Injured people suck at attacking, are like fish in a barrel to attackers
   if(mode==GAMEMODE_CHASEFOOT)
   {
      // If in a foot chase, double the debilitating effect of injuries
      healthmodroll(aroll,a);
      healthmodroll(droll,t);
      healthmodroll(droll,t);
   }
   else if (mode==GAMEMODE_CHASECAR)
   {
      // In a car chase, the driver is applying dodge rolls even for crippled people.
      healthmodroll(aroll,a);
      if (driver != NULL)
      {// if there is no driver, we already rolled a zero, so don't worry about further penalties.
         healthmodroll(droll,*driver);
      }
   }
   else
   {  
      // Any other case (site fight) normal penalties.
      healthmodroll(aroll,a);
      healthmodroll(droll,t);
   }

   // Prevent negative rolls
   if(aroll<0) aroll=0;
   if(droll<0) droll=0;

   // Weapon accuracy bonuses and penalties
   bonus+=attack_used->accuracy_bonus;

   //USE BULLETS
   int bursthits=0; // Tracks number of hits.

   int thrownweapons=0; // Used by thrown weapons to remove the weapons at the end of the turn if needed

   if(!a.is_armed()) //Move into WEAPON_NONE -XML
   {
      // Martial arts multi-strikes
      bursthits=1+LCSrandom(a.get_skill(SKILL_HANDTOHAND)/3+1);
      if(bursthits>5) bursthits=5;
      if(a.animalgloss) bursthits=1; // Whoops, must be human to use martial arts fanciness
   }
   else
   {
      if(mode==GAMEMODE_SITE && LCSrandom(100) < attack_used->fire.chance_causes_debris)
      {// TODO - In a car chase, debris should make driving harder for one round, or require a drive skill check to avoid damage
         sitechangest change(locx,locy,locz,SITEBLOCK_DEBRIS);
         location[cursite]->changes.push_back(change);
      }
      if(mode==GAMEMODE_SITE && LCSrandom(100) < attack_used->fire.chance)
      {// TODO - In a car chase, apply vehicle damage, with drive skill check to partially mitigate
         // Fire!
         if(!(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_END) ||
            !(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_PEAK) ||
            !(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_START) ||
            !(levelmap[locx][locy][locz].flag & SITEBLOCK_DEBRIS))
         {
            levelmap[locx][locy][locz].flag|=SITEBLOCK_FIRE_START;
            sitecrime+=3;
            addjuice(a,5,500);
            criminalizeparty(LAWFLAG_ARSON);
            sitestory->crime.push_back(CRIME_ARSON);
         }
      }

      for(int i=0;i<attack_used->number_attacks;i++)
      {
         if(attack_used->uses_ammo)
         {
            if(a.get_weapon().get_ammoamount() > 0)
               a.get_weapon().decrease_ammo(1);
            else break;
         }
         else if(attack_used->thrown)
         {
            if(a.count_weapons()-thrownweapons > 0)
               thrownweapons++;
            else break;
         }

         if(sneak_attack)
         {
            bursthits=1; // Backstab only hits once
            break;
         }
         // Each shot in a burst is increasingly less likely to hit
         if(aroll+bonus-i*attack_used->successive_attacks_difficulty > droll)
            bursthits++;
      }
   }

   //HIT!
   if(aroll+bonus>droll)
   {
      if(sneak_attack) strcat(str, " stabs ");
      else strcat(str," hits ");
      strcat(str,t.name);
      strcat(str, "'s ");
      int w;
      bool canhit=false;

      for(w=0;w<BODYPARTNUM;w++)
      {
         if(!(t.wound[w]&WOUND_CLEANOFF) && !(t.wound[w]&WOUND_NASTYOFF))
         {
            canhit=true;
            break;
         }
      }

      do
      {
         int offset=0;
         if(aroll>droll+5)
            offset=4;  // NICE SHOT; MORE LIKELY TO HIT BODY/HEAD
         if(aroll>droll+10 &&
            (!(t.wound[BODYPART_HEAD]&(WOUND_CLEANOFF|WOUND_NASTYOFF)) ||
             !(t.wound[BODYPART_BODY]&(WOUND_CLEANOFF|WOUND_NASTYOFF))))
            offset=8;  // NO LIMB HITS HERE YOU AWESOME PERSON
         if(sneak_attack)
            offset=10; // Backstab! 2/3 body, 1/3 head
         if(aroll>droll+15 &&
            !(t.wound[BODYPART_HEAD]&(WOUND_CLEANOFF|WOUND_NASTYOFF)))
            offset=12; // BOOM AUTOMATIC HEADSHOT MOTHA******
         //Weighted location roll:
         //200% chance to hit body
         //50% chance to hit head
         w=offset+LCSrandom(13-offset);

         switch(w)
         {
         case 12:w=BODYPART_HEAD; break;
         case 11:
         case 10:
         case 9:
         case 8:w=BODYPART_BODY; break;
         case 7:
         case 3:w=BODYPART_ARM_RIGHT; break;
         case 6:
         case 2:w=BODYPART_ARM_LEFT; break;
         case 5:
         case 1:w=BODYPART_LEG_RIGHT; break;
         case 4:
         case 0:w=BODYPART_LEG_LEFT; break;
         }
      } while(((t.wound[w]&WOUND_CLEANOFF) || (t.wound[w]&WOUND_NASTYOFF)) && canhit==true);

      if(t.animalgloss==ANIMALGLOSS_TANK)
         switch(w)
         {
         case BODYPART_HEAD:strcat(str,"turret"); break;
         case BODYPART_BODY:strcat(str,"front"); break;
         case BODYPART_ARM_RIGHT:strcat(str,"right side"); break;
         case BODYPART_ARM_LEFT:strcat(str,"left side"); break;
         case BODYPART_LEG_RIGHT:strcat(str,"right tread"); break;
         case BODYPART_LEG_LEFT:strcat(str,"left tread"); break;
         }
      else if(t.animalgloss==ANIMALGLOSS_ANIMAL) // FIXME: What about Six-legged Pigs?
         switch(w)
         {
         case BODYPART_HEAD:strcat(str,"head"); break;
         case BODYPART_BODY:strcat(str,"body"); break;
         case BODYPART_ARM_RIGHT:strcat(str,"right front leg"); break;
         case BODYPART_ARM_LEFT:strcat(str,"left front leg"); break;
         case BODYPART_LEG_RIGHT:strcat(str,"right rear leg"); break;
         case BODYPART_LEG_LEFT:strcat(str,"left rear leg"); break;
         }
      else
         switch(w)
         {
         case BODYPART_HEAD:strcat(str,"head"); break;
         case BODYPART_BODY:strcat(str,"body"); break;
         case BODYPART_ARM_RIGHT:strcat(str,"right arm"); break;
         case BODYPART_ARM_LEFT:strcat(str,"left arm"); break;
         case BODYPART_LEG_RIGHT:strcat(str,"right leg"); break;
         case BODYPART_LEG_LEFT:strcat(str,"left leg");break;
         }

      // show multiple hits
      if(bursthits>1 && a.is_armed()) // Only show if not melee
      {
         strcat(str,", ");
         if(!a.is_armed()) //Move into WEAPON_NONE? -XML
            strcat(str,"striking");
         else strcat(str,attack_used->hit_description);

         switch(bursthits)
         {
         case 1: break;
         case 2: strcat(str," twice"); break;
         case 3: strcat(str," three times"); break;
         case 4: strcat(str," four times"); break;
         case 5: strcat(str," five times"); break;
         default: strcat(str," "+std::to_string(bursthits)+" times"); break;
         }
      }
      else if(attack_used->always_describe_hit)
      {
         strcat(str,", ");
         strcat(str,attack_used->hit_description);
      }

      char damtype=0;
      int damamount=0;
      char strengthmin=1;
      char strengthmax=1;
      int severtype=-1;

      char damagearmor=0;
      char armorpiercing=0;
      int extraarmor=0;

      if (!a.is_armed())
      {
         strengthmin=5;
         strengthmax=10;
         while(bursthits) //Put into WEAPON_NONE -XML
         {
            damamount+=LCSrandom(5+a.get_skill(SKILL_HANDTOHAND))+1+a.get_skill(SKILL_HANDTOHAND);
            bursthits--;
         }
         if(!a.animalgloss) damtype|=WOUND_BRUISED;
         else
         {
            if(a.specialattack==ATTACK_CANNON)
            {
               damamount=LCSrandom(5000)+5000;
               armorpiercing=20;
               damtype|=WOUND_BURNED;
               damtype|=WOUND_TORN;
               damtype|=WOUND_SHOT;
               damtype|=WOUND_BLEEDING;
               strengthmin=0;
               strengthmax=0;
            }
            else if(a.specialattack==ATTACK_FLAME) damtype|=WOUND_BURNED;
            else if(a.specialattack==ATTACK_SUCK) damtype|=WOUND_CUT;
            else damtype|=WOUND_TORN;
            severtype=WOUND_NASTYOFF;
         }
      }
      else
      {
         if(attack_used->bruises) damtype|=WOUND_BRUISED;
         if(attack_used->cuts) damtype|=WOUND_CUT;
         if(attack_used->burns) damtype|=WOUND_BURNED;
         if(attack_used->tears) damtype|=WOUND_TORN;
         if(attack_used->shoots) damtype|=WOUND_SHOT;
         if(attack_used->bleeding) damtype|=WOUND_BLEEDING;

         strengthmin=attack_used->strength_min;
         strengthmax=attack_used->strength_max;
         severtype=attack_used->severtype;
         int random=attack_used->random_damage;
         int fixed=attack_used->fixed_damage;
         if(sneak_attack) fixed+=100;
         if(bursthits >= attack_used->critical.hits_required
            && LCSrandom(100) < attack_used->critical.chance)
         {
            if(attack_used->critical.random_damage_defined)
               random=attack_used->critical.random_damage;
            if(attack_used->critical.fixed_damage_defined)
               fixed=attack_used->critical.fixed_damage;
            if(attack_used->critical.severtype_defined)
               severtype=attack_used->critical.severtype;
         }
         while(bursthits > 0)
         {
            damamount+=LCSrandom(random)+fixed;
            bursthits--;
         }
         damagearmor=attack_used->damages_armor;
         armorpiercing=attack_used->armorpiercing;
      }

      // Coarse combat lethality reduction.
      //damamount/=2;

      if(t.squadid!=-1&&t.hireid==-1) // Plot Armor: if the founder is hit, inflict
         damamount/=2;                // 1/2 damage, because founders are cool

      int mod=0;

      if(strengthmax>strengthmin)
      {
         // Melee attacks: Maximum strength bonus, minimum
         // strength to deliver full damage
         int strength=a.attribute_roll(ATTRIBUTE_STRENGTH);
         if(strength>strengthmax) strength=(strengthmax+strength)/2;
         mod+=strength-strengthmin;
         armorpiercing+=(strength-strengthmin)/4;
      }

      //SKILL BONUS FOR GOOD ROLL
      mod+=aroll-droll;

      //DO THE HEALTH MOD ON THE WOUND
      mod-=t.attribute_roll(ATTRIBUTE_HEALTH);

      //Health and poor accuracy will only avoid critical hits, not stop low-damage attacks
      if(mod<0) mod=0;
      
      // In a car chase, the vehicle itself provides bonus armor
      int vehicleHitLocation = 0;
      if (mode==GAMEMODE_CHASECAR && vehicle != NULL)
      {
         vehicleHitLocation = vehicle->gethitlocation(w);
         extraarmor = vehicle->armorbonus(vehicleHitLocation);
         // TODO damage vehicle itself
      }

      int cardmg = damamount;
      damagemod(t,damtype,damamount,w,armorpiercing,mod,extraarmor);
      
      // Report vehicle protection effect
      if (mode==GAMEMODE_CHASECAR && vehicle != NULL && extraarmor > 0)
      {
         strcat(str, " through ");
         // Could the vehicle have bounced that round on its own?
         if (damamount==0)
         {
            Creature testDummy; // Spawn nude test dummy to see if body armor was needed to prevent damage
            damagemod(testDummy,damtype,cardmg,w,armorpiercing,mod,extraarmor);
            
            if (cardmg < 2) //fudge factor of 1 armor level due to randomness
            {
               strcpy(str, "The attack bounces off ");
            }
         }
         
         strcat(str,"the "+vehicle->shortname()+"'s ");
         strcat(str, vehicle->getpartname(vehicleHitLocation));
      }
      
      // Temporary debug output for the damage roll
      #ifdef SHOWMECHANICS
      {
         char str[200];
         clearmessagearea();
         move(16,1);
         addstr_f("(ATK %d, DEF %d, DAMMOD %d, DAMAGE %d, AP %d)",aroll,droll,mod,damamount,armorpiercing);

         getkey();
      }
      #endif

      // Bullets caught by armor should bruise instead of poke holes.
      if(damamount<4 && damtype & WOUND_SHOT)
      {
         damtype &= ~(WOUND_SHOT|WOUND_BLEEDING);
         damtype |= WOUND_BRUISED;
      }
      if(damamount>0)
      {
         Creature *target=0;

         if(t.squadid!=-1&&t.hireid==-1&& //if the founder is hit...
           (damamount>t.blood||damamount>=10)&& //and lethal or potentially crippling damage is done...
           (w==BODYPART_HEAD||w==BODYPART_BODY)) //to a critical bodypart...
         {
            //Oh Noes!!!! Find a liberal to jump in front of the bullet!!!
            for(int i=0;i<6;i++)
            {
               if(activesquad->squad[i]==NULL) break;
               if(activesquad->squad[i]==&t) break;
               if(activesquad->squad[i]->get_attribute(ATTRIBUTE_HEART,true)>8&&
                  activesquad->squad[i]->get_attribute(ATTRIBUTE_AGILITY,true)>4)
               {
                  target=activesquad->squad[i];

                  clearmessagearea();
                  set_color(COLOR_GREEN,COLOR_BLACK,1);

                  move(16,1);
                  addstr(target->name, gamelog);
                  if(!t.alive) addstr(" misguidedly", gamelog);
                  else addstr(" heroically", gamelog);
                  addstr(" shields ", gamelog);
                  addstr(t.name, gamelog);
                  if(!t.alive) addstr("'s corpse", gamelog);
                  addstr("!", gamelog);
                  gamelog.newline();

                  addjuice(*target,10,1000);//Instant juice!! Way to take the bullet!!

                  getkey();

                  break;
               }
            }
         }
         if(!target) target=&t;//If nobody jumps in front of the attack,

         target->wound[w]|=damtype;

         int severamount=100;

         switch(w)
         {
         case BODYPART_HEAD:severamount=100;break;
         case BODYPART_BODY:severamount=1000;break;
         case BODYPART_ARM_RIGHT:severamount=200;break;
         case BODYPART_ARM_LEFT:severamount=200;break;
         case BODYPART_LEG_RIGHT:severamount=400;break;
         case BODYPART_LEG_LEFT:severamount=400;break;
         }

         if(severtype!=-1 && damamount>=severamount)
            target->wound[w]|=(char)severtype;

         if(w!=BODYPART_HEAD && w!=BODYPART_BODY && target->blood-damamount<=0 &&
            target->blood>0)
         {
            do
            {
               if(LCSrandom(100) < attack_used->no_damage_reduction_for_limbs_chance)
                  break;
               else damamount>>=1;
            } while(target->blood-damamount<=0);
         }

         if(damagearmor) armordamage(target->get_armor(),w,damamount);

         target->blood-=damamount;

         levelmap[locx][locy][locz].flag|=SITEBLOCK_BLOODY;

         if((target->wound[BODYPART_HEAD] & WOUND_CLEANOFF)||
            (target->wound[BODYPART_BODY] & WOUND_CLEANOFF)||
            (target->wound[BODYPART_HEAD] & WOUND_NASTYOFF)||
            (target->wound[BODYPART_BODY] & WOUND_NASTYOFF)||
            target->blood<=0)
         {
            if((w==BODYPART_HEAD && target->wound[BODYPART_HEAD] & WOUND_NASTYOFF)||
               (w==BODYPART_BODY && target->wound[BODYPART_BODY] & WOUND_NASTYOFF))
               bloodblast(&target->get_armor());

            char alreadydead=!target->alive;

            if(!alreadydead)
            {
               target->die();

#if 0
               // NB work out how to do this
               if(t.align==-a.align)
                  addjuice(a,5+t.juice/20,1000); // Instant juice
               else addjuice(a,-(5+t.juice/20),-50);
#endif

               if(target->squadid!=-1)
               {
                  if(target->align == Alignment::LIBERAL) stat_dead++;
               }
               else if (target->enemy()
                     &&(t.animalgloss != ANIMALGLOSS_ANIMAL || law[LAW_ANIMALRESEARCH] == Alignment::ELITE_LIBERAL))
               {
                  stat_kills++;
                  if(location[cursite]->siege.siege) location[cursite]->siege.kills++;
                  if(location[cursite]->siege.siege && t.animalgloss==ANIMALGLOSS_TANK) location[cursite]->siege.tanks--;
                  if(location[cursite]->renting==RENTING_CCS)
                  {
                     if(target->type==CREATURE_CCS_ARCHCONSERVATIVE) ccs_boss_kills++;
                     ccs_siege_kills++;
                  }
               }
               if (target->squadid == -1
               && (target->animalgloss != ANIMALGLOSS_ANIMAL || law[LAW_ANIMALRESEARCH] == Alignment::ELITE_LIBERAL)
               && !sneak_attack)
               {
                  sitecrime+=10;
                  sitestory->crime.push_back(CRIME_KILLEDSOMEBODY);
                  if(a.squadid!=-1) criminalizeparty(LAWFLAG_MURDER);
               }
            }

            if(w==BODYPART_HEAD && target->wound[BODYPART_HEAD] & WOUND_CLEANOFF)
               strcat(str," CUTTING IT OFF!");
            else if(w==BODYPART_BODY && target->wound[BODYPART_BODY] & WOUND_CLEANOFF)
               strcat(str," CUTTING IT IN HALF!");
            else if(w==BODYPART_HEAD && target->wound[BODYPART_HEAD] & WOUND_NASTYOFF)
               strcat(str," BLOWING IT APART!");
            else if(w==BODYPART_BODY && target->wound[BODYPART_BODY] & WOUND_NASTYOFF)
               strcat(str," BLOWING IT IN HALF!");
            else strcat(str,attack_used->hit_punctuation);
            move(17,1);
            //set_color(COLOR_WHITE,COLOR_BLACK,1);
            if(goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
            else set_color(COLOR_RED,COLOR_BLACK,1);
            addstr(str, gamelog);
            gamelog.newline();

            getkey();

            if(!alreadydead)
            {
               severloot(t,groundloot);
               clearmessagearea();
               adddeathmessage(*target);

               getkey();

               if(target->prisoner!=NULL) freehostage(t,1);
            }
         }
         else
         {
            if(target->wound[w] & WOUND_CLEANOFF) strcat(str," CUTTING IT OFF!");
            else if(target->wound[w] & WOUND_NASTYOFF) strcat(str," BLOWING IT OFF!");
            else strcat(str,attack_used->hit_punctuation);

            if(target->wound[w] & WOUND_NASTYOFF) bloodblast(&target->get_armor());

            if(goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
            else set_color(COLOR_RED,COLOR_BLACK,1);
            move(17,1);
            //set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr(str, gamelog);
            gamelog.newline();

            printparty();
            if(mode==GAMEMODE_CHASECAR||
               mode==GAMEMODE_CHASEFOOT) printchaseencounter();
            else printencounter();

            getkey();

            //SPECIAL WOUNDS
            if(!(target->wound[w] & (WOUND_CLEANOFF|WOUND_NASTYOFF))&&
                !target->animalgloss)
            {
               char heavydam=0,breakdam=0,pokedam=0;
               if(damamount>=12) //JDS -- 2x damage needed
               {
                  if(damtype & WOUND_SHOT) heavydam=1;
                  if(damtype & WOUND_BURNED) heavydam=1;
                  if(damtype & WOUND_TORN) heavydam=1;
                  if(damtype & WOUND_CUT) heavydam=1;
               }

               if(damamount>=10) //JDS -- 2x damage needed
               {
                  if(damtype & WOUND_SHOT) pokedam=1;
                  if(damtype & WOUND_TORN) pokedam=1;
                  if(damtype & WOUND_CUT) pokedam=1;
               }

               if((damtype & WOUND_BRUISED)&&damamount>=50) breakdam=1;
               if((damtype & WOUND_SHOT)&&damamount>=50) breakdam=1;
               if((damtype & WOUND_TORN)&&damamount>=50) breakdam=1;
               if((damtype & WOUND_CUT)&&damamount>=50) breakdam=1;

               if(w==BODYPART_HEAD)
               {
                  clearmessagearea();
                  if(goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
                  else set_color(COLOR_RED,COLOR_BLACK,1);

                  switch(LCSrandom(7))
                  {
                     case 0:
                        if((target->special[SPECIALWOUND_RIGHTEYE]||
                            target->special[SPECIALWOUND_LEFTEYE]||
                            target->special[SPECIALWOUND_NOSE])&&heavydam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s face is blasted off!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("'s face is burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("'s face is torn off!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("'s face is cut away!", gamelog);
                           else addstr("'s face is removed!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_RIGHTEYE]=0;
                           target->special[SPECIALWOUND_LEFTEYE]=0;
                           target->special[SPECIALWOUND_NOSE]=0;
                           if(target->blood>20)target->blood=20;
                        }
                        break;
                     case 1:
                        if(target->special[SPECIALWOUND_TEETH]>0)
                        {
                           int teethminus=LCSrandom(TOOTHNUM)+1;
                           if(teethminus>target->special[SPECIALWOUND_TEETH])
                              teethminus=target->special[SPECIALWOUND_TEETH];

                           move(16,1);
                           if(teethminus>1)
                           {
                              if(teethminus==target->special[SPECIALWOUND_TEETH])
                                 addstr("All ", gamelog);
                              addstr(teethminus, gamelog);
                              addstr(" of ", gamelog);
                              addstr(target->name, gamelog);
                              addstr("'s teeth are ", gamelog);
                           }
                           else if(target->special[SPECIALWOUND_TEETH]>1)
                           {
                              addstr("One of ", gamelog);
                              addstr(target->name, gamelog);
                              addstr("'s teeth is ", gamelog);
                           }
                           else
                           {
                              addstr(target->name, gamelog);
                              addstr("'s last tooth is ", gamelog);
                           }

                           if(damtype & WOUND_SHOT)addstr("shot out!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("gouged out!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("cut out!", gamelog);
                           else addstr("knocked out!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_TEETH]-=teethminus;
                        }
                        break;
                     case 2:
                        if(target->special[SPECIALWOUND_RIGHTEYE]&&heavydam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s right eye is blasted out!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("'s right eye is burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("'s right eye is torn out!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("'s right eye is poked out!", gamelog);
                           else addstr("'s right eye is removed!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_RIGHTEYE]=0;
                           if(target->blood>50)target->blood=50;
                        }
                        break;
                     case 3:
                        if(target->special[SPECIALWOUND_LEFTEYE]&&heavydam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s left eye is blasted out!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("'s left eye is burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("'s left eye is torn out!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("'s left eye is poked out!", gamelog);
                           else addstr("'s left eye is removed!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_LEFTEYE]=0;
                           if(target->blood>50)target->blood=50;
                        }
                        break;
                     case 4:
                        if(target->special[SPECIALWOUND_TONGUE]&&heavydam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s tongue is blasted off!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("'s tongue is burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("'s tongue is torn out!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("'s tongue is cut off!", gamelog);
                           else addstr("'s tongue is removed!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_TONGUE]=0;
                           if(target->blood>50)target->blood=50;
                        }
                        break;
                     case 5:
                        if(target->special[SPECIALWOUND_NOSE]&&heavydam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s nose is blasted off!", gamelog);
                           else if(damtype & WOUND_BURNED)addstr("'s nose is burned away!", gamelog);
                           else if(damtype & WOUND_TORN)addstr("'s nose is torn off!", gamelog);
                           else if(damtype & WOUND_CUT)addstr("'s nose is cut off!", gamelog);
                           else addstr("'s nose is removed!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_NOSE]=0;
                           if(target->blood>50)target->blood=50;
                        }
                        break;
                     case 6:
                        if(target->special[SPECIALWOUND_NECK]&&breakdam)
                        {
                           move(16,1);
                           addstr(target->name, gamelog);
                           if(damtype & WOUND_SHOT)addstr("'s neck bones are shattered!", gamelog);
                           else addstr("'s neck is broken!", gamelog);
                           gamelog.newline();

                           getkey();

                           target->special[SPECIALWOUND_NECK]=0;
                           if(target->blood>20)target->blood=20;
                        }
                        break;
                  }
               }
               if(w==BODYPART_BODY)
               {
                  clearmessagearea();
                  //set_color(COLOR_MAGENTA,COLOR_BLACK,1);
                  if (goodguyattack) set_color(COLOR_GREEN,COLOR_BLACK,1);
                  else set_color(COLOR_RED,COLOR_BLACK,1);

                  switch(LCSrandom(11))
                  {
                  case 0:
                     if(target->special[SPECIALWOUND_UPPERSPINE]&&breakdam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT) addstr("'s upper spine is shattered!", gamelog);
                        else addstr("'s upper spine is broken!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_UPPERSPINE]=0;
                        if(target->blood>20) target->blood=20;
                     }
                     break;
                  case 1:
                     if(target->special[SPECIALWOUND_LOWERSPINE]&&breakdam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s lower spine is shattered!", gamelog);
                        else addstr("'s lower spine is broken!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_LOWERSPINE]=0;
                        if(target->blood>20) target->blood=20;
                     }
                     break;
                  case 2:
                     if(target->special[SPECIALWOUND_RIGHTLUNG]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s right lung is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s right lung is torn!", gamelog);
                        else addstr("'s right lung is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_RIGHTLUNG]=0;
                        if(target->blood>20) target->blood=20;
                     }
                     break;
                  case 3:
                     if(target->special[SPECIALWOUND_LEFTLUNG]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s left lung is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s left lung is torn!", gamelog);
                        else addstr("'s left lung is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_LEFTLUNG]=0;
                        if(target->blood>20) target->blood=20;
                     }
                     break;
                  case 4:
                     if(target->special[SPECIALWOUND_HEART]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s heart is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s heart is torn!", gamelog);
                        else addstr("'s heart is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_HEART]=0;
                        if(target->blood>3) target->blood=3;
                     }
                     break;
                  case 5:
                     if(target->special[SPECIALWOUND_LIVER]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s liver is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s liver is torn!", gamelog);
                        else addstr("'s liver is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_LIVER]=0;
                        if(target->blood>50) target->blood=50;
                     }
                     break;
                  case 6:
                     if(target->special[SPECIALWOUND_STOMACH]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s stomach is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s stomach is torn!", gamelog);
                        else addstr("'s stomach is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_STOMACH]=0;
                        if(target->blood>50) target->blood=50;
                     }
                     break;
                  case 7:
                     if(target->special[SPECIALWOUND_RIGHTKIDNEY]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s right kidney is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s right kidney is torn!", gamelog);
                        else addstr("'s right kidney is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_RIGHTKIDNEY]=0;
                        if(target->blood>50) target->blood=50;
                     }
                     break;
                  case 8:
                     if(target->special[SPECIALWOUND_LEFTKIDNEY]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s left kidney is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s left kidney is torn!", gamelog);
                        else addstr("'s left kidney is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_LEFTKIDNEY]=0;
                        if(target->blood>50) target->blood=50;
                     }
                     break;
                  case 9:
                     if(target->special[SPECIALWOUND_SPLEEN]&&pokedam)
                     {
                        move(16,1);
                        addstr(target->name, gamelog);
                        if(damtype & WOUND_SHOT)addstr("'s spleen is blasted!", gamelog);
                        else if(damtype & WOUND_TORN)addstr("'s spleen is torn!", gamelog);
                        else addstr("'s spleen is punctured!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_SPLEEN]=0;
                        if(target->blood>50) target->blood=50;
                     }
                     break;
                  case 10:
                     if(target->special[SPECIALWOUND_RIBS]>0&&breakdam)
                     {
                        int ribminus=LCSrandom(RIBNUM)+1;
                        if(ribminus>target->special[SPECIALWOUND_RIBS]) ribminus=target->special[SPECIALWOUND_RIBS];

                        move(16,1);
                        if(ribminus>1)
                        {
                           if(ribminus==target->special[SPECIALWOUND_RIBS])
                              addstr("All ", gamelog);
                           addstr(ribminus, gamelog);
                           addstr(" of ", gamelog);
                           addstr(target->name, gamelog);
                           addstr("'s ribs are ", gamelog);
                        }
                        else if(target->special[SPECIALWOUND_RIBS]>1)
                        {
                           addstr("One of ", gamelog);
                           addstr(target->name, gamelog);
                           addstr("'s rib is ", gamelog);
                        }
                        else
                        {
                           addstr(target->name, gamelog);
                           addstr("'s last unbroken rib is ", gamelog);
                        }

                        if(damtype & WOUND_SHOT)addstr("shot apart!", gamelog);
                        else addstr("broken!", gamelog);
                        gamelog.newline();

                        getkey();

                        target->special[SPECIALWOUND_RIBS]-=ribminus;
                     }
                     break;
                  }
               }

               severloot(*target,groundloot);
            }

            //set_color(COLOR_WHITE,COLOR_BLACK,1);
         }
      }
      else
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,1);
         strcat(str," to no effect.");
         move(17,1);
         addstr(str, gamelog);
         gamelog.newline();

         printparty();
         if(mode==GAMEMODE_CHASECAR||
            mode==GAMEMODE_CHASEFOOT) printchaseencounter();
         else printencounter();

         getkey();
      }
   }
   else
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);

      if(melee && aroll<droll-10 && t.blood>70 && t.animalgloss==ANIMALGLOSS_NONE
         && t.is_armed() && t.get_weapon().get_attack(false,true,true) != NULL)
      {

         strcpy(str,t.name);
         strcat(str," knocks the blow aside and counters!");
         move(17,1);
         addstr(str, gamelog);
         gamelog.newline();

         getkey();

         goodguyattack = !goodguyattack;
         char actual_dummy;
         attack(t,a,0,actual_dummy,true);
         goodguyattack = !goodguyattack;
      }//TODO if missed person, but vehicle is large, it might damage the car. 
      else
      {
         if(sneak_attack)
         {
             strcpy(str, t.name);
             switch(LCSrandom(4))
             {
                 case 0: strcat(str," notices at the last moment!"); break;
                 case 1: strcat(str," wasn't born yesterday!"); break;
                 case 2: strcat(str," spins and blocks the attack!"); break;
                 default: strcat(str," jumps back and cries out in alarm!"); break;
             }
             sitealarm=1;
         }
         else
         {
            if (mode==GAMEMODE_CHASECAR)
            {
               if(driver!=NULL)
                  strcpy(str,driver->name);
               switch(droll) // If no driver then droll is 0.
               {
                  case 1: strcpy(str, a.name); strcat(str," missed!"); break;
                  case 2: strcpy(str, a.name); strcat(str," just barely missed!"); break;
                  case 3: strcat(str," can't seem to keep the vehicle in either the lane or the line of fire!"); break;
                  case 4: strcat(str," swerves randomly!"); break;
                  case 5: strcat(str," cuts off another driver and the shot is blocked!"); break;
                  case 6: strcat(str," drops behind a hill in the road!"); break;
                  case 7: strcat(str," changes lanes at the last second!"); break;
                  case 8: strcat(str," accelerates suddenly and the shot goes short!"); break;
                  case 9: strcat(str," fakes a left, and goes right instead!"); break;
                  case 10: strcat(str," fakes a right, and goes left instead!"); break;
                  case 11: strcat(str," fakes with the brakes while powering ahead!"); break;
                  case 12: strcat(str," swerves to the other side of a truck!"); break;
                  case 13: strcat(str," weaves through a row of taxis!"); break;
                  case 14: strcat(str," dodges behind a hot dog cart!"); break;
                  case 15: strcat(str," squeezes between some bridge supports for cover!"); break;
                  case 16: strcat(str," squeals around a corner and behind a building!"); break;
                  case 17: strcat(str," power slides through a narrow gap in the traffic!"); break;
                  case 18: strcat(str," rolls the car onto two wheels to dodge the shot!"); break;
                  default: strcpy(str, a.name); strcat(str," misses completely!"); break; // You failed to hit someone who probably rolled a zero.  You should feel bad.
               }
            }else{
               strcpy(str, t.name);
               switch(droll)
               {
                  case 1: strcpy(str, a.name); strcat(str," missed!"); break;
                  case 2: strcpy(str, a.name); strcat(str," just barely missed!"); break;
                  case 3: strcat(str," tumbles out of the way!"); break;
                  case 4: strcat(str," jumps aside at the last moment!"); break;
                  case 5: strcat(str," leaps for cover!"); break;
                  case 6: strcat(str," ducks back behind cover!"); break;
                  case 7: strcat(str," wisely stays behind cover!"); break;
                  case 8: strcat(str," rolls away from the attack!"); break;
                  case 9: strcat(str," nimbly dodges away from the line of fire!"); break;
                  case 10: strcat(str," leaps over the attack!"); break;
                  case 11: strcat(str," gracefully dives to avoid the attack!"); break;
                  case 12: strcat(str," twists to avoid the attack!"); break;
                  case 13: strcat(str," spins to the side!"); break;
                  case 14: strcat(str," does the Matrix-dodge!"); break; // You saying I can dodge bullets?
                  case 15: strcat(str," avoids the attack with no difficulty at all!"); break;
                  case 16: strcat(str," flexes slightly to avoid being hit!"); break;
                  case 17: strcat(str," confidently allows the attack to miss!"); break;
                  case 18: strcat(str," seems to avoid the attack with only an angry glare!"); break; //When you're ready, you won't have to.
                  default: strcpy(str, a.name); strcat(str," misses completely!"); break; // You failed to hit someone who probably rolled a zero.  You should feel bad.
               }
            }
         }
         move(17,1);
         addstr(str, gamelog);
         gamelog.newline();
         printparty();
         if(mode==GAMEMODE_CHASECAR||
            mode==GAMEMODE_CHASEFOOT) printchaseencounter();
         else printencounter();

         getkey();
      }
   }

   for(;thrownweapons>0;thrownweapons--)
   {
      if(a.has_thrown_weapon)
         a.ready_another_throwing_weapon();
      a.drop_weapon(NULL);
   }

   actual=1;
   return;
}



/* modifies a combat roll based on the creature's critical injuries */
void healthmodroll(int &aroll,Creature &a)
{
   if(a.special[SPECIALWOUND_RIGHTEYE]!=1) aroll-=LCSrandom(2);
   if(a.special[SPECIALWOUND_LEFTEYE]!=1) aroll-=LCSrandom(2);
   if(a.special[SPECIALWOUND_RIGHTEYE]!=1&&
      a.special[SPECIALWOUND_LEFTEYE]!=1) aroll-=LCSrandom(20);
   if(a.special[SPECIALWOUND_RIGHTLUNG]!=1) aroll-=LCSrandom(8);
   if(a.special[SPECIALWOUND_LEFTLUNG]!=1) aroll-=LCSrandom(8);
   if(a.special[SPECIALWOUND_HEART]!=1) aroll-=LCSrandom(10);
   if(a.special[SPECIALWOUND_LIVER]!=1) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_STOMACH]!=1) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_RIGHTKIDNEY]!=1) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_LEFTKIDNEY]!=1) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_SPLEEN]!=1) aroll-=LCSrandom(4);
   if(a.special[SPECIALWOUND_LOWERSPINE]!=1) aroll-=LCSrandom(100);
   if(a.special[SPECIALWOUND_UPPERSPINE]!=1) aroll-=LCSrandom(200);
   if(a.special[SPECIALWOUND_NECK]!=1) aroll-=LCSrandom(300);
   if(a.special[SPECIALWOUND_RIBS]<RIBNUM) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_RIBS]<RIBNUM/2) aroll-=LCSrandom(5);
   if(a.special[SPECIALWOUND_RIBS]==0) aroll-=LCSrandom(5);
}



/* adjusts attack damage based on armor, other factors */
void damagemod(Creature &t,char &damtype,int &damamount,
               char hitlocation,char armorpenetration,int mod,int extraarmor)
{
   int armor=t.get_armor().get_armor(hitlocation);

   if(t.animalgloss==ANIMALGLOSS_TANK)
   {
      if(damtype!=WOUND_BURNED) armor=15;
      else armor=10;
   }


   //if(t.get_armor().get_quality()>1)
      armor-=t.get_armor().get_quality()-1;
   if(t.get_armor().is_damaged())
      armor-=1;

   if(armor<0) armor=0; // Possible from second-rate clothes
   armor += extraarmor; // Add vehicle armor 

   int mod2=armor+LCSrandom(armor+1)-armorpenetration;
   if(mod2>0) mod-=mod2*2;

   if(mod>10) mod=10; // Cap damage multiplier (every 5 points adds 1x damage)

   if(mod<=-20) damamount>>=8;  //Cars plus heavy armor can be really tough.
   else if(mod<=-14) damamount>>=7;
   else if(mod<=-8) damamount>>=6;
   else if(mod<=-6) damamount>>=5;
   else if(mod<=-4) damamount>>=4;
   else if(mod<=-3) damamount>>=3;
   else if(mod<=-2) damamount>>=2;
   else if(mod<=-1) damamount>>=1;
   else if(mod>=0) damamount=(int)((float)damamount * (1.0f + 0.2f*mod));

   // Firefighter's bunker gear reduces fire damage by 3/4
   if((damtype & WOUND_BURNED) && t.get_armor().has_fireprotection())
   {
      // Damaged gear isn't as effective as undamaged gear
      if(t.get_armor().is_damaged())
         damamount>>=1; // Only half as much damage reduction
      else
         damamount>>=2; // Full damage reduction
   }

   if(damamount<0)damamount=0;
}


void specialattack(Creature &a, Creature &t, char &actual)
{
   std::string s = "";
   static const char *judge_debate[]   =
   {
      "debates the death penalty with",
      "debates gay rights with",
      "debates free speech with",
      "debates the Second Amendment with"
   };

   static const char *conservative_ceo_debate[] =
   {
      "explains the derivatives market to",
      "justifies voodoo economics to",
      "extols the Reagan presidency to",
      "argues about tax cuts with",
      "explains Conservative philosophy to",
      "extends a dinner invitation to",
      "offers a VP position to",
      "shows a $1000 bill to",
      "debates fiscal policy with",
      "offers stock options to"
   };

   static const char *other_ceo_debate[] =
   {
      "debates fiscal policy with",
      "derides voodoo economics to",
      "dismisses the Reagan presidency to",
      "argues about tax cuts with",
      "explains Liberal philosophy to"
   };

   static const char *conservative_politician_debate[] =
   {
      "debates the death penalty with",
      "debates gay rights with",
      "debates free speech with",
      "debates the Second Amendment with",
      "justifies voodoo economics to",
      "extols the Reagan presidency to",
      "argues about tax cuts with",
      "explains Conservative philosophy to",
      "extends a dinner invitation to",
      "debates fiscal policy with",
      "chats warmly with",
      "smiles at"
   };

   static const char *other_politician_debate[] =
   {
      "debates the death penalty with",
      "debates gay rights with",
      "debates free speech with",
      "debates the Second Amendment with",
      "derides voodoo economics to",
      "dismisses the Reagan presidency to",
      "argues about tax cuts with",
      "explains Liberal philosophy to",
      "extends a dinner invitation to",
      "debates fiscal policy with",
      "chats warmly with",
      "smiles at"
   };

   static const char *media_debate[] =
   {
      "winks at",
      "smiles at",
      "smirks at",
      "chats warmly with",
      "yells slogans at"
   };

   static const char *military_debate[] =
   {
      "recites the Pledge of Allegiance to",
      "debates national security with",
      "debates terrorism with",
      "preaches about veterans to",
      "explains military spending to"
   };

   static const char *police_debate[] =
   {
      "reasons with ",
      "promises a fair trial to ",
      "offers a kind ear to ",
      "urges cooperation from ",
      "offers a hug to ",
      "suggests counseling to ",
      "gives a teddy bear to "
   };

   int resist=0;
   char str[200];

   clearmessagearea();
   set_color(COLOR_WHITE,COLOR_BLACK,1);

   strcpy(str,a.name);
   strcat(str," ");

   int attack=0;
   if (a.align != Alignment::LIBERAL)
      attack=a.attribute_roll(ATTRIBUTE_WISDOM)+t.get_attribute(ATTRIBUTE_WISDOM,false);
   else if (a.align == Alignment::LIBERAL)
      attack=a.attribute_roll(ATTRIBUTE_HEART)+t.get_attribute(ATTRIBUTE_HEART,false);

   switch(a.type)
   {
      case CREATURE_JUDGE_CONSERVATIVE:
      case CREATURE_JUDGE_LIBERAL:
         strcat(str,pickrandom(judge_debate));
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.skill_roll(SKILL_LAW)+
                   t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.skill_roll(SKILL_LAW)+
                   t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.skill_roll(SKILL_LAW);
         break;
      case CREATURE_SCIENTIST_EMINENT:
         switch(LCSrandom(3))
         {
            case 0:strcat(str,"debates scientific ethics with");break;
            case 1:if(a.align == Alignment::CONSERVATIVE)strcat(str,"explains the benefits of research to");
                   else strcat(str,"explains ethical research to");break;
            case 2:strcat(str,"discusses the scientific method with");break;
         }
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.skill_roll(SKILL_SCIENCE)+
                   t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.skill_roll(SKILL_SCIENCE)+
                   t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.skill_roll(SKILL_SCIENCE);
         break;
      case CREATURE_POLITICIAN:
         if(a.align == Alignment::CONSERVATIVE)
            strcat(str,pickrandom(conservative_politician_debate));
         else
            strcat(str,pickrandom(other_politician_debate));
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.skill_roll(SKILL_LAW)+
                   t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.skill_roll(SKILL_LAW)+
                   t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.skill_roll(SKILL_LAW);
         break;
      case CREATURE_CORPORATE_CEO:
         if(a.align == Alignment::CONSERVATIVE)
            strcat(str,pickrandom(conservative_ceo_debate));
         else
            strcat(str,pickrandom(other_ceo_debate));
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.skill_roll(SKILL_BUSINESS)+
                   t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.skill_roll(SKILL_BUSINESS)+
                   t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.skill_roll(SKILL_BUSINESS);
         break;
      case CREATURE_RADIOPERSONALITY:
      case CREATURE_NEWSANCHOR:
         strcat(str,pickrandom(media_debate));
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.attribute_roll(ATTRIBUTE_CHARISMA);
         break;
      case CREATURE_MILITARYOFFICER:
         strcat(str,pickrandom(military_debate));
         strcat(str," ");
         strcat(str,t.name);
         strcat(str,"!");
         if(t.align == Alignment::LIBERAL)
            resist=t.attribute_roll(ATTRIBUTE_HEART);
         else
            resist=t.attribute_roll(ATTRIBUTE_WISDOM);
         attack+=a.attribute_roll(ATTRIBUTE_CHARISMA);
         break;
      case CREATURE_COP:
         if(a.enemy())
         {
            strcat(str,pickrandom(police_debate));
            strcat(str,t.name);
            strcat(str,"!");

            resist=t.attribute_roll(ATTRIBUTE_HEART);

            attack+=a.skill_roll(SKILL_PERSUASION);
            break;
         }
         //No break. If the cop is a liberal it will do a musical attack instead.
      default:
         if(a.get_weapon().has_musical_attack() || a.type==CREATURE_COP)
         {
            switch(LCSrandom(5))
            {
               case 0:strcat(str,"plays a song for");break;
               case 1:strcat(str,"sings to");break;
               case 2:if(a.get_weapon().has_musical_attack())
                      {
                         strcat(str,"strums the ");
                         strcat(str,a.get_weapon().get_name());
                      }
                      else // let's use a small enough instrument for anyone to carry in their pocket
                         strcat(str,"blows a harmonica");
                      strcat(str," at");break;
               case 3:if(a.align == Alignment::LIBERAL)strcat(str,"plays protest songs at");
                      else strcat(str,"plays country songs at");
                      break;
               case 4:strcat(str,"rocks out at");break;
            }
            strcat(str," ");
            strcat(str,t.name);
            strcat(str,"!");

            attack=a.skill_roll(SKILL_MUSIC);

            if(t.align == Alignment::LIBERAL)
               resist=t.attribute_roll(ATTRIBUTE_HEART);
            else resist=t.attribute_roll(ATTRIBUTE_WISDOM);
            if(resist>0)
               a.train(SKILL_MUSIC,LCSrandom(resist)+1);
            else a.train(SKILL_MUSIC,1);
         }
         break;
   }

   move(16,1);
   addstr(str, gamelog);
   gamelog.newline();

   if((t.animalgloss==ANIMALGLOSS_TANK||(t.animalgloss==ANIMALGLOSS_ANIMAL&&law[LAW_ANIMALRESEARCH]!=Alignment::ELITE_LIBERAL))
    ||(a.enemy() && t.flag & CREATUREFLAG_BRAINWASHED))
   {
      move(17,1);
      addstr(s+t.name+" is immune to the attack!", gamelog);
   }
   else if (a.align == t.align)
   {
      move (17,1);
      addstr(s+t.name+" already agrees with "+a.name+".");
   }
   else if(attack>resist)
   {
      t.stunned+=(attack-resist)/4;
      if(a.enemy())
      {
         if(t.juice>100)
         {
            move(17,1);
            addstr(s+t.name+" loses juice!", gamelog);
            addjuice(t,-50,100);
         }
         else if(LCSrandom(15)>t.get_attribute(ATTRIBUTE_WISDOM,true) || t.get_attribute(ATTRIBUTE_WISDOM,true) < t.get_attribute(ATTRIBUTE_HEART,true))
         {
            move(17,1);
            addstr(s+t.name+" is tainted with Wisdom!", gamelog);
            t.adjust_attribute(ATTRIBUTE_WISDOM,+1);
         }
         else if(t.align==Alignment::LIBERAL && t.flag & CREATUREFLAG_LOVESLAVE)
         {
            move(17,1);
            addstr(s+t.name+" can't bear to leave!", gamelog);
         }
         else
         {
            if(a.align == Alignment::CONSERVATIVE)
            {
               move(17,1);
               addstr(s+t.name+" is turned Conservative", gamelog);
               t.stunned=0;
               if(t.prisoner!=NULL)
                  freehostage(t,0);
               addstr("!", gamelog);
            }
            else
            {
               move(17,1);
               addstr(s+t.name+" doesn't want to fight anymore", gamelog);
               t.stunned=0;
               if(t.prisoner!=NULL)
                  freehostage(t,0);
               addstr("!", gamelog);
            }

            for(int e=0;e<ENCMAX;e++)
            {
               if(encounter[e].exists==0)
               {
                  encounter[e]=t;
                  encounter[e].exists=1;
                  if (a.align == Alignment::CONSERVATIVE)
                    conservatise(encounter[e]);
                  encounter[e].cantbluff=2;
                  encounter[e].squadid=-1;
                  break;
               }
            }

            bool flipstart=0;
            for(int p=0;p<6;p++)
            {
               if(activesquad->squad[p]==&t)
               {
                  activesquad->squad[p]->die();
                  activesquad->squad[p]->location=-1;
                  activesquad->squad[p]=NULL;
                  flipstart=1;
               }
               if(flipstart&&p<5) activesquad->squad[p]=activesquad->squad[p+1];
            }
            if(flipstart) activesquad->squad[5]=NULL;
         }
      }
      else
      {
         if(t.juice>=100)
         {
            move(17,1);
            addstr(s+t.name+" seems less badass!", gamelog);
            addjuice(t,-50,99);
         }
         else if(!t.attribute_check(ATTRIBUTE_HEART,DIFFICULTY_AVERAGE) ||
            t.get_attribute(ATTRIBUTE_HEART,true) < t.get_attribute(ATTRIBUTE_WISDOM,true))
         {
            move(17,1);
            addstr(s+t.name+"'s Heart swells!", gamelog);
            t.adjust_attribute(ATTRIBUTE_HEART,+1);
         }
         else
         {
            move(17,1);
            addstr(s+t.name+" has turned Liberal!", gamelog);
            t.stunned=0;

            liberalize(t);
            t.infiltration/=2;
            t.flag|=CREATUREFLAG_CONVERTED;
            t.cantbluff=0;
         }
      }
   }
   else
   {
      move(17,1);
      addstr(s+t.name+" remains strong.", gamelog);
   }

   gamelog.newline();

   printparty();
   if(mode==GAMEMODE_CHASECAR||
      mode==GAMEMODE_CHASEFOOT) printchaseencounter();
   else printencounter();

   getkey();

   return;
}


/* destroys armor, masks, drops weapons based on severe damage */
void severloot(Creature &cr,vector<Item *> &loot)
{
   int armok=2;
   if((cr.wound[BODYPART_ARM_RIGHT] & WOUND_NASTYOFF)||
      (cr.wound[BODYPART_ARM_RIGHT] & WOUND_CLEANOFF)) armok--;
   if((cr.wound[BODYPART_ARM_LEFT] & WOUND_NASTYOFF)||
      (cr.wound[BODYPART_ARM_LEFT] & WOUND_CLEANOFF)) armok--;
   if(cr.special[SPECIALWOUND_NECK]!=1) armok=0;
   if(cr.special[SPECIALWOUND_UPPERSPINE]!=1) armok=0;

   if(cr.is_armed() && armok==0)
   {
      clearmessagearea();
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(16,1);
      addstr("The ", gamelog);
      addstr(cr.get_weapon().get_name(1), gamelog);
      addstr(" slips from", gamelog);
      move(17,1);
      addstr(cr.name, gamelog);
      addstr("'s grasp.", gamelog);
      gamelog.newline();

      getkey();

      if(mode==GAMEMODE_SITE) cr.drop_weapons_and_clips(&loot);
      else cr.drop_weapons_and_clips(NULL);
   }

   if((((cr.wound[BODYPART_BODY] & WOUND_CLEANOFF)||
        (cr.wound[BODYPART_BODY] & WOUND_NASTYOFF))&&
         cr.get_armor().covers(BODYPART_BODY))||
       ((cr.wound[BODYPART_HEAD] & WOUND_NASTYOFF)&&
         cr.get_armor().is_mask()))
   {
      clearmessagearea();
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(16,1);
      addstr(cr.name, gamelog);
      addstr("'s ", gamelog);
      addstr(cr.get_armor().get_name(), gamelog);
      addstr(" has been destroyed.", gamelog);
      gamelog.newline();

      getkey();

      cr.strip(NULL);
   }
}



/* damages the selected armor if it covers the body part specified */
void armordamage(Armor &armor,int bp, int damamount)
{
   if(armor.covers(bp) && LCSrandom(armor.get_durability()) < damamount)
   {
      if (armor.is_damaged())
      {
         armor.decrease_quality(LCSrandom(armor.get_durability()) < LCSrandom(damamount)/armor.get_quality() ? 1:0);
      }
      else
      {
         armor.set_damaged(true);
      }
   }
}



/* blood explosions */
void bloodblast(Armor* armor)
{
   //GENERAL
   if(armor!=NULL)
      armor->set_bloody(true);

   if(mode!=GAMEMODE_SITE)
      return;

   levelmap[locx][locy][locz].flag|=SITEBLOCK_BLOODY2;

   //HIT EVERYTHING
   for(int p=0;p<6;p++)
   {
      if(activesquad->squad[p]==NULL) continue;
      if(!LCSrandom(2))
         activesquad->squad[p]->get_armor().set_bloody(true);
   }

   for(int e=0;e<ENCMAX;e++)
   {
      if(!encounter[e].exists) continue;
      if(!LCSrandom(2))
         encounter[e].get_armor().set_bloody(true);
   }

   //REFRESH THE SCREEN
   printsitemap(locx,locy,locz);
   refresh();
}



/* kills the specified creature from the encounter, dropping loot */
void delenc(short e,char loot)
{
   //MAKE GROUND LOOT
   if(mode!=GAMEMODE_SITE) loot=0;
   if(loot) makeloot(encounter[e],groundloot);

   //BURY IT
   for(int en=e;en<ENCMAX;en++)
   {
      if(!encounter[en].exists) break;
      if(en<ENCMAX-1) encounter[en]=encounter[en+1];
   }
   encounter[ENCMAX-1].exists=0;
}



/* generates the loot dropped by a creature when it dies */
void makeloot(Creature &cr,vector<Item *> &loot)
{
   cr.drop_weapons_and_clips(&loot);
   cr.strip(&loot);

   if(cr.money>0 && mode == GAMEMODE_SITE)
   {
      loot.push_back(new Money(cr.money));
      cr.money=0;
   }
}



/* abandoned liberal is captured by conservatives */
void capturecreature(Creature &t)
{
   t.activity.type=ACTIVITY_NONE;
   t.drop_weapons_and_clips(NULL);
   //t.strip(NULL);
   Armor clothes=Armor(*armortype[getarmortype("ARMOR_CLOTHES")]);
   t.give_armor(clothes,NULL);

   freehostage(t,2); // situation 2 = no message; this may want to be changed to 0 or 1
   if(t.prisoner)
   {
      if(t.prisoner->squadid==-1)
         delete t.prisoner;
      t.prisoner=NULL; // Stop hauling people
   }
   if(t.flag & CREATUREFLAG_JUSTESCAPED)
   {
      t.location=cursite;
      if(sitetype==SITE_GOVERNMENT_PRISON||
         sitetype==SITE_GOVERNMENT_COURTHOUSE)
      {
         Armor prisoner=Armor(*armortype[getarmortype("ARMOR_PRISONER")]);
         t.give_armor(prisoner,NULL);
      }
      if(sitetype==SITE_GOVERNMENT_PRISON)
      {
         // Clear criminal record?
         t.heat=0;
         for(int i=0;i<LAWFLAGNUM;i++)
            t.crimes_suspected[i]=0;
      }
   }
   else
      t.location = find_police_station(cursite);

   t.squadid=-1;
}



/* checks if the creature can fight and prints flavor text if they can't */
char incapacitated(Creature &a,char noncombat,char &printed)
{
   printed=0;

   if(a.animalgloss==ANIMALGLOSS_TANK)
   {
      if(a.blood<=20||(a.blood<=50&&(LCSrandom(2)||a.forceinc)))
      {
         a.forceinc=0;
         if(noncombat)
         {
            clearmessagearea();

            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(16,1);
            addstr("The ", gamelog);
            addstr(a.name, gamelog);
            switch(LCSrandom(3))
            {
            case 0: addstr(" smokes...", gamelog); break;
            case 1: addstr(" smolders.", gamelog); break;
            case 2: addstr(" burns...", gamelog); break;
            }

            gamelog.newline();

            printed=1;
         }
         return 1;
      }

      return 0;
   }

   if(a.animalgloss==ANIMALGLOSS_ANIMAL)
   {
      if(a.blood<=20||(a.blood<=50&&(LCSrandom(2)||a.forceinc)))
      {
         a.forceinc=0;
         if(noncombat)
         {
            clearmessagearea();
            set_color(COLOR_WHITE,COLOR_BLACK,1);

            move(16,1);
            addstr("The ", gamelog);
            addstr(a.name);
            switch(LCSrandom(3))
            {
            case 0: addstr(" yelps in pain...", gamelog); break;
            case 1: if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE) addstr(" [makes a stinky].", gamelog);
                    else addstr(" soils the floor.", gamelog); break;
            case 2: addstr(" yowls pitifully...", gamelog); break;
            }

            gamelog.newline();

            printed=1;
         }
         return 1;
      }

      return 0;
   }

   if(a.blood<=20||(a.blood<=50&&(LCSrandom(2)||a.forceinc)))
   {
      a.forceinc=0;
      if(noncombat)
      {
         clearmessagearea();
         set_color(COLOR_WHITE,COLOR_BLACK,1);

         move(16,1);
         addstr(a.name);
         switch(LCSrandom(54))
         {
         case 0: addstr(" desperately cries out to Jesus."); break;
         case 1: if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE) addstr(" [makes a stinky].");
                 else addstr(" soils the floor."); break;
         case 2: addstr(" whimpers in a corner."); break;
         case 3: addstr(" begins to weep."); break;
         case 4: addstr(" vomits."); break;
         case 5: addstr(" chortles..."); break;
         case 6: addstr(" screams in pain."); break;
         case 7: addstr(" asks for mother."); break;
         case 8: addstr(" prays softly..."); break;
         case 9: addstr(" clutches at the wounds."); break;
         case 10: addstr(" reaches out and moans."); break;
         case 11: addstr(" hollers in pain."); break;
         case 12: addstr(" groans in agony."); break;
         case 13: addstr(" begins hyperventilating."); break;
         case 14: addstr(" shouts a prayer."); break;
         case 15: addstr(" coughs up blood."); break;
         case 16: if(mode!=GAMEMODE_CHASECAR) addstr(" stumbles against a wall.");
                  else addstr(" leans against the door."); break;
         case 17: addstr(" begs for forgiveness."); break;
         case 18: addstr(" shouts \"Why have you forsaken me?\""); break;
         case 19: addstr(" murmurs \"Why Lord?   Why?\""); break;
         case 20: addstr(" whispers \"Am I dead?\""); break;
         case 21: if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE) addstr(" [makes a mess], moaning.");
                  else addstr(" pisses on the floor, moaning."); break;
         case 22: addstr(" whispers incoherently."); break;
         case 23: if(a.special[SPECIALWOUND_RIGHTEYE]&&a.special[SPECIALWOUND_LEFTEYE])
                     addstr(" stares off into space.");
                  else if(a.special[SPECIALWOUND_RIGHTEYE]||a.special[SPECIALWOUND_LEFTEYE])
                     addstr(" stares into space with one empty eye.");
                  else addstr(" stares out with hollow sockets."); break;
         case 24: addstr(" cries softly."); break;
         case 25: addstr(" yells until the scream cracks dry."); break;
         case 26: if(a.special[SPECIALWOUND_TEETH]>1) addstr("'s teeth start chattering.");
                  else if(a.special[SPECIALWOUND_TEETH]==1) addstr("'s tooth starts chattering.");
                  else addstr("'s gums start chattering."); break;
         case 27: addstr(" starts shaking uncontrollably."); break;
         case 28: addstr(" looks strangely calm."); break;
         case 29: addstr(" nods off for a moment."); break;
         case 30: addstr(" starts drooling."); break;
         case 31: addstr(" seems lost in memories."); break;
         case 32: addstr(" shakes with fear."); break;
         case 33: addstr(" murmurs \"I'm so afraid...\""); break;
         case 34: addstr(" cries \"It can't be like this...\""); break;
         case 35: if(a.age<20 && !a.animalgloss) addstr(" cries \"Mommy!\"");
                  else switch(a.type) {
                  case CREATURE_GENETIC:
                     addstr(" murmurs \"What about my offspring?\""); break;
                  case CREATURE_GUARDDOG:
                     addstr(" murmurs \"What about my puppies?\""); break;
                  default:
                     addstr(" murmurs \"What about my children?\""); break;
                  } break;
         case 36: addstr(" shudders quietly."); break;
         case 37: addstr(" yowls pitifully."); break;
         case 38: addstr(" begins losing faith in God."); break;
         case 39: addstr(" muses quietly about death."); break;
         case 40: addstr(" asks for a blanket."); break;
         case 41: addstr(" shivers softly."); break;
         case 42: if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)addstr(" [makes a mess].");
                  else addstr(" vomits up a clot of blood."); break;
         case 43: if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)addstr(" [makes a mess].");
                  else addstr(" spits up a cluster of bloody bubbles."); break;
         case 44: addstr(" pleads for mercy."); break;
         case 45: addstr(" quietly asks for coffee."); break;
         case 46: addstr(" looks resigned."); break;
         case 47: addstr(" scratches at the air."); break;
         case 48: addstr(" starts to giggle uncontrollably."); break;
         case 49: addstr(" wears a look of pain."); break;
         case 50: addstr(" questions God."); break;
         case 51: addstr(" whispers \"Mama baby.  Baby loves mama.\""); break;
         case 52: addstr(" asks for childhood toys frantically."); break;
         case 53: addstr(" murmurs \"But I go to church...\""); break;
         }

         printed=1;
      }

      return 1;
   }
   else if(a.stunned)
   {
      if(noncombat)
      {
         a.stunned--;
         clearmessagearea();
         set_color(COLOR_WHITE,COLOR_BLACK,1);

         move(16,1);
         addstr(a.name, gamelog);
         switch(LCSrandom(11))
         {
         case 0: addstr(" seems hesitant.", gamelog); break;
         case 1: addstr(" is caught in self-doubt.", gamelog); break;
         case 2: addstr(" looks around uneasily.", gamelog); break;
         case 3: addstr(" begins to weep.", gamelog); break;
         case 4: addstr(" asks \"Is this right?\"", gamelog); break;
         case 5: addstr(" asks for guidance.", gamelog); break;
         case 6: addstr(" is caught in indecision.", gamelog); break;
         case 7: addstr(" feels numb.", gamelog); break;
         case 8: addstr(" prays softly.", gamelog); break;
         case 9: addstr(" searches for the truth.", gamelog); break;
         case 10: addstr(" tears up.", gamelog); break;
         }

         gamelog.newline();

         printed=1;
      }
      return 1;
   }

   if(a.special[SPECIALWOUND_NECK]==2||
      a.special[SPECIALWOUND_UPPERSPINE]==2)
   {
      if(!noncombat)
      {
         clearmessagearea();
         set_color(COLOR_WHITE,COLOR_BLACK,1);

         move(16,1);
         addstr(a.name, gamelog);
         switch(LCSrandom(5))
         {
         case 0: addstr(" looks on with authority.", gamelog); break;
         case 1: addstr(" waits patiently.", gamelog); break;
         case 2: addstr(" sits in thought.", gamelog); break;
         case 3: addstr(" breathes slowly.", gamelog); break;
         case 4: addstr(" considers the situation.", gamelog); break;
         }

         gamelog.newline();

         printed=1;
      }

      return 1;
   }

   return 0;
}



/* describes a character's death */
void adddeathmessage(Creature &cr)
{
   set_color(COLOR_YELLOW,COLOR_BLACK,1);

   move(16,1);
   char str[200];

   if((cr.wound[BODYPART_HEAD] & WOUND_CLEANOFF)||
      (cr.wound[BODYPART_HEAD] & WOUND_NASTYOFF))
   {
      strcpy(str,cr.name);
      switch(LCSrandom(4))
      {
      case 0:
         strcat(str," reaches once where there ");
         addstr(str, gamelog);
         move(17,1);
         if(mode!=GAMEMODE_CHASECAR)
            addstr("is no head, and falls.", gamelog);
         else addstr("is no head, and slumps over.", gamelog);
         break;
      case 1:
         if(mode!=GAMEMODE_CHASECAR)
            strcat(str," stands headless for a ");
         else strcat(str," sits headless for a ");
         addstr(str, gamelog);
         move(17,1);
         addstr("moment then crumples over.", gamelog);
         break;
      case 2:
         strcat(str," squirts ");
         if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)strcat(str,"[red water]");
         else strcat(str,"blood");
         strcat(str," out of the ");
         addstr(str, gamelog);
         move(17,1);
         if(mode!=GAMEMODE_CHASECAR)
            addstr("neck and runs down the hall.", gamelog);
         else addstr("neck and falls to the side.", gamelog);
         break;
      case 3:
         strcat(str," sucks a last breath through ");
         addstr(str, gamelog);
         move(17,1);
         addstr("the neck hole, then is quiet.", gamelog);
         break;
      }
   }
   else if((cr.wound[BODYPART_BODY] & WOUND_CLEANOFF)||
      (cr.wound[BODYPART_BODY] & WOUND_NASTYOFF))
   {
      strcpy(str,cr.name);
      switch(LCSrandom(2))
      {
      case 0:strcat(str," breaks into pieces."); break;
      case 1:strcat(str," falls apart and is dead."); break;
      }
      addstr(str, gamelog);
   }
   else
   {
      strcpy(str,cr.name);
      switch(LCSrandom(11))
      {
      case 0:
         strcat(str," cries out one last time ");
         addstr(str, gamelog);
         move(17,1);
         addstr("then is quiet.", gamelog);
         break;
      case 1:
         strcat(str," gasps a last breath and ");
         addstr(str, gamelog);
         move(17,1);
         if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)addstr("[makes a mess].", gamelog);
         else addstr("soils the floor.", gamelog);
         break;
      case 2:
         strcat(str," murmurs quietly, breathing softly. ");
         addstr(str, gamelog);
         move(17,1);
         addstr("Then all is silent.", gamelog);
         break;
      case 3:
         strcat(str," shouts \"FATHER!  Why have you ");
         addstr(str, gamelog);
         move(17,1);
         addstr("forsaken me?\" and dies in a heap.", gamelog);
         break;
      case 4:
         strcat(str," cries silently for mother, ");
         addstr(str, gamelog);
         move(17,1);
         addstr("breathing slowly, then not at all.", gamelog);
         break;
      case 5:
         strcat(str," breathes heavily, coughing up ");
         addstr(str, gamelog);
         move(17,1);
         addstr("blood...  then is quiet.", gamelog);
         break;
      case 6:
         strcat(str," silently drifts away, and ");
         addstr(str, gamelog);
         move(17,1);
         addstr("is gone.", gamelog);
         break;
      case 7:
         strcat(str," sweats profusely, murmurs ");
         addstr(str, gamelog);
         move(17,1);
         if(law[LAW_FREESPEECH] == Alignment::ARCH_CONSERVATIVE)addstr("something [good] about Jesus, and dies.", gamelog);
         else addstr("something about Jesus, and dies.", gamelog);
         break;
      case 8:
         strcat(str," whines loudly, voice crackling, ");
         addstr(str, gamelog);
         move(17,1);
         addstr("then curls into a ball, unmoving.", gamelog);
         break;
      case 9:
         strcat(str," shivers silently, whispering ");
         addstr(str, gamelog);
         move(17,1);
         addstr("a prayer, then all is still.", gamelog);
         break;
      case 10:
         strcat(str," speaks these final words: ");
         addstr(str, gamelog);
         move(17,1);
         switch(cr.align)
         {
         case Alignment::LIBERAL:
         case Alignment::ELITE_LIBERAL:
            addstr(slogan, gamelog); break;
         case Alignment::MODERATE:
            addstr("\"A plague on both your houses...\"", gamelog); break;
         default:
            addstr("\"Better dead than liberal...\"", gamelog); break;
         }
         break;
      }
   }
   gamelog.newline();
}



int count_live_liberals_at_location(int loc) {
    auto libnum = 0;
    for(int pl=0; pl < len(pool); pl++)
    {
        if (pool[pl]->location != loc)
            continue;
        if (pool[pl]->alive && pool[pl]->align == Alignment::LIBERAL)
            libnum++;
    }
    return libnum;
}

/* pushes people into the current squad (used in a siege) */
void auto_promote_available_liberals_into_squad(int loc) {
    if (activesquad == nullptr) return;
    if (count_live_members_in_squad(activesquad) == 6) return;
    if (count_members_in_squad(activesquad) == count_live_liberals_at_location(loc)) return;

    for (auto sq_index = 0; sq_index < 6; sq_index++) {
        if (activesquad->squad[sq_index] != nullptr) {
            if (activesquad->squad[sq_index]->alive) {
                continue;
            } else {
                activesquad->squad[sq_index]->squadid = -1;
                activesquad->squad[sq_index] = nullptr;
            }
        }

        for (auto pool_index = 0; pool_index < len(pool); pool_index++) {
            if (pool[pool_index]->location != loc) continue;
            if (pool[pool_index]->alive && pool[pool_index]->squadid == -1 &&
                pool[pool_index]->align == Alignment::LIBERAL) {
                activesquad->squad[sq_index] = pool[pool_index];
                activesquad->squad[sq_index]->squadid = activesquad->id;
                break;
            }
        }
    }
}

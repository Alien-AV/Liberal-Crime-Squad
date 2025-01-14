/**
 * Congressional manipulations.
 */
/*
 * Copyright (c) 2002,2003,2004 by Tarn Adams
 * Copyright 2017 Stephen M. Webb <stephen.webb@bregmasoft.ca>
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
 * This file was created by Chris Johnson (grundee@users.sourceforge.net) by
 * copying code from game.cpp.
*/
#include "politics/politics.h"

#include <algorithm>
#include <externs.h>
#include "politics/alignment.h"
#include "politics/amendments.h"

//TODO: Not sure if anything in here should be logged...Perhaps only a summary of the results? --Addictgamer

/**
 * Get the leaning of an issue voter for an election.
 *
 * Takes a random voter, calculates how liberal or conservative they are (if
 * @p stalin is true, calculates how libertarian or Stalinist they are instead)
 * and for each issue they roll for their opinion on a 50-point subset of the
 * spectrum, determined by bias -- high liberal/libertarian bias only rolls on
 * the liberal/libertarian end of the spectrum, high conservative/Stalinist bias
 * only rolls on the conservative/Stalinist end of the spectrum.
 */
static Alignment
getswingvoter(bool stalin)
{
   Alignment vote = Alignment::ARCH_CONSERVATIVE;
   int bias = publicmood(-1-stalin) - LCSrandom(100);
   if (bias >  25) bias= 25;
   if (bias < -25) bias=-25;
   for (int i=0; i<4; i++)
   {
      int issue = randomissue(true);
      if (25+LCSrandom(50)-bias<((stalin&&stalinview(issue,false))?100-attitude[issue]:attitude[issue]))
          vote = shift_left(vote);
   }
   return vote;
}


/**
 * Get the leaning of a partyline voter for an election.
 *
 * He vote here works by shifting the voter's leaning one step to the right, and
 * then possibly shifting left up to two times.  That means a Conservative party
 * supported might end up voting Arch Conservative, Conservative, or Moderate
 * and a Liberal supported might end up voting for Moderate, Liberal, or Elite
 * Liberal.  There is no mention of undecided or undeclared voters at this
 * point.  Maybe in a later version.
 *
 * No need for this to deal with Stalinism, this function deliberately only deals with the
 * liberal vs. conservative spectrum.
 */
static Alignment
getsimplevoter(Alignment leaning)
{
   Alignment vote = shift_right(leaning);
   for (int i=0; i<2; i++)
     if (LCSrandom(100) < attitude[randomissue(true)])
         vote = shift_left(vote);
   return vote;
}


/**
 * Appoint a figure to an executive office, based on the President's alignment.
 */
static void
fillCabinetPost(int position)
{
   // Set alignment
  if (is_extreme(exec[EXEC_PRESIDENT]))
    exec[position] = exec[EXEC_PRESIDENT];
  else
    exec[position] = shift(exec[EXEC_PRESIDENT], LCSrandom(3)-1);

  // Set name
  if (exec[position] == Alignment::ARCH_CONSERVATIVE)
    generate_name(execname[position], GENDER_WHITEMALEPATRIARCH);
  else if (exec[position] == Alignment::CONSERVATIVE)
    generate_name(execname[position], GENDER_MALE);
  else
    generate_name(execname[position]);
}


/**
 * Calculate presidential approval.
 */
int
presidentapproval()
{
   //Calculate Presidential approval rating
   int approval=0, i;
   for(i=0;i<1000;i++)
   {
      if(i%2==0&&LCSrandom(2)) approval++;    // Partyline supporter (~25%)
      else if(i%2==1&&LCSrandom(2)) continue; // Partyline opponent  (~25%)
      else                                    // Swing issue voter   (~50%) (should be more than in the real election)
      {
         // Get their leanings as an issue voter
         Alignment vote = getswingvoter(exec[EXEC_PRESIDENT]==Alignment::STALINIST);
         // If their views are close to the President's views, they should
         // approve, but might not if their party leaning conflicts with
         // the president's
         // Moderate president from the Conservative party is only supported
         // by moderates and Conservatives
         // Moderate president from the Liberal party is only supported
         // by moderates and Liberals
         Alignment presalign = exec[EXEC_PRESIDENT] == Alignment::STALINIST ? Alignment::ARCH_CONSERVATIVE
                                                                            : exec[EXEC_PRESIDENT];
         if (diff(presalign, vote) <= 1 && (presparty != LIBERAL_PARTY ? to_right_of(vote, Alignment::LIBERAL)
                                                                       : to_left_of(vote, Alignment::CONSERVATIVE)))
           approval++;
      }
   }
   return approval;
}


/**
 * Promote the Vice President to President, and replace VP.
 */
void
promoteVP()
{
   exec[EXEC_PRESIDENT] = exec[EXEC_VP]; // VP takes over as President
   strcpy(execname[EXEC_PRESIDENT], execname[EXEC_VP]);
   switch(exec[EXEC_PRESIDENT])
   { // make sure presparty is set correctly (if old President was moderate they could've been in Liberal OR Conservative Party,
   case Alignment::ARCH_CONSERVATIVE: // and new President might have a political alignment incompatible with that party)
   case Alignment::CONSERVATIVE:
      presparty=CONSERVATIVE_PARTY; break; // Conservatives and Arch-Conservatives align with the Conservative Party
   case Alignment::MODERATE:
      break; // same party as previous President (either Liberal Party or Conservative Party)
   case Alignment::LIBERAL:
   case Alignment::ELITE_LIBERAL:
      presparty=LIBERAL_PARTY; break; // Liberals and Elite Liberals align with the Liberal Party
   case Alignment::STALINIST:
      presparty=STALINIST_PARTY; break; // Stalinists align with the Stalinist Party
   }
   fillCabinetPost(EXEC_VP); // the vacant VP slot needs to be filled
   if (is_extreme(exec[EXEC_PRESIDENT]))
   {  // new President has extreme views... make sure all cabinet members agree with new President
      if (exec[EXEC_PRESIDENT] != exec[EXEC_STATE])
        fillCabinetPost(EXEC_STATE);
      if (exec[EXEC_PRESIDENT] != exec[EXEC_ATTORNEY])
        fillCabinetPost(EXEC_ATTORNEY);
   }
   else
   {  // new President has fairly moderate views, but anyone who differs too much from them must be replaced
      if (diff(exec[EXEC_PRESIDENT], exec[EXEC_STATE]) > 1)
        fillCabinetPost(EXEC_STATE);
      if (diff(exec[EXEC_PRESIDENT], exec[EXEC_ATTORNEY]) > 1)
        fillCabinetPost(EXEC_ATTORNEY);
   }
}


/**
 * Cause the people to vote (presidential, congressional, propositions).
 */
void
elections(bool clearformess,bool canseethings)
{
   int l,p,c;

   if(canseethings)
   {
      music.play(MUSIC_ELECTIONS);
      if(clearformess) erase();
      else makedelimiter();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(8,1);
      addstr("The Elections are being held today!");

      getkey();
   }

   //PRESIDENTIAL
   if(year%4==0)
   {
      if(canseethings)
      {
         erase();

         set_color(COLOR_WHITE,COLOR_BLACK,1);

         move(0,0);
         addstr("Presidential General Election ");
         addstr(year);

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(2,0);
         if(stalinmode) addstr("After a long primary campaign, the people have rallied around three leaders...");
         else addstr("After a long primary campaign, the people have rallied around two leaders...");
      }

      struct Candidate
      {
        Alignment   alignment;
        std::string name;
      } candidate[3];
      int votes[3]={0,0,0};

      //Primaries
      int approvepres=0, approveveep=0;              // presidential & vice-presidential approval within own party
      int libvotes[5]  = {0, 0, 0, 0 ,0};
      int consvotes[5] = {0, 0, 0, 0, 0}; // liberal & conservative parties' candidates votes received

      // run primaries for 100 voters
      for(int i=0; i<100; i++)
      {
         Alignment voters[2] =
         {
           getsimplevoter(Alignment::LIBERAL),
           getsimplevoter(Alignment::CONSERVATIVE)
         }; // liberal & conservative party voter both decide

         if (presparty != STALINIST_PARTY) // Stalinist Party doesn't do primaries
         {
            // Incumbent can win primary automatically if their approval in their party is over 50%,
            // so we need to know what their inter-party approval rating is.
            // check if this voter supports the president (1/2 chance if closely aligned)
            if (voters[presparty] == exec[EXEC_PRESIDENT]
             || (diff(exec[EXEC_PRESIDENT], voters[presparty]) == 1 && !LCSrandom(2)))
            {
              approvepres++;
            }

            // check if this voter supports the vice-president (1/3 chance if closely aligned)
            if (voters[presparty] == exec[EXEC_VP]
             || (diff(exec[EXEC_VP], voters[presparty]) == 1 && !LCSrandom(3)))
            {
              approveveep++;
            }
         }
         consvotes[to_index(voters[CONSERVATIVE_PARTY])]++;
         libvotes[to_index(voters[LIBERAL_PARTY])]++; // count ballots
      }

      // determine conservative winner
      if (consvotes[0] > consvotes[1] && consvotes[0] > consvotes[2])
        candidate[CONSERVATIVE_PARTY].alignment = Alignment::ARCH_CONSERVATIVE;
      else if (consvotes[1] > consvotes[2])
        candidate[CONSERVATIVE_PARTY].alignment = Alignment::CONSERVATIVE;
      else
        candidate[CONSERVATIVE_PARTY].alignment = Alignment::MODERATE;

      // determine liberal winner
      if (libvotes[2] > libvotes[3] && libvotes[2] > libvotes[4])
        candidate[LIBERAL_PARTY].alignment = Alignment::MODERATE;
      else if (libvotes[3] > libvotes[4])
        candidate[LIBERAL_PARTY].alignment = Alignment::LIBERAL;
      else
        candidate[LIBERAL_PARTY].alignment = Alignment::ELITE_LIBERAL;

      // Stalinist winner is always Stalinist
      candidate[STALINIST_PARTY].alignment=Alignment::STALINIST;
      // approval within own party of Stalinist pres and VP is 100%, since all Stalinist party members have the same political views
      if(presparty==STALINIST_PARTY) approvepres=100,approveveep=100;

      // name the candidates
      if (candidate[CONSERVATIVE_PARTY].alignment == Alignment::ARCH_CONSERVATIVE)
        candidate[CONSERVATIVE_PARTY].name = generate_name(GENDER_WHITEMALEPATRIARCH);
      else if (candidate[CONSERVATIVE_PARTY].alignment == Alignment::CONSERVATIVE)
        candidate[CONSERVATIVE_PARTY].name = generate_name(GENDER_MALE);
      else
        candidate[CONSERVATIVE_PARTY].name = generate_name();
      candidate[LIBERAL_PARTY].name = generate_name();
      candidate[STALINIST_PARTY].name = generate_name();

      // Special Incumbency Rules: If the incumbent president or vice president
      // has approval of over 50% in their party, they win their primary
      // automatically. Even if they don't have over 50%, if their alignment wins
      // using the normal primary process, they are the chosen candidate for
      // that alignment (this last bit only applies to President; unpopular VP
      // candidates just don't run, and if their alignment wins it will be
      // someone else).
      if(execterm==1) // President running for re-election
      {
         if (approvepres >= 50)
           candidate[presparty].alignment = exec[EXEC_PRESIDENT];
         if (candidate[presparty].alignment == exec[EXEC_PRESIDENT])
           candidate[presparty].name.copy(execname[EXEC_PRESIDENT], POLITICIAN_NAMELEN-1);
         else
           execterm=2; // Boom! Incumbent president was defeated in their
      }                   // own party. New candidate works with a clean slate.
      else if (approveveep >= 50 &&                                              // Vice-President running for President
              (presparty != LIBERAL_PARTY || exec[EXEC_VP] != Alignment::CONSERVATIVE) && // We don't want conservative liberals
              (presparty != CONSERVATIVE_PARTY || exec[EXEC_VP] != Alignment::LIBERAL))   // or liberal conservatives.
      {
         if (approvepres >= 50)
         {
           candidate[presparty].alignment = exec[EXEC_VP];
           candidate[presparty].name.copy(execname[EXEC_VP], POLITICIAN_NAMELEN-1);
         }
      }

      //Print candidates
      if(canseethings)
      {
         for(c=0;c<2+stalinmode;c++)
         {
            // Pick color by political orientation
            set_alignment_color(candidate[c].alignment,true);

            move(8-((c+1)%3)*2,0);
            // Choose title -- president or vice president special titles, otherwise
            // pick based on historically likely titles (eg, governor most likely...)
            if(c==presparty&&execterm==1) addstr("President ");
            else if(c==presparty&&!strcmp(candidate[c].name.c_str(),execname[EXEC_VP])) addstr("Vice President ");
            else if(LCSrandom(2)) addstr("Governor ");
            else if(LCSrandom(2)) addstr("Senator ");
            else if(LCSrandom(2)) addstr("Ret. General ");
            else if(LCSrandom(2)) addstr("Representative ");
            else if(LCSrandom(2)) addstr("Mr. ");
            else addstr("Mrs. ");

            addstr(candidate[c].name);
            addstr(", "+as_printable(candidate[c].alignment, false));
         }

         if(!disbanding)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(8+stalinmode*2,0);
            addstr("Press any key to watch the election unfold.");

            getkey();
         }
         else pause_ms(200);
      }

      int winner=-1;
      bool recount=false;

      for(int l=0;l<1000;l++) // 1000 Voters!
      {
         if(l%2==0&&LCSrandom(5))
         {  // Partyline Liberals (~40%) - except when stalin mode is enabled they can go Stalinist sometimes
            if (!stalinmode
                || getswingvoter(true) != Alignment::ARCH_CONSERVATIVE
                || getswingvoter(false) == Alignment::ELITE_LIBERAL) // if we aren't in stalin mode or they don't agree with Stalinism or they are an Elite Liberal
              votes[LIBERAL_PARTY]++;                                        // go with Liberal Party affiliation (Good Loyal Liberal!)
            else
              votes[STALINIST_PARTY]++;                                    // otherwise, this Liberal's gone Stalinist and betrayed Liberalism (TRAITOR!)
         }
         else if (l%2 == 1 && LCSrandom(5))
         {  // Partyline Conservatives (~40%) - except when stalin mode is enabled they can go Stalinist sometimes
            if (!stalinmode
                || getswingvoter(true) != Alignment::ARCH_CONSERVATIVE
                || getswingvoter(false) == Alignment::ARCH_CONSERVATIVE) // if we aren't in stalin mode or they don't agree with Stalinism or they are an Arch-Conservative
              votes[CONSERVATIVE_PARTY]++;                                    // go with Conservative Party affiliation (Good Loyal Conservative!)
            else
              votes[STALINIST_PARTY]++;                                     // otherwise, this Conservative's gone Stalinist and betrayed Conservatism (TRAITOR!)
         }
         else // Swing Issue Voters (~20%)
         {
            // Get the leanings of an issue voter
            Alignment vote = getswingvoter(false);
            // If they agree with the Stalinist candidate and Stalinist mode is enabled,
            // cast a vote for the Stalinist candidate
            if (stalinmode && getswingvoter(true) == Alignment::ARCH_CONSERVATIVE)
               votes[STALINIST_PARTY]++;
            // If they are to the left or equal to the liberal candidate,
            // and they disagree with the other candidate(s), cast a
            // vote for the liberal candidate.
            else if (vote >= candidate[LIBERAL_PARTY].alignment && vote != candidate[CONSERVATIVE_PARTY].alignment)
               votes[LIBERAL_PARTY]++;
            // If they are to the right or equal to the conservative candidate,
            // and they disagree with the other candidate(s), cast a vote
            // for the conservative candidate.
            else if (vote <= candidate[CONSERVATIVE_PARTY].alignment && vote != candidate[LIBERAL_PARTY].alignment)
               votes[CONSERVATIVE_PARTY]++;
            // If they disagree with all the candidates, vote randomly.
            else
              votes[LCSrandom(2+stalinmode)]++;
         }

         if(l%5==4)
         {
            int maxvote=0;
            for(c=0;c<2+stalinmode;c++) if(votes[c]>maxvote) maxvote=votes[c];
            vector<int> eligible;
            for(c=0;c<2+stalinmode;c++) if(votes[c]==maxvote) eligible.push_back(c);
            recount=len(eligible)>1;
            winner=pickrandom(eligible);

            if(canseethings)
            {
               for(c=0;c<2+stalinmode;c++)
               {
                  if(c==winner) set_color(COLOR_WHITE,COLOR_BLACK,1);
                  else set_color(COLOR_BLACK,COLOR_BLACK,1);
                  move(8-((c+1)%3)*2,45);
                  addstr(votes[c]/10);
                  addchar('.');
                  addstr(votes[c]%10);
                  addchar('%');
                  if(c==winner&&recount&&l==999) addstr(" (After Recount)");
               }

               if(!disbanding) pause_ms(40);
               else pause_ms(20);
            }
         }
      }

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(8+stalinmode*2,0);
         addstr("Press any key to continue the elections.   ");

         getkey();
      }
      //else if(disbanding) pause_ms(800);

      //CONSTRUCT EXECUTIVE BRANCH
      if (winner == presparty && execterm == 1)
        execterm = 2;
      else
      {
         presparty = winner;
         execterm = 1;
         exec[EXEC_PRESIDENT] = candidate[winner].alignment;
         candidate[winner].name.copy(execname[EXEC_PRESIDENT], POLITICIAN_NAMELEN);
         for (int e=EXEC_PRESIDENT+1; e<EXECNUM; e++)
           fillCabinetPost(e);
      }
   }

   if(year%2==0)
   {
      elections_senate((year%6)/2,canseethings); //SENATE
      elections_house(canseethings); //HOUSE
   }

   //PROPOSITIONS
   if(canseethings)
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);

      move(0,0);
      addstr("Important Propositions ");
      addstr(year);
   }

   vector<int> prop, propdir, canlaw;
   int pnum=LCSrandom(4)+4; // this expression for pnum has a maximum value of 7, important later on
   char lawtaken[LAWNUM];
   memset(lawtaken,0,LAWNUM*sizeof(char));

   int lawpriority[LAWNUM];
   memset(lawpriority,0,LAWNUM*sizeof(int));
   char lawdir[LAWNUM];
   memset(lawdir,0,LAWNUM*sizeof(char));

   //DETERMINE PROPS
   for(l=0;l<LAWNUM;l++)
   {
      int pmood=publicmood(l);
      Alignment xvote = Alignment::ARCH_CONSERVATIVE;
      for (int i=0; i<4; i++)
        if (LCSrandom(100) < pmood)
            xvote = shift_left(xvote);

      if (to_right_of(law[l], xvote)) lawdir[l] = 1;
      if (to_left_of(law[l], xvote)) lawdir[l] = -1;
      if (law[l] == Alignment::ARCH_CONSERVATIVE) lawdir[l]=1;
      if (law[l] == Alignment::ELITE_LIBERAL) lawdir[l]=-1;

      int pvote = to_index(law[l]) * 25; //CALC PRIORITY

      lawpriority[l] = DIFF(pvote,pmood)+LCSrandom(10)+public_interest[l];
   }

   prop.resize(pnum);
   propdir.resize(pnum);

   int propnums[7]={0,0,0,0,0,0,0}; // 7 is maximum value for pnum
   if(canseethings)
   {
      for(p=0;p<pnum;p++)
      {
         int propnum=0;
         do
         {  // this loop prevents duplicate proposition numbers from occurring
            switch(p)
            { // 7 entries, as 7 is maximum value for pnum
            case 0: propnum=2*(17-LCSrandom(2)*6)*(19-LCSrandom(2)*6); break;
            case 1: propnum=7*(17-LCSrandom(2)*6)*(19-LCSrandom(2)*6); break;
            case 2: propnum=3*(17-LCSrandom(2)*6)*(19-LCSrandom(2)*6); break;
            case 3: propnum=5*(17-LCSrandom(2)*6)*(2-LCSrandom(2)*1); break;
            case 4: propnum=11*(17-LCSrandom(2)*6)*(2-LCSrandom(2)*1); break;
            case 5: propnum=13*(17-LCSrandom(2)*6)*(2-LCSrandom(2)*1); break;
            case 6: propnum=17*(17-LCSrandom(2)*6)*(2-LCSrandom(2)*1); break;
            }
         } while(find(propnums,propnums+p,propnum)!=propnums+p); // redo loop if we already used this proposition number, end loop if it's unused
         propnums[p]=propnum; // put the proposition number we chose into the array, so we don't choose it again for a later proposition
      }
      sort(propnums,propnums+pnum);
   }

   for(p=0;p<pnum;p++)
   {
      int maxprior=0;
      for(l=0;l<LAWNUM;l++) if(lawpriority[l]>maxprior&&!lawtaken[l]) maxprior=lawpriority[l];
      canlaw.clear();
      for(l=0;l<LAWNUM;l++) if(lawpriority[l]==maxprior&&!lawtaken[l]) canlaw.push_back(l);

      prop[p]=pickrandom(canlaw);

      lawtaken[prop[p]]=1;

      propdir[p]=lawdir[prop[p]];

      if(canseethings)
      {
         move(p*3+2,0);
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         addstr("Proposition "+std::to_string(propnums[p])+':');
         move(p*3+2,18);
         addstr("To ");
         set_alignment_color(shift(Alignment::MODERATE, propdir[p]));
         switch(prop[p])
         {
            case LAW_ABORTION:
               if(propdir[p]==1)addstr("Strengthen Reproductive Freedom"); // was "Strengthen Abortion Rights"
               else addstr("Protect the Unborn Child");
               break;
            case LAW_ANIMALRESEARCH:
               if(propdir[p]==1)addstr("Limit Animal Cruelty");
               else addstr("Help Scientists Cure Diseases"); // was "Expand Animal Research"
               break;
            case LAW_POLICEBEHAVIOR:
               if(propdir[p]==1)addstr("Curtail Police Misconduct");
               else addstr("Help Police Fight Crime"); // was "Stop Harassment of Police Officers"
               break;
            case LAW_PRIVACY:
               if(propdir[p]==1)addstr("Enhance Privacy Protection");
               else addstr("Promote Transparency and Accountability"); // was "Allow Corporations Access to Information"
               break;
            case LAW_DEATHPENALTY:
               if(propdir[p]==1)addstr("Stop Barbaric Executions"); // was "Limit the Death Penalty"
               else addstr("Deter Criminals via Capital Punishment"); // "Expand Capital Punishment"
               break;
            case LAW_NUCLEARPOWER:
               if(propdir[p]==1)addstr("Prevent Nuclear Meltdowns"); // was "Limit Nuclear Power"
               else addstr("Promote Alternative Energy Sources"); // was "Expand Nuclear Power"
               break;
            case LAW_POLLUTION:
               if(propdir[p]==1)addstr("Protect our Environment"); // was "Punish Polluters"
               else addstr("Support American Manufacturing"); // was "Reward Industry"
               break;
            case LAW_LABOR:
               if(propdir[p]==1)addstr("Protect Workers' Rights"); // was "Enhance Labor Standards"
               else addstr("Fight Corrupt Union Thugs"); // was "End Undue Union Influence"
               break;
            case LAW_GAY:
               if(propdir[p]==1)addstr("Fight Homophobic Bigotry in our Laws"); // was "Expand Homosexual Rights"
               else addstr("Support the Sanctity of Marriage");
               break;
            case LAW_CORPORATE:
               if(propdir[p]==1)addstr("Stop Corporate Criminals");
               else addstr("Help Small Businesses Expand"); // was "Reward Small Businesses"
               break;
            case LAW_FREESPEECH:
               if(propdir[p]==1)addstr("Protect Free Speech");
               else addstr("Limit Hate Speech"); // was "Limit Hurtful Speech"
               break;
            case LAW_TAX:
               if(propdir[p]==1)addstr("Fight Income Inequality"); // was "Punish the Wealthy"
               else addstr("Cut Job-Killing Taxes"); // was "Stimulate Economic Growth"
               break;
            case LAW_FLAGBURNING:
               if(propdir[p]==1)addstr("Limit Prohibitions on Flag Burning");
               else addstr("Protect the Symbol of Our Nation");
               break;
            case LAW_GUNCONTROL:
               if(propdir[p]==1)addstr("Prevent Mass Shootings"); // was "Prevent Gun Violence"
               else addstr("Protect our Second Amendment Rights"); // was "Assert our Second Amendment Rights"
               break;
            case LAW_WOMEN:
               if(propdir[p]==1)addstr("Promote Gender Equality"); // was "Expand Women's Rights"
               else addstr("Preserve Traditional Gender Roles");
               break;
            case LAW_CIVILRIGHTS:
               if(propdir[p]==1)addstr("Promote Racial Equality"); // was "Expand Civil Rights"
               else addstr("Fight Reverse Discrimination");
               break;
            case LAW_DRUGS:
               if(propdir[p]==1)addstr("Repeal Oppressive Drug Laws"); // was "Limit Oppressive Drug Laws"
               else addstr("Fight Drug Trafficking"); // was "Strengthen the War On Drugs"
               break;
            case LAW_IMMIGRATION:
               if(propdir[p]==1)addstr("Protect Immigrant Rights");
               else addstr("Protect our Borders");
               break;
            case LAW_ELECTIONS:
               if(propdir[p]==1)addstr("Fight Political Corruption");
               else addstr("Limit Regulation of Political Speech"); // was "Limit Campaign Finance Reforms"
               break;
            case LAW_MILITARY:
               if(propdir[p]==1)addstr("Shrink the Military-Industrial Complex"); // was "Limit Military Spending"
               else addstr("Support our Troops in Defending America"); // was "Strengthen our National Defense"
               break;
            case LAW_TORTURE:
               if(propdir[p]==1)addstr("Protect Human Rights from Torture"); // was "Ban Torture Techniques"
               else addstr("Enhance Interrogations to Fight Terrorism"); // was "Permit Strong Tactics in Interrogations"
               break;
            case LAW_PRISONS:
               if(propdir[p]==1)
               {
                  if (law[LAW_PRISONS] == Alignment::LIBERAL)
                    addstr("Establish Prison Rehabilitation"); // was "Mandate Prison Rehabilitation"
                  else
                    addstr("Improve Prison Conditions"); // was "Expand Prisoners' Rights"
               }
               else
                 addstr("Enhance Prison Security"); // was "Limit Prisoners' Rights"
               break;
         }
         set_color(COLOR_WHITE,COLOR_BLACK,0);
      }
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to watch the elections unfold.");

      getkey();
   }

   for(p=0;p<pnum;p++)
   {
      bool yeswin=false,recount=false;
      int yesvotes=0,mood=publicmood(prop[p]);
      for(int l=0;l<1000;l++)
      {
         if(LCSrandom(100)<mood?propdir[p]==1:propdir[p]==-1) yesvotes++;
         /*vote=-2;
         for(int i=0;i<4;i++) if(LCSrandom(100)<mood) vote++;
         if((law[prop[p]]>vote&&propdir[p]==-1)||(law[prop[p]]<vote&&propdir[p]==1)) yesvotes++;*/

         if(l==999)
         {
            if(yesvotes>500) yeswin=true;
            else if(yesvotes==500) yeswin=(LCSrandom(100)<mood?propdir[p]==1:propdir[p]==-1),recount=true;
         }

         if(canseethings&&(l%10==9))
         {
            if((l!=999&&yesvotes>l/2)||(l==999&&yeswin)) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(yesvotes<l/2||l==999) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(p*3+2,70);
            addstr(yesvotes/10);
            addchar('.');
            addstr(yesvotes%10);
            addstr("% Yes");

            if((l!=999&&yesvotes<l/2)||(l==999&&!yeswin)) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(yesvotes>l/2||l==999) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(p*3+3,70);
            addstr((l+1-yesvotes)/10);
            addchar('.');
            addstr((l+1-yesvotes)%10);
            addstr("% No");

            pause_ms(10);
         }
      }

      if(canseethings&&recount)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(p*3+3,0);
         addstr("A Recount was Necessary");
      }

      if (yeswin)
        law[prop[p]] = shift(law[prop[p]], propdir[p]);
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to reflect on what has happened.");

      getkey();
   }
}


void
elections_senate(int senmod,bool canseethings)
{
   int mood=publicmood(LAW_MOOD);
   int stalinmood=publicmood(LAW_STALIN);
   if(canseethings)
   {
      music.play(MUSIC_ELECTIONS);
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);

      move(0,0);
      addstr("Senate Elections ");
      addstr(year);
   }

   int x=0,y=2,s=0;

   for(s=0;s<SENATENUM;s++)
   {
      if(senmod!=-1 && s%3!=senmod)continue;

      if(canseethings)
      {
         set_alignment_color(senate[s],true);
         mvaddstr(y,x,as_printable(senate[s], false));
      }

      x+=20;
      if(x>70) x=0,y++;
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to watch the elections unfold.");

      getkey();
   }

   int change[6]={0,0,0,0,0,0};
   Alignment vote;

   x=0,y=2;

   for(s=0;s<SENATENUM;s++)
   {
      if(senmod!=-1 && s%3!=senmod) continue;

      vote = Alignment::ARCH_CONSERVATIVE;
      for(int i=0; i<4; i++)
        if (mood > LCSrandom(100))
            vote = shift_left(vote);
      if (stalinmode&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100))
        vote = Alignment::STALINIST;

      change[to_index(senate[s])]--;
      if (termlimits)
        senate[s] = vote;
      else
      {
         Alignment vote2;
         bool first=true;
         do {
            vote2 = Alignment::ARCH_CONSERVATIVE;
            for (int i=0; i<4; i++)
              if (mood>LCSrandom(100))
                  vote2 = shift_left(vote2);
            if (stalinmode&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100))
              vote2 = Alignment::STALINIST;

            if (first)
              switch (law[LAW_ELECTIONS])
              {
                case Alignment::ARCH_CONSERVATIVE:
                  if( LCSrandom(3)) vote2=senate[s]; break; // 2/3 chance of incumbent winning no matter what (huge   advantage)
                case Alignment::CONSERVATIVE:
                  if( LCSrandom(2)) vote2=senate[s]; break; // 1/2 chance of incumbent winning no matter what (big    advantage)
                case  Alignment::MODERATE:
                  if(!LCSrandom(3)) vote2=senate[s]; break; // 1/3 chance of incumbent winning no matter what (medium advantage)
                case  Alignment::LIBERAL:
                  if(!LCSrandom(5)) vote2=senate[s]; break; // 1/5 chance of incumbent winning no matter what (small  advantage)
                case  Alignment::ELITE_LIBERAL:
                  if(!LCSrandom(8)) vote2=senate[s]; break; // 1/8 chance of incumbent winning no matter what (tiny   advantage)
              }
            first=false;
         } while(vote2!=senate[s]&&vote2!=vote);
         senate[s]=vote2;
      }
      change[to_index(senate[s])]++;

      if(canseethings)
      {
         set_alignment_color(senate[s],true);
         mvaddstr(y,x,"                    ");
         mvaddstr(y,x,as_printable(senate[s], false));
      }

      x+=20;
      if(x>70) x=0,y++;

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(20,0);
         addstr("Net change:");
         if(stalinmode)
         {
            addstr("   S: ");
            if(change[5]>0) addstr("+");
            addstr(change[5]);
         }
         addstr("   L+: ");
         if(change[4]>0) addstr("+");
         addstr(change[4]);
         addstr("   L: ");
         if(change[3]>0) addstr("+");
         addstr(change[3]);
         addstr("   m: ");
         if(change[2]>0) addstr("+");
         addstr(change[2]);
         addstr("   C: ");
         if(change[1]>0) addstr("+");
         addstr(change[1]);
         addstr("   C+: ");
         if(change[0]>0) addstr("+");
         addstr(change[0]);
         addstr("        ");

         if(!disbanding) pause_ms(30);
      }
   }

   if(canseethings)
   {
      move(21,0);
      Alignment winner;
      if(change[5]>0&&change[5]>change[0]+change[1]&&change[5]>change[3]+change[4]) // Stalinist increased and Stalinist gain is more than C or L side gain/loss
         winner = Alignment::STALINIST;
      else if(change[0]+change[1]>change[3]+change[4]) // C side gain/loss is more than L side gain/loss
      {
         if(change[1]<0 && mood<25)
           winner = Alignment::ARCH_CONSERVATIVE;
         else
           winner = Alignment::CONSERVATIVE;
      }
      else if(change[3]+change[4]>change[0]+change[1]) // L side gain/loss is more than C side gain/loss
      {
         if(change[3]<0 && mood>75)
           winner = Alignment::ELITE_LIBERAL;
         else
           winner = Alignment::LIBERAL;
      }
      else if(change[0]>0&&change[4]<=0&&change[5]<=0) // C+ increased and L+ and Stalinist both same or decreased
      {
         if(mood<25)
           winner = Alignment::ARCH_CONSERVATIVE;
         else
           winner = Alignment::CONSERVATIVE;
      }
      else if(change[4]>0&&change[0]<=0&&change[5]<=0) // L+ increased and C+ and Stalinist both same or decreased
      {
         if(mood>75)
           winner = Alignment::ELITE_LIBERAL;
         else
           winner = Alignment::LIBERAL;
      }
      else if(change[5]>0&&change[0]<=0&&change[4]<=0) // Stalinist increased and C+ and L+ both same or decreased
        winner = Alignment::STALINIST;
      else
        winner = Alignment::MODERATE; // nobody won
      switch(winner)
      {
      case Alignment::ARCH_CONSERVATIVE: addstr("The $$ U.S.A. Flag Eagle $$ Conservative Tea Party claims victory!"); break;
      case Alignment::CONSERVATIVE: addstr("The Conservative Party claims victory!"); break;
      case Alignment::MODERATE: addstr("The next two years promise to be more of the same."); break;
      case Alignment::LIBERAL: addstr("The Liberal Party claims victory!"); break;
      case Alignment::ELITE_LIBERAL: addstr("The Progressive Elite Social Liberal Green Party claims victory!"); break;
      case Alignment::STALINIST: addstr("The Stalinist Party claims victory!"); break;
      }

      move(22,0);
      addstr("Press any key to continue the elections.    ");

      getkey();
   }
}


void
elections_house(bool canseethings)
{
   int mood=publicmood(LAW_MOOD);
   int stalinmood=publicmood(LAW_STALIN);
   if(canseethings)
   {
      music.play(MUSIC_ELECTIONS);
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);

      move(0,0);
      addstr("House Elections ");
      addstr(year);
   }

   int x=0,y=2,h=0;

   for(h=0;h<HOUSENUM;h++)
   {
      if(canseethings)
      {
         move(y,x);

         if (house[h] == Alignment::ARCH_CONSERVATIVE)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("C+");
         }
         else if (house[h] == Alignment::CONSERVATIVE)
         {
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr("C ");
         }
         else if (house[h] == Alignment::MODERATE)
         {
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr("m ");
         }
         else if (house[h] == Alignment::LIBERAL)
         {
            set_color(COLOR_CYAN,COLOR_BLACK,1);
            addstr("L ");
         }
         else if (house[h] == Alignment::ELITE_LIBERAL)
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("L+");
         }
         else
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("S ");
         }
      }

      x+=3;
      if(x>78) x=0,y++;
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to watch the elections unfold.");

      getkey();
   }

   int change[6]={0,0,0,0,0,0};

   x=0,y=2;

   for(h=0;h<HOUSENUM;h++)
   {
      Alignment vote = Alignment::ARCH_CONSERVATIVE;
      for (int i=0; i<4; i++)
        if (mood>LCSrandom(100))
            vote = shift_left(vote);
      if(stalinmode&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100)&&stalinmood<LCSrandom(100))
        vote = Alignment::STALINIST;

      change[to_index(house[h])]--;
      if(termlimits) house[h]=vote;
      else
      {
         Alignment vote2;
         bool first=true;
         do {
            vote2 = Alignment::ARCH_CONSERVATIVE;
            for (int i=0; i<4; i++)
              if (mood>LCSrandom(100))
                  vote2 = shift_left(vote2);
            if (stalinmode && stalinmood < LCSrandom(100) && stalinmood < LCSrandom(100) && stalinmood < LCSrandom(100) && stalinmood <LCSrandom(100))
              vote2 = Alignment::STALINIST;

            if(first)
            {
              switch(law[LAW_ELECTIONS])
              {
                case Alignment::ARCH_CONSERVATIVE:
                  if (LCSrandom(3))
                    vote2 = house[h];
                  break; // 2/3 chance of incumbent winning no matter what (huge   advantage)
                case Alignment::CONSERVATIVE:
                  if (LCSrandom(2))
                    vote2 = house[h];
                  break; // 1/2 chance of incumbent winning no matter what (big    advantage)
                case  Alignment::MODERATE:
                  if (!LCSrandom(3))
                    vote2 = house[h];
                  break; // 1/3 chance of incumbent winning no matter what (medium advantage)
                case  Alignment::LIBERAL:
                  if (!LCSrandom(5))
                    vote2 = house[h];
                  break; // 1/5 chance of incumbent winning no matter what (small  advantage)
                case  Alignment::ELITE_LIBERAL:
                  if (!LCSrandom(8))
                    vote2=house[h];
                  break; // 1/8 chance of incumbent winning no matter what (tiny   advantage)
              }
            }
            first=false;
         } while(vote2!=house[h]&&vote2!=vote);
         house[h]=vote2;
      }
      change[to_index(house[h])]++;

      if(canseethings)
      {
         move(y,x);

         if (house[h] == Alignment::ARCH_CONSERVATIVE)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("C+");
         }
         else if (house[h] == Alignment::CONSERVATIVE)
         {
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr("C ");
         }
         else if (house[h] == Alignment::MODERATE)
         {
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr("m ");
         }
         else if (house[h] == Alignment::LIBERAL)
         {
            set_color(COLOR_CYAN,COLOR_BLACK,1);
            addstr("L ");
         }
         else if (house[h] == Alignment::ELITE_LIBERAL)
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("L+");
         }
         else
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("S ");
         }
      }

      x+=3;
      if(x>78) x=0,y++;

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);

         move(20,0);
         addstr("Net change:");
         if(stalinmode)
         {
            addstr("   S: ");
            if(change[5]>0) addstr("+");
            addstr(change[5]);
         }
         addstr("   L+: ");
         if(change[4]>0) addstr("+");
         addstr(change[4]);
         addstr("   L: ");
         if(change[3]>0) addstr("+");
         addstr(change[3]);
         addstr("   m: ");
         if(change[2]>0) addstr("+");
         addstr(change[2]);
         addstr("   C: ");
         if(change[1]>0) addstr("+");
         addstr(change[1]);
         addstr("   C+: ");
         if(change[0]>0) addstr("+");
         addstr(change[0]);
         addstr("        ");

         if(!disbanding) pause_ms(5);
      }
   }

   if(canseethings)
   {
      move(21,0);
      Alignment winner;
      if(change[5]>0&&change[5]>change[0]+change[1]&&change[5]>change[3]+change[4]) // Stalinist increased and Stalinist gain is more than C or L side gain/loss
         winner = Alignment::STALINIST;
      else if(change[0]+change[1]>change[3]+change[4]) // C side gain/loss is more than L side gain/loss
      {
         if(change[1]<0 && mood<25)
           winner = Alignment::ARCH_CONSERVATIVE;
         else
           winner = Alignment::CONSERVATIVE;
      }
      else if(change[3]+change[4]>change[0]+change[1]) // L side gain/loss is more than C side gain/loss
      {
         if (change[3]<0 && mood>75)
           winner = Alignment::ELITE_LIBERAL;
         else
           winner = Alignment::LIBERAL;
      }
      else if(change[0]>0&&change[4]<=0&&change[5]<=0) // C+ increased and L+ and Stalinist both same or decreased
      {
         if(mood<25)
           winner = Alignment::ARCH_CONSERVATIVE;
         else
           winner = Alignment::CONSERVATIVE;
      }
      else if(change[4]>0&&change[0]<=0&&change[5]<=0) // L+ increased and C+ and Stalinist both same or decreased
      {
         if(mood>75)
           winner = Alignment::ELITE_LIBERAL;
         else
           winner = Alignment::LIBERAL;
      }
      else if(change[5]>0&&change[0]<=0&&change[4]<=0) // Stalinist increased and C+ and L+ both same or decreased
        winner = Alignment::STALINIST;
      else
        winner = Alignment::MODERATE; // nobody won
      switch(winner)
      {
      case Alignment::ARCH_CONSERVATIVE: addstr("The $$ U.S.A. Flag Eagle $$ Conservative Tea Party claims victory!"); break;
      case Alignment::CONSERVATIVE: addstr("The Conservative Party claims victory!"); break;
      case Alignment::MODERATE: addstr("The next two years promise to be more of the same."); break;
      case Alignment::LIBERAL: addstr("The Liberal Party claims victory!"); break;
      case Alignment::ELITE_LIBERAL: addstr("The Progressive Elite Social Liberal Green Party claims victory!"); break;
      case Alignment::STALINIST: addstr("The Stalinist Party claims victory!"); break;
      }
      if(!disbanding)
      {
         move(22,0);
         addstr("Press any key to continue the elections.    ");

         getkey();
      }
      else pause_ms(800);
   }
}


/**
 * Cause the supreme court to hand down decisions.
 */
void
supremecourt(bool clearformess, bool canseethings)
{
   int c;
   if(canseethings)
   {
      music.play(MUSIC_ELECTIONS);
      if(clearformess) erase();
      else makedelimiter();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(8,1);
      addstr("The Supreme court is handing down decisions!");

      getkey();
   }

   //CHANGE THINGS AND REPORT
   if(canseethings)
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);

      move(0,0);
      addstr("Supreme Court Watch ");
      addstr(year);

      set_color(COLOR_WHITE,COLOR_BLACK,0);
   }

   vector<int> scase, scasedir;
   int cnum=LCSrandom(5)+2, bias=0;
   char lawtaken[LAWNUM];
   memset(lawtaken,0,LAWNUM*sizeof(char));

   scase.resize(cnum);
   scasedir.resize(cnum);
   for(c=0;c<cnum;c++)
   {
      do
      {
         scase[c]=LCSrandom(LAWNUM);
      } while(lawtaken[scase[c]]);

      lawtaken[scase[c]]=1;

      //Constitutional bias -- free speech, flag burning issues, supreme court
      //is extra liberal, gun control, supreme court is extra conservative
      if(scase[c]==LAW_FREESPEECH || scase[c]==LAW_FLAGBURNING)bias=1;
      else if(scase[c]==LAW_GUNCONTROL)bias=-1;
      else bias=0;

      if (law[scase[c]] == Alignment::ELITE_LIBERAL)
        scasedir[c]=-1;
      else if (law[scase[c]] == Alignment::ARCH_CONSERVATIVE)
        scasedir[c]=1;
      else
      {
         if(bias)scasedir[c]=bias;
         else if(!LCSrandom(2))scasedir[c]=1;
         else scasedir[c]=-1;
      }

      if(canseethings)
      {
         move(c*3+2,0);
         char name1[80], name2[80];
         if(!LCSrandom(5)) strcpy(name1,"United States");
         else lastname(name1);

         lastname(name2);
         if((scase[c]==LAW_LABOR||scase[c]==LAW_CORPORATE||
            scase[c]==LAW_ANIMALRESEARCH||scase[c]==LAW_POLLUTION)&&
            !LCSrandom(5))
         {
            lastname(name2,true);
            switch(LCSrandom(5))
            {
            case 0:strcat(name2,", Inc.");break;
            case 1:strcat(name2,", L.L.C.");break;
            case 2:strcat(name2," Corp.");break;
            case 3:strcat(name2," Co.");break;
            case 4:strcat(name2,", Ltd.");break;
            }
         }

         addstr(name1);
         addstr(" vs. ");
         addstr(name2);

         move(c*3+3,0);
         addstr("A Decision could ");
         if(scasedir[c]==1)set_color(COLOR_GREEN,COLOR_BLACK,1);
         else set_color(COLOR_RED,COLOR_BLACK,1);
         switch(scase[c])
         {
            case LAW_ABORTION:
               if(scasedir[c]==1)addstr("Strengthen Reproductive Freedom"); // was "Strengthen Abortion Rights"
               else addstr("Protect the Unborn Child");
               break;
            case LAW_ANIMALRESEARCH:
               if(scasedir[c]==1)addstr("Limit Animal Cruelty");
               else addstr("Help Scientists Cure Diseases"); // was "Expand Animal Research"
               break;
            case LAW_POLICEBEHAVIOR:
               if(scasedir[c]==1)addstr("Curtail Police Misconduct");
               else addstr("Help Police Fight Crime"); // was "Stop Harassment of Police Officers"
               break;
            case LAW_PRIVACY:
               if(scasedir[c]==1)addstr("Enhance Privacy Protection");
               else addstr("Promote Transparency and Accountability"); // was "Allow Corporations Access to Information"
               break;
            case LAW_DEATHPENALTY:
               if(scasedir[c]==1)addstr("Stop Barbaric Executions"); // was "Limit the Death Penalty"
               else addstr("Deter Criminals via Capital Punishment"); // "Expand Capital Punishment"
               break;
            case LAW_NUCLEARPOWER:
               if(scasedir[c]==1)addstr("Prevent Nuclear Meltdowns"); // was "Limit Nuclear Power"
               else addstr("Promote Alternative Energy Sources"); // was "Expand Nuclear Power"
               break;
            case LAW_POLLUTION:
               if(scasedir[c]==1)addstr("Protect our Environment"); // was "Punish Polluters"
               else addstr("Support American Manufacturing"); // was "Reward Industry"
               break;
            case LAW_LABOR:
               if(scasedir[c]==1)addstr("Protect Workers' Rights"); // was "Enhance Labor Standards"
               else addstr("Fight Corrupt Union Thugs"); // was "End Undue Union Influence"
               break;
            case LAW_GAY:
               if(scasedir[c]==1)addstr("Fight Homophobic Bigotry in our Laws"); // was "Expand Homosexual Rights"
               else addstr("Support the Sanctity of Marriage");
               break;
            case LAW_CORPORATE:
               if(scasedir[c]==1)addstr("Stop Corporate Criminals");
               else addstr("Help Small Businesses Expand"); // was "Reward Small Businesses"
               break;
            case LAW_FREESPEECH:
               if(scasedir[c]==1)addstr("Protect Free Speech");
               else addstr("Limit Hate Speech"); // was "Limit Hurtful Speech"
               break;
            case LAW_TAX:
               if(scasedir[c]==1)addstr("Fight Income Inequality"); // was "Punish the Wealthy"
               else addstr("Cut Job-Killing Taxes"); // was "Stimulate Economic Growth"
               break;
            case LAW_FLAGBURNING:
               if(scasedir[c]==1)addstr("Limit Prohibitions on Flag Burning");
               else addstr("Protect the Symbol of Our Nation");
               break;
            case LAW_GUNCONTROL:
               if(scasedir[c]==1)addstr("Prevent Mass Shootings"); // was "Prevent Gun Violence"
               else addstr("Protect our Second Amendment Rights"); // was "Assert our Second Amendment Rights"
               break;
            case LAW_WOMEN:
               if(scasedir[c]==1)addstr("Promote Gender Equality"); // was "Expand Women's Rights"
               else addstr("Preserve Traditional Gender Roles");
               break;
            case LAW_CIVILRIGHTS:
               if(scasedir[c]==1)addstr("Promote Racial Equality"); // was "Expand Civil Rights"
               else addstr("Fight Reverse Discrimination");
               break;
            case LAW_DRUGS:
               if(scasedir[c]==1)addstr("Repeal Oppressive Drug Laws"); // was "Limit Oppressive Drug Laws"
               else addstr("Fight Drug Trafficking"); // was "Strengthen the War On Drugs"
               break;
            case LAW_IMMIGRATION:
               if(scasedir[c]==1)addstr("Protect Immigrant Rights");
               else addstr("Protect our Borders");
               break;
            case LAW_ELECTIONS:
               if(scasedir[c]==1)addstr("Fight Political Corruption");
               else addstr("Limit Regulation of Political Speech"); // was "Limit Campaign Finance Reforms"
               break;
            case LAW_MILITARY:
               if(scasedir[c]==1)addstr("Shrink the Military-Industrial Complex"); // was "Limit Military Spending"
               else addstr("Support our Troops in Defending America"); // was "Strengthen our National Defense"
               break;
            case LAW_TORTURE:
               if(scasedir[c]==1)addstr("Protect Human Rights from Torture"); // was "Ban Torture Techniques"
               else addstr("Enhance Interrogations to Fight Terrorism"); // was "Permit Strong Tactics in Interrogations"
               break;
            case LAW_PRISONS:
               if(scasedir[c]==1)
               {
                  if (law[LAW_PRISONS] == Alignment::LIBERAL)
                    addstr("Establish Prison Rehabilitation"); // was "Mandate Prison Rehabilitation"
                  else
                    addstr("Improve Prison Conditions"); // was "Expand Prisoners' Rights"
               }
               else
                 addstr("Enhance Prison Security"); // was "Limit Prisoners' Rights"
               break;
         }
         set_color(COLOR_WHITE,COLOR_BLACK,0);

         refresh();
      }
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to watch the decisions unfold.");

      getkey();
   }

   for(c=0;c<cnum;c++)
   {
      char yeswin=0;
      int yesvotes=0;
      Alignment vote;
      //Constitutional bias -- free speech, flag burning issues, supreme court
      //is extra liberal, gun control, supreme court is extra conservative
      //"All court justices will vote according to alignment and bias and do not consult
      //popular opinion...---Servant Corps"
      if(scase[c]==LAW_FREESPEECH||scase[c]==LAW_FLAGBURNING) bias=1;
      else if(scase[c]==LAW_GUNCONTROL) bias=-1;
      else bias=0;

      for(int l=0;l<COURTNUM;l++)
      {
         vote = court[l];
         if (vote == Alignment::STALINIST)
         {
            if(stalinview(scase[c],true))
              vote = Alignment::ELITE_LIBERAL;
            else
              vote = Alignment::ARCH_CONSERVATIVE;
         }
         if (!is_extreme(vote))
           vote = shift(vote, bias);

         if (to_left_of(law[scase[c]], vote) && scasedir[c] == -1) yesvotes++;
         if (to_right_of(law[scase[c]], vote) && scasedir[c] == 1) yesvotes++;

         if(l==COURTNUM-1&&yesvotes>=COURTMAJORITY) yeswin=1;

         if(canseethings)
         {
            if(l==COURTNUM-1&&yeswin)set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==COURTNUM-1)set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+2,63);
            addstr(yesvotes);
            addstr(" for Change");

            if(l==COURTNUM-1&&!yeswin)set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==COURTNUM-1)set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+3,63);
            addstr(l+1-yesvotes);
            addstr(" for Status Quo");

            pause_ms(60);
         }
      }

      if (yeswin)
        law[scase[c]] = shift(law[scase[c]], scasedir[c]);
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to reflect on what has happened.");

      getkey();
   }

   //CHANGE A JUSTICE 40% OF THE TIME
   if(LCSrandom(10)>=6)
   {
      if(canseethings)
      {
         erase();

         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(0,0);
         addstr("Changing the Guard!");
      }

      int j=LCSrandom(COURTNUM);

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(2,0);
         addstr("Justice ");
         addstr(courtname[j]);
         addstr(", ");
         addstr(as_printable(court[j], false));
         addstr(", is stepping down.");

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(7,0);
         addstr("Press any key to see what happens.");

         getkey();
      }

      float president = (exec[EXEC_PRESIDENT] == Alignment::STALINIST ? -3.0f : float(to_index(exec[EXEC_PRESIDENT]) - 2));
      float sen=0;
      for (int s=0; s<100; s++)
        sen += (senate[s] == Alignment::STALINIST ? -3.0f : float(to_index(senate[s]) - 2));
      sen /= 100.0f;

      float consensus=(president+sen)*.5f;

      if(consensus<-2.1f) court[j]=Alignment::STALINIST;
      else if(consensus<-1.5f) court[j]=Alignment::ARCH_CONSERVATIVE;
      else if(consensus<-.5f) court[j]=Alignment::CONSERVATIVE;
      else if(consensus<.5f) court[j]=Alignment::MODERATE;
      else if(consensus<1.5f) court[j]=Alignment::LIBERAL;
      else court[j]=Alignment::ELITE_LIBERAL;

      do
      {
         if(court[j]==Alignment::ARCH_CONSERVATIVE)
            generate_name(courtname[j],GENDER_WHITEMALEPATRIARCH);
         else generate_name(courtname[j]);
      } while(len(courtname[j])>20);

      if(canseethings)
      {
         move(4,0);
         addstr("After much debate and televised testimony, a new justice,");
         move(5,0);
         if(court[j]==Alignment::STALINIST)
           addstr("Comrade ");
         else
           addstr("the Honorable ");
         addstr(courtname[j]);
         addstr(", ");
         addstr(as_printable(court[j], false));
         addstr(", is appointed to the bench.");

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(7,0);
         addstr("Press any key to reflect on what has happened.");

         getkey();
      }
   }
}


enum BillStatus
{
   BILL_SIGNED=-2,
   BILL_OVERRIDE_VETO=-1,
   BILL_PASSED_CONGRESS=0,
   BILL_FAILED=1
};


/**
 * Determine a politician's vote in congress.
 *
 * Some politicians listen to public opinion, but no politician will radically
 * deviate from their alignment.  More extreme politicians are less likely to
 * deviate from their views. Moderates always consult public opinion.
 */
static Alignment
determine_politician_vote(Alignment alignment, int law)
{
   Alignment vote = alignment;
   int mood=publicmood(law);
   if (vote==Alignment::STALINIST)
   {
      // Stalinist -- Will not accept public opinion
      if (stalinview(law,true))
        vote = Alignment::ELITE_LIBERAL;
      else
        vote = Alignment::ARCH_CONSERVATIVE;
   }
   else if (is_extreme(vote))
   {
      // Extremist -- Damn public opinion, I'm doing what I think is right
   }
   else if (vote == Alignment::CONSERVATIVE || vote == Alignment::LIBERAL)
   {
      // Partisan -- Listens to public opinion, but won't accept opposing views
      vote = Alignment::ARCH_CONSERVATIVE;
      for (int i=0; i<4; i++)
        if (LCSrandom(100) < mood)
          vote = shift_left(vote);
      if (diff(vote, alignment) > 1)
        vote = Alignment::MODERATE;
   }
   else
   {
      // Moderate -- Listens to public opinion, but won't accept either extreme
      vote = Alignment::ARCH_CONSERVATIVE;
      for (int i=0; i<4; i++)
        if (LCSrandom(100) < mood)
          vote = shift_left(vote);
      if (vote == Alignment::ARCH_CONSERVATIVE)
        vote = Alignment::CONSERVATIVE;
      else if (vote == Alignment::ELITE_LIBERAL)
        vote = Alignment::LIBERAL;
   }
   return vote;
}


/**
 * Causes congress to act on legislation.
 */
void
congress(bool clearformess, bool canseethings)
{
   int l, c;
   if(canseethings)
   {
      music.play(MUSIC_ELECTIONS);
      if(clearformess) erase();
      else makedelimiter();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(8,1);
      addstr("Congress is acting on legislation!");

      getkey();
   }

   //CHANGE THINGS AND REPORT
   if(canseethings)
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);

      move(0,0);
      addstr("Legislative Agenda ");
      addstr(year);
   }

   vector<int> bill,billdir,killbill;
   int cnum=LCSrandom(3)+1;
   char lawtaken[LAWNUM];
   memset(lawtaken,0,LAWNUM*sizeof(char));

   int lawpriority[LAWNUM];
   memset(lawpriority,0,LAWNUM*sizeof(int));
   char lawdir[LAWNUM];
   memset(lawdir,0,LAWNUM*sizeof(char));

   //DETERMINE BILLS
   int pup,pdown,pprior;
   for(l=0;l<LAWNUM;l++)
   {
      pup=0,pdown=0,pprior=0;

      // Consult House
      for(int cl=0; cl<HOUSENUM; cl++)
      {
         Alignment housealign = house[cl];
         if (housealign == Alignment::STALINIST)
         {
            if (stalinview(l,true))
              housealign = Alignment::ELITE_LIBERAL;
            else
              housealign = Alignment::ARCH_CONSERVATIVE;
         }
         if (to_right_of(law[l], housealign))
           pup++;
         else if (to_left_of(law[l], housealign))
           pdown++;
         pprior += diff(housealign, law[l]);
      }

      // Consult Senate
      for (int sl=0; sl<SENATENUM; sl++)
      {
         Alignment senatealign = senate[sl];
         if (senatealign==Alignment::STALINIST)
         {
            if(stalinview(l,true))
              senatealign = Alignment::ELITE_LIBERAL;
            else
              senatealign = Alignment::ARCH_CONSERVATIVE;
         }
         if (to_right_of(law[l], senatealign))
           pup += 4;
         else if (to_left_of(law[l], senatealign))
           pdown += 4;
         pprior += diff(senatealign, law[l]) * 4;
      }

      // Consult Public Opinion
      int mood=publicmood(l);
      Alignment public_position = Alignment::ARCH_CONSERVATIVE;
      for(int i=0; i<4; i++)
        if (10+20*i<mood)
            public_position = shift_left(public_position);
      if (to_right_of(law[l], public_position))
        pup += 600;
      if (to_left_of(law[l], public_position))
        pdown += 600;
      pprior += diff(public_position, law[l]) * 600;

      if (pup > pdown)
        lawdir[l] = 1;
      else if (pup == pdown)
        lawdir[l] = LCSrandom(2)*2-1;
      else
        lawdir[l]=-1;

      if (law[l] == Alignment::ARCH_CONSERVATIVE)
        lawdir[l] = 1;
      if (law[l] == Alignment::ELITE_LIBERAL)
        lawdir[l] = -1;

      //CALC PRIORITY
      lawpriority[l]=pprior;
   }

   vector<int> canlaw;

   bill.resize(cnum);
   billdir.resize(cnum);
   killbill.resize(cnum);
   for(c=0;c<cnum;c++)
   {
      killbill[c]=BILL_PASSED_CONGRESS;

      int maxprior=0;
      for(l=0;l<LAWNUM;l++)
         if(lawpriority[l]>maxprior&&!lawtaken[l])maxprior=lawpriority[l];
      canlaw.clear();
      for(l=0;l<LAWNUM;l++)
         if(lawpriority[l]==maxprior&&!lawtaken[l])
            canlaw.push_back(l);

      bill[c]=pickrandom(canlaw);

      lawtaken[bill[c]]=1;

      billdir[c]=lawdir[bill[c]];

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(c*3+2,0);
         addstr("Joint Resolution ");
         addstr(year);
         addchar('-');
         addstr(c+1);

         move(c*3+3,0);
         addstr("To ");
         if(billdir[c]==1)set_color(COLOR_GREEN,COLOR_BLACK,1);
         else set_color(COLOR_RED,COLOR_BLACK,1);
         switch(bill[c])
         {
            case LAW_ABORTION:
               if(billdir[c]==1) addstr("Strengthen Reproductive Freedom"); // was "Strengthen Abortion Rights"
               else addstr("Protect the Unborn Child");
               break;
            case LAW_ANIMALRESEARCH:
               if(billdir[c]==1) addstr("Limit Animal Cruelty");
               else addstr("Help Scientists Cure Diseases"); // was "Expand Animal Research"
               break;
            case LAW_POLICEBEHAVIOR:
               if(billdir[c]==1) addstr("Curtail Police Misconduct");
               else addstr("Help Police Fight Crime"); // was "Stop Harassment of Police Officers"
               break;
            case LAW_PRIVACY:
               if(billdir[c]==1) addstr("Enhance Privacy Protection");
               else addstr("Promote Transparency and Accountability"); // was "Allow Corporations Access to Information"
               break;
            case LAW_DEATHPENALTY:
               if(billdir[c]==1) addstr("Stop Barbaric Executions"); // was "Limit the Death Penalty"
               else addstr("Deter Criminals via Capital Punishment"); // "Expand Capital Punishment"
               break;
            case LAW_NUCLEARPOWER:
               if(billdir[c]==1) addstr("Prevent Nuclear Meltdowns"); // was "Limit Nuclear Power"
               else addstr("Promote Alternative Energy Sources"); // was "Expand Nuclear Power"
               break;
            case LAW_POLLUTION:
               if(billdir[c]==1) addstr("Protect our Environment"); // was "Punish Polluters"
               else addstr("Support American Manufacturing"); // was "Reward Industry"
               break;
            case LAW_LABOR:
               if(billdir[c]==1) addstr("Protect Workers' Rights"); // was "Enhance Labor Standards"
               else addstr("Fight Corrupt Union Thugs"); // was "End Undue Union Influence"
               break;
            case LAW_GAY:
               if(billdir[c]==1) addstr("Fight Homophobic Bigotry in our Laws"); // was "Expand Homosexual Rights"
               else addstr("Support the Sanctity of Marriage");
               break;
            case LAW_CORPORATE:
               if(billdir[c]==1) addstr("Stop Corporate Criminals");
               else addstr("Help Small Businesses Expand"); // was "Reward Small Businesses"
               break;
            case LAW_FREESPEECH:
               if(billdir[c]==1) addstr("Protect Free Speech");
               else addstr("Limit Hate Speech"); // was "Limit Hurtful Speech"
               break;
            case LAW_TAX:
               if(billdir[c]==1) addstr("Fight Income Inequality"); // was "Punish the Wealthy"
               else addstr("Cut Job-Killing Taxes"); // was "Stimulate Economic Growth"
               break;
            case LAW_FLAGBURNING:
               if(billdir[c]==1) addstr("Limit Prohibitions on Flag Burning");
               else addstr("Protect the Symbol of Our Nation");
               break;
            case LAW_GUNCONTROL:
               if(billdir[c]==1) addstr("Prevent Mass Shootings"); // was "Prevent Gun Violence"
               else addstr("Protect our Second Amendment Rights"); // was "Assert our Second Amendment Rights"
               break;
            case LAW_WOMEN:
               if(billdir[c]==1) addstr("Promote Gender Equality"); // was "Expand Women's Rights"
               else addstr("Preserve Traditional Gender Roles");
               break;
            case LAW_CIVILRIGHTS:
               if(billdir[c]==1) addstr("Promote Racial Equality"); // was "Expand Civil Rights"
               else addstr("Fight Reverse Discrimination");
               break;
            case LAW_DRUGS:
               if(billdir[c]==1) addstr("Repeal Oppressive Drug Laws"); // was "Limit Oppressive Drug Laws"
               else addstr("Fight Drug Trafficking"); // was "Strengthen the War On Drugs"
               break;
            case LAW_IMMIGRATION:
               if(billdir[c]==1) addstr("Protect Immigrant Rights");
               else addstr("Protect our Borders");
               break;
            case LAW_ELECTIONS:
               if(billdir[c]==1) addstr("Fight Political Corruption");
               else addstr("Limit Regulation of Political Speech"); // was "Limit Campaign Finance Reforms"
               break;
            case LAW_MILITARY:
               if(billdir[c]==1) addstr("Shrink the Military-Industrial Complex"); // was "Limit Military Spending"
               else addstr("Support our Troops in Defending America"); // was "Strengthen our National Defense"
               break;
            case LAW_TORTURE:
               if(billdir[c]==1) addstr("Protect Human Rights from Torture"); // was "Ban Torture Techniques"
               else addstr("Enhance Interrogations to Fight Terrorism"); // was "Permit Strong Tactics in Interrogations"
               break;
            case LAW_PRISONS:
               if (billdir[c] == 1)
               {
                  if (law[LAW_PRISONS] == Alignment::LIBERAL)
                    addstr("Establish Prison Rehabilitation"); // was "Mandate Prison Rehabilitation"
                  else
                    addstr("Improve Prison Conditions"); // was "Expand Prisoners' Rights"
               }
               else
                 addstr("Enhance Prison Security"); // was "Limit Prisoners' Rights"
               break;
         }
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         refresh();
      }
   }

   if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("Press any key to watch the votes unfold.");

      getkey();

      move(0,62);
      addstr("House");

      move(0,70);
      addstr("Senate");
   }

   for(c=0;c<cnum;c++)
   {
      char yeswin_h=0, yeswin_s=0;
      int yesvotes_h=0, yesvotes_s=0;
      int s=0;
      Alignment vote;

      for(int l=0;l<HOUSENUM;l++)
      {
         vote = determine_politician_vote(house[l],bill[c]);

         if(law[bill[c]]>vote&&billdir[c]==-1) yesvotes_h++;
         if(law[bill[c]]<vote&&billdir[c]==1) yesvotes_h++;

         if(l==HOUSENUM-1)
         {
            if(yesvotes_h>=HOUSEMAJORITY) yeswin_h=1;
            if(yesvotes_h>=HOUSESUPERMAJORITY) killbill[c]=BILL_OVERRIDE_VETO;
         }

         if(canseethings)
         {
            if(l==HOUSENUM-1&&yeswin_h) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==HOUSENUM-1) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+2,62);
            addstr(yesvotes_h);
            addstr(" Yea");

            if(l==HOUSENUM-1&&!yeswin_h) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==HOUSENUM-1) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+3,62);
            addstr(l+1-yesvotes_h);
            addstr(" Nay");
         }

         if(l%4==0&&s<SENATENUM)
         {
            vote = determine_politician_vote(senate[s++], bill[c]);

            if (to_left_of(law[bill[c]], vote) && billdir[c]==-1) yesvotes_s++;
            if (to_right_of(law[bill[c]], vote) && billdir[c]==1) yesvotes_s++;
         }

         if(l==HOUSENUM-1)
         {
            if(yesvotes_s>=SENATEMAJORITY) yeswin_s=1;
            if(yesvotes_s<SENATESUPERMAJORITY&&killbill[c]==BILL_OVERRIDE_VETO) killbill[c]=BILL_PASSED_CONGRESS;
            if(yesvotes_s==SENATEMAJORITY-1)
            {  //TIE BREAKER
               Alignment vote;
               if (!is_extreme(exec[EXEC_VP]) || !is_extreme(exec[EXEC_PRESIDENT]))
               {
                std::size_t sum = to_index(exec[EXEC_PRESIDENT]) // only consult Cabinet and random number generator if President
                                + to_index(exec[EXEC_VP])        // or VP is not an extremist.
                                + to_index(exec[EXEC_STATE])
                                + to_index(exec[EXEC_ATTORNEY])
                                + LCSrandom(9)
                                - 4;
                std::size_t avg = sum / 5;
                vote = shift_left(Alignment::ARCH_CONSERVATIVE, avg);
               }
               else
               {
                 vote = exec[EXEC_VP];
                 if (vote == Alignment::STALINIST)
                 {
                    if (stalinview(bill[c],true))
                      vote = Alignment::ELITE_LIBERAL;
                    else
                      vote = Alignment::ARCH_CONSERVATIVE;
                 }
               }

               if (to_left_of(law[bill[c]], vote) && billdir[c] == -1) yeswin_s=1;
               if (to_right_of(law[bill[c]], vote) && billdir[c] == 1) yeswin_s=1;

               //ASSURED SIGNING BY PRESIDENT IF VP VOTED YES
               if(yeswin_s) killbill[c]=BILL_SIGNED;
            }
         }

         if(canseethings)
         {
            if(l==HOUSENUM-1&&yeswin_s) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==HOUSENUM-1) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+2,70);
            addstr(yesvotes_s);
            addstr(" Yea");

            if(l==HOUSENUM-1&&yesvotes_s==SENATEMAJORITY-1&&yeswin_s)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(c*3+2,78);
               addstr("VP");
            }

            if(l==HOUSENUM-1&&!yeswin_s) set_color(COLOR_WHITE,COLOR_BLACK,1);
            else if(l==HOUSENUM-1) set_color(COLOR_BLACK,COLOR_BLACK,1);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(c*3+3,70);
            addstr(s-yesvotes_s);
            addstr(" Nay");

            if(l==HOUSENUM-1&&yesvotes_s==SENATEMAJORITY-1&&!yeswin_s)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(c*3+3,78);
               addstr("VP");
            }

            if(l%5==0) pause_ms(5);
         }
      }

      if(!yeswin_h||!yeswin_s) killbill[c]=BILL_FAILED;
   }

   int havebill=0;
   for(c=0;c<cnum;c++) if(killbill[c]!=BILL_FAILED) havebill++;

   if(havebill)
   {
      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(23,0);
         addstr("Press any key to watch the President.                   ");

         getkey();

         move(0,35);
         addstr("President");

         pause_ms(500);
      }

      for(int c=0;c<len(bill);c++)
      {
         if (killbill[c] != BILL_FAILED)
         {
            Alignment vote;
            if (!is_extreme(exec[EXEC_PRESIDENT]))
            {
              std::size_t sum = to_index(exec[EXEC_PRESIDENT]) // only consult Cabinet and random number generator if President
                              + to_index(exec[EXEC_VP])        // is not an extremist.
                              + to_index(exec[EXEC_STATE])
                              + to_index(exec[EXEC_ATTORNEY])
                              + LCSrandom(9)
                              - 4;
              std::size_t avg = sum / 5;
              vote = shift_left(Alignment::ARCH_CONSERVATIVE, avg);
            }
            else
            {
               vote = exec[EXEC_PRESIDENT];
               if (vote == Alignment::STALINIST)
               {
                  if(stalinview(bill[c],true))
                    vote = Alignment::ELITE_LIBERAL;
                  else
                    vote = Alignment::ARCH_CONSERVATIVE;
               }
            }

            if ((law[bill[c]] > vote && billdir[c] == -1) || (law[bill[c]] < vote && billdir[c] == 1))
              killbill[c] = BILL_SIGNED;
         }

         if(canseethings)
         {
            move(c*3+2,35);
            if(killbill[c]==BILL_SIGNED)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               addstr(execname[EXEC_PRESIDENT]);
            }
            else if(killbill[c]==BILL_FAILED)
            {
               set_color(COLOR_BLACK,COLOR_BLACK,1);
               addstr("Dead in Congress");
            }
            else if(killbill[c]==BILL_OVERRIDE_VETO)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               addstr("FORCED BY CONGRESS");
            }
            else
            {
               set_color(COLOR_RED,COLOR_BLACK,1);
               addstr("*** VETO ***");
            }

            pause_ms(500);
         }

         if (killbill[c]==BILL_SIGNED||killbill[c]==BILL_OVERRIDE_VETO)
           law[bill[c]] = shift(law[bill[c]], billdir[c]);
      }

      if(canseethings)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(23,0);
         addstr("Press any key to reflect on what has happened.    ");

         getkey();
      }
   }
   else if(canseethings)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(23,0);
      addstr("None of the items made it to the President's desk.");
      move(24,0);
      addstr("Press any key to reflect on what has happened.    ");

      getkey();
   }

   //CONGRESS CONSTITUTION CHANGES
   int housemake[6]={0,0,0,0,0,0};
   for(int h=0; h<HOUSENUM; h++)
     housemake[to_index(house[h])]++;

   int senatemake[6]={0,0,0,0,0,0};
   for(int s=0; s<SENATENUM; s++)
     senatemake[to_index(senate[s])]++;

   // Throw out non-L+ Justices?
   bool tossj=false;
   for(int j=0; j<COURTNUM; j++)
     if(court[j]!=Alignment::ELITE_LIBERAL) tossj=true;
   if(housemake[4]+housemake[3]/2>=HOUSESUPERMAJORITY&&senatemake[4]+senatemake[3]/2>=SENATESUPERMAJORITY&&tossj&&!nocourtpurge)
      tossjustices(canseethings);

   // Purge Congress, implement term limits, and hold new elections?
   if((housemake[4]+housemake[3]/2<HOUSESUPERMAJORITY||senatemake[4]+senatemake[3]/2<SENATESUPERMAJORITY)&&publicmood(-1)>80&&!notermlimit)
      amendment_termlimits(canseethings);

   // LET ARCH-CONSERVATIVES REPEAL THE CONSTITUTION AND LOSE THE GAME?
   if(housemake[0]>=HOUSESUPERMAJORITY&&senatemake[0]>=SENATESUPERMAJORITY)
      reaganify(canseethings);

   // LET STALINISTS REPEAL THE CONSTITUTION AND LOSE THE GAME?
   if(housemake[5]>=HOUSESUPERMAJORITY&&senatemake[5]>=SENATESUPERMAJORITY)
      stalinize(canseethings);
}


/**
 * Checks if the game is won.
 */
char
wincheck()
{
   for(int e=0;e<EXECNUM;e++) if(exec[e]<Alignment::ELITE_LIBERAL) return 0;

   if(wincondition==WINCONDITION_ELITE) for(int l=0;l<LAWNUM;l++) if(law[l]<Alignment::ELITE_LIBERAL) return 0;
   else
   {
      int liberalLaws=0,eliteLaws=0;
      for(int l=0;l<LAWNUM;l++)
      {
         if(law[l]<Alignment::LIBERAL) return 0;
         if(law[l]==Alignment::LIBERAL) liberalLaws++;
         else eliteLaws++;
      }
      if(eliteLaws<liberalLaws) return 0;
   }

   int housemake[6]={0,0,0,0,0,0};
   for(int h=0; h<HOUSENUM; h++)
     housemake[to_index(house[h])]++;
   if(housemake[4]+housemake[3]/2<((wincondition==WINCONDITION_ELITE)?HOUSESUPERMAJORITY:HOUSECOMFYMAJORITY)) return 0; // Elite Libs plus half of Libs >= 3/5 for easy, 2/3 for elite
   if(housemake[4]<((wincondition==WINCONDITION_ELITE)?HOUSECOMFYMAJORITY:HOUSEMAJORITY)) return 0; // Elite Libs themselves >= 1/2 for easy, 3/5 for elite

   int senatemake[6]={0,0,0,0,0,0};
   for(int s=0; s<SENATENUM; s++)
     senatemake[to_index(senate[s])]++;
   if(senatemake[4]+senatemake[3]/2<((wincondition==WINCONDITION_ELITE)?SENATESUPERMAJORITY:SENATECOMFYMAJORITY)) return 0; // Elite Libs plus half of Libs >= 3/5 for easy, 2/3 for elite
   if (wincondition != WINCONDITION_ELITE)
     senatemake[to_index(exec[EXEC_VP])]++; // VP counts as Senator only for breaking ties (so counts for 1/2 fraction but not higher fractions)
   if(senatemake[4]<((wincondition==WINCONDITION_ELITE)?SENATECOMFYMAJORITY:SENATEMAJORITY)) return 0; // Elite Libs themselves >= 1/2 for easy, 3/5 for elite

   int elibjudge=0,libjudge=0;
   for(int c=0; c<COURTNUM; c++)
   {
      if(court[c]==Alignment::ELITE_LIBERAL) elibjudge++;
      if(court[c]==Alignment::LIBERAL) libjudge++;
   }
   if(elibjudge<COURTMAJORITY&&(wincondition==WINCONDITION_ELITE||elibjudge+libjudge/2<COURTSUPERMAJORITY)) return 0;

   return 1;
}

/*      FIXED:
                At the present time, VIEW_CIVILRIGHTS has far too much sway.
                However, before this was the case, as an example, LAW_ABORTION
                and LAW_WOMEN, had the same "return attitude[]" attribute, and
                both returned VIEW_WOMEN. What I think is needed, is some sort
                of indicators for things such as:
                        1) Desire for change, which could be based on the
                                approval rate of the president.
                        2) Liberal, Conservative, and Stalinist percentage of
                                the country, i.e. how much support each party
                                has.
                        3) VIEW_HUMANRIGHTS, which should be based off of the
                                two previous things, (i.e. A higher liberal
                                percentage, and a higher desire for change would
                                make VIEW_HUMANRIGHTS get closer to 100.)

        -- LiteralKa

        ADDENDUM (20090812): Keeping this for historical purposes, and to
                possibly improve future changes to this issue.
*/
/*
FIXME, PART1:
        LAW_HUMANRIGHTS is added as a sort of an indicator, but it relies on
                all the other Human Rights issue, rather than affecting the
                issues to be more "pro-Human Rights". Essentially, if you
                support Gay Rights but not Abortion Rights, you will not be
                considered as someone who supports 'human rights'.

        ---Servant Corps

/////                                                                      /////

@Servant:
        As it stands, (revision 316) the only alarming thing is that this may
                influence LAW_*s that are affected by LAW_HUMANRIGHTS. This is
                only mildly alarming because the LAW_* itself is effected, and
                not the attitude[VIEW_*].
        -- LiteralKa
*/

/**
 * Check the prevailing attitude on a specific law, or overall.
 */
int
publicmood(int l)
{
   switch(l)
   {  // All laws should be affected by exactly one issue if there is a direct
      // correlation between that law and an issue. For example, police behavior
      // as a law should depend only upon police behavior as an issue. This keeps
      // the game logical to the player and ensures that the public opinion polls
      // displayed in-game accurately predict how people will vote in specific
      // issues. For a handful of laws, we might not have a directly correlating
      // issue; for example, as of this writing, there is no issue asking people's
      // opinions on torture. In this case, we can use the nearest issue, or we
      // can mix two closely related ones. As a general principle, try to avoid
      // getting too complicated here; this is under-the-hood stuff the player
      // will never appreciate, so it should be kept as simple and transparent as
      // possible so as to avoid creating unexpected results that will only confuse
      // players, like people refusing to further regulate nuclear power because
      // one of the other issues besides nuclear power is conservative, even when
      // the nuclear power issue is 100% Liberal. - Jonathan S. Fox
      case LAW_ABORTION: return attitude[VIEW_WOMEN]; //XXX: There is no ``VIEW_ABORTION''!
      case LAW_ANIMALRESEARCH: return attitude[VIEW_ANIMALRESEARCH];
      case LAW_POLICEBEHAVIOR: return attitude[VIEW_POLICEBEHAVIOR];
      case LAW_PRIVACY: return attitude[VIEW_INTELLIGENCE];
      case LAW_DEATHPENALTY: return attitude[VIEW_DEATHPENALTY];
      case LAW_NUCLEARPOWER: return attitude[VIEW_NUCLEARPOWER];
      case LAW_POLLUTION: return attitude[VIEW_POLLUTION];
      case LAW_LABOR: return attitude[VIEW_SWEATSHOPS];
      case LAW_GAY: return attitude[VIEW_GAY];
      case LAW_CORPORATE: return (attitude[VIEW_CORPORATECULTURE] + attitude[VIEW_CEOSALARY])/2; // <-- We'll be merging these two views here because there is no CEO salary law.
      case LAW_FREESPEECH: return attitude[VIEW_FREESPEECH];                                     // The issue is there for flavor, and falls under the same umbrella of
      case LAW_TAX: return attitude[VIEW_TAXES];                                                 // corporate regulation. - Jonathan S. Fox
      case LAW_FLAGBURNING: return attitude[VIEW_FREESPEECH];  // <-- I'm keeping this pure free speech instead of free speech
      case LAW_WOMEN: return attitude[VIEW_WOMEN];             // plus political violence. Ideologically, there's no association
      case LAW_CIVILRIGHTS: return attitude[VIEW_CIVILRIGHTS]; // between flag burning and violence. - Jonathan S. Fox
      case LAW_DRUGS: return attitude[VIEW_DRUGS];
      case LAW_IMMIGRATION: return attitude[VIEW_IMMIGRATION]; //XXX: VIEW_DRUGS?
      case LAW_MILITARY: return attitude[VIEW_MILITARY];
      case LAW_TORTURE: return attitude[VIEW_TORTURE];
      case LAW_GUNCONTROL: return attitude[VIEW_GUNCONTROL];
      case LAW_PRISONS: return attitude[VIEW_PRISONS];
      case LAW_STALIN:
         l=0;
         for(int v=0;v<VIEWNUM-3;v++)
            if(stalinview(v,false)) l+=100-attitude[v];
            else l+=attitude[v];
         return l/(VIEWNUM-3);
      case LAW_ELECTIONS:
      case LAW_MOOD:
      default: //eg. -1
         l=0;
         for(int v=0;v<VIEWNUM-3;v++) l+=attitude[v];
         return l/(VIEWNUM-3);
   }
}

/**
 * Get the Stalinist view of a law or view.
 *
 * Returns true if Stalinists agree with Elite Liberals on a view/law, false if
 * they strongly disagree with libs the input bool islaw, if true, returns
 * Stalinist opinion on laws, if false, returns Stalinist opinion on views
 */
bool
stalinview(short view,bool islaw)
{
   if(islaw) switch(view)
   {
   case LAW_STALIN: return false; // Liberals and Stalinists don't get along
   case LAW_MOOD: return false; // Liberals and Stalinists don't get along
   case LAW_ABORTION: return true; // Stalinists agree that abortion is good, although technically they don't let women choose
   case LAW_ANIMALRESEARCH: return false; // Stalinists are in favor of unethical research
   case LAW_POLICEBEHAVIOR: return false; // Stalinists use police for brutal repression
   case LAW_PRIVACY: return false; // Stalinists don't believe in privacy
   case LAW_DEATHPENALTY: return false; // Stalinists execute lots of people
   case LAW_NUCLEARPOWER: return false; // Stalinists believe the more nuclear, the better
   case LAW_POLLUTION: return false; // Stalinists don't care about pollution
   case LAW_LABOR: return true; // Stalinists say, Workers of the world unite!
   case LAW_GAY: return false; // Stalinists discriminate against gay people
   case LAW_CORPORATE: return true; // Stalinists hate rich people and corporations
   case LAW_FREESPEECH: return false; // Stalinists don't allow any dissent
   case LAW_FLAGBURNING: return true; // Stalinists regularly burn flags
   case LAW_GUNCONTROL: return true; // Stalinists don't want any armed resistance
   case LAW_TAX: return true; // Stalinists support communist income redistribution
   case LAW_WOMEN: return false; // Stalinists discriminate against women
   case LAW_CIVILRIGHTS: return false; // Stalinists discriminate against ethnic groups
   case LAW_DRUGS: return false; // Stalinists only allow vodka
   case LAW_IMMIGRATION: return false; // Stalinists maintained tight border security at the Iron Curtain
   case LAW_ELECTIONS: return false; // Stalinists don't even have elections
   case LAW_MILITARY: return false; // Stalinists use the military for brutal repression
   case LAW_PRISONS: return false; // Stalinists annex Canada to fill it with gulags
   case LAW_TORTURE: return false; // Stalinists torture their enemies
   case LAWNUM: return false; // Liberals and Stalinists don't get along
   default: return false; // Liberals and Stalinists don't get along
   }
   else switch(view)
   {
   case VIEW_STALIN: return false; // Liberals and Stalinists don't get along
   case VIEW_MOOD: return false; // Liberals and Stalinists don't get along
   case VIEW_GAY: return false; // Stalinists discriminate against gay people
   case VIEW_DEATHPENALTY: return false; // Stalinists execute lots of people
   case VIEW_TAXES: return true; // Stalinists support communist income redistribution
   case VIEW_NUCLEARPOWER: return false; // Stalinists believe the more nuclear, the better
   case VIEW_ANIMALRESEARCH: return false; // Stalinists are in favor of unethical research
   case VIEW_POLICEBEHAVIOR: return false; // Stalinists use police for brutal repression
   case VIEW_TORTURE: return false; // Stalinists torture their enemies
   case VIEW_INTELLIGENCE: return false; // Stalinists don't believe in privacy
   case VIEW_FREESPEECH: return false; // Stalinists don't allow any dissent
   case VIEW_GENETICS: return false; // Stalinists are in favor of unethical research
   case VIEW_JUSTICES: return false; // Liberals and Stalinists don't get along
   case VIEW_GUNCONTROL: return true; // Stalinists don't want any armed resistance
   case VIEW_SWEATSHOPS: return true; // Stalinists say, Workers of the world unite!
   case VIEW_POLLUTION: return false; // Stalinists don't care about pollution
   case VIEW_CORPORATECULTURE: return true; // Stalinists hate rich people and corporations
   case VIEW_CEOSALARY: return true; // Stalinists hate rich people and corporations
   case VIEW_WOMEN: return false; // Stalinists discriminate against women
   case VIEW_CIVILRIGHTS: return false; // Stalinists discriminate against ethnic groups
   case VIEW_DRUGS: return false; // Stalinists only allow vodka
   case VIEW_IMMIGRATION: return false; // Stalinists maintained tight border security at the Iron Curtain
   case VIEW_MILITARY: return false; // Stalinists use the military for brutal repression
   case VIEW_PRISONS: return false; // Stalinists annex Canada to fill it with gulags
   case VIEW_AMRADIO: return true; // Stalinists agree that Conservatives are bad
   case VIEW_CABLENEWS: return true; // Stalinists agree that Conservatives are bad
   //case VIEW_POLITICALVIOLENCE: return true; // the LCS and Stalinists both like using political violence
   case VIEW_LIBERALCRIMESQUAD: return false; // Liberals and Stalinists don't get along
   case VIEW_LIBERALCRIMESQUADPOS: return false; // Liberals and Stalinists don't get along
   case VIEW_CONSERVATIVECRIMESQUAD: return true; // Stalinists agree that Conservatives are bad
   case VIEWNUM: return false; // Liberals and Stalinists don't get along
   default: return false; // Liberals and Stalinists don't get along
   }
}

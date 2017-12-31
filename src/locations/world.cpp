/*
 * Copyright (c) 2002,2003,2004 by Tarn Adams
 * Copyright 2009, 2013 Jonathan Stickles  <jonathansfox@users.sourceforge.net>
 * Copyright 2013, 2014 Rich McGrew  (yetisyny)
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
#include "locations/world.h"

#include <algorithm>
#include "externs.h"
#include <iostream> // temporary for warning message  @TODO remove me
#include "log/log.h"
#include "tinyxml2.h"


#define City(X)                  location.push_back(city = new Location(X)); \
                                 city->id = id++;

#define District(X, Y)           if(city) district = city->addchild(X); \
                                 else location.push_back(district = new Location(X)); \
                                 district->id = id++; \
                                 district->area = Y;

#define DistrictProperty(X, Y)   district->X = Y;
#define DistrictName(Y)          strcpy(district->name, Y);
#define DistrictShortname(Y)     strcpy(district->shortname, Y);

#define Site(X)                  site = district->addchild(X); \
                                 site->id = id++;

#define SiteProperty(X, Y)       site->X = Y;
#define SiteName(Y)              strcpy(site->name, Y);
#define SiteShortname(Y)         strcpy(site->shortname, Y);


namespace
{
  std::string const WORLD_XML_ELEMENT{"world"};
  std::string const WORLD_XML_CITY_ELEMENT{"city"};
  std::string const WORLD_XML_CURRENT_CITY_ELEMENT{"current_city"};
  std::string const CITY_XML_NAME_ATTRIBUTE{"name"};

  std::string const default_city_xml{"<world>"
                                       "<city name=\"DEFAULT\">"
                                         "<description>Default City</description>"
                                         "<districts>"
                                           "<district idname=\"DOWNTOWN\">"
                                           "</district>"
                                           "<district idname=\"COMMERCIAL\">"
                                           "</district>"
                                           "<district idname=\"UNIVERSITY\">"
                                           "</district>"
                                           "<district idname=\"INDUSTRIAL\">"
                                           "</district>"
                                           "<district idname=\"OUTOFTOWN\">"
                                           "</district>"
                                         "</districts>"
                                       "</city>"
                                       "<current_city>DEFAULT</current_city>"
                                     "</world>"};
} // anonymous namespace


Location* find_site_by_id(int id)
{
   for(int i=0;i<len(location);i++)
      if(location[i]->id==id)
         return location[i];
   return NULL;
}


Location* find_site_in_city(int site_type, int city)
{
   int i=find_site_index_in_city(site_type,city);
   if(i!=-1) return location[i];
   else return NULL;
}


int find_site_index_in_city(int site_type, int city)
{
   for(int i=0;i<len(location);i++)
      if(location[i]->type==site_type&&(!multipleCityMode||city==-1||location[i]->city==city))
         return i;
   return -1;
}


int
find_site_index_in_same_city(int site_type, int site_index)
{
   int city=-1;
   if (site_index >= 0)
     city = location[site_index]->city;
   return find_site_index_in_city(site_type, city);
}


int
find_site_index_in_same_city(int site_type, Creature const& cr)
{ return find_site_index_in_same_city(site_type, cr.location); }


int
find_police_station(int site_index)
{ return find_site_index_in_same_city(SITE_GOVERNMENT_POLICESTATION, site_index); }


int
find_police_station(const Creature& cr)
{ return find_police_station(cr.location); }


int
find_clinic(int site_index)
{ return find_site_index_in_same_city(SITE_HOSPITAL_CLINIC, site_index); }


int
find_clinic(const Creature& cr)
{ return find_clinic(cr.location); }


int
find_homeless_shelter(int site_index)
{ return find_site_index_in_same_city(SITE_RESIDENTIAL_SHELTER, site_index); }


int
find_homeless_shelter(const Creature& cr)
{ return find_homeless_shelter(cr.location); }


int
find_courthouse(int site_index)
{ return find_site_index_in_same_city(SITE_GOVERNMENT_COURTHOUSE, site_index); }


int
find_courthouse(const Creature& cr)
{ return find_courthouse(cr.location); }


int
find_hospital(int site_index)
{ return find_site_index_in_same_city(SITE_HOSPITAL_UNIVERSITY, site_index); }


int
find_hospital(const Creature& cr) 
{ return find_hospital(cr.location); }


void make_classic_world(bool hasmaps)
{
   Location* city = NULL;
   Location* district = NULL;
   Location* site = NULL;
   int id = 0;

   District(SITE_DOWNTOWN, 0)
      DistrictProperty(mapped, hasmaps) // for some reason this property isn't inherited by downtown locations so it's manually added for each one, need to debug why this happens
      Site(SITE_RESIDENTIAL_APARTMENT_UPSCALE)
         SiteProperty(mapped, hasmaps)
      Site(SITE_GOVERNMENT_POLICESTATION)
         SiteProperty(mapped, hasmaps)
      Site(SITE_GOVERNMENT_COURTHOUSE)
         SiteProperty(mapped, hasmaps)
      Site(SITE_BUSINESS_BANK)
         SiteProperty(mapped, hasmaps)
      Site(SITE_GOVERNMENT_FIRESTATION)
         SiteProperty(mapped, hasmaps)
      Site(SITE_MEDIA_AMRADIO)
         SiteProperty(mapped, hasmaps)
      Site(SITE_MEDIA_CABLENEWS)
         SiteProperty(mapped, hasmaps)
      Site(SITE_BUSINESS_CIGARBAR)
         SiteProperty(mapped, hasmaps)
      Site(SITE_BUSINESS_LATTESTAND)
         SiteProperty(mapped, hasmaps)
      Site(SITE_BUSINESS_BARANDGRILL)
         SiteProperty(renting, RENTING_CCS)
         SiteProperty(hidden, true)
         SiteProperty(mapped, false)
   District(SITE_COMMERCIAL, 0)
      Site(SITE_BUSINESS_DEPTSTORE)
      Site(SITE_BUSINESS_PAWNSHOP)
      Site(SITE_BUSINESS_HALLOWEEN)
      Site(SITE_BUSINESS_CARDEALERSHIP)
   District(SITE_UDISTRICT, 0)
      Site(SITE_RESIDENTIAL_APARTMENT)
      Site(SITE_HOSPITAL_UNIVERSITY)
      Site(SITE_HOSPITAL_CLINIC)
      Site(SITE_LABORATORY_GENETIC)
      Site(SITE_LABORATORY_COSMETICS)
      Site(SITE_BUSINESS_VEGANCOOP)
      Site(SITE_BUSINESS_JUICEBAR)
      Site(SITE_BUSINESS_INTERNETCAFE)
      Site(SITE_OUTDOOR_PUBLICPARK)
   District(SITE_INDUSTRIAL, 0)
      Site(SITE_RESIDENTIAL_SHELTER)
         SiteProperty(renting, RENTING_PERMANENT)
      Site(SITE_INDUSTRY_WAREHOUSE)
         SiteProperty(renting, RENTING_PERMANENT)
         SiteProperty(upgradable, true)
      Site(SITE_RESIDENTIAL_TENEMENT)
      Site(SITE_INDUSTRY_POLLUTER)
      Site(SITE_INDUSTRY_SWEATSHOP)
      Site(SITE_BUSINESS_CRACKHOUSE)
         SiteProperty(upgradable, true)
      Site(SITE_RESIDENTIAL_BOMBSHELTER)
         SiteProperty(renting, RENTING_CCS)
         SiteProperty(hidden, true)
   District(SITE_OUTOFTOWN, 1)
      Site(SITE_GOVERNMENT_PRISON)
      Site(SITE_GOVERNMENT_INTELLIGENCEHQ)
      Site(SITE_INDUSTRY_NUCLEAR)
      Site(SITE_CORPORATE_HEADQUARTERS)
      Site(SITE_CORPORATE_HOUSE)
      Site(SITE_GOVERNMENT_ARMYBASE)
      Site(SITE_OUTDOOR_BUNKER)
         SiteProperty(renting, RENTING_CCS)
         SiteProperty(hidden, true)
   District(SITE_TRAVEL, 1)
      Site(SITE_GOVERNMENT_WHITE_HOUSE)
}


void make_world(bool hasmaps)
{
   if(!multipleCityMode)
   {
      make_classic_world(hasmaps);
      return;
   }

   //MAKE LOCATIONS
   Location* city = NULL;
   Location* district = NULL;
   Location* site = NULL;
   int id = 0;

   // Seattle
   City(SITE_CITY_SEATTLE)
      District(SITE_DOWNTOWN, 0)
         DistrictProperty(mapped, hasmaps) // for some reason this property isn't inherited by downtown locations so it's manually added for each one, need to debug why this happens
         Site(SITE_RESIDENTIAL_APARTMENT_UPSCALE)
            SiteProperty(mapped, hasmaps)
         Site(SITE_GOVERNMENT_POLICESTATION)
            SiteProperty(mapped, hasmaps)
         Site(SITE_GOVERNMENT_COURTHOUSE)
            SiteProperty(mapped, hasmaps)
         Site(SITE_BUSINESS_BANK)
            SiteProperty(mapped, hasmaps)
         Site(SITE_GOVERNMENT_FIRESTATION)
            SiteProperty(mapped, hasmaps)
         Site(SITE_MEDIA_AMRADIO)
            SiteProperty(mapped, hasmaps)
         Site(SITE_BUSINESS_CIGARBAR)
            SiteProperty(mapped, hasmaps)
         Site(SITE_BUSINESS_LATTESTAND)
            SiteProperty(mapped, hasmaps)
         Site(SITE_BUSINESS_DEPTSTORE)
            SiteProperty(mapped, hasmaps)
         Site(SITE_BUSINESS_BARANDGRILL)
            SiteProperty(renting, RENTING_CCS)
            SiteProperty(hidden, true)
            SiteProperty(mapped, false)
      District(SITE_UDISTRICT, 0)
         Site(SITE_RESIDENTIAL_APARTMENT)
         Site(SITE_HOSPITAL_UNIVERSITY)
         Site(SITE_HOSPITAL_CLINIC)
         Site(SITE_LABORATORY_GENETIC)
         Site(SITE_LABORATORY_COSMETICS)
         Site(SITE_BUSINESS_VEGANCOOP)
         Site(SITE_BUSINESS_JUICEBAR)
         Site(SITE_BUSINESS_INTERNETCAFE)
         Site(SITE_OUTDOOR_PUBLICPARK)
         Site(SITE_BUSINESS_HALLOWEEN)
      District(SITE_INDUSTRIAL, 0)
         Site(SITE_RESIDENTIAL_SHELTER)
            SiteProperty(renting, RENTING_PERMANENT)
         Site(SITE_INDUSTRY_WAREHOUSE)
            SiteProperty(renting, RENTING_PERMANENT)
            SiteProperty(upgradable, true)
         Site(SITE_RESIDENTIAL_TENEMENT)
         Site(SITE_INDUSTRY_POLLUTER)
         Site(SITE_INDUSTRY_SWEATSHOP)
         Site(SITE_BUSINESS_CRACKHOUSE)
            SiteProperty(upgradable, true)
         Site(SITE_BUSINESS_PAWNSHOP)
         Site(SITE_BUSINESS_CARDEALERSHIP)
      District(SITE_OUTOFTOWN, 1)
         Site(SITE_GOVERNMENT_PRISON)
         Site(SITE_GOVERNMENT_INTELLIGENCEHQ)
         //Site(SITE_INDUSTRY_NUCLEAR)
         Site(SITE_CORPORATE_HEADQUARTERS)
         //Site(SITE_CORPORATE_HOUSE)
         Site(SITE_GOVERNMENT_ARMYBASE)
   // New York City
   City(SITE_CITY_NEW_YORK)
      District(SITE_DOWNTOWN, 0)
         DistrictName("Manhattan Island")
         DistrictShortname("Manhattan")
         //DistrictProperty(mapped, hasmaps)
         Site(SITE_RESIDENTIAL_APARTMENT_UPSCALE)
         Site(SITE_GOVERNMENT_POLICESTATION)
         Site(SITE_GOVERNMENT_COURTHOUSE)
         Site(SITE_BUSINESS_BANK)
         Site(SITE_CORPORATE_HEADQUARTERS)
         Site(SITE_MEDIA_AMRADIO)
         Site(SITE_MEDIA_CABLENEWS)
         Site(SITE_BUSINESS_CIGARBAR)
         //Site(SITE_CORPORATE_HOUSE)
         //Site(SITE_GOVERNMENT_INTELLIGENCEHQ)
         Site(SITE_OUTDOOR_PUBLICPARK)
         Site(SITE_BUSINESS_DEPTSTORE)
         Site(SITE_GOVERNMENT_PRISON)
      District(SITE_UDISTRICT, 0)
         DistrictName("Brooklyn & Queens")
         DistrictShortname("Long Island")
         Site(SITE_INDUSTRY_WAREHOUSE)
            SiteProperty(renting, RENTING_PERMANENT)
            SiteProperty(upgradable, true)
         Site(SITE_RESIDENTIAL_APARTMENT)
         Site(SITE_GOVERNMENT_FIRESTATION)
         Site(SITE_HOSPITAL_UNIVERSITY)
         Site(SITE_HOSPITAL_CLINIC)
         Site(SITE_BUSINESS_JUICEBAR)
         Site(SITE_BUSINESS_INTERNETCAFE)
         Site(SITE_INDUSTRY_POLLUTER)
         Site(SITE_LABORATORY_GENETIC)
         Site(SITE_GOVERNMENT_ARMYBASE)
         Site(SITE_RESIDENTIAL_BOMBSHELTER)
            SiteProperty(renting, RENTING_CCS)
            SiteProperty(hidden, true)
      District(SITE_INDUSTRIAL, 0)
         DistrictName("The Bronx")
         DistrictShortname("The Bronx")
         Site(SITE_RESIDENTIAL_SHELTER)
            SiteProperty(renting, RENTING_PERMANENT)
         Site(SITE_RESIDENTIAL_TENEMENT)
         Site(SITE_INDUSTRY_POLLUTER)
         Site(SITE_INDUSTRY_SWEATSHOP)
         Site(SITE_LABORATORY_COSMETICS)
         Site(SITE_BUSINESS_VEGANCOOP)
         Site(SITE_BUSINESS_PAWNSHOP)
         Site(SITE_BUSINESS_CARDEALERSHIP)
         Site(SITE_BUSINESS_CRACKHOUSE)
            SiteProperty(upgradable, true)
         Site(SITE_OUTDOOR_PUBLICPARK)
      District(SITE_OUTOFTOWN, 1)
         Site(SITE_INDUSTRY_NUCLEAR)
   // Los Angeles
   City(SITE_CITY_LOS_ANGELES)
      District(SITE_DOWNTOWN, 0)
         //DistrictProperty(mapped, hasmaps)
         Site(SITE_RESIDENTIAL_SHELTER)
            SiteProperty(renting, RENTING_PERMANENT)
         Site(SITE_RESIDENTIAL_APARTMENT)
         Site(SITE_GOVERNMENT_POLICESTATION)
         Site(SITE_GOVERNMENT_COURTHOUSE)
         Site(SITE_BUSINESS_BANK)
         Site(SITE_GOVERNMENT_FIRESTATION)
         Site(SITE_CORPORATE_HEADQUARTERS)
         Site(SITE_HOSPITAL_UNIVERSITY)
         Site(SITE_BUSINESS_DEPTSTORE)
      District(SITE_UDISTRICT, 0)
         DistrictName("Greater Hollywood")
         DistrictShortname("Hollywood")
         Site(SITE_RESIDENTIAL_APARTMENT_UPSCALE)
         Site(SITE_BUSINESS_VEGANCOOP)
         Site(SITE_BUSINESS_HALLOWEEN)
         Site(SITE_BUSINESS_CIGARBAR)
         //Site(SITE_MEDIA_CABLENEWS)
         Site(SITE_MEDIA_AMRADIO)
         Site(SITE_OUTDOOR_PUBLICPARK)
         Site(SITE_CORPORATE_HOUSE)
      District(SITE_INDUSTRIAL, 0)
         DistrictName("Seaport Area")
         DistrictShortname("Seaport")
         Site(SITE_INDUSTRY_WAREHOUSE)
            SiteProperty(renting, RENTING_PERMANENT)
            SiteProperty(upgradable, true)
         Site(SITE_RESIDENTIAL_TENEMENT)
         Site(SITE_HOSPITAL_CLINIC)
         Site(SITE_LABORATORY_GENETIC)
         Site(SITE_LABORATORY_COSMETICS)
         Site(SITE_INDUSTRY_POLLUTER)
         Site(SITE_BUSINESS_PAWNSHOP)
         Site(SITE_INDUSTRY_SWEATSHOP)
         Site(SITE_BUSINESS_CARDEALERSHIP)
         Site(SITE_BUSINESS_CRACKHOUSE)
            SiteProperty(upgradable, true)
      District(SITE_OUTOFTOWN, 1)
         DistrictName("Outskirts & Orange County")
         Site(SITE_GOVERNMENT_PRISON)
         //Site(SITE_GOVERNMENT_INTELLIGENCEHQ)
         Site(SITE_INDUSTRY_NUCLEAR)
         Site(SITE_GOVERNMENT_ARMYBASE)
         Site(SITE_OUTDOOR_BUNKER)
            SiteProperty(renting, RENTING_CCS)
            SiteProperty(hidden, true)
   // Washington, DC
   City(SITE_CITY_WASHINGTON_DC)
      District(SITE_DOWNTOWN, 0)
         DistrictName("Downtown")
         //DistrictProperty(mapped, hasmaps)
         Site(SITE_GOVERNMENT_POLICESTATION)
         Site(SITE_GOVERNMENT_FIRESTATION)
         Site(SITE_GOVERNMENT_COURTHOUSE)
         Site(SITE_BUSINESS_BANK)
         Site(SITE_BUSINESS_CARDEALERSHIP)
         Site(SITE_HOSPITAL_CLINIC)
         Site(SITE_HOSPITAL_UNIVERSITY)
         Site(SITE_BUSINESS_DEPTSTORE)
         Site(SITE_RESIDENTIAL_SHELTER)
            SiteProperty(renting, RENTING_PERMANENT)
      District(SITE_UDISTRICT, 0)
         DistrictName("National Mall")
         DistrictShortname("Mall")
         Site(SITE_OUTDOOR_PUBLICPARK)
         Site(SITE_GOVERNMENT_WHITE_HOUSE)
      District(SITE_OUTOFTOWN, 1)
         DistrictName("Arlington")
         Site(SITE_GOVERNMENT_PRISON)
         Site(SITE_GOVERNMENT_INTELLIGENCEHQ)
         Site(SITE_GOVERNMENT_ARMYBASE)

   //City(SITE_CITY_CHICAGO);
   //City(SITE_CITY_DETROIT);
   //City(SITE_CITY_ATLANTA);
   //City(SITE_CITY_MIAMI);
}


World::
World(TypeCache const& type_cache)
{
  if (!multipleCityMode)
  {
    this->load_from_xml(type_cache, default_city_xml);
  }
  else
  {
    std::cerr << "WARNING multi-city mode is not yet supported.\n";
  }
}


void World::
load_from_xml(TypeCache const& type_cache, std::string const& xml)
{
  std::string current_city_name;

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError err = doc.Parse(xml.c_str());
  if (err != tinyxml2::XML_SUCCESS)
  {
    const char* err = doc.GetErrorStr1();
    if (err)
    {
      xmllog.log(err);
    }
    else
    {
      xmllog.log("unknown error loading World from XML");
    }
    return;
  }

  auto e = doc.FirstChildElement();
  if ((e != nullptr) && (e->Name() == WORLD_XML_ELEMENT))
  {
    for (auto element = e->FirstChildElement(); element; element = element->NextSiblingElement())
    {
      std::string const tag{element->Name()};
      if (tag == WORLD_XML_CITY_ELEMENT)
      {
        const char* city_name = element->Attribute(CITY_XML_NAME_ATTRIBUTE.c_str());
        if (city_name == nullptr)
        {
          xmllog.log("city name attribute required");
          continue;
        }

        tinyxml2::XMLPrinter printer;
        element->Accept(&printer);

        City* city = this->find_city_by_name(city_name);
        if (city)
        {
          city->load_from_xml(type_cache, printer.CStr());
        }
        else
        {
          this->city_.emplace_back();
          this->city_.back().load_from_xml(type_cache, printer.CStr());
        }
      }
      else if (tag == WORLD_XML_CURRENT_CITY_ELEMENT)
      {
        char const* val = element->GetText();
        if (val)
        {
          current_city_name = val;
        }
      }
    }
  }

  if (current_city_name.length() > 0)
  {
    this->current_city_ = this->find_city_by_name(current_city_name);
  }
}


World::CityIterator World::
cities_begin() const
{ return this->city_.begin(); }


World::CityIterator World::
cities_end() const
{ return this->city_.end(); }


City* World::
find_city_by_name(std::string const& name)
{
  auto it = std::find_if(this->city_.begin(), this->city_.end(), [name](City const& city)
            {
              return city.name() == name;
            });
  if (it != cities_end())
    return &(*it);
  return nullptr;
}


City* World::
current_city() const
{ return this->current_city_; }


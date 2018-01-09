/**
 * Implementation of the LocationTypeCache component.
 */
/*
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
#include "locations/locationtypecache.h"

#include "log/log.h"
#include "tinyxml2.h"


namespace
{
  const std::string LOCATIONTYPECACHE_XML_ELEMENT{"locations"};

  void
  init_hardcoded_locations(LocationTypeCache* dtc)
  {
    dtc->load_from_xml("<" + LOCATIONTYPECACHE_XML_ELEMENT + ">"
                        "<locationtype idname=\"RESIDENTIAL_APARTMENT\">"
                          "<name>Residential Apartment</name>"
                        "</locationtype>"
                        "<locationtype idname=\"HOSPITAL_UNIVERSITY\">"
                          "<name>University Hospital</name>"
                        "</locationtype>"
                        "<locationtype idname=\"HOSPITAL_CLINIC\">"
                          "<name>Free Clinic</name>"
                        "</locationtype>"
                        "<locationtype idname=\"BUSINESS_VEGAN_COOP\">"
                          "<name>Vegan Co-op</name>"
                        "</locationtype>"
                        "<locationtype idname=\"BUSINESS_JUICEBAR\">"
                          "<name>Vegan Juice Bar</name>"
                        "</locationtype>"
                       "</" + LOCATIONTYPECACHE_XML_ELEMENT + ">");
  }
} // anonymous namespace


LocationTypeCache::
LocationTypeCache()
{
  init_hardcoded_locations(this);
}


LocationTypeCache::
~LocationTypeCache()
{ }


void LocationTypeCache::
load_from_xml(std::string const& xml)
{
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError err = doc.Parse(xml.c_str());
  if (err != tinyxml2::XML_SUCCESS)
  {
    const char* err = doc.GetErrorStr1();
    if (err)
      xmllog.log(err);
    else
      xmllog.log("unknown error loading LocationTypeCache from XML");
    return;
  }

  for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
  {
    if (element->Name() == LOCATIONTYPECACHE_XML_ELEMENT)
    {
      for (auto e = element->FirstChildElement(); e; e = e->NextSiblingElement())
      {
        this->location_type_bag_.push_back(LocationType());
        tinyxml2::XMLPrinter printer;
        e->Accept( &printer );
        this->location_type_bag_.back().load_from_xml(printer.CStr());
      }
    }
  }
}


LocationType const* LocationTypeCache::
get_by_idname(std::string const& idname) const
{
  for (auto const& d: location_type_bag_)
  {
    if (d.idname() == idname)
    {
      return &d;
    }
  }
  return nullptr;
}



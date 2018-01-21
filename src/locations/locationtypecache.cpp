/**
 * Implementation of the LocationTypeCache component.
 */
/*
 * Copyright 2017,2018 Stephen M. Webb  <stephen.webb@bregmasoft.ca>
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

#include <algorithm>
#include "locations/locationtype.h"
#include "log/log.h"
#include "tinyxml2.h"
#include "typecache.h"


namespace
{
  const std::string LOCATIONTYPECACHE_XML_ELEMENT{"locations"};

  void
  init_hardcoded_locations(TypeCache& type_cache, LocationTypeCache* dtc)
  {
    dtc->load_from_xml(type_cache,
                       "<" + LOCATIONTYPECACHE_XML_ELEMENT + ">"
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
LocationTypeCache(TypeCache& type_cache)
{
  init_hardcoded_locations(type_cache, this);
}


LocationTypeCache::
~LocationTypeCache()
{ }


LocationType const* LocationTypeCache::
load_from_xml(TypeCache& type_cache, std::string const& xml)
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
    return nullptr;
  }

  for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
  {
    // Loading a single LocationType
    if (element->Name() == LocationType::LOCATIONTYPE_XML_ELEMENT)
    {
      LocationType* lt = nullptr;
      const char* val = element->Attribute(LocationType::LOCATIONTYPE_XML_IDNAME_ATTRIBUTE.c_str());
      if (val)
      {
        auto it = std::find_if(std::begin(this->location_type_bag_), std::end(this->location_type_bag_),
                              [&val](LocationType const& rhs){ return val == rhs.idname(); });
        if (it != std::end(this->location_type_bag_))
        {
          lt = &*it;
        }
      }

      // Otherwise, create a new location type in the cache and use it.
      if (!lt)
      {
        this->location_type_bag_.push_back(LocationType());
        lt = &this->location_type_bag_.back();
      }

      lt->load_from_xml(xml);
      return lt;
    }

    // Loading a collection of LocationTypes
    else if (element->Name() == LOCATIONTYPECACHE_XML_ELEMENT)
    {
      for (auto e = element->FirstChildElement(); e; e = e->NextSiblingElement())
      {
        LocationType* lt = nullptr;

        // If an idname is given for the location type and that idname is
        // already in the cache, use it.
        const char* val = e->Attribute(LocationType::LOCATIONTYPE_XML_IDNAME_ATTRIBUTE.c_str());
        if (val)
        {
          auto it = std::find_if(std::begin(this->location_type_bag_), std::end(this->location_type_bag_),
                                [&val](LocationType const& rhs){ return val == rhs.idname(); });
          if (it != std::end(this->location_type_bag_))
          {
            lt = &*it;
          }
        }

        // Otherwise, create a new location type in the cache and use it.
        if (!lt)
        {
          this->location_type_bag_.push_back(LocationType());
          lt = &this->location_type_bag_.back();
        }

        tinyxml2::XMLPrinter printer;
        e->Accept(&printer);
        lt->load_from_xml(printer.CStr());
      }
    }
  }
  return nullptr;
}


LocationType const* LocationTypeCache::
get_by_idname(std::string const& idname) const
{
  auto it = std::find_if(std::begin(this->location_type_bag_), std::end(this->location_type_bag_),
                        [&idname](LocationType const& rhs){ return idname == rhs.idname(); });
  if (it != std::end(this->location_type_bag_))
  {
    return &*it;
  }
  return nullptr;
}


LocationType const* LocationTypeCache::
clone_by_idname(std::string const& idname)
{
  LocationType const* lt = nullptr;
  auto it = std::find_if(std::begin(this->location_type_bag_), std::end(this->location_type_bag_),
                        [&idname](LocationType const& rhs){ return idname == rhs.idname(); });
  if (it != std::end(this->location_type_bag_))
  {
    lt = it->clone();
    this->location_type_bag_.push_back(*lt); // ugh @TODO move to using unique_ptr instead of by-value
    delete lt;
    lt = &this->location_type_bag_.back();
  }
  return lt;
}


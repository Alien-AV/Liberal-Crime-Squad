/**
 * Implementation of the DistrictTypeCache component.
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
#include "locations/districttypecache.h"

#include "log/log.h"
#include "tinyxml2.h"
#include "typecache.h"


namespace
{
  const std::string DISTRICTTYPECACHE_XML_ELEMENT{"districts"};

  void
  init_hardcoded_districts(TypeCache& type_cache, DistrictTypeCache* dtc)
  {
    dtc->load_from_xml(type_cache,
                       "<" + DISTRICTTYPECACHE_XML_ELEMENT + ">"
                        "<districttype idname=\"DOWNTOWN\">"
                          "<name>Downtown</name>"
                        "</districttype>"
                        "<districttype idname=\"COMMERCIAL\">"
                          "<name>Commercial</name>"
                        "</districttype>"
                        "<districttype idname=\"INDUSTRIAL\">"
                          "<name>Industrial</name>"
                        "</districttype>"
                        "<districttype idname=\"OUTOFTOWN\">"
                          "<name>Out of Town</name>"
                        "</districttype>"
                        "<districttype idname=\"UNIVERSITY\">"
                          "<name>University</name>"
                        "</districttype>"
                       "</" + DISTRICTTYPECACHE_XML_ELEMENT + ">");
  }
} // anonymous namespace


DistrictTypeCache::
DistrictTypeCache(TypeCache& type_cache)
{
  init_hardcoded_districts(type_cache, this);
}


DistrictTypeCache::
~DistrictTypeCache()
{ }


void DistrictTypeCache::
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
      xmllog.log("unknown error loading DistrictTypeCache from XML");
    return;
  }

  for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
  {
    if (element->Name() == DISTRICTTYPECACHE_XML_ELEMENT)
    {
      for (auto e = element->FirstChildElement(); e; e = e->NextSiblingElement())
      {
        this->district_type_bag_.push_back(DistrictType());
        tinyxml2::XMLPrinter printer;
        e->Accept( &printer );
        this->district_type_bag_.back().load_from_xml(type_cache, printer.CStr());
      }
    }
  }
}


DistrictType const* DistrictTypeCache::
get_by_idname(std::string const& idname) const
{
  for (auto const& d: district_type_bag_)
  {
    if (d.idname() == idname)
    {
      return &d;
    }
  }
  return nullptr;
}



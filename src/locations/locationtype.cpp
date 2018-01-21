/**
 * Implementation of the LocationType component.
 */
/*
 * Copyright 2018 Stephen M. Webb  <stephen.webb@bregmasoft.ca>
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
#include "locations/locationtype.h"

#include "log/log.h"
#include "tinyxml2.h"


namespace
{
  const std::string LOCATIONTYPE_XML_NAME_ELEMENT{"name"};
  const std::string LOCATIONTYPE_XML_DESCRIPTION_ELEMENT{"description"};

  const std::string DEFAULT_IDNAME{"LOCATION"};
  std::size_t location_type_id{0};
} // anonymous namespace

const std::string LocationType::DEFAULT_NAME{"A Location"};
const std::string LocationType::DEFAULT_DESCRIPTION{"Unknown"};
const std::string LocationType::LOCATIONTYPE_XML_ELEMENT{"locationtype"};
const std::string LocationType::LOCATIONTYPE_XML_IDNAME_ATTRIBUTE{"idname"};


LocationType::
LocationType()
: idname_(DEFAULT_IDNAME + std::to_string(++location_type_id))
, name_(DEFAULT_NAME)
, description_(DEFAULT_DESCRIPTION)
{ }


void LocationType::
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
      xmllog.log("unknown error loading DistrictTypeCache from XML");
    return;
  }

  auto element = doc.FirstChildElement();
  if ((element != nullptr) && (element->Name() == LOCATIONTYPE_XML_ELEMENT))
  {
    const char* val = element->Attribute(LOCATIONTYPE_XML_IDNAME_ATTRIBUTE.c_str());
    if (val)
    {
      this->idname_ = val;
    }

    for (auto e = element->FirstChildElement(); e; e = e->NextSiblingElement())
    {
      std::string tag = e->Name();
      if (tag == LOCATIONTYPE_XML_NAME_ELEMENT)
      {
        char const* val = e->GetText();
        if (val)
        {
          this->name_ = val;
        }
      }
      else if (tag == LOCATIONTYPE_XML_DESCRIPTION_ELEMENT)
      {
        char const* val = e->GetText();
        if (val)
        {
          this->description_ = val;
        }
      }
    }
  }
}


LocationType* LocationType::
clone() const
{
  LocationType* the_clone = new LocationType;
  the_clone->idname_ = this->idname_ + "/" + std::to_string(++location_type_id);
  return the_clone;
}


std::string const& LocationType::
idname() const
{ return this->idname_; }


std::string const& LocationType::
name() const
{ return this->name_; }


std::string const& LocationType::
description() const
{ return this->description_; }


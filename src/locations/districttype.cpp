/**
 * Implementation of the DistrictType component.
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
#include "locations/districttype.h"

#include "tinyxml2.h"


namespace
{
  const std::string DISTRICTTYPE_XML_ELEMENT{"districttype"};
  const std::string DISTRICTTYPE_XML_IDNAME_ATTRIBUTE{"idname"};
  const std::string DISTRICTTYPE_XML_NAME_ELEMENT{"name"};
  const std::string DISTRICTTYPE_XML_DESCRIPTION_ELEMENT{"description"};

  std::size_t district_type_id{0};
} // anonymous namespace

const std::string DistrictType::DEFAULT_IDNAME{"UNKNOWN"};
const std::string DistrictType::DEFAULT_NAME{"UNNAMED"};
const std::string DistrictType::DEFAULT_DESCRIPTION{"(no description)"};


DistrictType::
DistrictType()
: idname_(DEFAULT_IDNAME + std::to_string(++district_type_id))
, name_(DEFAULT_NAME)
, description_(DEFAULT_DESCRIPTION)
{ }


std::string const& DistrictType::
idname() const
{ return this->idname_; }


std::string const& DistrictType::
name() const
{ return this->name_; }


std::string const& DistrictType::
description() const
{ return this->description_; }


void DistrictType::
load_from_xml(std::string const& xml)
{
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError err = doc.Parse(xml.c_str());
  if (err != tinyxml2::XML_SUCCESS)
  {
    doc.PrintError();
    return;
  }

  auto e = doc.FirstChildElement();
  if ((e != nullptr) && (e->Name() == DISTRICTTYPE_XML_ELEMENT))
  {
    const char* val = e->Attribute(DISTRICTTYPE_XML_IDNAME_ATTRIBUTE.c_str());
    if (val)
    {
      this->idname_ = val;
    }

    for (auto element = e->FirstChildElement(); element; element = element->NextSiblingElement())
    {
      std::string tag = element->Name();
      if (tag == DISTRICTTYPE_XML_NAME_ELEMENT)
      {
        char const* val = element->GetText();
        if (val)
        {
          this->name_ = val;
        }
      }
      else if (tag == DISTRICTTYPE_XML_DESCRIPTION_ELEMENT)
      {
        char const* val = element->GetText();
        if (val)
        {
          this->description_ = val;
        }
      }
    }
  }
}


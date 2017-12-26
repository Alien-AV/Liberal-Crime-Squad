/**
 * Implementation of the City submodule.
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
#include "locations/city.h"

#include "tinyxml2.h"


namespace
{
  const std::string CITY_XML_ELEMENT{"city"};
  const std::string CITY_XML_NAME_ATTRIBUTE{"name"};
  const std::string CITY_XML_DESCRIPTION_ELEMENT{"description"};
  const std::string CITY_XML_DISTRICTS_ELEMENT{"districts"};
} // anonmymous namespace

const std::string City::DEFAULT_NAME{"UNKNOWN"};
const std::string City::DEFAULT_DESCRIPTION{"(no description)"};


City::
City()
: name_(DEFAULT_NAME)
, shortname_(this->name_)
, description_(DEFAULT_DESCRIPTION)
{ }


void City::
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
  if ((e != nullptr) && (e->Name() == CITY_XML_ELEMENT))
  {
    const char* name = e->Attribute(CITY_XML_NAME_ATTRIBUTE.c_str());
    if (name)
    {
      this->name_ = name;
    }

    for (auto element = e->FirstChildElement(); element; element = element->NextSiblingElement())
    {
      std::string tag = element->Name();
      if (tag == CITY_XML_DESCRIPTION_ELEMENT)
      {
        char const* val = element->GetText();
        if (val)
        {
          this->description_ = val;
        }
      }
      else if (tag == CITY_XML_DISTRICTS_ELEMENT)
      {
      }
    }
  }

  if (this->shortname_ == DEFAULT_NAME)
  {
    this->shortname_ = this->name_;
  }
}


std::string const& City::
name() const
{ return this->name_; }


std::string const& City::
shortname() const
{ return this->shortname_; }


std::string const& City::
description() const
{ return this->description_; }


City::DistrictIterator City::
districts_begin() const
{ return this->districts_.begin(); }


City::DistrictIterator City::
districts_end() const
{ return this->districts_.end(); }



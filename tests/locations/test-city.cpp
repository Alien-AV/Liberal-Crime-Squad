/**
 * Unit tests for the locations City component.
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
#include "catch/catch.hpp"
#include "locations/city.h"
#include "locations/district.h"
#include "locations/districttypecache.h"
#include "typecache.h"


SCENARIO("default-constructed City")
{
  GIVEN("a default-constructed city")
  {
    City city;
    THEN("it has expected default values")
    {
      REQUIRE(city.name() == City::DEFAULT_NAME);
      REQUIRE(city.shortname() == City::DEFAULT_NAME);
      REQUIRE(city.description() == City::DEFAULT_DESCRIPTION);
      REQUIRE(city.districts_begin() == city.districts_end());
    }
  }
}

SCENARIO("City loaded from XML")
{
  TypeCache test_typecache;

  GIVEN("a default-constructed city")
  {
    City city;
    WHEN("loaded with basic XML")
    {
      std::string test_name{"Paris"};
      std::string test_description{"City of Lights"};
      std::string test_xml{"<city name=\"" + test_name + "\">"
                             "<description>" + test_description + "</description>"
                             "<districts>"
                               "<district idname=\"DOWNTOWN\"/>"
                               "<district idname=\"UNIVERSITY\">"
                                 "<name>State College</name>"
                               "</district>"
                               "<district idname=\"UNIVERSITY\">"
                                 "<name>Ivy-League College</name>"
                               "</district>"
                             "</districts>"
                           "</city>"};
      city.load_from_xml(test_typecache, test_xml);
      THEN("it has expected values")
      {
        REQUIRE(city.name() == test_name);
        REQUIRE(city.shortname() == test_name);
        REQUIRE(city.description() == test_description);
        REQUIRE(city.districts_end() - city.districts_begin() == 3);

        auto d = city.districts_begin();
        ++d;
        REQUIRE((*d)->name() == "State College");
      }
    }
  }
}


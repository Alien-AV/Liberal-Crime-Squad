/**
 * Unit tests for the LocationTypeCache component.
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
#include "catch/catch.hpp"
#include "locations/locationtypecache.h"
#include <string>


SCENARIO("default-constructed LocationTypeCache")
{
  GIVEN("a default-constructed LocationTypeCache")
  {
    LocationTypeCache ltc;
    THEN("it should have 5 default types loaded.")
    {
      REQUIRE(ltc.size() == 5);
    }
  }
}

SCENARIO("loading valid XML into a LocationTypeCache")
{
  LocationTypeCache ltc;

  // preconditions
  REQUIRE(ltc.size() == 5);

  WHEN("an XML with just one LocationType definition is loaded")
  {
    std::string xml{"<locations>\n"
                    "  <locationtype>\n"
                    "  </locationtype>\n"
                    "</locations>"};
    ltc.load_from_xml(xml);
    THEN("it will have size 6 (5 defaults plus 1 loaded).")
    {
      REQUIRE(ltc.size() == 6);
    }
  }

  WHEN("an XML with two LocationType definitions are loaded")
  {
    std::string const test_location_idname{"RESIDENTIAL"};

    std::string xml{"<locations>\n"
                    "  <locationtype idname=\"" + test_location_idname + "\">\n"
                    "  </locationtype>\n"
                    "  <locationtype idname=\"RED LIGHT\">\n"
                    "  </locationtype>\n"
                    "</locations>"};
    ltc.load_from_xml(xml);
    THEN("it will have size 7")
    {
      REQUIRE(ltc.size() == 7);
    }
    AND_THEN("a load location type can be retrieved.")
    {
      LocationType const* ct = ltc.get_by_idname(test_location_idname);
      REQUIRE(ct != nullptr);
      REQUIRE(ct->idname() ==  test_location_idname);
    }
  }
}

SCENARIO("loading invalid XML into a LocationTypeCache")
{
  GIVEN("An empty location type cache")
  {
    LocationTypeCache ltc;

    // preconditions
    REQUIRE(ltc.size() == 5);

    WHEN("an attempt is made to load invalid XML")
    {
      ltc.load_from_xml("garbage");
      THEN("the cache remains unchanged.")
      {
        REQUIRE(ltc.size() == 5);
      }
    }
  }
}

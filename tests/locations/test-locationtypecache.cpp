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
#include "typecache.h"


SCENARIO("default-constructed LocationTypeCache")
{
  TypeCache tc;
  GIVEN("a default-constructed LocationTypeCache")
  {
    LocationTypeCache ltc(tc);
    THEN("it should have 5 default types loaded.")
    {
      REQUIRE(ltc.size() == 5);
    }
  }
}

SCENARIO("loading valid XML into a LocationTypeCache")
{
  TypeCache tc;
  LocationTypeCache ltc(tc);

  // preconditions
  REQUIRE(ltc.size() == 5);

  WHEN("an XML with just one LocationType definition is loaded")
  {
    std::string xml{"<locations>\n"
                    "  <locationtype>\n"
                    "  </locationtype>\n"
                    "</locations>"};
    ltc.load_from_xml(tc, xml);
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
    ltc.load_from_xml(tc, xml);
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
  TypeCache tc;
  GIVEN("An empty location type cache")
  {
    LocationTypeCache ltc(tc);

    // preconditions
    REQUIRE(ltc.size() == 5);

    WHEN("an attempt is made to load invalid XML")
    {
      ltc.load_from_xml(tc, "garbage");
      THEN("the cache remains unchanged.")
      {
        REQUIRE(ltc.size() == 5);
      }
    }
  }
}

SCENARIO("clone an existing LocationType")
{
  TypeCache tc;
  GIVEN("A default-constructed location type cache")
  {
    LocationTypeCache ltc(tc);
    std::size_t original_size = ltc.size();
    REQUIRE(original_size > 0);
    LocationType const* existing_lt = ltc.get_by_idname("BUSINESS_JUICEBAR");
    REQUIRE(existing_lt != nullptr);

    WHEN("an existing cached LocationType is cloned")
    {
      LocationType const* new_lt = ltc.clone_by_idname(existing_lt->idname());
      THEN("the cache has grown and the new object has a different idname")
      {
        REQUIRE(ltc.size() > original_size);
        REQUIRE(new_lt->idname() != existing_lt->idname());
      }
    }
  }
}

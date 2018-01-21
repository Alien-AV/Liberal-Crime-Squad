/**
 * Unit tests for the DistrictTypeCache component.
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
#include "locations/districttypecache.h"
#include <string>
#include "typecache.h"


SCENARIO("default-constructed DistrictTypeCache")
{
  TypeCache tc;
  GIVEN("a default-constructed DistrictTypeCache")
  {
    DistrictTypeCache dtc(tc);
    THEN("it should have 5 default types loaded.")
    {
      REQUIRE(dtc.size() == 5);
    }
  }
}

SCENARIO("loading valid XML into a DistrictTypeCache")
{
  TypeCache tc;
  DistrictTypeCache dtc(tc);

  // preconditions
  REQUIRE(dtc.size() == 5);

  WHEN("an XML with just one DistrictType definition is loaded")
  {
    std::string xml{"<districts>\n"
                    "  <districttype>\n"
                    "  </districttype>\n"
                    "</districts>"};
    dtc.load_from_xml(tc, xml);
    THEN("it will have size 6 (5 defaults plus 1 loaded).")
    {
      REQUIRE(dtc.size() == 6);
    }
  }

  WHEN("an XML with two DistrictType definitions are loaded")
  {
    std::string const test_district_idname{"RESIDENTIAL"};

    std::string xml{"<districts>\n"
                    "  <districttype idname=\"" + test_district_idname + "\">\n"
                    "  </districttype>\n"
                    "  <districttype idname=\"RED LIGHT\">\n"
                    "  </districttype>\n"
                    "</districts>"};
    dtc.load_from_xml(tc, xml);
    THEN("it will have size 7")
    {
      REQUIRE(dtc.size() == 7);
    }
    AND_THEN("a load district type can be retrieved.")
    {
      DistrictType const* ct = dtc.get_by_idname(test_district_idname);
      REQUIRE(ct != nullptr);
      REQUIRE(ct->idname() ==  test_district_idname);
    }
  }
}

SCENARIO("loading invalid XML into a DistrictTypeCache")
{
  TypeCache tc;
  GIVEN("An empty district type cache")
  {
    DistrictTypeCache dtc(tc);

    // preconditions
    REQUIRE(dtc.size() == 5);

    WHEN("an attempt is made to load invalid XML")
    {
      dtc.load_from_xml(tc, "garbage");
      THEN("the cache remains unchanged.")
      {
        REQUIRE(dtc.size() == 5);
      }
    }
  }
}

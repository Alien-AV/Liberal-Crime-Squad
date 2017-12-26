/**
 * Unit tests for the DistrictTypeCache component.
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
#include "locations/districttypecache.h"
#include <string>


SCENARIO("default-constructed DistrictTypeCache")
{
  GIVEN("a default-constructed DistrictTypeCache")
  {
    DistrictTypeCache dtc;
    THEN("it should have zero size")
    {
      REQUIRE(dtc.size() == 0);
    }
  }
}

SCENARIO("loading valid XML into a DistrictTypeCache")
{
  DistrictTypeCache dtc;
  REQUIRE(dtc.size() == 0);

  WHEN("an XML with just one DistrictType definition is loaded")
  {
    std::string xml{"<districts>\n"
                    "  <districttype>\n"
                    "  </districttype>\n"
                    "</districts>"};
    dtc.load_from_xml_string(xml);
    THEN("it will have size 1")
    {
      REQUIRE(dtc.size() == 1);
    }
  }

  WHEN("an XML with two DistrictType definitions are loaded")
  {
    std::string xml{"<districts>\n"
                    "  <districttype idname=\"DOWNTOWN\">\n"
                    "  </districttype>\n"
                    "  <districttype idname=\"UNIVERSITY\">\n"
                    "  </districttype>\n"
                    "</districts>"};
    dtc.load_from_xml_string(xml);
    THEN("it will have size 2")
    {
      REQUIRE(dtc.size() == 2);
    }
    AND_THEN("a load district type can be retrieved.")
    {
      DistrictType const* ct = dtc.get_by_idname("DOWNTOWN");
      REQUIRE(ct != nullptr);
      REQUIRE(ct->idname() == "DOWNTOWN");
    }
  }
}

SCENARIO("loading invalid XML into a DistrictTypeCache")
{
  GIVEN("An empty district type cache")
  {
    DistrictTypeCache dtc;
    REQUIRE(dtc.size() == 0);
    WHEN("an attempt is made to load invalid XML")
    {
      dtc.load_from_xml_string("garbage");
      THEN("the cache remains unchanged.")
      {
        REQUIRE(dtc.size() == 0);
      }
    }
  }
}

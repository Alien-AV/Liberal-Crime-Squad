/**
 * Unit tests for the locations DistrictType component.
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
#include "locations/districttype.h"
#include <string>


SCENARIO("default-created DistrictType")
{
  GIVEN("a default-created DistrictType")
  {
    DistrictType dt;
    THEN("it has appropriate default values.")
    {
      REQUIRE(dt.name() == DistrictType::DEFAULT_NAME);
      REQUIRE(dt.description() == DistrictType::DEFAULT_DESCRIPTION);
    }
  }
  GIVEN("two default-created DistrictType object")
  {
    DistrictType dt1;
    DistrictType dt2;
    THEN("they have distinct idnames")
    {
      REQUIRE(dt1.idname() != dt2.idname());
    }
  }
}

SCENARIO("DistrictType creation from XML")
{
  DistrictType dt;
  GIVEN("a minimal DistrictType XML snippet")
  {
    std::string test_name{"Suburbia"};
    std::string test_xml{"<districttype>"
                           "<name>" + test_name + "</name>"
                         "</districttype>"};
    WHEN("it is used to customize a DistrictType")
    {
      dt.load_from_xml(test_xml);
      THEN("the minimum required data are set")
      {
        REQUIRE(dt.name() == test_name);
        REQUIRE(dt.description() == DistrictType::DEFAULT_DESCRIPTION);
      }
    }
  }
  GIVEN("an ill-formed DistrictType XML snippet")
  {
    std::string test_xml{"<districttype><garbage></districttype>"};
    WHEN("it is used to customize a DistrictType")
    {
      dt.load_from_xml(test_xml);
      THEN("the DistrictType remains unchanged")
      {
        REQUIRE(dt.name() == DistrictType::DEFAULT_NAME);
        REQUIRE(dt.description() == DistrictType::DEFAULT_DESCRIPTION);
      }
    }
  }
}

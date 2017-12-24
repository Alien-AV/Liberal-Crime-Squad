/**
 * Unit tests for the locations District component.
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
#include "locations/district.h"

#include "locations/districttype.h"


SCENARIO("basic District construction")
{
  GIVEN("a District default-constructed from a default DistrictType")
  {
    DistrictType dt;
    District d(&dt);
    THEN("it has type-default values.")
    {
      REQUIRE(d.type() == &dt);
      REQUIRE(d.name() == DistrictType::DEFAULT_NAME);
      REQUIRE(d.description() == DistrictType::DEFAULT_DESCRIPTION);
    }
  }

  GIVEN("a District fully constructed from a default DistrictType")
  {
    DistrictType dt;
    std::string test_name{"a name"};
    std::string test_description{"a description"};
    District d(&dt, test_name, test_description);
    THEN("it has type-default values.")
    {
      REQUIRE(d.type() == &dt);
      REQUIRE(d.name() == test_name);
      REQUIRE(d.description() == test_description);
    }
  }
}

/**
 * Unit tests for the locations LocationType component.
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
#include "catch/catch.hpp"
#include "locations/locationtype.h"


SCENARIO("default-created LocationType")
{
  GIVEN("a default-created LocationType")
  {
    LocationType lt;
    THEN("it has appropriate default values.")
    {
      REQUIRE(lt.name() == LocationType::DEFAULT_NAME);
      REQUIRE(lt.description() == LocationType::DEFAULT_DESCRIPTION);
    }
  }
  GIVEN("two default-created LocationType object")
  {
    LocationType lt1;
    LocationType lt2;
    THEN("they have distinct idnames")
    {
      REQUIRE(lt1.idname() != lt2.idname());
    }
  }
}

SCENARIO("LocationType creation from XML")
{
  LocationType lt;
  GIVEN("a minimal LocationType XML snippet")
  {
    std::string test_name{"Department Store"};
    std::string test_xml{"<locationtype idname=\"DEPT_STORE\">"
                           "<name>" + test_name + "</name>"
                         "</locationtype>"};
    WHEN("it is used to customize a LocationType")
    {
      lt.load_from_xml(test_xml);
      THEN("the minimum required data are set")
      {
        REQUIRE(lt.name() == test_name);
        REQUIRE(lt.description() == LocationType::DEFAULT_DESCRIPTION);
      }
    }
  }
  GIVEN("an ill-formed LocationType XML snippet")
  {
    std::string test_xml{"<locationtype><garbage></locationtype>"};
    WHEN("it is used to customize a LocationType")
    {
      lt.load_from_xml(test_xml);
      THEN("the LocationType remains unchanged")
      {
        REQUIRE(lt.name() == LocationType::DEFAULT_NAME);
        REQUIRE(lt.description() == LocationType::DEFAULT_DESCRIPTION);
      }
    }
  }
}

SCENARIO("Clone existing location type")
{
  GIVEN("an existing valid location type")
  {
    LocationType lt0;
    WHEN("it is cloned")
    {
      LocationType* lt1 = lt0.clone();
      THEN("the clone is an exact copy except for the idname")
      {
        REQUIRE(lt0.idname() != lt1->idname());
        REQUIRE(lt0.name() == lt1->name());
        REQUIRE(lt0.description() == lt1->description());
      }
    }
  }
}

/**
 * Unit tests for the locations World component.
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
#include "locations/world.h"
#include "typecache.h"


SCENARIO("a default-constructed World")
{
  TypeCache test_typecache;

  GIVEN("a default-constructed World")
  {
    World world(test_typecache);
    THEN("it has at least 1 City")
    {
      REQUIRE(world.cities_end() - world.cities_begin() == 1);
      REQUIRE(world.current_city() != nullptr);
    }
  }
}


SCENARIO("World loaded from XML")
{
  TypeCache test_typecache;
  std::string const city_name{"Centreville"};
  std::string const city_xml{"<city name=\"" + city_name + "\">"
                               "<districts>"
                                 "<district idname=\"DOWNTOWN\"/>"
                                 "<district idname=\"UNIVERSITY\"/>"
                               "</districts>"
                             "</city>"};

  GIVEN("A default-constructed World")
  {
    World world(test_typecache);

    WHEN("loaded with a single-city XML")
    {
      world.load_from_xml(test_typecache, "<world>" + city_xml + "</world>");
      THEN("that city can be found")
      {
        City* city = world.find_city_by_name(city_name);
        REQUIRE(city != nullptr);
        REQUIRE(city->name() == city_name);
      }
    }
  }
}

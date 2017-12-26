/**
 * Interface for the City submodule.
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
#ifndef LCS_LOCATIONS_CITY_H
#define LCS_LOCATIONS_CITY_H

#include "locations/district.h"
#include <string>
#include <vector>


/**
 * A playable sub-region within the World.
 */
class City
{
public:
  using Districts = std::vector<District>;
  using DistrictIterator = Districts::const_iterator;

  static const std::string DEFAULT_NAME;
  static const std::string DEFAULT_DESCRIPTION;

public:
  City();

  void
  load_from_xml(std::string const& xml);

  std::string const&
  name() const;

  std::string const&
  shortname() const;

  std::string const&
  description() const;

  DistrictIterator
  districts_begin() const;

  DistrictIterator
  districts_end() const;

private:
  std::string name_;
  std::string shortname_;
  std::string description_;
  Districts   districts_;
};

#endif /* LCS_LOCATIONS_CITY_H */

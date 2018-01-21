/**
 * Interface for the Location submodule.
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
#ifndef LCS_LOCATIONS_LOCATION_H
#define LCS_LOCATIONS_LOCATION_H

#include <string>


class LocationType;

/**
 * A sub-region within a City.
 *
 * Think of this as a neighbourhood.
 */
class Location
{
public:
  static const std::string DEFAULT_NAME;
  static const std::string DEFAULT_DESCRIPTION;

public:
  Location(LocationType const* type,
           std::string const&  name = DEFAULT_NAME,
           std::string const&  description = DEFAULT_DESCRIPTION);

  ~Location() = default;

  std::string const&
  name() const;

  std::string const&
  description() const;

  LocationType const*
  type() const;

private:
  LocationType const* type_;
  std::string         name_;
  std::string         description_;
};

#endif /* LCS_LOCATIONS_LOCATION_H */

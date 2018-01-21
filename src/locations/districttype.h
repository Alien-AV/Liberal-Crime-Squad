/**
 * Interface for the DistrictType component.
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
#ifndef LCS_LOCATIONS_DISTRICTTYPE_H
#define LCS_LOCATIONS_DISTRICTTYPE_H


#include <string>
#include <vector>


class LocationType;
class TypeCache;


class DistrictType
{
public:
  DistrictType();

  void
  load_from_xml(TypeCache& type_cache, std::string const& xml);

  std::string const&
  idname() const;

  std::string const&
  name() const;

  std::string const&
  description() const;

  static const std::string DEFAULT_IDNAME;
  static const std::string DEFAULT_NAME;
  static const std::string DEFAULT_DESCRIPTION;

private:
  using LocationTypeBag = std::vector<LocationType const*>;

  std::string     idname_;
  std::string     name_;
  std::string     description_;
  LocationTypeBag location_types_;
};

#endif /* LCS_LOCATIONS_DISTRICTTYPE_H */


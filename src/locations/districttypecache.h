/**
 * Interface for the DistrictTypeCache component.
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
#ifndef LCS_LOCATION_DISTRICTTYPECACHE_H
#define LCS_LOCATION_DISTRICTTYPECACHE_H

#include "locations/districttype.h"
#include <vector>


/**
 * A collection of DistrictType objects.
 */
class DistrictTypeCache
{
public:
  using size_type = std::size_t;

public:
  ~DistrictTypeCache();

  /** Create a district type cache from a string containing XML. */
  void
  load_from_xml_string(std::string const& xml);

  /** Get a district type by idname. */
  DistrictType const*
  get_by_idname(std::string const& idname) const;

  /** Get the number of district type definitions in the cache. */
  size_type
  size() const
  { return district_type_bag_.size(); }

private:
  using DistrictTypeBag = std::vector<DistrictType>;

  DistrictTypeBag district_type_bag_;
};

#endif /* LCS_LOCATION_DISTRICTTYPECACHE_H */


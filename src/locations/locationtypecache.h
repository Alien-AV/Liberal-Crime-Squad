/**
 * Interface for the LocationTypeCache component.
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
#ifndef LCS_LOCATION_LOCATIONTYPECACHE_H
#define LCS_LOCATION_LOCATIONTYPECACHE_H

#include "locations/locationtype.h"
#include <vector>


class TypeCache;


/**
 * A collection of LocationType objects.
 */
class LocationTypeCache
{
public:
  using size_type = std::size_t;

public:
  LocationTypeCache(TypeCache& type_cache);
  ~LocationTypeCache();

  /** Create a location type cache from a string containing XML. */
  LocationType const*
  load_from_xml(TypeCache& type_cache, std::string const& xml);

  /** Get a location type by idname. */
  LocationType const*
  get_by_idname(std::string const& idname) const;

  LocationType const*
  clone_by_idname(std::string const& idname);

  /** Get the number of location type definitions in the cache. */
  size_type
  size() const
  { return location_type_bag_.size(); }

private:
  using LocationTypeBag = std::vector<LocationType>;

  LocationTypeBag location_type_bag_;
};

#endif /* LCS_LOCATION_LOCATIONTYPECACHE_H */


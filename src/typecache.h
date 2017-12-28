/**
 * Interface for the TypeCache component.
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
#ifndef LCS_TYPECACHE_H
#define LCS_TYPECACHE_H

class DistrictTypeCache;


/**
 * The master Type cache.
 *
 * This is really a collection of type-specific caches.
 *
 * The contained type caches are held by pointer so they can forward declared
 * without puling in all their defnitions.  The builds are already slow enough.
 */
class TypeCache
{
public:
  TypeCache();
  ~TypeCache();

public:
  DistrictTypeCache* district_type_cache;
};

#endif /* LCS_TYPECACHE_H */

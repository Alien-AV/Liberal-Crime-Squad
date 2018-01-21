/**
 * Implementation of the Location component.
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
#include "locations/location.h"


const std::string Location::DEFAULT_NAME{"Nowhere"};
const std::string Location::DEFAULT_DESCRIPTION{"no description"};


Location::
Location(LocationType const* type,
         std::string const&  name,
         std::string const&  description)
{ }


std::string const& Location::
name() const
{ return this->name_; }


std::string const& Location::
description() const
{ return this->description_; }


LocationType const* Location::
type() const
{ return this->type_; }


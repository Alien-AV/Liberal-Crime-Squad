/**
 * Interface for the LocationType component.
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
#ifndef LCS_LOCATIONS_LOCATIONTYPE_H
#define LCS_LOCATIONS_LOCATIONTYPE_H

#include <string>
#include <vector>


class LocationType
{
public:
  static const std::string LOCATIONTYPE_XML_ELEMENT;
  static const std::string LOCATIONTYPE_XML_IDNAME_ATTRIBUTE;

public:
  LocationType();

  void
  load_from_xml(std::string const& xml);

  LocationType*
  clone() const;

  std::string const&
  idname() const;

  std::string const&
  name() const;

  std::string const&
  description() const;

  static const std::string DEFAULT_NAME;
  static const std::string DEFAULT_DESCRIPTION;

private:
  using AdjectiveList = std::vector<std::string>;
  using NounList = std::vector<std::string>;

  // Default location data.
  std::string   idname_;
  std::string   name_;
  std::string   description_;

  // Rules for generating names.
  //
  // The name_template is a string with the following substitution markers.
  // - %S surname
  // - %M given name (male)
  // - %F given name (female)
  // - %P given name (patriarchal)
  // - %A adjective (chosen from adjective_list)
  // - %N noun (chosen from noun list)
  //
  std::string   name_template_;
  AdjectiveList adjective_list_;
  NounList      noun_list_;

  // Sitemap data.
  std::string   mapfile_name_;

  // Rules for generating loot, specias, and encounters.
  // - loot types
  // - specials types
  // - weighted creature types
};

#endif /* LCS_LOCATIONS_LOCATIONTYPE_H */


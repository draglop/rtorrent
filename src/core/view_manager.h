// rTorrent - BitTorrent client
// Copyright (C) 2005-2011, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#ifndef RTORRENT_CORE_VIEW_MANAGER_H
#define RTORRENT_CORE_VIEW_MANAGER_H

#include <string>
#include <vector>

#include "view.h"

namespace core {

class ViewManager
{
public:
  typedef std::list<std::string> filter_args;

  ViewManager() {}
  ~ViewManager() { clear(); }

  // Ffff... Just throwing together an interface, need to think some
  // more on this.

  void                clear();

  void                updates_enable(bool enable);

  void                insert(const std::string& name);
  void                insert_throw(const std::string& name) { insert(name); }
  void                remove_throw(const std::string& name);

  // When erasing, just 'disable' the view so that the users won't
  // suddenly find their pointer dangling?

  View*               find(const std::string& name);
  View*               find_throw(const std::string& name);
  View*               find_ptr_throw(const std::string& name) { return find_throw(name); }
  View*               find_by_index(uint8_t index);

  template<typename Fn>
  void                for_each(Fn fn);

  // If View::last_changed() is less than 'timeout' seconds ago, don't
  // sort.
  //
  // Find a better name for 'timeout'.
  void                sort(const std::string& name, uint32_t timeout = 0);

  // These could be moved to where the command is implemented.
  void                set_sort_new(const std::string& name, const torrent::Object& cmd)     { find_throw(name)->set_sort_new(cmd); }
  void                set_sort_current(const std::string& name, const torrent::Object& cmd) { find_throw(name)->set_sort_current(cmd); }

  void                set_filter(const std::string& name, const torrent::Object& cmd);
  void                set_filter_temp(const std::string& name, const torrent::Object& cmd);
  void                set_filter_on(const std::string& name, const filter_args& args);

  void                set_event_added(const std::string& name, const torrent::Object& cmd)   { find_throw(name)->set_event_added(cmd); }
  void                set_event_removed(const std::string& name, const torrent::Object& cmd) { find_throw(name)->set_event_removed(cmd); }

private:
  std::vector<View*> m_views;
};

template <typename Fn>
void ViewManager::for_each(Fn fn)
{
  std::for_each(m_views.begin(), m_views.end(), fn);
}
}
#endif

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

#include "config.h"

#include <algorithm>
#include <rak/functional.h>
#include <torrent/exceptions.h>
#include <torrent/object.h>

#include "globals.h"
#include "control.h"
#include "rpc/parse_commands.h"

#include "download.h"
#include "download_list.h"
#include "manager.h"
#include "view.h"
#include "view_manager.h"

namespace core {

void
ViewManager::clear() {
  std::for_each(m_views.begin(), m_views.end(), [](View* view){delete view;});
  m_views.clear();
}

void
ViewManager::insert(const std::string& name) {
  if (name.empty())
    throw torrent::input_error("View with empty name not supported.");

  if (find(name) != nullptr)
    throw torrent::input_error("View with same name already inserted.");

  View *view = new View;
  view->initialize(name);
  m_views.push_back(view);
}

View*
ViewManager::find(const std::string& name) {
  auto it = std::find_if(m_views.begin(), m_views.end(), [&name](View* view){return name == view->name();});
  return it != m_views.end() ? *it : nullptr;
}

View *
ViewManager::find_by_index(uint8_t index)
{
  if (m_views.empty() || index > m_views.size() - 1) {
    return nullptr;
  }
  auto it = m_views.begin();
  while (index--)
    ++it;

  return *it;
}

View*
ViewManager::find_throw(const std::string& name) {
  View* view = find(name);

  if (view == nullptr)
    throw torrent::input_error("Could not find view: " + name);

  return view;
}

void
ViewManager::remove_throw(const std::string& name) {
  if (name.empty())
    throw torrent::input_error("View with empty name not supported.");

  auto it = std::find_if(m_views.begin(), m_views.end(), [&name](View* view){return name == view->name();});
  if (it == m_views.end())
    throw torrent::input_error("Could not find view: " + name);

  delete (*it);
  m_views.erase(it);
}

void
ViewManager::sort(const std::string& name, uint32_t timeout) {
  View* view = find_throw(name);

  if (view->last_changed() + rak::timer::from_seconds(timeout) > cachedTime)
    return;

  // Should we rename sort, or add a seperate function?
  view->filter();
  view->sort();
}

void
ViewManager::set_filter(const std::string& name, const torrent::Object& cmd) {
  View* view = find_throw(name);

  view->set_filter(cmd);
  view->filter();
}

void
ViewManager::set_filter_temp(const std::string& name, const torrent::Object& cmd) {
  View* view = find_throw(name);

  view->set_filter_temp(cmd);
  view->filter();
}

void
ViewManager::set_filter_on(const std::string& name, const filter_args& args) {
  View* view = find_throw(name);

  view->clear_filter_on();

  // TODO: Ensure the filter keys are rlookup.
  std::for_each(args.begin(), args.end(), [&view](const std::string &arg)
                { view->set_filter_on_event(arg); });
}

void
ViewManager::updates_enable(bool enable) {
  if (enable) {
    for(View* view : m_views) {
      view->reset_focus();
      view->updates_enable(true);
    }
  } else {
    for(View* view : m_views) {
      view->updates_enable(false);
    }
  }
}

}

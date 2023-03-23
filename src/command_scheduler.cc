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

#include <sys/types.h>

#include "core/manager.h"
#include "core/download.h"
#include "core/download_list.h"
#include "core/view.h"
#include "core/view_manager.h"

#include "globals.h"
#include "control.h"
#include "command_helpers.h"

torrent::Object
cmd_scheduler_simple_update(core::Download* download) {
  const std::size_t max_active = static_cast<std::size_t>(rpc::call_command("scheduler.max_active", torrent::Object()).as_value());
  const std::vector<core::Download *>& actives = control->core()->download_list()->actives();
  const std::vector<core::Download *>& starteds = control->core()->download_list()->starteds();

  if ((actives.size() < max_active) && (starteds.size() > actives.size())) {
    std::size_t download_to_active_count = max_active - actives.size();
    std::vector<core::Download*> downloads_to_active;
    downloads_to_active.reserve(download_to_active_count);

    for (core::Download* download : starteds) {
      if (!download->is_active()) {
        downloads_to_active.push_back(download);

        --download_to_active_count;
        if (download_to_active_count == 0) {
          break;
        }
      }
    }

    for (core::Download* download : downloads_to_active) {
      control->core()->download_list()->resume(download);
    }
  } else if (actives.size() > max_active) {
    std::size_t download_to_pause_count = actives.size() - max_active;
    std::vector<core::Download*> downloads_to_pause;
    downloads_to_pause.reserve(download_to_pause_count);

    for (core::Download* download : actives) {
      downloads_to_pause.push_back(download);

      --download_to_pause_count;
      if (download_to_pause_count == 0) {
        break;
      }
    }

    for (core::Download* download : downloads_to_pause) {
      control->core()->download_list()->pause(download);
    }
  }

  return torrent::Object();
}

void
initialize_command_scheduler() {
  CMD2_VAR_VALUE("scheduler.max_active", int64_t(-1));

  CMD2_DL("scheduler.simple.update",  std::bind(&cmd_scheduler_simple_update, std::placeholders::_1));
}

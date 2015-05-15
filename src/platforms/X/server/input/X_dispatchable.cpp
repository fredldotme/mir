/*
 * Copyright © 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Cemil Azizoglu <cemil.azizoglu@canonical.com>
 */

#include "X_dispatchable.h"
#include "mir/events/event_private.h"
#include "../debug.h"

#include <chrono>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/input.h>
#include <inttypes.h>

namespace mi = mir::input;
namespace mix = mi::X;
namespace md = mir::dispatch;

extern ::Display *x_dpy;

mix::XDispatchable::XDispatchable(int raw_fd)
    : fd(raw_fd), sink(nullptr)
{
    CALLED
}

mir::Fd mix::XDispatchable::watch_fd() const
{
    CALLED

    return fd;
}

bool mix::XDispatchable::dispatch(md::FdEvents /*events*/)
{
    XEvent xev;

    CALLED

    XNextEvent(x_dpy, &xev);

    if (sink)
    {
        switch (xev.type)
        {
        case KeyPress:
        case KeyRelease:
        {
        	MirEvent event;
            XKeyEvent &xkev = (XKeyEvent &)xev;
            static const int STRMAX = 32;
            char str[STRMAX];
            KeySym keysym;

            auto count = XLookupString(&xkev, str, STRMAX, &keysym, NULL);

            mir::log_info("Key event : type=%d, serial=%u, send_event=%d, display=%p, window=%p, root=%p, subwindow=%p, time=%d, x=%d, y=%d, x_root=%d, y_root=%d, state=%0X, keycode=%d, same_screen=%d",
                xkev.type, xkev.serial, xkev.send_event, xkev.display, xkev.window, xkev.root, xkev.subwindow, xkev.time, xkev.x, xkev.y, xkev.x_root, xkev.y_root, xkev.state, xkev.keycode, xkev.same_screen);

            event.key.type = mir_event_type_key;
            event.key.device_id = 0;
            event.key.source_id = 0;
            event.key.action = xev.type == KeyPress ?
                                   mir_key_action_down : mir_key_action_up;
            event.key.modifiers = mir_input_event_modifier_none;
            event.key.key_code = keysym;
            event.key.scan_code = xkev.keycode-8;
            event.key.repeat_count = 0;

            // TODO: read time from xkev
            event.key.event_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                       std::chrono::system_clock::now().time_since_epoch()).count();

            for (int i=0; i<count; i++)
                mir::log_info("buffer[%d]='%c', key_code=%d, scan_code=%d, event_time=%" PRId64, i, str[i], keysym, xkev.keycode-8, event.key.event_time);

            sink->handle_input(event);
            break;
        }

        case MappingNotify:
            mir::log_info("Mapping changed at server. Refreshing the cache.");
        	XRefreshKeyboardMapping((XMappingEvent*)&xev);
        	break;

        default:
            mir::log_info("Uninteresting event");
            break;
        }
    }
    else
        mir::log_info("input event detected with no sink to handle it");

    return true;
}

md::FdEvents mix::XDispatchable::relevant_events() const
{
    CALLED

    return md::FdEvent::readable;
}

void mix::XDispatchable::set_input_sink(mi::InputSink *input_sink)
{
    CALLED

    sink = input_sink;
}

void mix::XDispatchable::unset_input_sink()
{
    CALLED

    sink = nullptr;
}

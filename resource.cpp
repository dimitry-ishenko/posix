////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-18 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "posix/error.hpp"
#include "posix/resource.hpp"

#include <stdexcept>
#include <poll.h>

////////////////////////////////////////////////////////////////////////////////
namespace posix
{

////////////////////////////////////////////////////////////////////////////////
bool resource::wait_for(const msec& time, event e) const
{
    if(empty()) throw std::logic_error(
        "Attempt to access empty posix::resource instance"
    );

    short events = (e == read) ? POLLIN | POLLPRI : (e == write) ? POLLOUT : 0;
    pollfd fd_poll = { desc_, events, 0 };

    auto count = ::poll(&fd_poll, 1,
        time == msec::max() ? -1 : static_cast<int>(time.count())
    );
    if(count == -1) throw posix::errno_error();

    return fd_poll.events & fd_poll.revents;
}

////////////////////////////////////////////////////////////////////////////////
}

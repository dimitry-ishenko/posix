////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "posix/error.hpp"
#include "posix/resource.hpp"

#include <poll.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
namespace posix
{

////////////////////////////////////////////////////////////////////////////////
void resource::cancel() noexcept
{ if(cancel_fd_ != invalid) ::write(cancel_fd_, "X", 1); }

////////////////////////////////////////////////////////////////////////////////
bool resource::wait_for(const msec& time, event e)
{
    int fd_pipe[2];
    if(::pipe(fd_pipe)) throw posix::errno_error();

    ////////////////////
    short events = (e == read) ? POLLIN : (e == write) ? POLLOUT : 0;
    pollfd fd_poll[] =
    {
        { fd_, events, 0 },
        { fd_pipe[0], POLLIN, 0 },
    };

    cancel_fd_ = fd_pipe[1];
    auto count = ::poll(fd_poll,
        sizeof(fd_poll) / sizeof(fd_poll[0]),
        time == msec::max() ? -1 : static_cast<int>(time.count())
    );
    cancel_fd_ = invalid;

    posix::errno_error error;

    ////////////////////
    ::close(fd_pipe[0]);
    ::close(fd_pipe[1]);

    if(count == -1) throw error;

    return fd_poll[0].events & fd_poll[0].revents;
}

////////////////////////////////////////////////////////////////////////////////
}

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
void resource::clear() noexcept { fd_ = invalid; wait_fd_ = invalid; }

////////////////////////////////////////////////////////////////////////////////
void resource::cancel() noexcept { ::write(wait_fd_.load(), "X", 1); }

////////////////////////////////////////////////////////////////////////////////
bool resource::poll(const msec& time, event e)
{
    int fd_pipe[2];
    if(::pipe(fd_pipe)) throw posix::errno_error();

    ////////////////////
    pollfd fd_poll[] =
    {
        { fd_, (short)(e == read ? POLLIN : e == write ? POLLOUT : 0), 0 },
        { fd_pipe[0], POLLIN, 0 },
    };

    wait_fd_ = fd_pipe[1];
    auto count = ::poll(fd_poll, sizeof(fd_poll) / sizeof(fd_poll[0]),
        time == msec::max() ? -1 : time.count());
    wait_fd_ = invalid;

    auto errc = static_cast<posix::errc>(errno);

    ////////////////////
    ::close(fd_pipe[0]);
    ::close(fd_pipe[1]);

    if(count == -1) throw std::system_error(errc);

    return fd_poll[0].events & fd_poll[0].revents;
}

////////////////////////////////////////////////////////////////////////////////
}

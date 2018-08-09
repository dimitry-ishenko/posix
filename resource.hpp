////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef POSIX_RESOURCE_HPP
#define POSIX_RESOURCE_HPP

////////////////////////////////////////////////////////////////////////////////
#include <chrono>

////////////////////////////////////////////////////////////////////////////////
namespace posix
{

////////////////////////////////////////////////////////////////////////////////
// Resource with a descriptor (eg, file, socket, pipe, etc).
//
// Enables resource owner to check for or block until a read/recv or write/send
// operation can be performed.
//
class resource
{
public:
    ////////////////////
    resource() noexcept = default;
    explicit resource(int fd) noexcept : fd_(fd) { }

    resource(const resource&) = delete;
    resource(resource&& rhs) noexcept { swap(rhs); }

    resource& operator=(const resource&) = delete;
    resource& operator=(resource&& rhs) noexcept { swap(rhs); return (*this); }

    void swap(resource& rhs) noexcept
    {
        using std::swap;
        swap(fd_, rhs.fd_);
    }

    ////////////////////
    bool empty() const noexcept { return fd_ == invalid; }
    explicit operator bool() const noexcept { return !empty(); }

    auto fd() const noexcept { return fd_; }

    bool try_read();
    bool try_read_forever();

    template<typename Rep, typename Period>
    bool try_read_for(const std::chrono::duration<Rep, Period>&);

    template<typename Clock, typename Duration>
    bool try_read_until(const std::chrono::time_point<Clock, Duration>&);

    bool try_write();
    bool try_write_forever();

    template<typename Rep, typename Period>
    bool try_write_for(const std::chrono::duration<Rep, Period>&);

    template<typename Clock, typename Duration>
    bool try_write_until(const std::chrono::time_point<Clock, Duration>&);

private:
    ////////////////////
    static constexpr int invalid = -1;
    int fd_ = invalid;

    using msec = std::chrono::milliseconds;
    enum event { read, write };
    bool wait_for(const msec&, event);
};

////////////////////////////////////////////////////////////////////////////////
inline bool resource::try_read() { return try_read_for(msec::zero()); }
inline bool resource::try_read_forever() { return try_read_for(msec::max()); }

////////////////////////////////////////////////////////////////////////////////
template<typename Rep, typename Period>
inline bool
resource::try_read_for(const std::chrono::duration<Rep, Period>& time)
{ return wait_for(std::chrono::duration_cast<msec>(time), read); }

////////////////////////////////////////////////////////////////////////////////
template<typename Clock, typename Duration>
inline bool
resource::try_read_until(const std::chrono::time_point<Clock, Duration>& tp)
{
    auto now = Clock::now();
    return try_read_for(tp - (tp < now ? tp : now));
}

////////////////////////////////////////////////////////////////////////////////
inline bool resource::try_write() { return try_write_for(msec::zero()); }
inline bool resource::try_write_forever() { return try_write_for(msec::max()); }

////////////////////////////////////////////////////////////////////////////////
template<typename Rep, typename Period>
inline bool
resource::try_write_for(const std::chrono::duration<Rep, Period>& time)
{ return wait_for(std::chrono::duration_cast<msec>(time), write); }

////////////////////////////////////////////////////////////////////////////////
template<typename Clock, typename Duration>
inline bool
resource::try_write_until(const std::chrono::time_point<Clock, Duration>& tp)
{
    auto now = Clock::now();
    return try_write_for(tp - (tp < now ? tp : now));
}

////////////////////////////////////////////////////////////////////////////////
inline void swap(resource& lhs, resource& rhs) noexcept { lhs.swap(rhs); }

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif

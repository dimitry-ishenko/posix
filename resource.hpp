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
using desc_t = int;

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
    resource(desc_t desc) noexcept : desc_(desc) { }

    resource(const resource&) = delete;
    resource(resource&& rhs) noexcept { swap(rhs); }

    resource& operator=(const resource&) = delete;
    resource& operator=(resource&& rhs) noexcept { swap(rhs); return (*this); }

    void swap(resource& rhs) noexcept
    {
        using std::swap;
        swap(desc_, rhs.desc_);
    }

    ////////////////////
    bool empty() const noexcept { return desc_ < 0; }
    explicit operator bool() const noexcept { return !empty(); }

    auto desc() const noexcept { return desc_; }
    operator desc_t() const noexcept { return desc(); }

    bool try_read() const;
    bool try_read_forever() const;

    template<typename Rep, typename Period>
    bool try_read_for(const std::chrono::duration<Rep, Period>&) const;

    template<typename Clock, typename Duration>
    bool try_read_until(const std::chrono::time_point<Clock, Duration>&) const;

    bool try_write() const;
    bool try_write_forever() const;

    template<typename Rep, typename Period>
    bool try_write_for(const std::chrono::duration<Rep, Period>&) const;

    template<typename Clock, typename Duration>
    bool try_write_until(const std::chrono::time_point<Clock, Duration>&) const;

private:
    ////////////////////
    desc_t desc_ = -1;

    using msec = std::chrono::milliseconds;
    enum event { read, write };
    bool wait_for(const msec&, event) const;
};

////////////////////////////////////////////////////////////////////////////////
inline bool resource::try_read() const { return try_read_for(msec::zero()); }
inline bool resource::try_read_forever() const { return try_read_for(msec::max()); }

////////////////////////////////////////////////////////////////////////////////
template<typename Rep, typename Period>
inline bool
resource::try_read_for(const std::chrono::duration<Rep, Period>& time) const
{ return wait_for(std::chrono::duration_cast<msec>(time), read); }

////////////////////////////////////////////////////////////////////////////////
template<typename Clock, typename Duration>
inline bool
resource::try_read_until(const std::chrono::time_point<Clock, Duration>& tp) const
{
    auto now = Clock::now();
    return try_read_for(tp - (tp < now ? tp : now));
}

////////////////////////////////////////////////////////////////////////////////
inline bool resource::try_write() const { return try_write_for(msec::zero()); }
inline bool resource::try_write_forever() const { return try_write_for(msec::max()); }

////////////////////////////////////////////////////////////////////////////////
template<typename Rep, typename Period>
inline bool
resource::try_write_for(const std::chrono::duration<Rep, Period>& time) const
{ return wait_for(std::chrono::duration_cast<msec>(time), write); }

////////////////////////////////////////////////////////////////////////////////
template<typename Clock, typename Duration>
inline bool
resource::try_write_until(const std::chrono::time_point<Clock, Duration>& tp) const
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

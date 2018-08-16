# Library of POSIX related C++ classes

## Getting Started

### Prerequisites

None.

### Installation

Add as a submodule into an existing project:
```
git submodule add https://github.com/dimitry-ishenko/posix.git
```

### Usage

Example:
```cpp
#include "posix/error.hpp"
#include "posix/resource.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include <unistd.h>

int main()
{
    using namespace std::chrono_literals;
    char message[] = "Hello World!";

    // open unidirectional pipe
    int fd_pipe[2];
    if(::pipe(fd_pipe)) throw posix::errno_error();

    // send message on a thread
    std::thread thread1([&]()
    {
        // adopt write end of the pipe
        posix::resource res { fd_pipe[1], posix::adopt };

        std::this_thread::sleep_for(3000ms);

        std::cout << "sending message" << std::endl;
        if(::write(res, message, sizeof(message)) == -1) throw posix::errno_error();

        // fd_pipe[1] will be closed automatically
    });

    // receive message on another thread
    std::thread thread2([&]()
    {
        posix::resource res;
        // adopt read end of the pipe
        res.adopt(fd_pipe[0]);

        std::cout << "receiving message" << std::endl;
        do
        {
            // wait a second to see if anything arrived
            if(res.try_read_for(1000ms)) break;
            std::cout << "message didn't arrive" << std::endl;

            // wait another second
            if(res.try_read_for(1000ms)) break;
            std::cout << "message still didn't arrive" << std::endl;

            // wait forever
            res.try_read_forever();
        }
        while(false);

        // read message piece by piece
        std::cout << "received message: ";
        while(res.can_read())
        {
            char recv[8];

            long count = ::read(res, recv, sizeof(recv) - 1);
            if(count == -1) throw posix::errno_error();

            recv[count] = 0;
            std::cout << recv;
        }
        std::cout << std::endl;

        // fd_pipe[0] will be closed automatically
    });

    thread1.join();
    thread2.join();
    return 0;
}
```

## Authors

* **Dimitry Ishenko** - dimitry (dot) ishenko (at) (gee) mail (dot) com

## License

This project is distributed under the GNU GPL license. See the
[LICENSE.md](LICENSE.md) file for details.

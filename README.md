# libuvcpp

libuvcpp is c++ wrapper for libuv.

Feature
-------
* Easy and automated memory management for handle
* Event handling with lambda function(c++11) as like javascript callback style
* Recycling read/write buffer memory for io

Requirements
------------
* libuv v1.0 or later

Coding Style
------------
```bash
using namespace uvcpp;
// ...
UvContext::open(); // make a context instance in thread local storage.
UvTcp tcp;
tcp.init();
tcp.connect("127.0.0.1", 80, [&](int status) {
  if(!status) {
    // tcp connected
    tcp.readStart([&](upReadBuffer upbuf) {
      // proc read data
      // ...
    });
  } else {
    // disconnected
    tcp.close();
  }
});

UvContext::run(); // start event loop.
UvContext::close();
```

Documentation
-------------
Refer to wiki.

# libuvcpp

libuvcpp is c++ wrapper for libuv.

Feature
-------
* Easy and automated memory management for handle
* Event handling with lambda function(c++11) as like javascript callback style
* Recycling read/write buffer memory for io


Coding Style
------------
```cpp
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

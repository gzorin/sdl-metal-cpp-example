# sdl-metal-cpp-example

This project is an example of using the [Metal API's C++ interface][1] to draw
a triangle. The project uses [SDL 2][2] to create the window.

I've added a few things to Apple's C++ bindings:

* [CAMetalLayer.hpp][3] provides a C++ binding for Core Animation's
  [`CAMetalLayer`][7]. Note that [the included example][4] uses this extension, and
  will therefore not work with the upstream version of 'metal-cpp'.

* [shared_ptr.hpp][5] is an implementation of the [standard C++ `shared_ptr`][6]
  that calls `retain()` and `release()` to manage the lifetime of the object.

  Assigning a `NSObject`, or passing it into the constructor, will always
  increase the object's reference count. To manage the lifetime of a
  newly-created `NSObject`, which presumably has its reference count set to 1,
  use the function `make_owned()`, which creates a new `shared_ptr` without
  increasing the object's reference count.

[1]: https://developer.apple.com/metal/cpp/
[2]: https://www.libsdl.org
[3]: metal-cpp/QuartzCore/CAMetalLayer.hpp
[7]: https://developer.apple.com/documentation/quartzcore/cametallayer
[4]: main.cpp
[5]: metal-cpp/Metal/shared_ptr.hpp
[6]: https://en.cppreference.com/w/cpp/memory/shared_ptr
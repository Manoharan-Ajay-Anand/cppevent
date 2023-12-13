add_library(cppevent_base
  async_signal.cpp
  event_bus.cpp
  event_callback.cpp
  event_loop.cpp
  io_listener.cpp
  io_service.cpp
  timer.cpp
  util.cpp
)

target_include_directories(cppevent_base PUBLIC "${CMAKE_SOURCE_DIR}/src")

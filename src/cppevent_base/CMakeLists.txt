add_library(cppevent_base OBJECT
  async_signal.cpp
  event_bus.cpp
  event_callback.cpp
  event_loop.cpp
  io_listener.cpp
  io_service.cpp
  status_awaiter.cpp
  suspended_coro.cpp
  timer.cpp
  util.cpp
)

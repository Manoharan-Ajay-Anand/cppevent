add_library(cppevent_base OBJECT
  async_signal.cpp
  event_bus.cpp
  event_callback.cpp
  event_loop.cpp
  io_listener.cpp
  io_service.cpp
  spin_lock.cpp
  status_awaiter.cpp
  status_store.cpp
  timer.cpp
  types.cpp
  util.cpp
)

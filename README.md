# cppevent

This library enables users to implement a single-threaded event loop architecture in their applications on Linux using C++ 20 coroutines. 

Traditionally, threads were used to implement concurrency. Multi-threading enables tasks to run in parallel on multiple CPU cores, speeding up workloads. However, for I/O bound workloads such as handling network requests or timers, the bottleneck is typically the speed of the I/O operations rather than CPU processing. Having too many threads that are just waiting for I/O operations can lead to increased memory usage and overhead due to context switching. Multi-threading also introduces additional challenges that will need to be carefully managed such as race conditions, deadlocks and synchronization. 

The single-threaded event loop architecture is a design pattern commonly used in programming to handle asynchronous tasks efficiently. It consists of the following:

- Single Thread: There is only one main thread of execution that executes tasks sequentially.
  
- Event Queue: Asynchronous operations, such as I/O operations, generate events that are then placed into an event queue.

- Event Loop: The event loop is a continuously running process that checks the event queue for new events. It waits for events and processes them one by one in a sequential manner.

- Non-Blocking Processing: When the event loop processes an event that requires an I/O operation (eg: reading from a network socket), the operation is delegated to the system. The event loop continues to process the next event in the event queue immediately. This ensures that the events can be continuously processed without waiting for I/O operations to complete. 
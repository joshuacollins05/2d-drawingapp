# Multithreaded ASCII Art Drawer – C++  

**Overview**  
This project is a distributed ASCII art drawer that uses **POSIX threads, processes, and TCP sockets** to allow concurrent editing and rendering of ASCII-based images. It demonstrates systems-level programming concepts such as multithreading, interprocess communication, and client–server design.  

**Tech Stack**  
- **Language:** C++  
- **Concurrency:** POSIX Threads (pthreads), `fork()`  
- **Networking:** TCP Sockets  
- **Synchronization:** Mutexes, Semaphores  

**Features**  
- Client program spawns multiple threads to decode image rows concurrently.  
- Server handles multiple client connections using `fork()` and renders shapes into a shared ASCII canvas.  
- Efficient row decoding through binary search for position lookups.  
- Encodes sparse binary data to minimize communication overhead.  

**My Contributions**  
- Designed and implemented both client and server programs from scratch.  
- Implemented socket-based communication protocol between client and server.  
- Added thread-level parallelism for row-by-row decoding.  
- Managed concurrency issues using mutexes and semaphores.  

**Learning Outcomes**  
- Strengthened understanding of concurrent programming and synchronization.  
- Built hands-on experience with **low-level socket programming** in C++.  
- Applied process management (`fork()`, `waitpid`) and signal handling for multiprocess coordination.  

**Example Run**  
Client reads width, height, symbols, and positions → sends data to server → server draws ASCII canvas:  

**Note:**  
This project was developed in an academic setting to practice concurrency, networking, and distributed programming concepts.

# EventStream

Simple project demonstrating a thread-safe queue and worker pool.

## Structure

- `main.cpp` - entry point
- `ThreadSafeQueue.h` - thread-safe queue template
- `WorkerPool.h` - basic thread pool implementation

## Building

Use CMake to configure and build the project.

## Build & Run (recommended)

This project uses Crow for HTTP routes (headers are in `Crow/include`). Crow requires a modern C++ toolchain (full C++17 support) and an Asio implementation (Boost.Asio or standalone Asio).

On Linux / WSL (recommended):

```
sudo apt update
sudo apt install build-essential g++-11 cmake libboost-system-dev
mkdir build && cd build
cmake ..
cmake --build .
./eventstream
```

On Windows (MSYS2 MinGW):

1. Install MSYS2 from https://www.msys2.org
2. Open MSYS2 MinGW64 shell and run:

```
pacman -Syu
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-boost
cmake -G "MinGW Makefiles" ..
mingw32-make
./eventstream.exe
```

Quick manual compile (if toolchain and Boost are available):

```
g++ -std=c++17 main.cpp -I Crow/include -lboost_system -pthread -o eventstream
```

OR use a single-header HTTP server (recommended if you don't want Boost/Asio):

1. Download `httplib.h` (cpp-httplib single header) into the project root:

```
curl -Lo d:\EventStream\httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
```

2. Build:

```
g++ -std=c++17 main.cpp -pthread -o eventstream
```

Windows notes for cpp-httplib:
- Requires **64-bit** Windows 10+ and a 64-bit compiler (e.g., MinGW-w64 or MSVC).
- When using g++ on Windows, you may need to link Winsock:

```
g++ -std=c++17 main.cpp -pthread -lws2_32 -o eventstream
```

Performance build (recommended):

```
g++ -O2 -DNDEBUG -std=c++17 main.cpp -pthread -lws2_32 -o eventstream
```

Test client (optional):

```
g++ -std=c++17 test_client.cpp -pthread -lws2_32 -o eventstream_client
```

PowerShell quick test (run while server is running):

```
PowerShell -File .\test_requests.ps1
```

Notes:
- The repo includes a minimal compatibility shim for older libstdc++ headers, but a modern compiler (g++ 9+) is still recommended.
- If you use the `httplib.h` route, no Boost/Asio is required.

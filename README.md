# Web Crawler Application

A desktop web crawler built with C++, Qt 6, and QML.

The application allows users to specify a target URL, configure crawl depth and page limits, and monitor discovered URLs and crawling statistics in real time through a graphical user interface.

---

## Pictures

<img width="1602" height="1306" alt="Screenshot from 2026-09-10 18-43-42" src="https://github.com/user-attachments/assets/119e161c-48e0-4004-874a-ea73019142ce" />

---

## Features

- **Asynchronous & Multithreaded**
   Uses `QNetworkAccessManager` for non-blocking HTTP requests.

- **Multithreaded Processing**
   Uses `QThreadPool` and `QRunnable` workers for concurrent HTML/link processing without blocking the UI.

- **Depth & Page Limiting**
   Configure maximum crawl depth and total page limits (including an "Unlimited" option).
   
- **Real-time Statistics**
   Live tracking of discovered, queued, fetched, and failed URLs, alongside elapsed time and current operational state.
   
- **Interactive UI**
   Built with Qt Quick / QML and includes:
   - URL results table
   - Crawler controls
   - Statistics
   - Collapsible console panel
   - Colored log levels

- **Copy to Clipboard**
   Easily copy discovered URLs directly from the table view.
   
- **Robust Error Handling**
   Handles network timeouts, HTTP status errors, and gracefully aborts requests on stop.

---

## Technologies

- **Language**: C++23
- **Framework**: Qt 6 (Core, Network, Gui, Qml, Quick)
- **Build System**: CMake (version 3.16 or higher)
- **Testing Framework**: Google Test (GTest)

---

## Architecture

The project follows a clean separation of concerns:
- **Core (`src/core`)**: Handles the crawling logic, network fetching (`UrlFetcher`), queue management (`QueueHandler`), link extraction (`Worker`), and state tracking.
- **Controller (`src/controller`)**: Bridges the core components with the UI layer, exposing models and states safely to QML.
- **UI (`src/ui`)**: QML-based user interface components, including tables, status displays, header bars, control panels, and a console panel backed by a singleton `LogModel`.

---

## Requirements

- C++23 compatible compiler (GCC, Clang, or MSVC)
- Qt 6 development libraries (specifically `Qt6Core`, `Qt6Network`, `Qt6Gui`, `Qt6Qml`, `Qt6Quick`)
- CMake 3.16+
- Google Test (optional, for running tests)

---

## Qt Modules

- `Qt6::Core`
- `Qt6::Network`
- `Qt6::Gui`
- `Qt6::Qml`
- `Qt6::Quick`

---

## Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/WebCrawlerApp.git
   cd WebCrawlerApp
   ```

2. Install Qt 6:
   Make sure Qt 6 is installed on your system.
   
   If Qt is not available in your system environment, you can specify its installation path using CMAKE_PREFIX_PATH.
   
   For example, if Qt 6.9.1 is installed at:
   
   `~/Qt/6.9.1/gcc_64`

   you can use this path when configuring the project.

---

## Build
Create a build directory and configure the project with CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt

```

Then build the project:

`cmake --build build`

---

## Run

./build/src/WebCrawlerApp

---

## Usage

1. Start the application.
2. Enter the target URL.
3. Select the maximum crawl depth.
4. Select the maximum number of URLs to crawl.
5. Start the crawler.
6. Observe discovered URLs in real time.

---

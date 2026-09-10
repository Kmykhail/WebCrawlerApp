# Web Crawler Application

A high-performance, multithreaded web crawler built with C++ and Qt 6 (QML for the user interface). It features asynchronous network requests, depth and limit controls, real-time logging, live statistics, and a modern graphical user interface.

---

## Features

- **Asynchronous & Multithreaded**: Utilizes `QThreadPool`, `QNetworkAccessManager`, and custom `QRunnable` workers for concurrent crawling without blocking the UI.
- **Depth & Page Limiting**: Configure maximum crawl depth and total page limits (including an "Unlimited" option).
- **Real-time Statistics**: Live tracking of discovered, queued, fetched, and failed URLs, alongside elapsed time and current operational state.
- **Interactive UI**: Built with Qt Quick / QML, featuring a sortable/filterable URL table view, collapsible console panel with colored log levels, and control panels.
- **Copy to Clipboard**: Easily copy discovered URLs directly from the table view.
- **Robust Error Handling**: Handles network timeouts, HTTP status errors, and gracefully aborts requests on stop.

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
   git clone https://github.com/your-username/web-cr  awler.git
   cd web-crawler
   ```

2. Ensure Qt 6 is installed and configured in your environment path (or set `CMAKE_PREFIX_PATH`).

---


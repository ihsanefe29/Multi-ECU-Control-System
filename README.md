# TEI ECU Control & CAN FD Communication System

A Qt-based ECU communication, monitoring, and control application designed around CAN/CAN FD.

The project currently provides a complete simulation environment using Linux SocketCAN and virtual CAN (`vcan`) interfaces. Multiple virtual ECUs can be discovered dynamically, controlled from a Qt/QML GUI, monitored through heartbeat/status messages, and used as live telemetry sources.

The architecture is designed so that the simulated ECUs can later be replaced by real embedded ECUs without requiring the GUI to understand CAN IDs or low-level CAN communication.

==> Building project section: 15. Installing Dependencies

---

# 1. Project Overview

The system consists of three main layers:

1. **Virtual ECU simulation**
2. **CAN/CAN FD communication core**
3. **Qt/QML GUI application**

The current communication path is:

    Virtual ECUs
         │
         │ CAN / CAN FD
         ▼
    Linux SocketCAN / vcan
         │
         ▼
    CanChannelWorker
         │
         ▼
    CommunicationCore
         │
         ▼
    Dispatcher
         │
         ├── ModuleSession
         │
         ├── LiveSignalSource
         │
         ├── SignalFileLoader
         │
         └── RawSignalLoader
                │
                ▼
              QML GUI

The GUI never needs to know CAN IDs, SocketCAN interface names, telemetry frame layouts, or worker implementation details.

It communicates through the `Dispatcher`, which acts as the bridge between the UI and the communication backend.

The Signal Display page can work with both live and offline data. Live ECU telemetry can be streamed to the graph, while previously recorded signal files or raw ECU memory data can also be loaded and visualized.

---

# 2. Current Features

The current implementation supports:

- Automatic SocketCAN interface discovery
- Multiple CAN interfaces
- CAN FD support detection
- One CAN worker per discovered CAN interface
- Dynamic ECU discovery
- Automatic ECU-to-CAN-channel routing
- ECU heartbeat monitoring
- ECU online/offline detection
- Power commands
- ECU status requests
- CAN FD telemetry reception
- Telemetry frame decoding
- Live telemetry frequency measurement
- ECU-specific telemetry selection
- Qt/QML GUI integration
- Live graph data streaming
- Signal file loading
- Raw TXT / hex data loading
- CSV parameter definition loading
- Spreadsheet-style CSV data visualization
- Offline graph generation
- Raw ECU memory decoding
- Parameter conversion from raw values
- Virtual ECU simulation
- Multiple virtual CAN buses

The current simulator contains ECUs such as:

| ECU | Interface | Command | Status | Heartbeat | Telemetry |
|---|---|---:|---:|---:|---:|
| A | vcan0 | 0x100 | 0x101 | 0x102 | 0x103 |
| B | vcan0 | 0x110 | 0x111 | 0x112 | 0x113 |
| C | vcan1 | 0x120 | 0x121 | 0x122 | 0x123 |
| D | vcan2 | 0x130 | 0x131 | 0x132 | 0x133 |
| G | vcan3 | 0x130 | 0x131 | 0x132 | 0x133 |

CAN IDs do not need to be globally unique across different physical CAN interfaces because the channel is also part of the route.

For example, ECU D and ECU G may use the same CAN IDs as long as they exist on different CAN interfaces.

---

# 3. Architecture

## 3.1 CommunicationCore

`CommunicationCore` is the main CAN communication manager.

Its responsibilities include:

- discovering SocketCAN interfaces
- creating CAN workers
- maintaining ECU routes
- broadcasting ECU discovery requests
- processing incoming CAN frames
- parsing discovery responses
- parsing status frames
- parsing heartbeat frames
- parsing telemetry frames
- sending ECU commands
- monitoring heartbeat timeouts

Example ECU route:

    ECU B
      │
      ├── Channel: vcan0
      ├── Command ID: 0x110
      ├── Status ID: 0x111
      ├── Heartbeat ID: 0x112
      └── Telemetry ID: 0x113

Routes are discovered dynamically rather than hardcoded into the GUI.

---

## 3.2 CanChannelWorker

Each discovered CAN interface receives its own `CanChannelWorker`.

For example:

    vcan0 → CanChannelWorker
    vcan1 → CanChannelWorker
    vcan2 → CanChannelWorker
    vcan3 → CanChannelWorker

The workers interact with Qt's CAN bus API and SocketCAN.

This keeps CAN I/O separate from the GUI and allows multiple interfaces to operate independently.

The worker reports received frames back to `CommunicationCore`.

---

## 3.3 ECU Discovery

The application does not assume which ECU exists on which CAN bus.

Instead, `CommunicationCore` performs discovery.

Conceptually:

    CommunicationCore
          │
          ├── Discovery Request → vcan0
          ├── Discovery Request → vcan1
          ├── Discovery Request → vcan2
          └── Discovery Request → vcan3

Virtual ECUs respond with their identity and CAN IDs.

The response contains information such as:

    ECU identifier
    Command CAN ID
    Status CAN ID
    Heartbeat CAN ID
    Telemetry CAN ID

`CommunicationCore` builds its routing table from these responses.

This means ECUs can be moved between CAN interfaces without changing the GUI.

---

# 4. Heartbeat and Online Detection

Each ECU periodically sends heartbeat frames.

When a heartbeat arrives:

    CAN frame
       ↓
    CommunicationCore
       ↓
    heartbeat timestamp updated
       ↓
    ecuOnlineChanged(ecu, true)

If heartbeat messages stop arriving for longer than the configured timeout:

    ECU → OFFLINE

and:

    ecuOnlineChanged(ecu, false)

is emitted.

The GUI receives this information through `Dispatcher` and updates the corresponding `ModuleSession`.

This controls properties such as:

    session.connected
    session.ledState

which are directly observed by QML.

---

# 5. Power Control

QML sends semantic commands rather than CAN frames.

For example:

    PowerSwitchItem
          ↓
    Dispatcher.setPower("B", 1, "On")
          ↓
    CommunicationCore::powerOn('B')
          ↓
    ECU routing lookup
          ↓
    CAN command frame
          ↓
    vcan / SocketCAN
          ↓
    Virtual ECU B

The reverse path updates the GUI:

    ECU status frame
          ↓
    CommunicationCore
          ↓
    ecuStatus(...)
          ↓
    Dispatcher::onEcuStatus(...)
          ↓
    ModuleSession::applyPowerState(...)
          ↓
    QML

Therefore QML does not need to know that `"On"` corresponds to a particular CAN command value.

---

# 6. Telemetry

Virtual ECUs periodically generate telemetry data.

The simulator currently sends telemetry approximately every:

    0.1 seconds

which corresponds to:

    1 / 0.1 = 10 Hz

Telemetry is transmitted as CAN FD because the payload is larger than a classic CAN frame.

Example simulator logic:

    update telemetry
         ↓
    create telemetry payload
         ↓
    send CAN FD frame
         ↓
    sleep 100 ms
         ↓
    repeat

The current telemetry payload contains multiple values encoded as:

    [RAM address: 4 bytes]
    [raw value:   4 bytes]

Each telemetry entry therefore occupies:

    8 bytes

For a 48-byte telemetry payload:

    48 / 8 = 6 telemetry values

`CommunicationCore` decodes these entries into `TelemetryValue` objects and emits an `EcuTelemetry` structure.

---

# 7. Live Telemetry Frequency

The GUI currently demonstrates the live telemetry pipeline by plotting the measured telemetry frequency.

`Dispatcher::onTelemetryReceived()` records the time between consecutive telemetry messages from the selected ECU.

For example:

    previous frame → t = 1000 ms
    current frame  → t = 1101 ms

Therefore:

    Δt = 101 ms

and:

    frequency = 1000 / 101
              ≈ 9.90 Hz

This is why values such as:

    10.0 Hz
    9.90099 Hz

may appear in the application.

---

# 8. ECU-Specific Live Data

Telemetry may arrive simultaneously from several ECUs.

For example:

    ECU B telemetry
    ECU C telemetry
    ECU D telemetry
    ECU G telemetry

The GUI, however, should display data belonging only to the ECU whose Data View was selected.

`Dispatcher` therefore stores the currently selected ECU.

Conceptually:

    incoming telemetry
          │
          ├── ECU C ──┐
          ├── ECU D   │ ignored
          ├── ECU G ──┘
          │
          └── ECU B → selected
                        ↓
                  calculate Hz
                        ↓
                 LiveSignalSource
                        ↓
                      QML

This keeps `CommunicationCore` capable of receiving telemetry from every ECU while allowing the GUI to choose which stream it displays.

---

# 9. LiveSignalSource

`LiveSignalSource` provides the interface expected by the existing QML graph.

The current path is:

    Dispatcher
        │
        │ pushHz(hz)
        ▼
    LiveSignalSource
        │
        │ sampleGenerated(time, hz)
        ▼
    SignalChartScreen.qml

`LiveSignalSource` therefore acts as a small adapter between backend telemetry and the existing QML chart.

The current graph uses telemetry arrival frequency as its live value. The same pipeline can later carry actual ECU parameters such as RPM, temperature, pressure, voltage, or current.

---

# 10. Signal File, Raw Data, and Spreadsheet Visualization

The Signal Display page supports multiple data sources on the same visualization screen.

In addition to live CAN/CAN FD telemetry, the user can load previously recorded data files and generate graphs from them.

The visualization system currently contains three main data-source components:

    LiveSignalSource
    SignalFileLoader
    RawSignalLoader

Conceptually:

                    SignalChartScreen
                           ▲
                           │
             ┌─────────────┼─────────────┐
             │             │             │
      LiveSignalSource SignalFileLoader RawSignalLoader
             ▲             ▲             ▲
             │             │             │
          CAN FD       Signal / CSV   CSV Parameters
         Telemetry        Data        + Raw TXT/Hex

This allows the same visualization page to be used for both real-time monitoring and offline analysis.

---

## 10.1 Live CAN/CAN FD Streaming

Live telemetry follows this path:

    ECU
      ↓
    CAN / CAN FD
      ↓
    CommunicationCore
      ↓
    EcuTelemetry
      ↓
    Dispatcher
      ↓
    LiveSignalSource
      ↓
    SignalChartScreen.qml
      ↓
    Live Graph

The currently selected ECU is used as the active live data source.

Telemetry from other ECUs can still be received by `CommunicationCore`, but the Dispatcher only forwards the selected ECU's live signal to the graph.

---

## 10.2 Signal File Visualization

The user can also load previously recorded signal data.

`SignalFileLoader` handles complete signal files rather than continuously arriving live values.

Conceptually:

    Signal / CSV File
           ↓
    SignalFileLoader
           ↓
       parse data
           ↓
     time + value
           ↓
    SignalChartScreen
           ↓
         Graph

This allows graph visualization without requiring:

- running virtual ECUs
- an active CAN connection
- physical CAN hardware

It is useful for:

- recorded test data
- offline debugging
- comparing previous test runs
- graph development
- reviewing previously captured ECU measurements

---

## 10.3 Raw Data Upload

The user can load a raw TXT or hex data file containing ECU memory data.

The raw file is handled by:

    RawSignalLoader

The processing pipeline is:

    Raw TXT / Hex File
           ↓
    RawSignalLoader
           ↓
    remove whitespace
           ↓
    convert hex pairs into bytes
           ↓
    raw byte buffer
           ↓
    extract parameter bytes
           ↓
    decode values
           ↓
    apply conversion formulas
           ↓
    display / graph values

This allows previously captured ECU memory data to be analyzed without requiring an active CAN connection.

---

## 10.4 CSV Parameter Definitions

Raw memory values alone do not describe what each value represents.

A CSV parameter definition file is therefore used to describe how the raw memory should be interpreted.

The parameter definitions may contain information such as:

    Address
    Offset
    Width
    Type
    Name
    Formula

The CSV tells `RawSignalLoader`:

- where a parameter exists in the raw data
- how many bytes belong to the parameter
- how those bytes should be interpreted
- the human-readable parameter name
- which conversion formula should be applied

Conceptually:

    Parameter CSV
          │
          │ defines memory layout
          ▼
    RawSignalLoader
          ▲
          │ provides raw bytes
          │
    Raw TXT / Hex File

The result is a set of decoded ECU parameters that can then be displayed or plotted.

---

## 10.5 RawSignalLoader

`RawSignalLoader` is responsible for converting raw ECU memory data into meaningful parameter values.

The process can be divided into three stages.

### Stage 1 — Parameter Definition Loading

    CSV
     ↓
    parameter definitions

Each parameter can describe:

    RAM address
    offset
    width
    type
    name
    conversion formula

### Stage 2 — Raw Data Loading

    Raw TXT / Hex
          ↓
    remove spaces / newlines
          ↓
    hex pair → byte
          ↓
       QByteArray

### Stage 3 — Decode and Convert

    raw bytes
       ↓
    locate parameter
       ↓
    decode raw representation
       ↓
    apply conversion
       ↓
    engineering value

For example:

    raw RPM value
         ↓
    conversion formula
         ↓
    actual RPM

or:

    raw temperature
         ↓
    scaling / offset
         ↓
    °C

This separates raw ECU memory representation from the engineering values shown to the user.

---

## 10.6 Spreadsheet / Excel Data Concept

The Signal Display architecture can also support spreadsheet-based measurement data.

For example:

| Time | RPM | Temperature | Pressure |
|---:|---:|---:|---:|
| 0.0 | 1500 | 24.0 | 1.1 |
| 0.1 | 1520 | 24.2 | 1.1 |
| 0.2 | 1560 | 24.4 | 1.2 |

A spreadsheet-style dataset can conceptually follow:

    CSV / Spreadsheet
           ↓
    SignalFileLoader
           ↓
    parse rows and columns
           ↓
    select signal
           ↓
    time + value samples
           ↓
    SignalChartScreen
           ↓
          Graph

CSV provides the simplest spreadsheet-compatible format for the current implementation.

Native Microsoft Excel `.xlsx` parsing is not currently part of the implemented loader and can be added later if required.

The important architectural concept is that the graph does not need to know where its samples originally came from.

All sources can eventually produce a common representation such as:

    time
    value
    signal name
    unit

Therefore the visualization layer can remain reusable for:

    Live CAN FD
    CSV signal data
    future Excel XLSX data
    Raw ECU memory data

---

## 10.7 Shared Visualization Architecture

The overall visualization architecture is:

    ┌────────────────────────┐
    │ Live CAN/CAN FD        │
    │ LiveSignalSource       │
    └────────────┬───────────┘
                 │
                 │
                 ▼
        ┌───────────────────┐
        │                   │
        │ SignalChartScreen │
        │                   │
        └───────────────────┘
                 ▲
                 │
        ┌────────┴───────────┐
        │                    │
        │                    │
    SignalFileLoader    RawSignalLoader
        ▲                    ▲
        │                    │
    Signal / CSV       Parameter CSV
       Data             + Raw TXT/Hex

This design means a separate graph implementation is not required for every data source.

Instead, loaders and live sources convert their input into data that can be consumed by the same QML visualization layer.

---

# 11. Dispatcher

`Dispatcher` is the main interface between QML and the backend.

QML calls methods such as:

    Dispatcher.connectModule(...)
    Dispatcher.disconnectModule(...)
    Dispatcher.setMode(...)
    Dispatcher.setPower(...)
    Dispatcher.selectFirmware(...)

The Dispatcher also receives signals from `CommunicationCore`:

    ecuDiscovered
    ecuOnlineChanged
    ecuStatus
    ecuHeartbeat
    telemetryReceived
    errorOccurred

It converts these backend events into changes to the QML-facing `ModuleSession` objects.

The Dispatcher also provides access to visualization data sources such as:

    LiveSignalSource
    SignalFileLoader
    RawSignalLoader

This provides a useful separation:

    QML
     │
     ▼
    Dispatcher
     │
     ├── CommunicationCore
     │        │
     │        ▼
     │       CAN
     │
     ├── LiveSignalSource
     ├── SignalFileLoader
     └── RawSignalLoader

QML therefore remains independent from the CAN protocol implementation and the details of file parsing.

---

# 12. ModuleSession

Each GUI ECU panel is backed by a `ModuleSession`.

It exposes QML-facing state such as:

    ecuId
    moduleNumber
    connected
    mode
    powerState
    ledState

When backend state changes, Dispatcher updates the corresponding session.

For example:

    ecuOnlineChanged('B', true)
              ↓
    Dispatcher
              ↓
    ModuleSession B
              ↓
    applyConnected(true)
    applyLedState("success")
              ↓
    QML automatically updates

Qt property notifications therefore keep the GUI synchronized with communication state.

---

# 13. Project Structure

The repository is organized roughly as:

    TEI-app/
    │
    ├── can-lab/
    │   └── src/
    │       ├── CommunicationCore.cpp
    │       ├── CommunicationCore.h
    │       ├── CanChannelWorker.cpp
    │       ├── CanChannelWorker.h
    │       ├── CanFrame.h
    │       └── EcuTelemetry.h
    │
    ├── Singleton_Updated/
    │   └── ecu-control/
    │       ├── EcuControl.pro
    │       │
    │       ├── src/
    │       │   ├── main.cpp
    │       │   │
    │       │   └── core/
    │       │       ├── dispatcher.cpp
    │       │       ├── dispatcher.h
    │       │       ├── modulesession.cpp
    │       │       ├── modulesession.h
    │       │       ├── livesignalsource.cpp
    │       │       ├── livesignalsource.h
    │       │       ├── signalfileloader.cpp
    │       │       ├── signalfileloader.h
    │       │       ├── rawsignalloader.cpp
    │       │       └── rawsignalloader.h
    │       │
    │       ├── qml/
    │       │   ├── Main.qml
    │       │   ├── EcuPanel.qml
    │       │   ├── SignalChartScreen.qml
    │       │   ├── FirmwareUploadScreen.qml
    │       │   └── components/
    │       │
    │       └── data/
    │           ├── ecu_parameters.csv
    │           └── raw_data.txt
    │
    ├── virtual-env/
    │   └── python/
    │       └── virtual_ecus.py
    │
    ├── frontend/
    ├── backend/
    └── tests/

The exact directory layout may evolve as the project is consolidated.

---

# 14. Requirements

Current development environment:

- Linux or WSL2 Ubuntu
- Qt 6
- Qt SerialBus
- Qt QML / Qt Quick
- Qt Quick Controls
- Qt Charts
- SocketCAN
- Python 3
- python-can
- qmake
- make / g++

Ubuntu 22.04 under WSL2 has been used during development.

---

# 15. Installing Dependencies

Update packages:

    sudo apt update

Install the basic build tools:

    sudo apt install build-essential qmake6 qt6-base-dev

Install Qt SerialBus support:

    sudo apt install libqt6serialbus6 libqt6serialbus6-dev

Install Qt QML / Quick development packages if required:

    sudo apt install qt6-declarative-dev qt6-declarative-dev-tools

Install CAN utilities:

    sudo apt install can-utils

Python dependencies can be installed using pip:

    python3 -m pip install python-can

The exact Qt package names can vary slightly between Ubuntu releases.

---

# 16. Creating Virtual CAN Interfaces

The simulator requires Linux virtual CAN interfaces.

Load the `vcan` kernel module:

    sudo modprobe vcan

Create the interfaces:

    sudo ip link add dev vcan0 type vcan
    sudo ip link add dev vcan1 type vcan
    sudo ip link add dev vcan2 type vcan
    sudo ip link add dev vcan3 type vcan

Bring them online:

    sudo ip link set up vcan0
    sudo ip link set up vcan1
    sudo ip link set up vcan2
    sudo ip link set up vcan3

Verify them:

    ip link show

You should see:

    vcan0
    vcan1
    vcan2
    vcan3

If the interfaces already exist, they do not need to be recreated.

---

# 17. Running the Virtual ECUs

Navigate to:

    cd virtual-env/python

Run:

    python3 virtual_ecus.py

Expected startup output resembles:

    Virtual ecus running:
    A:vcan0 CMD=0x100 STATUS=0x101 HB=0x102 TELEM=0x103
    B:vcan0 CMD=0x110 STATUS=0x111 HB=0x112 TELEM=0x113
    C:vcan1 CMD=0x120 STATUS=0x121 HB=0x122 TELEM=0x123
    D:vcan2 CMD=0x130 STATUS=0x131 HB=0x132 TELEM=0x133
    G:vcan3 CMD=0x130 STATUS=0x131 HB=0x132 TELEM=0x133

Keep this terminal running.

---

# 18. Building the Qt Application

Open another terminal and navigate to the Qt GUI project:

    cd Singleton_Updated/ecu-control

Create a clean build directory:

    rm -rf build
    mkdir build
    cd build

Generate the Makefile:

    qmake6 ../EcuControl.pro

Build:

    make -j$(nproc)

If the build succeeds, the executable should be available as:

    ./EcuControl

---

# 19. Running the Full System

The recommended startup order is:

### Terminal 1 — Virtual CAN Setup

Make sure:

    vcan0
    vcan1
    vcan2
    vcan3

exist and are UP.

### Terminal 2 — Virtual ECUs

Run:

    cd virtual-env/python
    python3 virtual_ecus.py

### Terminal 3 — Qt Application

Run:

    cd Singleton_Updated/ecu-control/build
    ./EcuControl

The Qt application should then discover the available CAN interfaces and virtual ECUs automatically.

---

# 20. Expected Runtime Flow

At startup:

    Qt application starts
          ↓
    CommunicationCore::start()
          ↓
    scan SocketCAN interfaces
          ↓
    discover vcan0/vcan1/vcan2/vcan3
          ↓
    create CAN workers
          ↓
    broadcast ECU discovery
          ↓
    virtual ECUs respond
          ↓
    routes stored
          ↓
    heartbeat received
          ↓
    ECU becomes ONLINE
          ↓
    QML LED/session updates

When the user turns an ECU ON:

    QML PowerSwitch
          ↓
    Dispatcher::setPower()
          ↓
    CommunicationCore::powerOn()
          ↓
    CAN command
          ↓
    Virtual ECU
          ↓
    status response
          ↓
    CommunicationCore::ecuStatus
          ↓
    Dispatcher
          ↓
    ModuleSession
          ↓
    QML

When live telemetry is viewed:

    Virtual ECU
          ↓
    CAN FD telemetry @ ~10 Hz
          ↓
    CommunicationCore
          ↓
    EcuTelemetry
          ↓
    Dispatcher
          ↓
    selected ECU filter
          ↓
    measured telemetry frequency
          ↓
    LiveSignalSource
          ↓
    QML chart

When recorded signal data is loaded:

    Signal / CSV File
          ↓
    SignalFileLoader
          ↓
    parsed samples
          ↓
    SignalChartScreen
          ↓
    graph

When raw ECU data is loaded:

    Parameter CSV + Raw TXT/Hex
               ↓
         RawSignalLoader
               ↓
        decode parameters
               ↓
      engineering values
               ↓
       display / graph

---

# 21. Debugging CAN Traffic

`can-utils` can be useful for inspecting raw CAN traffic.

Install it with:

    sudo apt install can-utils

Monitor a bus:

    candump vcan0

or:

    candump vcan1

Monitor all relevant interfaces in separate terminals when debugging routing.

This is particularly useful for determining whether a problem originates from:

- the virtual ECU
- SocketCAN
- CommunicationCore
- Dispatcher
- QML

---

# 22. Common Issues

## `socketcan: cannot open shared object file`

Qt may report messages involving `libsocketcan`.

Ensure the SocketCAN-related packages and libraries are installed.

---

## Virtual CAN Interface Does Not Exist

Check:

    ip link show

If required:

    sudo modprobe vcan
    sudo ip link add dev vcan0 type vcan
    sudo ip link set up vcan0

Repeat for the other required interfaces.

---

## Application Builds but No ECU Is Discovered

Check that:

1. The `vcan` interfaces exist.
2. The interfaces are UP.
3. `virtual_ecus.py` is running.
4. CAN traffic is visible with `candump`.
5. The Qt application is using the SocketCAN plugin.

---

# 23. Key Design Insights

## Keep the GUI Independent from CAN

The GUI should express intent:

    "Turn ECU B on"

rather than:

    "Send 0x01 to CAN ID 0x110 on vcan0"

The latter belongs to the communication layer.

This makes it possible to change the underlying CAN topology without rewriting QML.

---

## Discover Routes Instead of Hardcoding Them

Hardcoding:

    ECU A → vcan0
    ECU B → vcan0
    ECU C → vcan1

works for a prototype but does not scale well.

Dynamic discovery allows the application to determine ECU locations and CAN IDs at runtime.

---

## CAN ID Alone Is Not Always a Unique ECU Identifier

Two ECUs can use the same CAN IDs on different physical buses.

Therefore a route is effectively identified by information such as:

    channel + CAN ID

rather than CAN ID alone.

---

## CommunicationCore Should Receive All Communication

The GUI may currently display only one selected ECU, but `CommunicationCore` should generally remain aware of all ECU traffic.

Filtering for a selected graph belongs at a higher layer such as Dispatcher.

This preserves the possibility of later adding:

- logging
- fault detection
- recording
- alarms
- multiple simultaneous charts

without redesigning the CAN layer.

---

## Do Not Add Threads Without a Reason

A thread is useful when it isolates blocking or asynchronous I/O.

It is not useful simply to avoid a cheap comparison such as:

    if (ecu != selectedEcu)
        return;

Filtering four or five ECU telemetry streams is extremely inexpensive.

Unnecessary threads add synchronization, lifetime, scheduling, and debugging complexity.

---

## Signals and Slots Form the Boundaries Between Layers

The architecture heavily uses Qt signals and slots:

    CAN worker
        → CommunicationCore
        → Dispatcher
        → ModuleSession / LiveSignalSource
        → QML

This keeps individual components loosely coupled.

---

## Separate Transport Data from UI State

`EcuTelemetry` represents communication data.

`ModuleSession` represents UI-facing state.

They are related, but they solve different problems and should not become the same object.

---

## Reuse the Visualization Layer

The graph should not care whether a value originally came from:

    Live CAN FD
    Signal file
    CSV spreadsheet
    Raw ECU memory

Instead, the backend should convert these sources into a representation suitable for visualization.

Conceptually:

    different data sources
            ↓
       loader / adapter
            ↓
        time + value
            ↓
          graph

This avoids implementing a different graph screen for every type of data source.

---

## Keep Raw Data Interpretation Outside QML

QML should not be responsible for:

- RAM address calculations
- byte extraction
- signed/unsigned decoding
- raw hexadecimal parsing
- conversion formulas

These operations belong in backend classes such as `RawSignalLoader`.

QML should receive already meaningful values for presentation.

---

## Simulation Should Resemble Real Hardware

Using:

    Python ECU
        ↓
    SocketCAN
        ↓
    Qt CAN stack

instead of directly calling GUI functions from the simulator is important.

It forces the software to use approximately the same communication path that will eventually be used with real ECUs.

---

# 24. Current Development Status

The following pipeline is currently functional:

    Virtual ECU
        ✓
    CAN / CAN FD
        ✓
    SocketCAN / vcan
        ✓
    CAN interface discovery
        ✓
    ECU discovery
        ✓
    Dynamic routing
        ✓
    Heartbeat
        ✓
    Online/offline detection
        ✓
    Power control
        ✓
    Status response
        ✓
    CAN FD telemetry
        ✓
    Telemetry decoding
        ✓
    Dispatcher integration
        ✓
    ECU-specific selection
        ✓
    Live telemetry frequency
        ✓
    QML live chart pipeline
        ✓
    Signal file loading
        ✓
    Raw TXT / hex loading
        ✓
    CSV parameter definitions
        ✓
    Raw parameter decoding
        ✓
    Offline graph architecture
        ✓

The system has therefore moved beyond a GUI mockup: the UI is connected to an actual simulated CAN/CAN FD communication pipeline while also supporting offline data visualization.

---

# 25. Planned Work

Possible next steps include:

- Plot actual telemetry values instead of telemetry arrival frequency
- Map live telemetry RAM addresses to named parameters
- Reuse CSV parameter definitions for live CAN telemetry
- Add engineering-unit conversion to live telemetry
- Support RPM, temperature, voltage, current, pressure, etc.
- Add multiple simultaneous telemetry signals
- Add configurable telemetry sampling
- Add native `.xlsx` Excel file parsing
- Improve spreadsheet column selection
- Compare live and recorded signals on the same graph
- Export recorded telemetry
- Implement firmware upload
- Improve ECU/module abstraction
- Add telemetry logging
- Add fault/event handling
- Add automated tests
- Integrate Renode-based embedded ECU simulation
- Replace virtual ECUs with real STM32 ECUs
- Test with physical CAN/CAN FD adapters

A natural next telemetry step is:

    CAN FD telemetry
          ↓
      RAM address
          ↓
    parameter definition
          ↓
       raw value
          ↓
    scaling / formula
          ↓
    engineering value
          ↓
    RPM / °C / V / A
          ↓
       QML chart

This would also allow live and offline data to use the same parameter interpretation system.

---

# 26. Long-Term Goal

The long-term objective is to keep the upper software architecture largely unchanged when moving from simulation to hardware.

Development:

    Python Virtual ECU
          ↓
        vcan
          ↓
    CommunicationCore

Future hardware:

    STM32 ECU
          ↓
    Physical CAN FD
          ↓
    CommunicationCore

Above `CommunicationCore`, the rest of the application should require minimal changes:

    CommunicationCore
          ↓
      Dispatcher
          ↓
    ModuleSession
          ↓
         QML

The visualization architecture follows the same principle.

Live hardware data:

    Physical ECU
        ↓
      CAN FD
        ↓
    CommunicationCore
        ↓
    Dispatcher
        ↓
    LiveSignalSource
        ↓
    SignalChartScreen

Offline data:

    CSV / Raw Data
          ↓
    SignalFileLoader /
    RawSignalLoader
          ↓
    SignalChartScreen

This separation between transport, protocol, application state, data interpretation, and presentation is one of the central design goals of the project.

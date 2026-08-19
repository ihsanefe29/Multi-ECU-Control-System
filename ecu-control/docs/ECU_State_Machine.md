# ECU Control Panel - State Machine

## Overview

This document defines the operational state transitions of the ISU Control Panel.

The purpose is to establish a clear software behavior before implementing the controller and CAN communication layers.

---

## System States

| State | Description |
|--------|-------------|
| Offline | No communication with the ECU. |
| Standby | Connected but waiting for operation. |
| Ready | Connected and switch is ON. System is ready to operate. |
| Powered | Output power is enabled. |
| Emergency | Emergency stop activated. All operations are disabled. |

---

## State Transition Table

| Current State | Action | Next State |
|---------------|--------|------------|
| Offline | Connect | Standby |
| Standby | Switch ON | Ready |
| Ready | Power ON | Powered |
| Powered | Power OFF | Ready |
| Ready | Switch OFF | Standby |
| Standby | Disconnect | Offline |
| Ready | Disconnect | Offline |
| Powered | Disconnect | Offline |
| Any State | Kill | Emergency |

---

## Functional Rules

### Connect

- Establishes communication with the ECU.
- Does **not** enable power.
- Default state after connection is **Standby**.

---

### Disconnect

- Terminates communication.
- Automatically disables power.
- Switch returns to OFF.
- State becomes **Offline**.

---

### Switch OFF

- Device remains connected.
- Device is not operational.
- State becomes **Standby**.

---

### Switch ON

- Only allowed when connected.
- Device becomes ready for operation.
- State becomes **Ready**.

---

### Power ON

- Only allowed while in Ready state.
- Enables ECU output power.
- State becomes **Powered**.

---

### Power OFF

- Disables output power.
- Returns system to Ready state.

---

### Emergency Stop (KILL)

Emergency Stop has the highest priority.

Effects:

- Communication terminated
- Power disabled
- Switch locked
- State becomes Emergency

No command except a future Reset operation should clear this state.

---

## Future Improvements

The following components will implement this behavior:

- UICollector
- Dispatcher
- ECUController
- CAN Connector
- CAN Receiver
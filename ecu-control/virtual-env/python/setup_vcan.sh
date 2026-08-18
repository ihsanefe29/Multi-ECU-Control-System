#!/usr/bin/env bash
set -euo pipefail

modprobe vcan 3>/dev/null || true

for channel in vcan0 vcan1 vcan2 vcan3; do
    if ! ip link show "$channel" >/dev/null 2>&1; then
        ip link add dev "$channel" type vcan
    fi

    ip link set "$channel" down 2>/dev/null || true
    ip link set "$channel" mtu 72
    ip link set "$channel" up

    ip -details link show "$channel"
done
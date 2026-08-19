#!/usr/bin/env python3
import asyncio
from dataclasses import dataclass
from enum import IntEnum

import can

DISCOVERY_REQUEST_ID = 0x7A0
DISCOVERY_RESPONSE_BASE_ID = 0x7A1
DISCOVERY_REQUEST = 0xD1
DISCOVERY_RESPONSE = 0xD2


class PowerState(IntEnum):
    OFF = 0
    ON = 1
    KILL = 2


class Command(IntEnum):
    ON = 1
    OFF = 2
    KILL = 3
    RESET = 4
    REQUEST_STATUS = 5


@dataclass
class Virtualecu:
    name: str
    channel: str
    command_id: int
    status_id: int
    heartbeat_id: int
    state: PowerState = PowerState.OFF
    module1: bool = False
    module2: bool = False
    heartbeat_counter: int = 0

    def apply(self, command_value: int) -> int:
        try:
            command = Command(command_value)
        except ValueError:
            return 1

        if command == Command.ON:
            if self.state == PowerState.KILL:
                return 1
            self.state = PowerState.ON
        elif command == Command.OFF:
            if self.state != PowerState.KILL:
                self.state = PowerState.OFF
        elif command == Command.KILL:
            self.state = PowerState.KILL
            self.module1 = False
            self.module2 = False
        elif command == Command.RESET:
            if self.state == PowerState.KILL:
                self.state = PowerState.OFF
        return 0

    def status_payload(self, result: int = 0) -> bytes:
        return bytes(
            [self.state, result, self.module1, self.module2]
        )

    def heartbeat_payload(self) -> bytes:
        self.heartbeat_counter = (self.heartbeat_counter + 1) & 0xFF
        return bytes(
            [ord(self.name), self.state, self.heartbeat_counter, 0]
        )

    def discovery_payload(self) -> bytes:
        return bytes([
            DISCOVERY_RESPONSE,
            ord(self.name),
            (self.command_id >> 8) & 0xFF,
            self.command_id & 0xFF,
            (self.status_id >> 8) & 0xFF,
            self.status_id & 0xFF,
            (self.heartbeat_id >> 8) & 0xFF,
            self.heartbeat_id & 0xFF,
        ])


ecuS = [
    Virtualecu("A", "vcan0", 0x100, 0x101, 0x102),
    Virtualecu("B", "vcan0", 0x110, 0x111, 0x112),
    Virtualecu("C", "vcan1", 0x120, 0x121, 0x122),
    Virtualecu("D", "vcan2", 0x130, 0x131, 0x132),
    Virtualecu("G", "vcan4", 0x130, 0x131, 0x132),
]


class VirtualecuNetwork:
    def __init__(self):
        self.buses = {
            channel: can.Bus(
                interface="socketcan",
                channel=channel,
                fd=True,
            )
            for channel in {ecu.channel for ecu in ecuS}
        }
        self.readers = {}
        self.notifiers = []

        loop = asyncio.get_running_loop()
        for channel, bus in self.buses.items():
            reader = can.AsyncBufferedReader()
            self.readers[channel] = reader
            self.notifiers.append(can.Notifier(bus, [reader], loop=loop))

    def send(self, ecu: Virtualecu, frame_id: int, payload: bytes) -> None:
        self.buses[ecu.channel].send(
            can.Message(
                arbitration_id=frame_id,
                data=payload,
                is_extended_id=False,
                is_fd=False,
            )
        )

    async def receive_channel(self, channel: str) -> None:
        reader = self.readers[channel]
        channel_ecus = [ecu for ecu in ecuS if ecu.channel == channel]
        routes = {
            ecu.command_id: ecu for ecu in channel_ecus
        }

        while True:
            message = await reader.get_message()

            if (
                message.arbitration_id == DISCOVERY_REQUEST_ID
                and message.data
                and message.data[0] == DISCOVERY_REQUEST
            ):
                for ecu in channel_ecus:
                    index = ord(ecu.name) - ord("A")
                    self.send(
                        ecu,
                        DISCOVERY_RESPONSE_BASE_ID + index,
                        ecu.discovery_payload(),
                    )
                    print(f"ecu-{ecu.name} discovery response on {channel}")
                continue

            ecu = routes.get(message.arbitration_id)
            if ecu is None or not message.data:
                continue

            result = ecu.apply(message.data[0])
            self.send(ecu, ecu.status_id, ecu.status_payload(result))
            print(
                f"ecu-{ecu.name} command={message.data[0]:02X} "
                f"state={ecu.state.name} result={result}"
            )

    async def heartbeat(self) -> None:
        while True:
            for ecu in ecuS:
                self.send(ecu, ecu.heartbeat_id, ecu.heartbeat_payload())
            await asyncio.sleep(0.1)

    async def run(self) -> None:
        print("Virtual ecus running: ")
        for ecu in ecuS:
            print(ecu.name + ":" + ecu.channel + " ")

        
        await asyncio.gather(
            *(self.receive_channel(channel) for channel in self.buses),
            self.heartbeat(),
        )

    def close(self) -> None:
        for notifier in self.notifiers:
            notifier.stop()
        for bus in self.buses.values():
            bus.shutdown()


async def main() -> None:
    network = VirtualecuNetwork()
    try:
        await network.run()
    finally:
        network.close()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass

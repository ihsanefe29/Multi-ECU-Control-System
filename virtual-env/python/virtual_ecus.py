#!/usr/bin/env python3

import asyncio
from dataclasses import dataclass
from enum import IntEnum

import can


# ============================================================
# PROTOCOL CONSTANTS
# ============================================================

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


# ============================================================
# VIRTUAL ECU
# ============================================================

@dataclass
class Virtualecu:
    name: str
    channel: str

    command_id: int
    status_id: int
    heartbeat_id: int
    telemetry_id: int

    state: PowerState = PowerState.OFF

    module1: bool = False
    module2: bool = False

    heartbeat_counter: int = 0

    # --------------------------------------------------------
    # Simulated telemetry values
    # --------------------------------------------------------

    # RAM 0
    n1: int = 0

    # RAM 2
    egt: int = 25

    # RAM 4
    fuel_flow: int = 0

    # RAM 6
    bleed_valve: int = 0

    # RAM 8
    # Raw value. Conversion can later be x * 0.01.
    vibration: int = 0

    # RAM 10
    major_version: int = 1

    # ========================================================
    # COMMAND HANDLING
    # ========================================================

    def apply(self, command_value: int) -> int:
        try:
            command = Command(command_value)
        except ValueError:
            return 1

        if command == Command.ON:
            if self.state == PowerState.KILL:
                return 1

            self.state = PowerState.ON
            self.module1 = True
            self.module2 = True

        elif command == Command.OFF:
            if self.state != PowerState.KILL:
                self.state = PowerState.OFF
                self.module1 = False
                self.module2 = False

        elif command == Command.KILL:
            self.state = PowerState.KILL
            self.module1 = False
            self.module2 = False

        elif command == Command.RESET:
            if self.state == PowerState.KILL:
                self.state = PowerState.OFF

            self.module1 = False
            self.module2 = False

        elif command == Command.REQUEST_STATUS:
            pass

        return 0

    # ========================================================
    # STATUS
    # ========================================================

    def status_payload(self, result: int = 0) -> bytes:
        return bytes([
            self.state,
            result,
            self.module1,
            self.module2,
        ])

    # ========================================================
    # HEARTBEAT
    # ========================================================

    def heartbeat_payload(self) -> bytes:
        self.heartbeat_counter = (
            self.heartbeat_counter + 1
        ) & 0xFF

        return bytes([
            ord(self.name),
            self.state,
            self.heartbeat_counter,
            0,
        ])

    # ========================================================
    # DISCOVERY
    # ========================================================

    def discovery_payload(self) -> bytes:
        """
        Discovery response:

        Byte 0    : DISCOVERY_RESPONSE
        Byte 1    : ECU name
        Byte 2-3  : command ID
        Byte 4-5  : status ID
        Byte 6-7  : heartbeat ID
        Byte 8-9  : telemetry ID
        """

        return bytes([
            DISCOVERY_RESPONSE,
            ord(self.name),

            (self.command_id >> 8) & 0xFF,
            self.command_id & 0xFF,

            (self.status_id >> 8) & 0xFF,
            self.status_id & 0xFF,

            (self.heartbeat_id >> 8) & 0xFF,
            self.heartbeat_id & 0xFF,

            (self.telemetry_id >> 8) & 0xFF,
            self.telemetry_id & 0xFF,
        ])

    # ========================================================
    # TELEMETRY
    # ========================================================

    @staticmethod
    def telemetry_entry(
        ram_address: int,
        raw_value: int
    ) -> bytes:
        """
        Core expects exactly:

        4 bytes RAM address
        4 bytes raw value

        Both big endian.

        Example:

        RAM address = 2
        raw value   = 700

        00 00 00 02 00 00 02 BC
        """

        return (
            ram_address.to_bytes(
                4,
                byteorder="big",
                signed=False
            )
            +
            raw_value.to_bytes(
                4,
                byteorder="big",
                signed=False
            )
        )

    def update_telemetry(self) -> None:
        """
        Generate simple simulated engine behaviour.

        ON:
            N1 ramps up
            EGT increases
            fuel flow increases
            vibration increases slightly

        OFF:
            N1 spins down
            EGT cools
            fuel flow becomes zero

        KILL:
            everything quickly moves towards safe/off state
        """

        if self.state == PowerState.ON:

            # -----------------------------
            # N1
            # -----------------------------

            if self.n1 < 6500:
                self.n1 += 100

            if self.n1 > 6500:
                self.n1 = 6500

            # -----------------------------
            # EGT
            # -----------------------------

            target_egt = 300 + self.n1 // 12

            if self.egt < target_egt:
                self.egt += min(
                    10,
                    target_egt - self.egt
                )

            # -----------------------------
            # Fuel flow
            # -----------------------------

            self.fuel_flow = (
                400 + self.n1 // 6
            )

            # -----------------------------
            # Bleed valve
            # -----------------------------

            self.bleed_valve = 1

            # -----------------------------
            # Vibration
            #
            # Raw 135 means 1.35 if
            # conversion is raw * 0.01.
            # -----------------------------

            self.vibration = (
                80 + self.n1 // 100
            )

        elif self.state == PowerState.OFF:

            # Engine spins down gradually.

            self.n1 = max(
                0,
                self.n1 - 150
            )

            self.egt = max(
                25,
                self.egt - 8
            )

            self.fuel_flow = 0
            self.bleed_valve = 0

            if self.n1 > 0:
                self.vibration = (
                    50 + self.n1 // 150
                )
            else:
                self.vibration = 0

        elif self.state == PowerState.KILL:

            self.n1 = max(
                0,
                self.n1 - 300
            )

            self.egt = max(
                25,
                self.egt - 15
            )

            self.fuel_flow = 0
            self.bleed_valve = 0
            self.vibration = 0

    def telemetry_payload(self) -> bytes:
        """
        Each telemetry value occupies 8 bytes.

        RAM 0  -> N1
        RAM 2  -> EGT
        RAM 4  -> Fuel Flow
        RAM 6  -> Bleed Valve
        RAM 8  -> Vibration
        RAM 10 -> Major Version

        Total:
            6 * 8 = 48 bytes

        Therefore this frame must be CAN FD.
        """

        payload = bytearray()

        payload += self.telemetry_entry(
            0,
            self.n1
        )

        payload += self.telemetry_entry(
            2,
            self.egt
        )

        payload += self.telemetry_entry(
            4,
            self.fuel_flow
        )

        payload += self.telemetry_entry(
            6,
            self.bleed_valve
        )

        payload += self.telemetry_entry(
            8,
            self.vibration
        )

        payload += self.telemetry_entry(
            10,
            self.major_version
        )

        return bytes(payload)


# ============================================================
# ECU DEFINITIONS
# ============================================================

ecuS = [
     Virtualecu(
         "A",
         "vcan0",
         0x100,
         0x101,
         0x102,
         0x103,
     ),

    Virtualecu(
        "B",
        "vcan0",
        0x110,     # command
        0x111,     # status
        0x112,     # heartbeat
        0x113,     # telemetry
    ),

    Virtualecu(
        "C",
        "vcan1",
        0x120,
        0x121,
        0x122,
        0x123,
    ),

    Virtualecu(
        "D",
        "vcan2",
        0x130,
        0x131,
        0x132,
        0x133,
    ),

    Virtualecu(
        "G",
        "vcan3",
        0x130,
        0x131,
        0x132,
        0x133,
    ),
]


# ============================================================
# VIRTUAL ECU NETWORK
# ============================================================

class VirtualecuNetwork:

    def __init__(self):
        self.buses = {
            channel: can.Bus(
                interface="socketcan",
                channel=channel,
                fd=True,
            )
            for channel in {
                ecu.channel for ecu in ecuS
            }
        }

        self.readers = {}
        self.notifiers = []

        loop = asyncio.get_running_loop()

        for channel, bus in self.buses.items():

            reader = can.AsyncBufferedReader()

            self.readers[channel] = reader

            self.notifiers.append(
                can.Notifier(
                    bus,
                    [reader],
                    loop=loop
                )
            )

    # ========================================================
    # SEND
    # ========================================================

    def send(
        self,
        ecu: Virtualecu,
        frame_id: int,
        payload: bytes,
        is_fd: bool = False
    ) -> None:

        try:
            self.buses[ecu.channel].send(
                can.Message(
                    arbitration_id=frame_id,
                    data=payload,
                    is_extended_id=False,
                    is_fd=is_fd,
                )
            )

        except can.CanError as error:
            print(
                f"ecu-{ecu.name} send error: "
                f"{error}"
            )

    # ========================================================
    # RECEIVE COMMANDS / DISCOVERY
    # ========================================================

    async def receive_channel(
        self,
        channel: str
    ) -> None:

        reader = self.readers[channel]

        channel_ecus = [
            ecu
            for ecu in ecuS
            if ecu.channel == channel
        ]

        routes = {
            ecu.command_id: ecu
            for ecu in channel_ecus
        }

        while True:

            message = await reader.get_message()

            # ------------------------------------------------
            # Discovery request
            # ------------------------------------------------

            if (
                message.arbitration_id
                == DISCOVERY_REQUEST_ID

                and message.data

                and message.data[0]
                == DISCOVERY_REQUEST
            ):

                for ecu in channel_ecus:

                    index = (
                        ord(ecu.name)
                        - ord("A")
                    )

                    self.send(
                        ecu,
                        DISCOVERY_RESPONSE_BASE_ID
                        + index,
                        ecu.discovery_payload(),
                        is_fd=True,
                    )

                    print(
                        f"ecu-{ecu.name} "
                        f"discovery response "
                        f"on {channel}"
                    )

                continue

            # ------------------------------------------------
            # Command frame
            # ------------------------------------------------

            ecu = routes.get(
                message.arbitration_id
            )

            if (
                ecu is None
                or not message.data
            ):
                continue

            result = ecu.apply(
                message.data[0]
            )

            self.send(
                ecu,
                ecu.status_id,
                ecu.status_payload(result)
            )

            print(
                f"ecu-{ecu.name} "
                f"command="
                f"{message.data[0]:02X} "
                f"state="
                f"{ecu.state.name} "
                f"result={result}"
            )

    # ========================================================
    # HEARTBEAT TASK
    # ========================================================

    async def heartbeat(self) -> None:

        while True:

            for ecu in ecuS:

                self.send(
                    ecu,
                    ecu.heartbeat_id,
                    ecu.heartbeat_payload()
                )

            await asyncio.sleep(0.1)

    # ========================================================
    # TELEMETRY TASK
    # ========================================================

    async def telemetry(self) -> None:

        while True:

            for ecu in ecuS:

                # Update simulated engine values.
                ecu.update_telemetry()

                payload = (
                    ecu.telemetry_payload()
                )

                # 48-byte payload => CAN FD
                if ecu.state == PowerState.ON:
                    self.send(
                        ecu,
                        ecu.telemetry_id,
                        payload,
                        is_fd=True,
                    )
                

            # 10 Hz telemetry
            await asyncio.sleep(0.1)

    # ========================================================
    # RUN
    # ========================================================

    async def run(self) -> None:

        print("Virtual ecus running:")

        for ecu in ecuS:
            print(
                f"{ecu.name}:{ecu.channel} "
                f"CMD=0x{ecu.command_id:03X} "
                f"STATUS=0x{ecu.status_id:03X} "
                f"HB=0x{ecu.heartbeat_id:03X} "
                f"TELEM=0x{ecu.telemetry_id:03X}"
            )

        await asyncio.gather(

            *(
                self.receive_channel(channel)
                for channel in self.buses
            ),

            self.heartbeat(),

            self.telemetry(),
        )

    # ========================================================
    # CLOSE
    # ========================================================

    def close(self) -> None:

        for notifier in self.notifiers:
            notifier.stop()

        for bus in self.buses.values():
            bus.shutdown()


# ============================================================
# MAIN
# ============================================================

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

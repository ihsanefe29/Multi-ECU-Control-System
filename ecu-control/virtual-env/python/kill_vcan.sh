#!/usr/bin/env bash
set -euo pipefail

echo "========== CAN Interface Manager =========="
echo
echo "Commands:"
echo "  search <type>          e.g. search vcan"
echo "  search can"
echo "  kill all"
echo "  kill vcan0"
echo "  kill vcan0 vcan1"
echo "  exit"
echo

while true; do
    read -rp "> " INPUT

    # Split input into tokens
    read -ra TOKENS <<< "$INPUT"

    [[ ${#TOKENS[@]} -eq 0 ]] && continue

    case "${TOKENS[0]}" in

        exit)
            echo "Exiting..."
            exit 0
            ;;

        search)
            if [[ ${#TOKENS[@]} -lt 2 ]]; then
                echo "Usage: search <type>"
                continue
            fi

            TYPE="${TOKENS[1]}"

            mapfile -t FOUND < <(
                ip -o link show |
                awk -F': ' '{print $2}' |
                grep "^${TYPE}[0-9]*$" || true
            )

            if [[ ${#FOUND[@]} -gt 0 ]]; then
                echo
                echo "Found ${TYPE} interfaces:"
                printf "  %s\n" "${FOUND[@]}"
            else
                echo
                echo "Could not find any '${TYPE}' interfaces."
            fi

            # Always show all CAN-like interfaces for reference
            mapfile -t ALL < <(
                ip -o link show |
                awk -F': ' '{print $2}' |
                grep -E '^(vcan|can)[0-9]+$' || true
            )

            if [[ ${#ALL[@]} -gt 0 ]]; then
                echo
                echo "Existing CAN interfaces:"
                printf "  %s\n" "${ALL[@]}"
            else
                echo
                echo "No CAN interfaces exist."
            fi
            ;;

        kill)

            if [[ ${#TOKENS[@]} -lt 2 ]]; then
                echo "Usage:"
                echo "  kill all"
                echo "  kill vcan0 vcan1 ..."
                continue
            fi

            TARGETS=()

            if [[ "${TOKENS[1]}" == "all" ]]; then
                mapfile -t TARGETS < <(
                    ip -o link show |
                    awk -F': ' '{print $2}' |
                    grep -E '^(vcan|can)[0-9]+$' || true
                )

                if [[ ${#TARGETS[@]} -eq 0 ]]; then
                    echo "No CAN interfaces to delete."
                    continue
                fi
            else
                TARGETS=("${TOKENS[@]:1}")
            fi


            for iface in "${TARGETS[@]}"; do

                if ! ip link show "$iface" >/dev/null 2>&1; then
                    echo "$iface does not exist."
                    continue
                fi

                echo
                echo "Are you sure you want to delete:"
                printf "  %s\n" "$iface}"
                read -rp "[y/N]: " ANSWER

                [[ ! "$ANSWER" =~ ^[Yy]$ ]] && {
                    echo "Cancelled."
                    continue
                }

                echo "Deleting $iface..."

                ip link set "$iface" down 2>/dev/null || true

                if ip link del dev "$iface"; then
                    echo "✓ $iface deleted"
                else
                    echo "Failed to delete $iface"
                fi
            done
            ;;

        *)
            echo "Unknown command."
            echo "Available commands:"
            echo "  search <type>"
            echo "  kill all"
            echo "  kill <iface1> <iface2> ..."
            echo "  exit"
            ;;
    esac
done
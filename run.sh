#!/usr/bin/env bash
set -e
pebble build
# pebble install --emulator basalt
# pebble install --emulator diorite
# pebble install --emulator emery
pebble install --phone 10.1.1.102
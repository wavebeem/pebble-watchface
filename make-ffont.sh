#!/usr/bin/env bash
set -e

PATH="$(npm bin):$PATH"

font="NotoSansUI-Regular.svg"
chars="[ 0-9A-Za-z:,./%-]"

fctx-compiler "$font" -r "$chars"
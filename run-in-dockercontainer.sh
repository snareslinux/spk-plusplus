#!/usr/bin/env bash

set -euo pipefail

IMAGE_TAG="pkgmgr-testenv"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="${SCRIPT_DIR}"

docker build -t "${IMAGE_TAG}" -f "${SCRIPT_DIR}/Containerfile" "${SCRIPT_DIR}"

docker run --rm -it \
    -v "${SRC_DIR}:/src" \
    -w /src \
    "${IMAGE_TAG}" \
    "${@:-/bin/bash}"
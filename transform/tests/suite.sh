#!/usr/bin/env bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT="${SCRIPT_DIR}/../.."

on_exit() {
    if [ "$?" -eq 0 ]; then
        echo -e "\nTests passed"
    else
        echo -e "\nTests failed"
    fi
}
trap on_exit EXIT

date
WD=$(mktemp -d -t haar-tests.XXXXXXXXXX)

for f in ${SCRIPT_DIR}/data/*.in.txt; do
    tput bold; tput setaf 3
    echo "TEST: $(basename ${f})"
    tput sgr0

    OUT_FILE=${WD}/$(basename ${f%.*}).out
    RES_EXPECTED=$(grep -m 1 '^RESULT' ${f} | cut -c9-)

    set +e
    grep -m 1 '^INPUT' ${f} | cut -c9- | ${ROOT}/bin/haar-transform > ${OUT_FILE}
    RES=$?
    set -e

    if [ ${RES} != "${RES_EXPECTED}" ]; then
        echo "Response code mismatch. Got ${RES}, expected ${RES_EXPECTED}."
        false
    fi

    diff -B <(cat ${OUT_FILE}) <(grep -m1 '^OUTPUT' ${f} | cut -c9-)
done

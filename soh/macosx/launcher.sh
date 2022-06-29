#!/bin/bash
APPPATH="${0%/*}"
cd "${APPPATH}"
APPPATH=$(pwd)
APPSUPPORT=$(./appsupport)
mkdir -p "${APPSUPPORT}/com.shipofharkinian.soh"
cd "${APPSUPPORT}/com.shipofharkinian.soh"
cp "${APPPATH}/oot.otr" .
${APPPATH}/soh

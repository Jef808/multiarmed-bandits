#!/usr/bin/env sh

cmake -S . -B build
cmake --build build
pushd src/aiviz/frontend
npm install
npm run build
popd

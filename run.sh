#!/usr/bin/env sh

./build/serverbackend/server 0.0.0.0 8080 &
cd frontend-vuetify
npm run dev &
echo 'Started websocket backend and frontend vite server'

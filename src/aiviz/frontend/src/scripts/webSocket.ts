import { ref } from 'vue'
import { useWebSocket as _useWebSocket } from '@vueuse/core';
import { useQueryHistoryStore } from '@/store/queryHistory.store';
import { useResultHistoryStore } from '@/store/resultHistory.store';

const defaultWsUrl = 'ws://localhost:8080';
const wsUrl = ref(defaultWsUrl);

async function useWebSocket() {
    const {
        status: wsStatus,
        send: wsSend,
        open: wsOpen,
        close: wsClose,
        ws: ws
    } = _useWebSocket(wsUrl.value, {
        autoReconnect:
        {
            retries: -1,
            delay: 2000,
            onFailed() {
                console.error("Failed to connect WebSocket after 3 retries");
            },
        },
        heartbeat: {
            message: "ping",
            interval: 5000,
            pongTimeout: 5000,
        },
        onConnected: () => {
            console.log("Successfully connected to WebSocket at ", wsUrl.value);
        },
        onMessage: (_, event) => function(resultStore: any) { resultStore.push(event.data); },// resultHistory.push(event.data),
        onDisconnected: (_: WebSocket, event: CloseEvent) => {
            console.warn("Web Socket Disconnected", event);
        },
        onError: (_, event) => {
            console.error(event);
        },
    });

    function wsReset() {
        wsUrl.value = defaultWsUrl;
        if (wsStatus.value !== "CLOSED") {
            wsClose(undefined, "Explicit Reset");
        }
        wsOpen();
    }
    function wsForceClose(reason?: string) {
        wsClose(undefined, reason);
    }

    return {
        wsStatus,
        wsSend,
        wsOpen,
        wsClose: wsForceClose,
        ws: ws,
    };
}

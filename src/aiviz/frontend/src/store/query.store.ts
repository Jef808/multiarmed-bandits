import uniqueId from "lodash.uniqueid";
import { readonly, ref, watch, type Ref, shallowReactive } from "vue";
import { useWebSocket } from "@vueuse/core";
import { defineStore } from "pinia";
import { useFormStore } from "@/store/form.store";
import { validateMsgFront2Back, validateMsgBack2Front } from "@/scripts/messageValidators";
import type { QueryFormModel, QueryResult, WithId, Model, Policy, Options } from "@/data/types";

export const getQuery = (): QueryFormModel => {
    const {
        model: { name: modelName, parameters: mParameters },
        policy: { name: policyName, parameters: pParameters },
        options: { parameters: oParameters },
    } = useFormStore().currentData;
    return {
        id: uniqueId('query-'),
        modelName,
        modelParameters: Object.fromEntries(
          mParameters.map((param) => [param.name, param.value])
        ),
        policyName,
        policyParameters: Object.fromEntries(
          pParameters.map((param) => [param.name, param.value])
        ),
        options: Object.fromEntries(
          oParameters.map((param) => [param.name, param.value])
        ),
    };
};

export const useQueryStore = defineStore("queryStore", () => {
    const queryHistory = shallowReactive([] as QueryFormModel[]);
    const resultHistory = shallowReactive([] as QueryResult[]);

    const currentResult = ref("");

    const wsUrl = ref("ws://localhost:8080");

    // A websocket for two-way communication with the backend
    const {
        status: wsStatus,
        data: wsData,
        send: wsSend,
        open: wsOpen,
        close,
        ws
    } = useWebSocket(wsUrl.value, {
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
        onMessage: (_, event) => {
          if (event.data === "pong")
            return;

          console.log("Received", event);

          try {
            const rep = JSON.parse(event.data);
            const validationResult = validateMsgBack2Front(rep);

            if (validationResult !== 'Ok') {
                console.error("Error while receiving response ", validationResult, JSON.stringify(rep, null, 2));
                return;
            }
            else {
                console.log(validationResult);
            }

            resultHistory.push(rep);
            currentResult.value = rep.id;
          }
          catch (err) {
            console.error("Failed to parse data of response", event);
          }
        },
        onDisconnected: (_: WebSocket, event: CloseEvent) => {
            console.warn("Web Socket Disconnected", event);
        },
        onError: (_, event) => {
            console.error(event);
        },
    });

    function wsReset() {
        wsUrl.value = "ws://localhost:8080";
        if (wsStatus.value !== "CLOSED") close(undefined, "Explicit Reset");
        wsOpen();
    }
    function wsClose(reason?: string) {
        close(undefined, reason);
    }

    /**
     * Send the curent query to the backend through the WebSocket
     */
    async function submitQuery(query: QueryForm) {
        const nbQueries = queryHistory.length;

        const duplicateEntry = queryHistory.find((q) => {
          if (query.modelName !== q.modelName
            || query.policyName !== q.policyName) {
              return false;
            }
          if (!Object.entries(query.modelParameters)
            .every((keyVal) => q.modelParameters[keyVal[0] as keyof Model] === keyVal[1])) {
            return false;
          }
          if (!Object.entries(query.policyParameters)
            .every((keyVal) => q.policyParameters[keyVal[0] as keyof Policy] === keyVal[1])) {
            return false;
          }
          if (!Object.entries(query.options)
             .every((keyVal) => q.options[keyVal[0] as keyof Options] === keyVal[1])) {
            return false;
          }
          return true;
        });

        // Do not resend the query if it was previously computed.
        if (duplicateEntry !== undefined) {
            console.log("Found query in cache");
            currentResult.value = duplicateEntry.id;
            return;
        }

        // Otherwise, send the query through the websocket (if its status is okay).
        if (wsStatus.value !== "OPEN") {
            console.warn(
                "submitQuery called with non-open websocket, query will be buffered",
                ws
            );
        }

        try {
            const req = JSON.stringify(query);
            const validationResult = validateMsgFront2Back(req);

            if (validationResult !== 'Ok') {
                console.error("Error while validating: ", validationResult, req);
                return;
            }
            else {
                console.log(validationResult);
            }

            wsSend(req);
            queryHistory.push(query);
            console.log("Query sent");
        } catch (err) {
            console.error("Error while sending query", query.id, err);
            queryHistory.length = nbQueries;
            if (resultHistory.length !== queryHistory.length) {
                console.warn(
                    "WARNING: query.store.ts: 'resultHistory' stack is out of sync with queryHistory"
                );
            }
        }
    }

    const setWebSocketUrl = (url: string) => {
        wsUrl.value = url;
    };

    return {
        wsClose,
        wsReset,
        wsStatus: readonly(wsStatus),
        wsUrl: readonly(wsUrl),
        submitQuery,
        queryHistory: readonly(queryHistory),
        resultHistory: readonly(resultHistory),
        currentResult: readonly(currentResult),
        setWebSocketUrl,
    };
});

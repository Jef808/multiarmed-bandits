import uniqueId from "lodash.uniqueid";
import { readonly, ref, shallowReactive } from "vue";
import { useWebSocket } from "@vueuse/core";
import { defineStore, storeToRefs } from "pinia";
import { useFormStore } from "@/store/form.store";
import { validateMsgBack2Front, ValidationResult } from "@/scripts/messageValidators";
import type { Query, QueryForm, QueryResult, Parameter } from "@/data/types";

function cmpEqual(a: QueryForm) {
    const _cmpEqual = (
        queryParameters: Record<string, number>,
        queryFormParameters: Parameter[]
    ): boolean => {
        return Object.entries(queryParameters).every(
            ([name, value]) => queryFormParameters.find(p => p.name === name)?.value === value
        )
    }
    return (b: Query) => {
        return a.model.name === b.modelName && a.policy.name === b.policyName
            && _cmpEqual(b.modelParameters, a.model.parameters)
            && _cmpEqual(b.policyParameters, a.policy.parameters)
            && _cmpEqual(b.options, a.options);
    }
}

function makeQuery(queryForm: QueryForm): Query {
    const {
        model: { name: modelName, parameters: modelParameters },
        policy: { name: policyName, parameters: policyParameters },
        options,
    } = queryForm;
    return {
        id: uniqueId('query-'),
        modelName,
        modelParameters: Object.fromEntries(
            modelParameters.map((param) => [param.name, param.value])
        ),
        policyName,
        policyParameters: Object.fromEntries(
            policyParameters.map((param) => [param.name, param.value])
        ),
        options: Object.fromEntries(
            options.map((param) => [param.name, param.value])
        ),
    };
};

export const useQueryStore = defineStore("queryStore", () => {
    const queryData = shallowReactive(new Map<Query['id'], Query>());
    const resultsData = shallowReactive(new Map<Query['id'], QueryResult>())
    const currentId = ref("")

    const { model, policy, options } = storeToRefs(useFormStore());

    const wsUrl = ref("ws://localhost:8080");

    // A websocket for two-way communication with the backend
    const {
        status: wsStatus,
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
            if (event.data === "pong") {
                return;
            }

            console.log("WS: Received message");
            const { validation, response } = validateMsgBack2Front(event.data);

            if (validation !== ValidationResult.Ok) {
                console.error("WS: Invalid result:", validation, response ? JSON.stringify(response, null, 2) : event.data);
                return;
            }

            const rep = JSON.parse(event.data);
            console.log("WS: received response:", rep);

            resultsData.set(rep.id, rep);
            currentId.value = rep.id;
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
     * Send the current query to the backend through the WebSocket
     */
    async function submit() {
        const queryForm = { model: model.value, policy: policy.value, options: options.value };

        const _cmpEqual = cmpEqual(queryForm);

        // Immediately get results from cache if query has already been processed.
        for (const [id, q] of queryData.entries()) {
            if (_cmpEqual(q)) {
                console.log("Query found in cache:", id);
                currentId.value = id;
                return;
            }
        }

        // Create new query otherwise
        const query = makeQuery(queryForm);

        // If websocket is still connecting, don't try sending request.
        if (wsStatus.value === 'CONNECTING') {
            console.error(
                "submitQuery called with websocket still connecting, query will not be sent.",
                ws
            );
            return;
        }
        // If websocket status is anything but 'OPEN', emit a warning.
        if (wsStatus.value !== 'OPEN') {
            console.warn(
                "submitQuery called with non-open websocket, query will be buffered",
                ws
            );
        }

        // Submit query and add it to history if no error occurs.
        wsSend(JSON.stringify(query));
        queryData.set(query.id, query);

        console.log("Submitted query", query.id);
    }

    // const setWebSocketUrl = (url: string) => {
    //     wsUrl.value = url;
    // };

    return {
        wsClose,
        wsReset,
        wsStatus: readonly(wsStatus),
        submit,
        queryData: readonly(queryData),
        resultsData: readonly(resultsData),
        currentId: readonly(currentId),
    };
});

<script setup lang="ts">
    import {
        computed,
        ref,
        toRef,
        nextTick
    } from "vue";
    import {
        storeToRefs
    } from "pinia";
    import type {
        ModelName,
        PolicyName
    } from "./data/types";
    import QueryForm from "./components/QueryForm.vue";
    import {
        useFormStore
    } from "./store/form.store";
    import {
        useQueryStore
    } from "./store/query.store";
    import QueryTextView from "./components/QueryTextView.vue";
    import ParametersForm from "./components/ParametersForm.vue";
    import D3LineChart from "./components/D3LineChart.vue";
    import D3ViolinChart from "./components/D3ViolinChart.vue";
    import ArmFactory, {
        type Arm,
        type Sample
    } from './scripts/arm';

    //const formStore = useFormStore();
    //const queryStore = useQueryStore();

    const {
        options,
        onUpdate,
        onSelect
    } = useFormStore();
    const {
        models,
        policies,
        model,
        policy
    } = storeToRefs(useFormStore());

    const selectedModel = ref(models.value[0].name);
    const selectedPolicy = ref(policies.value[0].name);

    const justSelected = ref(false);

    const {
        submit,
        wsReset,
        wsClose
    } = useQueryStore();
    const {
        currentId,
        wsStatus,
        queryData,
        resultsData,
        wsUrl
    } = storeToRefs(
        useQueryStore()
    );

    const debug = ref(true);
    const showWsInfo = ref(true);
    const panel = ref("");

    const haveResult = computed(() => {
        const ret = currentId.value !== undefined && currentId.value !== "";
        if (ret) {
            console.log("Have result: ", currentId.value);
        }
        return ret;
    });

    function onSelectModel() {
        onSelect("models", selectedModel.value);
        justSelected.value = true;
    }

    function onSelectPolicy() {
        onSelect("policies", selectedPolicy.value);
        justSelected.value = true;
    }

    function onUpdateModel(values: number[]) {
        onUpdate("models", values);
        panel.value = "policy";
        justSelected.value = false;
    }

    function onUpdatePolicy(values: number[]) {
        onUpdate("policies", values);
        panel.value = "options";
        justSelected.value = false;
    }

    function onUpdateOptions(values: number[]) {
        onUpdate("options", values);
        panel.value = "";
        justSelected.value = false;
    }

    function onCancel() {
        if (justSelected) {
            justSelected.value = false;
            return;
        }
        panel.value = "";
    }

    function onSubmitQuery() {
        submit();
    }

    function onResetWs() {
        wsReset();
    }

    function onCloseWs() {
        wsClose("Explicit Stop");
    }

    function onDebug() {
        console.log("model: ", model);
        console.log("policy: ", policy);
    }
    const wsColor = computed(() => {
        switch (wsStatus.value) {
            case "OPEN":
                return "green-lighten-4";
            case "CONNECTING":
                return "yellow-accent-1";
            case "CLOSED":
                return "red-darken-2";
            default:
                throw "Invalid wsState";
        }
    });

    const chartType = ref('lineChart'
        as 'lineChart' | 'violinChart');

    // Compute the value of the first series of the last result.
    const currentValues = computed(() => {
        const resultId = currentId.value;
        console.log("Result Id: ", resultId);

        const results = resultsData.value.get(resultId);
        console.log("Result: ", results);

        if (results !== undefined) {
            return results[0].values; // Pick the first series' values
        }
    });

    const chartProps = computed(() => ({
        id: currentId.value, // the id of the corresponding query
        name: "rewards", // the name of the series containing the results
        values: currentValues.value?.map(({
            step,
            value
        }) => ({
            x: step as number,
            y: value as number,
        })),
        width: 1000,
        height: 500,
        xPadding: 30,
        yPadding: 30,
    }));

    const violinChartProps = computed(() => {
        if (model.value.name !== 'mab') {
            console.error('violin chart unimplemented for model', model.value.name);
            throw "Unimplemented method";
        }
        const nbOfArms = model.value.parameters['numberOfArms'];
        const arms = [] as Arm[];
        for (let i = 0; i < nbOfArms; ++i) {
            arms.push(ArmFactory(i, model.value.parameters[i].mean, model.value.parameters[i].stdDev));
        }
        currentValues.value.forEach(({
            action,
            step,
            value
        }) => {
            arms[action].pushSample({
                id: step,
                value
            });
        });
        return {
            data: arms
        };
    });
</script>

<template>
    <v-app>
        <v-main>
            <div>
                <v-btn @click="onDebug">Debug</v-btn>
                <br />
                <v-btn @click="chartType = 'lineChart'">Line Chart</v-btn>
                <v-btn @click="chartType = 'violinChart'">Violin Chart</v-btn>
            </div>
            <v-row>
                <v-col cols="8">
                    <v-row>
                        <v-col cols="auto">
                            <v-container style="border:1px solid;border-down: none" v-if="haveResult">
                                <D3LineChart v-bind="chartProps" v-if="chartType === 'lineChart'"></D3LineChart>
                                <D3ViolinChart v-bind="violinChartProps" v-else-if="chartType === 'violinChart'">
                                </D3ViolinChart>
                            </v-container>
                            <v-container v-else>No results to display...</v-container>
                        </v-col>
                    </v-row>
                    <v-container style="border:1px solid">
                        <v-checkbox id="toggle-debug" v-model="debug" label="Debug Info"></v-checkbox>
                        <v-divider></v-divider>
                        <v-row v-if="debug && haveResult">
                            <v-col cols="6">
                                <h2>Query History</h2>
                                {{ queryData.size }} queries
                                <v-list v-if="queryData.size > 0">
                                    <v-list-item v-for="(query, idx) in queryData.values()" :key="idx">
                                        <pre>{{ query }}</pre>
                                    </v-list-item>
                                </v-list>
                            </v-col>
                            <v-col cols="6">
                                <v-row>
                                    <v-col cols="auto">
                                        <h2>Result History</h2>
                                        {{ resultsData.size }} results
                                    </v-col>
                                </v-row>
                                <v-row>
                                    <v-col cols="auto">
                                        <h2>Current Result</h2>
                                        {{ currentId }}
                                        <v-list>
                                            <v-list-item v-for="(value, idx) in currentValues" :key="idx">
                                                <pre>{{ value }}</pre>
                                            </v-list-item>
                                        </v-list>
                                    </v-col>
                                </v-row>
                            </v-col>
                        </v-row>
                    </v-container>
                </v-col>
                <v-col cols="4">
                    <v-container id="div-query-form">
                        <QueryForm />
                    </v-container>
                </v-col>
                <v-container id="websocket-tile">
                    <v-expansion-panels>
                        <v-expansion-panel>
                            <v-expansion-panel-title :color="wsColor">
                                <v-row>
                                    <v-col cols="6" class="d-flex align-center">
                                        <v-list-item-title>WebSocket Info</v-list-item-title>
                                    </v-col>
                                    <v-col cols="6" class="d-flex justify-center">
                                        <v-list-item title="Status" :subtitle="wsStatus"></v-list-item>
                                    </v-col>
                                </v-row>
                            </v-expansion-panel-title>
                            <v-expansion-panel-text>
                                <v-form>
                                    <v-row>
                                        <v-col cols="12" class="d-flex justify-end space-between">
                                            <v-row>
                                                <v-col cols="8">
                                                    <v-label for="wsUrl-input">Websocket address</v-label>
                                                    <v-text-field v-model="wsUrl" disabled></v-text-field>
                                                </v-col>
                                                <v-spacer></v-spacer>
                                                <v-col cols="auto">
                                                    <v-btn type="button" @click="onResetWs">Reset</v-btn>
                                                </v-col>
                                                <v-col cols="auto">
                                                    <v-btn type="button" @click="onCloseWs">Stop</v-btn>
                                                </v-col>
                                            </v-row>
                                        </v-col>
                                    </v-row>
                                </v-form>
                            </v-expansion-panel-text>
                        </v-expansion-panel>
                    </v-expansion-panels>
                </v-container>
            </v-row>
        </v-main>
    </v-app>
</template>
<script setup lang="ts">
 import { computed, reactive, ref, onMounted } from "vue";
 import { vOnClickOutside } from "@vueuse/components";
 import { storeToRefs } from "pinia";
 import { useFormStore } from "@/store/form.store";
 import { getQuery, useQueryStore } from "@/store/query.store";
 import QueryTextView from "@/components/QueryTextView.vue";
 import ParametersForm from "@/components/ParametersForm.vue";
 import D3LineChart from "@/components/D3LineChart.vue";
 import type { QueryResult } from "@/data/types";

 const store = useFormStore();
 const queryStore = useQueryStore();

 const {
   initialModelName,
   initialPolicyName,
   modelSelectionItems,
   policySelectionItems,
   onUpdate,
   showRaw,
   onSelectModel,
   onSelectPolicy,
 } = store;

 const { modelParameters, policyParameters, optionsParameters } =
   storeToRefs(store);

 const { submitQuery, setWebSocketUrl, wsReset, wsClose } = queryStore;
 const { queryHistory, resultHistory, currentResult, wsStatus, wsUrl } =
   storeToRefs(queryStore);

 const wsUrlRef = ref(wsUrl);
 const debug = ref(true);
 const showWsInfo = ref(true);

 const panel = ref("");

 const selectedModel = ref(initialModelName);
 const selectedPolicy = ref(initialPolicyName);

 const haveResult = computed(() => {
     const ret = currentResult.value !== undefined && currentResult.value !== "";
     if (ret) {
         console.log("Have result: ", currentResult.value);
     }
     return ret;
 });

 function selectModel(name: string) {
   selectedModel.value = name;
   onSelectModel(name);
 }
 function selectPolicy(name: string) {
   selectedPolicy.value = name;
   onSelectPolicy(name);
 }

 function updateModel(values: number[]) {
   onUpdate("model", values);
   panel.value = "policy";
 }
 function updatePolicy(values: number[]) {
   onUpdate("policy", values);
   panel.value = "options";
 }
 function updateOptions(values: number[]) {
   onUpdate("options", values);
   panel.value = "";
 }

 function onCancel() {
   panel.value = "";
 }

 function onSubmitQuery() {
   const query = getQuery();
   console.log("Submiting query", query);
   submitQuery(query);
 }

 function onSubmitWsUrl() {
   if (wsUrlRef.value === wsUrl.value) return;
   setWebSocketUrl(wsUrlRef.value);
 }

 function onResetWs() {
   wsReset();
 }

 function onCloseWs() {
   wsClose("Explicit Stop");
 }

 const wsColor = computed(() => {
   const { [`${wsStatus.value}`]: color } = {
     OPEN: "green-lighten-4",
     CONNECTING: "yellow-accent-1",
     CLOSED: "red-darken-2",
   };
   return color;
 });

 const currentValues = computed(() => {
   const resultId = currentResult.value;
   console.log("Result Id: ", resultId);
   const result = resultHistory.value.find((r) => {
     return r.id === resultId;
   });
   console.log("Result: ", result);
   return result.data[0].values;
 });

 const chartProps = computed(() => ({
   id: currentResult.value,
   name: "rewards",
   values: currentValues.value,
   width: 1000,
   height: 500,
   xPadding: 30,
   yPadding: 20
 }));
</script>

<template>
  <v-app>
    <v-main>
      <v-row>
        <v-col cols="8">
          <v-row>
            <v-col cols="auto">
              <v-container
                v-if="haveResult"
              >
                <D3LineChart v-bind="chartProps"></D3LineChart>
              </v-container>
              <v-container
                v-else
              >
                No result to display...
              </v-container>
            </v-col>
          </v-row>
          <v-row v-if="debug && haveResult">
            <v-col cols="6">
              <h2>Query History</h2>
              {{ queryHistory.length }} queries
              <v-list v-if="queryHistory.length > 0">
                <v-list-item v-for="(query, idx) in queryHistory" :key="idx">
                  <QueryTextView :query="query"></QueryTextView>
                </v-list-item>
              </v-list>
            </v-col>
            <v-col cols="6">
              <v-row>
                <v-col cols="auto">
                  <h2>Result History</h2>
                  {{ resultHistory.length }} results
                </v-col>
              </v-row>
              <v-row>
                <v-col cols="auto">
                  <h2>Current Result</h2>
                  {{ currentResult }}
                  <v-list>
                    <v-list-item v-for="(result, idx) in resultHistory" :key="idx">
                      <pre>{{ result.data }}</pre>
                    </v-list-item>
                  </v-list>
                </v-col>
              </v-row>
            </v-col>
          </v-row>
        </v-col>
        <v-col cols="4">
          <div class="form-container">
            <v-form @submit.prevent="onSubmitQuery">
              <v-expansion-panels v-model="panel" v-on-click-outside="onCancel">
                <v-expansion-panel class="text-grey" value="model" title="MODEL">
                  <v-expansion-panel-text class="text-primary">
                    <v-select
                      :model-value="selectedModel"
                      @update:modelValue="selectModel"
                      :items="modelSelectionItems"
                      item-title="label"
                      item-value="name"
                    ></v-select>
                    <ParametersForm
                      :data-name="selectedModel"
                      :items="modelParameters"
                      @change="updateModel"
                      @cancel="onCancel"
                    ></ParametersForm>
                  </v-expansion-panel-text>
                </v-expansion-panel>
                <v-expansion-panel class="text-grey" value="policy" title="POLICY">
                  <v-expansion-panel-text class="text-primary">
                    <v-select
                      :model-value="selectedPolicy"
                      @update:modelValue="selectPolicy"
                      :items="policySelectionItems"
                      item-title="label"
                      item-value="name"
                    ></v-select>
                    <ParametersForm
                      :data-name="selectedPolicy"
                      :items="policyParameters"
                      @change="updatePolicy"
                      @cancel="onCancel"
                    ></ParametersForm>
                  </v-expansion-panel-text>
                </v-expansion-panel>
                <v-expansion-panel class="text-grey" value="options" title="OPTIONS">
                  <v-expansion-panel-text class="text-primary">
                    <ParametersForm
                      :items="optionsParameters"
                      @change="updateOptions"
                      @cancel="onCancel"
                    ></ParametersForm>
                  </v-expansion-panel-text>
                </v-expansion-panel>
              </v-expansion-panels>
              <v-container>
                <v-btn type="submit" color="primary" block>Submit</v-btn>
              </v-container>
            </v-form>
          </div>
          <v-checkbox id="toggle-debug" v-model="debug" label="Debug Info"></v-checkbox>
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
                  <v-form @submit.prevent="onSubmitWsUrl">
                    <v-text-field label="Url" v-model="wsUrlRef"></v-text-field>
                    <v-row>
                      <v-col cols="12" class="d-flex justify-end space-between">
                        <v-row>
                          <v-spacer></v-spacer>
                          <v-col cols="auto">
                            <v-btn type="button" @click="onResetWs">Reset</v-btn>
                          </v-col>
                          <v-col cols="auto">
                            <v-btn type="button" @click="onCloseWs">Stop</v-btn>
                          </v-col>
                          <v-col cols="auto">
                            <v-btn type="submit">Save</v-btn>
                          </v-col>
                        </v-row>
                      </v-col>
                    </v-row>
                  </v-form>
                </v-expansion-panel-text>
              </v-expansion-panel>
            </v-expansion-panels>
          </v-container>
        </v-col>
      </v-row>
    </v-main>
  </v-app>
</template>

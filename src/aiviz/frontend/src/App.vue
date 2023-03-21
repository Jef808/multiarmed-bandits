<script setup lang="ts">
 import { computed, ref } from "vue";
 import { vOnClickOutside } from "@vueuse/components";
 import { storeToRefs } from "pinia";
 import type { ModelName, PolicyName } from "./data/types";
 import { useFormStore } from "./store/form.store";
 import { useQueryStore } from "./store/query.store";
 import QueryTextView from "./components/QueryTextView.vue";
 import ParametersForm from "./components/ParametersForm.vue";
 import D3LineChart from "./components/D3LineChart.vue";

 //const formStore = useFormStore();
 //const queryStore = useQueryStore();

 const {
   modelNames,
   policyNames,
   options,
   onUpdate,
   onSelect,
 } = useFormStore();
 const { model, policy } =
     storeToRefs(useFormStore());

 const { submit, wsReset, wsClose } = useQueryStore();
 const { currentId, wsStatus, queryData, resultsData } =
   storeToRefs(useQueryStore());

 // const selectedModel = computed(() => currentData.value.model);
 // const selectedPolicy = computed(() => currentData.value.policy);

 // const wsUrlRef = ref(wsUrl);
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

 function onSelectModel(name: ModelName) {
     onSelect('models', name);
 }
 function onSelectPolicy(name: PolicyName) {
     onSelect('policies', name);
 }
 function updateModel(values: number[]) {
   onUpdate('models', values);
   panel.value = 'policy';
 }
 function updatePolicy(values: number[]) {
   onUpdate('policies', values);
   panel.value = 'options';
 }
 function updateOptions(values: number[]) {
   onUpdate('options', values);
   panel.value = '';
 }
 function onCancel() {
   panel.value = '';
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
 const wsColor = computed(() => {
     switch (wsStatus.value) {
        case 'OPEN': return 'green-lighten-4';
        case 'CONNECTING': return 'yellow-accent-1';
        case 'CLOSED': return 'red-darken-2';
        default: throw 'Invalid wsState';
     }
 });

 // Compute the value of the first series of the last result.
 const currentValues = computed(() => {
   const resultId = currentId.value;
   console.log("Result Id: ", resultId);

   const results = resultsData.value.get(resultId);
   console.log("Result: ", results);

   return results[0].values;   // Pick the first series' values
 });

 const chartProps = computed(() => ({
   id: currentId.value,  // the id of the corresponding query
   name: "rewards",      // the name of the series containing the results
   values: currentValues.value.map(({step, value}) => ({ x: step, y: value })),
   width: 1000,
   height: 500,
   xPadding: 30,
   yPadding: 30
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
                  <D3LineChart
                      v-bind="chartProps"
                  ></D3LineChart>
              </v-container>
              <v-container
                v-else
              >
                No results to display...
              </v-container>
            </v-col>
          </v-row>
          <v-row v-if="debug && haveResult">
            <v-col cols="6">
              <h2>Query History</h2>
              {{ queryData.size }} queries
              <v-list v-if="queryData.size > 0">
                <v-list-item v-for="(query, idx) in queryData.values()" :key="idx">
                    <span>{{ query.id }}:</span>
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
        </v-col>
        <v-col cols="4">
          <div class="form-container">
            <v-form @submit.prevent="onSubmitQuery">
              <v-expansion-panels v-model="panel" v-on-click-outside="onCancel">
                <v-expansion-panel class="text-grey" value="model" title="MODEL">
                  <v-expansion-panel-text class="text-primary">
                    <v-select
                      :model-value="model.name"
                      @update:modelValue="onSelectModel"
                      :items="modelNames"
                      item-title="label"
                      item-value="name"
                    ></v-select>
                    <ParametersForm
                      :data-name="model.name"
                      :items="model.parameters"
                      @change="updateModel"
                      @cancel="onCancel"
                    ></ParametersForm>
                  </v-expansion-panel-text>
                </v-expansion-panel>
                <v-expansion-panel class="text-grey" value="policy" title="POLICY">
                  <v-expansion-panel-text class="text-primary">
                    <v-select
                      :model-value="policy.name"
                      @update:modelValue="onSelectPolicy"
                      :items="policyNames"
                      item-title="label"
                      item-value="name"
                    ></v-select>
                    <ParametersForm
                      :data-name="policy.name"
                      :items="policy.parameters"
                      @change="updatePolicy"
                      @cancel="onCancel"
                    ></ParametersForm>
                  </v-expansion-panel-text>
                </v-expansion-panel>
                <v-expansion-panel class="text-grey" value="options" title="OPTIONS">
                  <v-expansion-panel-text class="text-primary">
                    <ParametersForm
                      :items="options"
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
                  <v-form>
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

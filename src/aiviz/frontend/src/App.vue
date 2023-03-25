<script setup lang="ts">
import {computed, ref} from "vue";
import {storeToRefs} from "pinia";
import {useQueryStore} from "./store/query.store";
import QueryForm from "./components/QueryForm.vue";
import D3LineChart from "./components/D3LineChart.vue";

const {wsStatus, wsUrl, currentId} = storeToRefs(useQueryStore());
const {wsReset, wsClose} = useQueryStore();

const tab = ref("query");

function onResetWs() {
  wsReset();
}

function onCloseWs() {
  wsClose("Explicit Stop");
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

const chartType = ref("lineChart" as "lineChart" | "violinChart");

// Compute the value of the first series of the last result.
// const currentValues = computed(() => {
//   const resultId = currentId.value;
//   console.log("Result Id: ", resultId);
//
//   const results = resultsData.value.get(resultId);
//   console.log("Result: ", results);
//
//   if (results !== undefined) {
//     return results[0].values; // Pick the first series' values
//   }
// });

// const chartProps = computed(() => ({
//   id: currentId.value, // the id of the corresponding query
//   name: "rewards", // the name of the series containing the results
//   values: currentValues.value?.map(({step, value}) => ({
//     x: step as number,
//     y: value as number,
//   })),
//   width: 1000,
//   height: 500,
//   xPadding: 30,
//   yPadding: 30,
// }));
</script>

<template>
  <v-app>
    <v-app-bar>
      <v-app-bar-nav-icon></v-app-bar-nav-icon>
      <v-app-bar-title>MLView</v-app-bar-title>

      <v-spacer></v-spacer>

      <v-btn icon>
        <v-icon>mdi-magnify</v-icon>
      </v-btn>

      <template v-slot:extension>
        <v-tabs v-model="tab" align-tabs="title" grow>
          <v-tab value="viewer">Viewer</v-tab>

          <v-tab value="query">New Query</v-tab>

          <v-tab value="debug">Debug</v-tab>
        </v-tabs>
      </template>
    </v-app-bar>

    <v-main>
      <v-window v-model="tab">
        <v-window-item value="query">
          <template #default>
            <QueryForm />
          </template>
        </v-window-item>

        <v-window-item value="viewer">
          <template #default>
            <v-card flat>
              <v-card-text>Place viewer here...</v-card-text>
            </v-card>
          </template>
        </v-window-item>

        <v-window-item value="debug">
          <template #default>
            <v-card flat>
              <v-card-text>Place debug info here...</v-card-text>
            </v-card>
          </template>
        </v-window-item>
      </v-window>
    </v-main>

    <v-spacer></v-spacer>

    <v-col cols="auto">
      <v-footer :color="wsColor">
        Websocket status: <b>{{ wsStatus }}</b>

        <v-spacer></v-spacer>

        Address: <b>{{ wsUrl }}</b>
      </v-footer>
    </v-col>
  </v-app>
</template>

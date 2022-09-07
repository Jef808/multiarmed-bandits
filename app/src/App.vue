<template>
  <div>
    <h1>Multi-armed bandits visualization</h1>
  </div>
  <metrics-chart></metrics-chart>
</template>

<script>
import { JSC } from "jscharting-vue";
import MetricsChart from "./components/MetricsChart.vue";

export default {
  name: "App",
  components: {
    MetricsChart,
  },
};
</script>

<style>
.btn {
  background-color: #0876ff;
  border-radius: 4px;
  border-width: 0;
  color: white;
  padding: 0.5em;
  font-family: sans-serif;
  font-size: 0.9em;
  margin-right: 10px;
  margin-top: 5px;
  min-width: 60px;
}

.btn:focus,
.btn:hover,
.btn.selected {
  background-color: #05418c;
}
</style>

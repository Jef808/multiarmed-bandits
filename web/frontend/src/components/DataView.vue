<script setup lang="ts">
import { ref, PropType } from "vue";
import { Bar } from "vue-chartjs";
import {
  Chart as ChartJS,
  Title,
  Tooltip,
  Legend,
  BarElement,
  CategoryScale,
  LinearScale,
  Plugin,
} from "chart.js";

ChartJS.register(
  Title,
  Tooltip,
  Legend,
  BarElement,
  CategoryScale,
  LinearScale
);

defineProps({
  chartId: {
    type: String,
    default: "bar-chart",
  },
  width: {
    type: Number,
    default: 400,
  },
  height: {
    type: Number,
    default: 400,
  },
  cssClasses: {
    type: String,
    default: "",
  },
  styles: {
    type: Object as PropType<Partial<CSSStyleDeclaration>>,
    default: () => {},
  },
  plugins: {
    type: Array as PropType<Plugin<"bar">[]>,
    default: () => [],
  },
});

const chartOptions = {
  responsive: true,
  maintainAspectRatio: false,
};

const chartData = ref({
  labels: [
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
  ],
  datasets: [
    {
      label: "Data One",
      backgroundColor: "#f87979",
      data: [40, 20, 12, 39, 10, 40, 39, 80, 40, 20, 12, 11],
    },
  ],
});
</script>

<template>
  <div>
      <Bar
        :chart-data="chartData"
        :chart-options="chartOptions"
        :chart-id="chartId"
        :width="width"
        :height="height"
        :css-classes="cssClasses"
        :styles="styles"
        :plugins="plugins"
      />
  </div>
</template>

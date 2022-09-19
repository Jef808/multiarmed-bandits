<script setup lang="ts">
import { watchEffect, onMounted, onUnmounted } from "vue";
import MABInteractive from "./MABInteractive.vue";
import { withGaussianNoise } from "./../scripts/utils";

interface Props {
  numberOfArms: number;
  valueMean?: number;
  valueStdDev?: number;
  noiseMean?: number;
  noiseStdDev?: number;
}

interface Arm {
  id: number;
  value: number;
  visits: number;
  total: number;
  sample: () => number;
}

const {
  numberOfArms,
  valueMean = 0.0,
  valueStdDev = 1.0,
  noiseMean = 0.0,
  noiseStdDev = 1.0,
} = defineProps<Props>();

let arms: Array<Arm> = $ref([]);
let dataReady: boolean = $ref(false);

onMounted(() => {
  const genValue = withGaussianNoise({
    mean: valueMean,
    stdDev: valueStdDev,
  });

  arms = Array.from({ length: numberOfArms }, (v, i) => {
    const value = genValue(0.0);
    return {
      id: i + 1,
      value: value,
      visits: 0,
      total: 0.0,
      sample: () => {
        const reward = genValue(value);
        arms[i].visits += 1;
        arms[i].total += reward;
        return reward;
      },
    };
  });

  dataReady = true;
  console.log("arms:", arms);
});

onUnmounted(() => {
  arms.length = 0;
  dataReady = false;
});

watchEffect(() => {
  console.log(
    "Number of arms:",
    numberOfArms,
    "\nNoise avg/stdDev:",
    noiseMean,
    noiseStdDev,
    "\nData ready:",
    dataReady
  );
});
</script>

<template>
  <h1 class="text-3xl font-bold underline">
    MAB with {{ numberOfArms }} arms.
  </h1>
  <MABInteractive v-if="dataReady" :arms="arms" />
  <br />
  <ol>
    <li v-for="arm in arms" :key="arm.id">
      Arm {{ arm.id }}: value: {{ arm.value.toFixed(2) }}, visits:
      {{ arm.visits }}, total: {{ arm.total.toFixed(2) }}
    </li>
  </ol>
</template>

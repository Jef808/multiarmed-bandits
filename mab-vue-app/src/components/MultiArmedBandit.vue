<script setup lang="ts">
import { watchEffect, onMounted } from "vue";
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
}

const {
  numberOfArms,
  valueMean = 0.0,
  valueStdDev = 1.0,
  noiseMean = 0.0,
  noiseStdDev = 1.0,
} = defineProps<Props>();

let arms: Array<Arm> = $ref();
const dataReady: bool = $ref(false);

onMounted(() => {
  const genValue: () => number = withGaussianNoise(valueMean, valueStdDev);

  arms = Array.from({ length: numberOfArms }, (v, i) => {
    return { id: i + 1, value: genValue(), visits: 0 };
  });

  dataReady = true;
  console.log("arms:", arms);
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
  <ol>
    <li v-for="arm in arms" :key="arm.id">
      Arm {{ arm.id }}: value: {{ arm.value }}, visits: {{ arm.visits }}
    </li>
  </ol>
  <MABInteractive v-if="dataReady" :arms="arms" />
</template>

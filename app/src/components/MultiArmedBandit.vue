<template>
  <div>
    <div>
      <h1>Multi-armed bandit</h1>
      <ViolinChart :data="arms" />
    </div>
  </div>
</template>

<script setup lang="ts">
import { defineProps, onMounted, ref } from "vue";
import { useRandomNormal } from "./randomNormal";
import { Arm } from "./arm";
import ViolinChart from "./ViolinChart.vue";

export interface Props {
  numberOfArms: number;
  initialNumberOfSamples: number;
}

const props = withDefaults(defineProps<Props>(), {
  numberOfArms: 10,
  initialNumberOfSamples: 100,
});

const arms = ref(
  Array.from({ length: props.numberOfArms }, (v, i) => {
    return new Arm(i, useRandomNormal(0, 1));
  })
);

const onSample = (id: number) => {
  if (id < 0 || id >= arms.value.length) {
    console.log("Sample id out of bounds");
    return;
  }
  arms.value[id].sample();
};

onMounted(() => {
  arms.value.forEach((a) => {
    Array.from({ length: props.initialNumberOfSamples }, () => onSample(a.id));
  });
});
</script>

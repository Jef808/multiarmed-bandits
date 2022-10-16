<template>
  <div class="select-policy">
    <v-select v-model="selected" label="name" :options="policies" />
  </div>
</template>

<script setup lang="ts">
import { ref, type Ref } from "vue";
import type { Policy } from "../../views/HomeView.vue";

const policies: Policy[] = [
  {
    name: "Epsilon Greedy",
    parameters: {
      epsilon: 0.1,
    },
  },
  {
    name: "UCB",
    parameters: {
      exploration: 0.7,
    },
  },
];

const selected: Ref<Policy> = ref({ name: "", parameters: {} });
</script>

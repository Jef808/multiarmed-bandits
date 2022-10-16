<template>
  <div class="input-parameters">
    <label for="input-numberOfStep">Number of steps: </label>
    <vue-number-input
      id="input-numberOfSteps"
      v-model="numberOfSteps"
      :min="0"
      :step="50"
      size="small"
      inline
      center
      controls
    ></vue-number-input>
  </div>
</template>

<script setup lang="ts">
import { ref, type Ref } from "vue";

const numberOfSteps: Ref<number> = ref(500);
</script>

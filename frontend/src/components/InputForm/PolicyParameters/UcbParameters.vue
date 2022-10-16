<template>
  <div class="parameters container">
    <label for="input-exploration">Exploration: </label>
    <vue-number-input
      id="input-exploration"
      v-model="exploration"
      :min="0.0"
      :max="1.0"
      :step="0.05"
      inline
      controls
    ></vue-number-input>
  </div>
</template>

<script setup lang="ts">
import { ref, type Ref } from "vue";

const exploration: Ref<number> = ref(0.7);
</script>

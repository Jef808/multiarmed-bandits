<template>
  <div class="parameters container">
    <label for="input-epsilon">Epsilon: </label>
    <vue-number-input
      id="input-epsilon"
      v-model="epsilon"
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

const epsilon: Ref<number> = ref(0.1);
</script>

<template>
  <div class="parameters container">
    <label for="input-numberOfArms">Number of arms: </label>
    <vue-number-input
      id="input-numberOfArms"
      v-model="numberOfArms"
      :min="2"
      inline
      controls
    ></vue-number-input>
  </div>
</template>

<script setup lang="ts">
import { ref, type Ref } from "vue";

const numberOfArms: Ref<number> = ref(2);
</script>

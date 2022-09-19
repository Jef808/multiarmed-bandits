<template>
  <div></div>
  <!-- <multi-armed-bandit :numberOfArms="10" /> -->
</template>

<script setup lang="ts">
import { onMounted } from "vue";
//import MultiArmedBandit from "./components/MultiArmedBandit.vue";
import test_mab from "./scripts/test/mab/test_multi-armed-bandit";

onMounted(() => {
  test_mab();
});
</script>

<style scoped></style>

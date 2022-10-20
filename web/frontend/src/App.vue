<script setup lang="ts">
// This starter template is using Vue 3 <script setup> SFCs
// Check out https://vuejs.org/api/sfc-script-setup.html#script-setup
import FormRequest from "./components/FormRequest.vue";
import SubmitRequest from "./components/SubmitRequest.vue";
</script>

<template>
  <div class="controls">
    <FormRequest />
    <SubmitRequest />
  </div>
</template>

<style scoped>
.controls {
    display: flex;
}
</style>

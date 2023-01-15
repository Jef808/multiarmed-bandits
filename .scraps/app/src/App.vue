<script setup lang="ts">
import { ref } from "vue";
import MultiArmedBandit from "./components/MultiArmedBandit.vue";
import axiosSamples from "./axiosSamples";

const baseUrl = ref("127.0.0.1:5558");
const serverUrl = ref("/");

function onFetch() {
  axiosSamples(serverUrl.value);
}
</script>

<template>
  <main>
    <MultiArmedBandit :numberOfArms="10" :initialNumberOfSamples="10" />
    <p>
      <label>Base URL: </label>
      <input id="input-base-url" v-model="baseUrl" />
      <label>Server URL: </label>
      <input id="input-server-url" v-model="serverUrl" />
      <button @click="onFetch">Fetch</button>
    </p>
  </main>
</template>

<style scoped>
@media (min-width: 1024px) {
  header {
    display: flex;
    place-items: center;
    padding-right: calc(var(--section-gap) / 2);
  }

  .logo {
    margin: 0 2rem 0 0;
  }

  header .wrapper {
    display: flex;
    place-items: flex-start;
    flex-wrap: wrap;
  }
}
</style>

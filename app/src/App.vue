<script setup lang="ts">
import MultiArmedBandit from "./components/MultiArmedBandit.vue";
</script>

<template>
  <main>
    <MultiArmedBandit :numberOfArms="10" />
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

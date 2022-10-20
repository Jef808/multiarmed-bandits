<template>
  <button @click="onClick">Submit</button>
</template>

<script setup lang="ts">
import * as axios from "axios";

function onClick() {
  console.log("Clicked");
}
</script>

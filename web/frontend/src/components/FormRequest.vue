<script setup lang="ts">
import InputSelection from "./input/Selection.vue";
import * as Input from "./input";
import * as Options from "./options";

const modelOptions = Options.models();
const policyOptions = Options.policies();

function onClick() {
  console.log("Clicked");
}
</script>

<template>
<InputSelection :options="modelOptions" class="box" />
<InputSelection :options="policyOptions" class="box" />
</template>

<style scoped>
.box {
  display: flex;
}
</style>

<script setup lang="ts">
import { withDefaults } from "vue";
import type { Parameter } from "./index";

const props = defineProps<{ parameters: Parameter[] }>();
</script>

<template>
  <ul>
    <li v-for="parameter in parameters">
      <div class="flex">
        <h4 class="label-wrapper">
          <label for="parameter.name">{{ parameter.label }}: </label>
        </h4>
      </div>
      <div class="flex">
        <vue-number-input
          v-model="parameter.value"
          :min="parameter.min"
          :max="parameter.max"
          :step="parameter.steps"
          controls
        ></vue-number-input>
      </div>
    </li>
  </ul>
</template>

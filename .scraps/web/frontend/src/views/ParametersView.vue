<template>
  <v-list dense>
    <v-list-item>
      <v-row
        v-for="param in parameters"
        :key="param.id"
        justify="space-between"
      >
        <v-col cols="auto">
          <v-list-item-content>{{ param.label }}</v-list-item-content>
        </v-col>
        <v-col cols="auto">
          <v-list-item-content>{{ param.value }}</v-list-item-content>
        </v-col>
      </v-row>
    </v-list-item>
  </v-list>
</template>

<script setup lang="ts">
import type { Parameter } from "@/types";

defineProps({
  parameters: Parameter,
});
</script>

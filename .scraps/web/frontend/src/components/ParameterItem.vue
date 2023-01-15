<template>
  <v-row justify="space-between">
    <v-col col="auto">
      <v-list-item-content>{{ props.label }}:</v-list-item-content>
    </v-col>
    <v-col col="auto">
      <v-list-item-content>{{ props.value }}</v-list-item-content>
    </v-col>
  </v-row>
</template>

<script setup lang="ts">
type Props = {
  id: string;
  label: string;
  min: number;
  max: number;
  value: number;
  step: number;
};

const props = defineProps<Props>();
</script>

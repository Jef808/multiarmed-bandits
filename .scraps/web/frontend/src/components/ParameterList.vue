<template>
  <v-list dense>
    <v-list-item v-for="param in props.parameters" :key="param.id">
      <ParameterItem v-bind="param" />
    </v-list-item>
  </v-list>
</template>

<script setup lang="ts">
import type { Item } from "@/types";
import ParameterItem from "@/components/ParameterItem.vue";

type Props = {
  parameters: Item;
};

const props = defineProps<Props>();
</script>

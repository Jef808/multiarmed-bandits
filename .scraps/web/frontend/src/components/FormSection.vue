<template>
  <v-card>
    <v-card-title>
      <h5>{{ props.label }}</h5>
    </v-card-title>
    <v-divider></v-divider>
    <ParameterList v-bind="props.parameters"> </ParameterList>
  </v-card>
</template>

<script setup lang="ts">
import ParameterList from "@/components/ParameterList.vue";
import type { Model, Policy } from "@/types";

type Props = {
  label: string; // 'Model' or 'Policy'
  valueModel: Model | Policy;
};

const props = defineProps<Props>();
</script>

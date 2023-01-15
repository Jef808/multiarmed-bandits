<template>
  <v-card>
    <v-card-title>
      <h5>{{ value.label }}</h5>
    </v-card-title>
    <v-divider></v-divider>
  </v-card>
</template>

<script setup lang="ts">
import { ref } from "vue";
import type { Model, Policy, QueryOption } from "@/types";

type Props = {
  valueModel: Model | Policy;
};

const props = defineProps<Props>();
const value = ref(props.valueModel);
</script>

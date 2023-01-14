<script setup lang="ts">
import { computed, withDefaults } from "vue";
import type { QueryForm } from "@/data/types";
export interface Props {
  query: QueryForm;
  selected?: boolean;
}
const props = withDefaults(defineProps<Props>(), {
  selected: false,
});

// Converts an object of property-values {{ objName: objValue }, ... } into
// an array of named and valued records [{ name: objName, value: objValue }, ... ]
const extractParameters = (obj: { [name: string]: number }) => {
  return Object.entries(obj).map(([name, value]) => ({ name, value }));
};

const modelParameters = computed(() => {
  return extractParameters(props.query.modelParameters);
});
const policyParameters = computed(() => {
  return extractParameters(props.query.policyParameters);
});
const optionsParameters = computed(() => {
  return extractParameters(props.query.options);
});

// Change variant when selected?
const variant = computed(() => {});
</script>

<template>
  <v-card variant="outlined">
    <v-list>
      <v-list-subheader>
        {{ query.id }}
      </v-list-subheader>
      <v-list-subheader>
        Model: {{ query.modelName }}
      </v-list-subheader>
        <v-card>
          <v-list-item
            v-for="param in modelParameters"
            :title="param.name"
            :subtitle="param.value"
            :value="param.value"
          ></v-list-item>
        </v-card>
      <v-list-subheader>
          Policy: {{ query.policyName }}
      </v-list-subheader>
        <v-card>
          <v-list-item
            v-for="param in policyParameters"
            :key="param.name"
            :title="param.name"
            :subtitle="param.value"
            :value="param.value"
          ></v-list-item>
        </v-card>
      <v-list-subheader title="Options"></v-list-subheader>
        <v-card>
          <v-list-item
            v-for="param in optionsParameters"
            :key="param.name"
            :title="param.name"
            :subtitle="param.value"
            :value="param.value"
          ></v-list-item>
        </v-card>
    </v-list>
  </v-card>
</template>

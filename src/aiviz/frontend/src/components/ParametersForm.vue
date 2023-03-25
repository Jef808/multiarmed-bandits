<script setup lang="ts">
import {reactive, ref, watch, onMounted} from "vue";
import type {Parameter} from "../data/types";

export interface Props {
  name: string;
  label: string;
  modelValue: Parameter[];
}
interface Emits {
  (e: "update:modelValue", values: Parameter[]): void;
}

const props = defineProps<Props>();
const emit = defineEmits<Emits>();

// local copy of parameter values.
// TODO: Parse input coming from the textfield as int64 (same as slider input)
// const modelValues = reactive(props.items.map(({value}) => value));
const modelValues = reactive([]);

onMounted(() => {
  resetModelValues();
});

// Repopulate `modelValues` upon change of props.name
// Note: Cannot watch a property of a reactive object, so use a getter as watched value
// Question: Should we use watchEffect with a callback that lets us avoid the onMounted callback?
watch(
  () => props.name,
  () => resetModelValues(),
);

function resetModelValues() {
  modelValues.length = props.modelValue.length;
  modelValues.forEach((_, idx, arr) => {
    arr[idx] = ref(props.modelValue[idx].value);
  });
}

function onSave() {
  emit(
    "update:modelValue",
    props.modelValue.map((param, i) => ({value: modelValues[i], ...param})),
  );
}
function onCancel() {
  resetModelValues();
}
</script>

<template>
  <v-form>
    <v-list lines="two">
      <v-list-item v-for="(param, idx) in modelValue" :key="idx">
        <v-list-item-title>
          <span>{{ param.label }}: </span>
        </v-list-item-title>
        <v-list-item-subtitle>
          <v-slider
            v-model="modelValues[idx]"
            :min="param.min"
            :max="param.max"
            :step="param.step"
            density="compact"
            hide-details
          >
            <template #append>
              <v-text-field
                v-model="modelValues[idx]"
                density="compact"
                type="number"
                style="width: 80px"
                hide-details
                single-line
              >
              </v-text-field>
            </template>
          </v-slider>
        </v-list-item-subtitle>
      </v-list-item>
    </v-list>
    <v-btn color="success" @click="onSave">Save</v-btn>
    <v-btn color="error" @click="onCancel">Cancel</v-btn>
  </v-form>
</template>

<script lang="ts">
export default {
  name: "ParametersForm",
};
</script>

<script setup lang="ts">
import {ref, watch} from "vue";
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
const modelValues = ref(props.modelValue.map(({value}) => value));

// Repopulate `modelValues` upon change of props.name
// Note: Cannot watch a property of a reactive object, so use a getter as watched value
// Question: Should we use watchEffect with a callback that lets us avoid the onMounted callback?
watch(
  () => props.name,
  () => {
    resetModelValues();
  },
);

function resetModelValues() {
  modelValues.value = props.modelValue.map(({value}) => value);
}

function onSave() {
  emit(
    "update:modelValue",
    props.modelValue.map((param, i) => ({
      ...param,
      value: modelValues.value[i],
    })),
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
          <span>{{ param.label }}:</span>
        </v-list-item-title>
        <v-list-item-subtitle>
          <pre>modelValues[idx]: {{ modelValues[idx] }}</pre>
          <v-slider
            v-model="modelValues[idx]"
            class="align-center"
            :min="param.min"
            :max="param.max"
            :step="param.step"
            hide-details
          >
            <template #append>
              <v-text-field
                v-model="modelValues[idx]"
                hide-details
                density="compact"
                type="number"
                style="width: 80px"
                single-line
              ></v-text-field>
            </template>
          </v-slider>
        </v-list-item-subtitle>
      </v-list-item>
    </v-list>
    <v-btn color="success" @click="onSave">Save</v-btn>
    <v-btn color="error" @click="onCancel">Cancel</v-btn>
  </v-form>
</template>

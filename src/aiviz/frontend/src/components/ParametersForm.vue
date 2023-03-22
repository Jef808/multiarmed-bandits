<script setup lang="ts">
import { reactive, watch } from "vue";
import type { Parameter } from "../data/types";

export interface Props {
  dataName?: string;
  items: Parameter[];
}
interface Emits {
  // (e: "select", value: typeof props.items[number])
  // (e: "submit", values: number[]): void;
  (e: "change", values: number[]): void;
  (e: "cancel"): void;
}

const props = withDefaults(defineProps<Props>(), {
  dataName: "options",
});
const emit = defineEmits<Emits>();

// local copy of each values into an array
// TODO: Parse input coming from the textfield as int64 (same as slider input)
const modelValues = reactive(props.items.map(({ value }) => value));

// Repopulate `modelValues` upon change of props.dataName
// Note: Cannot watch a property of a reactive object, so use a getter as watched value
watch(
  () => props.dataName,
  () => resetModelValues()
);

function resetModelValues() {
  modelValues.length = props.items.length;
  modelValues.forEach((_, idx, arr) => (arr[idx] = props.items[idx].value));
}

function onSave() {
  emit("change", modelValues);
}
function onCancel() {
  emit("cancel");
}
</script>

<template>
  <v-form>
    <v-list lines="two">
      <v-list-item v-for="(item, idx) in items" :key="idx">
        <v-list-item-title>
          <span>{{ item.label }}: </span>
        </v-list-item-title>
        <v-list-item-subtitle>
          <v-slider
            v-model="modelValues[idx]"
            density="compact"
            hide-details
            :min="item.min"
            :max="item.max"
            :step="item.step"
          >
            <template #append>
              <v-text-field
                v-model="modelValues[idx]"
                hide-details
                single-line
                density="compact"
                type="number"
                style="width: 80px"
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

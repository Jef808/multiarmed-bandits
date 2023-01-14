<script lang="ts">
export default {
  name: "ParametersForm",
};
</script>

<script setup lang="ts">
import { computed, reactive, ref, toRefs, onMounted, watch } from "vue";
import type { Parameter } from "@/data/types";

export interface Props {
  dataName?: string;
  items: {
    label: string;
    value: number;
    min?: number;
    max?: number;
    step?: number;
  }[];
}

interface Emits {
  (e: "change", values: number[]): void;
  (e: "cancel"): void;
}

const props = withDefaults(defineProps<Props>(), {
  dataName: "options",
});
const emit = defineEmits<Emits>();

// local copy of each values into an array
// TODO: Parse input coming from the textfield as int64 (same as slider input)
let modelValues = reactive(props.items.map(({ value }) => value));

// Repopulate `modelValues` upon change of data source
// Note1: Cannot watch a property of a reactive object, so use a getter
watch(
  () => props.dataName,
  (newName, oldName) => {
    resetModelValues();
  }
);

function resetModelValues() {
  modelValues.forEach((value, idx) => (value = props.items[idx].value));
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
      <v-list-item v-for="(item, idx) in items">
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

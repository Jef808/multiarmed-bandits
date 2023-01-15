<template>
  <div class="input-field">
    <label :for="properties.id">{{ properties.label }}</label>
    <input
      type="number"
      :id="properties.id"
      :step="properties.sliderStep"
      :min="properties.min"
      :max="properties.max"
      :value="modelValue"
      @change="onChange(parseFloat(($event.target as HTMLInputElement).value))"
    />
  </div>
</template>

<script setup lang="ts">
import { computed, ref, unref } from "vue";

type Props = {
  properties: {
    id: string;
    name: string;
    label: string;
    min: number;
    max: number;
    sliderStep: number;
  };
  modelValue: number;
};

const props = defineProps<Props>();

const emit = defineEmits<{
  (e: "change", id: string, value: number): void;
}>();

function onChange(value: number) {
  emit("change", props.properties.id, value);
}
</script>

<style scoped></style>

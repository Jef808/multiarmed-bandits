<template>
  <div>
    <div class="label-wrapper">
      <label :for="name">{{ props.label }}</label>
    </div>
    <input :id="name" type="number" :value="value" :disabled="disabled" />
  </div>
  <div class="button-group">
    <button @click="onEditOrSave">{{ btnLabel }}</button>
    <button v-if="!disabled" @click="onCancel">Cancel</button>
  </div>
</template>

<script setup lang="ts">
import { computed, ref, unref } from "vue";

export type Props = {
  name: string;
  label: string;
  modelValue: number;
  min: number;
  max: number;
  sliderStep: number;
};

const props = defineProps<Props>();

const emit = defineEmits<{
  (e: "update", name: string, value: number): void;
}>();

const disabled = ref(true);
const value = ref(props.modelValue);

const btnLabel = computed(() => {
  return disabled.value ? "Edit" : "Save";
});

function toggleEditMode() {
  disabled.value = !disabled.value;
}

function onEditOrSave() {
  if (!disabled.value) {
    emit("update", props.name, unref(value));
  }
  toggleEditMode();
}

function onCancel() {
  value.value = props.modelValue;
  toggleEditMode();
}
</script>

<style scoped>
.label {
}
.input {
}
.button {
}
</style>

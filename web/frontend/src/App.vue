<script setup lang="ts">
import {
  ref,
  type Ref,
  toRefs,
  computed,
  onRenderTracked,
  onRenderTriggered,
} from "vue";
import uniqueId from "lodash.uniqueid";
import { models } from "@/models";
import { policies } from "@/policies";
import { queryOptions } from "@/query";
import InputParameter from "@/components/InputParameter.vue";

const selectedModel = ref(models[0]);
const selectedPolicy = ref(policies[0]);

function onUpdate(name: string, newValue: number) {
  if (name.startsWith("option-")) {
    let toUpdate = options.find((opt) => name.endsWith(opt.value.name));
    if (toUpdate !== undefined) {
      toUpdate.value.modelValue = newValue;
    }
  } else if (name.startsWith("policy-")) {
    let toUpdate = selectedPolicy.value.parameters.find((param) =>
      name.endsWith(param.name)
    );
    if (toUpdate !== undefined && toUpdate.modelValue !== undefined) {
      toUpdate.modelValue = newValue;
    }
  } else if (name.startsWith("model-")) {
    let toUpdate = selectedModel.value.parameters.find((param) =>
      name.endsWith(param.name)
    );
    if (toUpdate !== undefined && toUpdate.modelValue !== undefined) {
      toUpdate.modelValue = newValue;
    }
  }
}

function onSubmit() {}
</script>

<template>
  <form @submit.prevent="onSubmit">
    <div>
      <h2>
        <label for="select-model">Model</label>
      </h2>
      <select id="select-model" v-model="selectedModel">
        <option disabled value="">Select Model</option>
        <option v-for="model in models" :key="model.name">
          {{ model.label }}
        </option>
      </select>
    </div>
    <div>
      <h2>
        <label for="select-model">Policy</label>
      </h2>
      <select id="select-model" v-model="selectedPolicy">
        <option disabled value="">Select Policy</option>
        <option v-for="policy in policies" :key="policy.name">
          {{ policy.label }}
        </option>
      </select>
    </div>
    <div>
      <h2>Options</h2>
      <InputParameter
        v-for="option in queryOptions"
        :key="option.name"
        v-bind="option"
        @update="onUpdate"
      />
    </div>
    <div>
      <button type="submit">Submit</button>
    </div>
  </form>
</template>

<style>
.visually-hidden {
  position: absolute;
  height: 1px;
  width: 1px;
  overflow: hidden;
  clip: rect(1px 1px 1px 1px);
  clip: rect(1px, 1px, 1px, 1px);
  clip-path: rect(1px, 1px, 1px, 1px);
  white-space: nowrap;
}
</style>

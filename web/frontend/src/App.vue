<script setup lang="ts">
import {
  reactive,
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

const selectedModel = ref(models[0].label);
const selectedPolicy = ref(policies[0].label);
const options = ref(queryOptions);

const selectedModelParameters = computed(() => {
  let model = models.find((m) => m.label === selectedModel.value);
  if (model !== undefined) {
    return model.parameters;
  }
});
const selectedPolicyParameters = computed(() => {
  let policy = policies.find((p) => p.label === selectedPolicy.value);
  if (policy !== undefined) {
    return policy.parameters;
  }
});

function onSubmit() {}
</script>

<template>
  <form @submit.prevent="onSubmit">
    <div class="header">
      <div class="container">
        <h2>
          <label for="select-model">Model</label>
        </h2>
        <select id="select-model" v-model="selectedModel">
          <option disabled value="">Select Model</option>
          <option v-for="model in models" :key="model.name">
            {{ model.label }}
          </option>
        </select>
        <InputParameter
          v-for="param in selectedModelParameters"
          :key="param.name"
          v-bind="param"
        />
      </div>
      <div class="container">
        <h2>
          <label for="select-policy">Policy</label>
        </h2>
        <select id="select-policy" v-model="selectedPolicy">
          <option disabled value="">Select Policy</option>
          <option v-for="policy in policies" :key="policy.name">
            {{ policy.label }}
          </option>
        </select>
        <InputParameter
          v-for="param in selectedPolicyParameters"
          :key="param.name"
          v-bind="param"
        />
      </div>
      <div class="container">
        <h2>Options</h2>
        <InputParameter
          v-for="option in options"
          :key="option.name"
          v-bind="option"
        />
      </div>
      <div>
        <button type="submit">Submit</button>
      </div>
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

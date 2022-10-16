<script setup lang="ts">
import { ref, type Ref } from "vue";
import ModelForm from "../components/InputForm/ModelSelection.vue";
import PolicyForm from "../components/InputForm/PolicySelection.vue";
import ParametersForm from "../components/InputForm/ParametersSelection.vue";

export interface Model {
  name: string;
  parameters: object;
}
export interface Policy {
  name: string;
  parameters: object;
}
export interface Parameters {
  numberOfSteps: number;
}

const model: Ref<Model> = ref({ name: "", parameters: {} });
const policy: Ref<Policy> = ref({ name: "", parameters: {} });
const parameters: Ref<Parameters> = ref({ numberOfSteps: 0 });
</script>

<template>
  <main>
    <div>
      <div>
        <h1>Model</h1>
      </div>
      <div>
        <model-form></model-form>
      </div>
    </div>
    <div>
      <div>
        <h1>Policy</h1>
      </div>
      <div>
        <policy-form></policy-form>
      </div>
    </div>
    <div>
      <div>
        <h1>Query Parameters</h1>
      </div>
      <div>
        <parameters-form></parameters-form>
      </div>
    </div>
  </main>
</template>

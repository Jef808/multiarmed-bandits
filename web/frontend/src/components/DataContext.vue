<script setup lang="ts">
import { ref } from "vue";

const models = ref([
  {
    name: "multiArmedBandit",
    label: "Multiarmed bandit",
    parameters: [
      {
        name: "numberOfArms",
        label: "Number of Arms",
        value: 2,
        min: 1,
        max: Infinity,
      },
    ],
  },
]);

const policies = ref([
  {
    name: "epsilonGreedy",
    label: "Epsilon Greedy",
    parameters: [
      {
        name: "epsilon",
        label: "epsilon",
        value: 0.1,
        min: 0.0,
        max: 1.0,
        steps: 0.05,
      },
    ],
  },
  {
    name: "ucb",
    label: "Ucb",
    parameters: [
      {
        name: "exploration",
        label: "exploration",
        value: 0.7,
        min: 0.0,
        max: Infinity,
        steps: 0.05,
      },
    ],
  },
]);

const dataOptions = ref([
  {
    name: "dataOptions",
    label: "Options",
    parameters: [
      {
        name: "numberOfSteps",
        label: "Number of Steps",
        value: 1,
        min: 1,
        max: Infinity,
        steps: 1,
      },
    ],
  },
]);

const selectedModel = ref(models.value[0]);
const selectedPolicy = ref(policies.value[0]);
</script>

<template>
  <form>
    <div class="flex container">
      <div id="input-model" class="flex container">
        <div class="flex">
          <h2><label for="select-model">Model:</label></h2>
        </div>
        <div class="flex">
          <select id="select-model" v-model="selectedModel">
            <option v-for="model in models">{{ model.label }}</option>
          </select>
        </div>
        <div class="flex">
          <ul>
            <li v-for="parameter in selectedModel.parameters">
              <div>
                <h3>
                  <label for="`model-parameter-${parameter.name}`">
                    {{ parameter.label }}
                  </label>
                </h3>
              </div>
              <div>
                <input
                  :id="`model-parameter-${parameter.name}`"
                  type="number"
                  v-model="parameter.value"
                />
              </div>
            </li>
          </ul>
        </div>
      </div>
      <div id="input-policy" class="flex container">
        <div class="flex">
          <h2><label for="select-model">Policy:</label></h2>
        </div>
        <div class="flex">
          <select v-model="selectedPolicy">
            <option v-for="policy in policies">{{ policy.label }}</option>
          </select>
        </div>
        <div class="flex">
          <ul>
            <li v-for="parameter in selectedPolicy.parameters">
              <div>
                <h3>
                  <label for="">{{ parameter.label }}</label>
                </h3>
              </div>
              <div>
                <input
                  :id="`policy-parameter-${parameter.name}`"
                  type="number"
                  v-model="parameter.value"
                />
              </div>
            </li>
          </ul>
        </div>
      </div>
    </div>
  </form>
</template>

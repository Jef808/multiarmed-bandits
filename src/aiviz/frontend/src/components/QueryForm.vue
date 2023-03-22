<script setup lang="ts">
import type {Model, Policy} from "../data/types";
import {useFormStore} from "../store/form.store";
import {computed, reactive, ref} from "vue";
import ParametersForm from "./ParametersForm.vue";
import {useQueryStore} from "../store/query.store";

const {models, policies, options, onUpdate} = useFormStore();

const selectedModelName = ref(models[0].name);
const selectedPolicyName = ref(policies[0].name);

const selectedModel = ref(models[0]);
const selectedPolicy = ref(policies[0]);

const expandedPanels = ref([] as string[]);

const panels = reactive({
  model: {
    expanded: computed(() => expandedPanels.value.includes("model")),
    modified: false,
  },
  policy: {
    expanded: computed(() => expandedPanels.value.includes("policy")),
    modified: false,
  },
  options: {
    expanded: computed(() => expandedPanels.value.includes("options")),
    modified: false,
  },
});

function onSubmit() {
  useQueryStore().submit({
    model: selectedModel.value,
    policy: selectedPolicy.value,
    options,
  });
  foldPanels();
}
function onCancel() {
  foldPanels();
}
function onUpdateModel(values: number[]) {
  selectedModel.value.parameters.forEach((param, idx) => {
    param = {...param, value: values[idx]};
  });
  panels.model.expanded = false;
  panels.model.modified = true;
  if (!panels.policy.modified) {
    panels.policy.expanded = true;
  }
}
function onUpdatePolicy(values: number[]) {
  selectedPolicy.value.parameters.forEach((param, idx) => {
    param = {...param, value: values[idx]};
  });
  panels.policy.expanded = false;
  panels.policy.modified = true;
  if (!panels.options.modified) {
    panels.options.expanded = true;
  }
}
function onUpdateOptions(values: number[]) {
  options.forEach((param, idx) => {
    param = {...param, value: values[idx]};
  });
  panels.options.expanded = false;
  panels.options.modified = true;
}

function foldPanels() {
  expandedPanels.value = [];
}
function openPanels() {
  expandedPanels.value = ["model", "policy", "options"];
}
</script>

<template>
  <div class="form-container" style="border: 1px solid">
    <v-form @submit.prevent="onSubmit">
      <v-expansion-panels
        v-model="expandedPanels"
        v-click-outside="foldPanels"
        multiple
      >
        <v-expansion-panel class="text-grey" value="model" title="MODEL">
          <v-expansion-panel-subtitle>
            <h3>{{ selectedModel.label }}</h3>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-subtitle
            v-for="param in selectedModel.parameters"
            :key="param.name"
          >
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <v-select
              v-model="selectedModel"
              :items="models"
              item-title="label"
              item-value="name"
            ></v-select>
            <ParametersForm
              :dataName="selectedModelName"
              :items="selectedModel.parameters"
              @change="onUpdateModel"
              @cancel="onCancel"
            ></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
        <v-expansion-panel class="text-grey" value="policy" title="POLICY">
          <v-expansion-panel-subtitle>
            <h3>{{ selectedPolicy.label }}</h3>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-subtitle
            v-for="param in selectedPolicy.parameters"
            :key="param.name"
          >
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <v-select
              v-model="selectedPolicy"
              :items="policies"
              item-title="label"
              item-value="name"
            ></v-select>
            <ParametersForm
              :dataName="selectedPolicyName"
              :items="selectedPolicy.parameters"
              @change="onUpdatePolicy"
              @cancel="onCancel"
            ></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
        <v-expansion-panel class="text-grey" value="options" title="OPTIONS">
          <v-expansion-panel-subtitle
            v-for="param in options"
            :key="param.name"
          >
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <ParametersForm
              :items="options"
              @change="onUpdateOptions"
              @cancel="onCancel"
            ></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
      </v-expansion-panels>
      <v-container>
        <v-btn type="submit" color="primary" block>Submit</v-btn>
      </v-container>
    </v-form>
  </div>
</template>

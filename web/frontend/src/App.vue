<script setup lang="ts">
import {
  ref,
  toRef,
  toRefs,
  toRaw,
  computed,
  onRenderTracked,
  onRenderTriggered,
  onBeforeMount,
  onMounted,
} from "vue";
import { storeToRefs } from "pinia";
import type { Model, Policy, QueryOption, NamedParameter, List } from "@/types";
import { useModelStore } from "@/stores/models";
import { usePolicyStore } from "@/stores/policies";
import { useQueryStore } from "@/stores/query";
// import { useStore } from "@/stores/store";
import FormSection from "@/components/FormSection.vue";

const { models, selectedModel, modelLoading } = storeToRefs(useModelStore());
const { fetchModels, addModelDocument } = useModelStore();

const { policies, selectedPolicy, policyLoading } = storeToRefs(
  usePolicyStore()
);
const { fetchPolicies, addPolicyDocument } = usePolicyStore();

const { queryOptions } = storeToRefs(useQueryStore());

// const selectedModel = toRef(models.value, "0");
// const selectedPolicy = toRef(policies.value, "0");

onBeforeMount(() => {
  fetchModels();

  addModelDocument({
    name: "mab",
    label: "Multiarmed Bandit",
    parameters: [
      {
        name: "numberOfArms",
        label: "Number of Arms",
        value: 2,
        min: 2,
      },
    ],
  });

  addPolicyDocument({
    name: "epsilonGreedy",
    label: "Epsilon Greedy",
    parameters: [
      {
        name: "epsilon",
        label: "Epsilon",
        value: 0.1,
        min: 0,
        max: 1,
        step: 0.05,
      },
    ],
  });

  addPolicyDocument({
    name: "ucb",
    label: "Upper Confidence Bound",
    parameters: [
      {
        name: "exploration",
        label: "Exploration Constant",
        value: 0.7,
        min: 0,
        step: 0.05,
      },
    ],
  });

  fetchModels();
  // console.log("%cBefore Mount", "font-size:18px");
  // console.log("Models:", models.value);
  // console.log("Policies:", policies.value);
  // console.log("Options:", queryOptions.value);
  // console.log("Selected Model:", selectedModel);
  // console.log("Selected Policy:", selectedPolicy);
});

onMounted(() => {
  // console.log("%cMounted", "font-size:18px");
  // console.log("Models:", JSON.stringify(models.value, null, 2));
  // console.log("Policies:", JSON.stringify(policies.value, null, 2));
  // console.log("Options:", JSON.stringify(queryOptions.value, null, 2));
  //
  // console.log("Selected Model:", JSON.stringify(selectedModel.value, null, 2));
  // console.log(
  //   "Selected Policy:",
  //   JSON.stringify(selectedPolicy.value, null, 2)
  // );
});
</script>

<template>
  <v-app id="app">
    <v-main class="bg-grey-lighten-3">
      <div class="d-flex align-center flex-column">
        <div class="text-subtitle-2">Model</div>
        <v-card width="400">
          <template v-slot:title> MODEL </template>
          <template v-slot:subtitle>
            <v-select
              v-model="selectedModel"
              :items="models.value"
              item-title="label"
              item-value="parameters"
              label="Select Model"
              persistent-hint
              return-object
            >
              <!-- <template v-slot:selection="{ item }">
                   <span>{{ item.label }}</span>
                   </template> -->
            </v-select>
          </template>
          <template v-slot:text> ParameterList... </template>
        </v-card>
      </div>
      <div class="d-flex align-center flex-column">
        <div class="text-subtitle-2">Policy</div>
        <v-card>
          <template v-slot:title> POLICY </template>
          <template v-slot:subtitle>
            <v-select
              v-model="selectedPolicy"
              :items="policies.value"
              item-title="label"
              item-value="parameters"
              label="Select Policy"
              persistent-hint
              return-object
            >
              <!-- <template v-slot:selection="{ item }">
                     <span>{{ item.label }}</span>
                     </template> -->
            </v-select>
          </template>
          <template v-slot:text> ParameterList... </template>
        </v-card>
      </div>
      <!-- <v-form>
           <v-container>
           <v-row>
           <v-col cols="12" md="4">
           <div>
           <v-select v-bind="models"></v-select>
           </div>
           </v-col>
           </v-row>
           </v-container>
           </v-form> -->
    </v-main>
  </v-app>
</template>

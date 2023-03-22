import { readonly, reactive } from "vue";
import { defineStore } from "pinia";
import { items as _models } from "../data/models.json" assert { type: "json" };
import { items as _policies } from "../data/policies.json" assert { type: "json" };
import { items as _options } from "../data/options.json" assert { type: "json" };
import type {
  DataModel,
  Model,
  Policy,
  Option,
} from "../data/types";

/**
 * Pinia store holding the form data for queries.
 */
export const useFormStore = defineStore("formStore", () => {
  const models = reactive(_models as Model[]);
  const policies = reactive(_policies as Policy[]);
  const options = reactive(_options as Option[]);

  // const selectedModelName = ref(models[0].name);
  // const selectedPolicyName = ref(policies[0].name);

  // const selectedModel = computed(() => {
  //   return models.find((m) => m.name === selectedModelName.value) as DataModel;
  // });
  // const selectedPolicy = computed(() => {
  //   return policies.find(
  //     (p) => p.name === selectedPolicyName.value
  //   ) as DataModel;
  // });
  // let selectedModel = ref(models[0]);
  // let selectedPolicy = ref(policies[0]);
  // const selected = {
  //   model: ref(models[0]),
  //   policy: ref(policies[0]),
  // };

  // function updateModel(model: Model, values: number[]) {
  //   values.forEach((val, idx) => {
  //     model.parameters[idx].value = val;
  //   });
  // }

  // function updatePolicy(policy: Policy, values: number[]) {
  //   values.forEach((val, idx) => {
  //     policy.parameters[idx].value = val;
  //   });
  // }

  // function updateOptions(options: Option[], values: number[]) {
  //   values.forEach((val, idx) => {
  //     options[idx].value = val;
  //   });
  // }

  // Update the data parameters corresponding to the input values.
  function onUpdate(data: DataModel | Option[], values: number[]) {
    let params = data instanceof Array ? data : data.parameters;
    values.forEach((val, idx) => {
      params[idx].value = val;
    });
  }

  // // ON SELECTION
  // function selectModel(name: ModelName) {
  //   selectedModelName.value = name;
  // }
  // function selectPolicy(name: PolicyName) {
  //   selectedPolicyName.value = name;
  // }
  // function onSelect(category: CategoryName, name: string) {
  //   switch (category) {
  //     case Category.models:
  //       selectModel(name as ModelName);
  //       break;
  //     case Category.policies:
  //       selectPolicy(name as PolicyName);
  //       break;
  //   }
  // }

  return {
    models: readonly(models),
    policies: readonly(policies),
    // model: selectedModel,
    // policy: selectedPolicy,
    options: readonly(options),
    onUpdate,
    // onSelect,
  };
});

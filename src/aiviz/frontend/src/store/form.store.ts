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

  // Update the data parameters corresponding to the input values.
  function onUpdate(data: DataModel | Option[], values: number[]) {
    let params = data instanceof Array ? data : data.parameters;
    values.forEach((val, idx) => {
      params[idx].value = val;
    });
  }

  return {
    models: readonly(models),
    policies: readonly(policies),
    options: readonly(options),
    onUpdate,
  };
});

import type { Model, List } from "@/types";
import { computed } from 'vue'
import { defineStore } from 'pinia'
import { defineItem as defineModel } from '@/stores/utils'

export type ModelName = "mab";
export type ModelParameterName = "numberOfArms";

export const useModelsStore = defineStore("models", () => {
  const models = computed(() => {
    return [
      defineModel("mab", "Multiarmed Bandit", [
        {
          name: "numberOfArms",
          label: "Number of Arms",
          min: 2,
          defaultValue: 2,
          idPrefix: "model-param-",
        },
      ]),
    ] as List<Model>;
  });

  return { models }
});

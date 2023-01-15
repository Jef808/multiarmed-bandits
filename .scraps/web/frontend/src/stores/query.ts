import { defineStore } from "pinia";
import { computed } from "vue";
import { defineQueryOptions } from "./utils";

export const useQueryStore = defineStore("queries", () => {
  const queryOptions = computed(() => {
    return defineQueryOptions([
      {
        name: "numberOfSteps",
        label: "Number of Steps",
        defaultValue: 1,
        min: 1,
        step: 1,
        idPrefix: "option-param-",
      },
    ]);
  });

  return { queryOptions };
})

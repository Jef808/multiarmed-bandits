import { defineStore, storeToRefs } from "pinia";
import { reactive, ref, computed } from "vue";
import { useModelsStore } from "@/stores/models";
import { usePoliciesStore } from "@/stores/policies";
import { useQueriesStore } from "@/stores/query";

export const useStore = defineStore("dataStore", () => {
  const models = useModelsStore();
  const policies = usePoliciesStore();
  const queryOptions = useQueriesStore();

  const getModels = computed(() => {
    return useModelsStore();
  })

  return {
    models,
    policies,
    queryOptions,
  };
});

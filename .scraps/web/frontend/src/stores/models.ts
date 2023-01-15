import type { Model, ModelProps } from "@/types";
import { computed, ref, toRef, type Ref } from "vue";
import { fetchData, addDocument, modelPolicyConverter } from "@/stores/utils";
import { defineStore } from "pinia";
import { collection } from "firebase/firestore";
import { db } from "@/plugins/firebase";

export const useModelStore = defineStore("models", () => {
  const models = ref([]) as Ref<Model[]>;

  let model: Ref<Model> | undefined = undefined;

  const modelLoading = ref(false);

  const selectedModel = computed({
    get() {
      return model!! ? model.value : undefined;
    },

    set(newModel) {
      selectModel(newModel?.name);
    },
  });

  async function init() {
    modelLoading.value = true;
    await fetchData(collection(db, "models"), models);

    if (model === undefined) selectFirstAvailable();

    modelLoading.value = false;
  }

  async function addModelDocument(model: ModelProps) {
    return addDocument(model, "models");
  }

  function selectFirstAvailable() {
    model = undefined;
    if (models.value.length) model = toRef(models.value, 0);
  }

  function selectModel(name: string | undefined) {
    if (name === undefined) model = undefined;
    model = toRef(
      models.value,
      models.value.findIndex((e) => e.name === name)
    );
  }

  return {
    init,
    models,
    selectedModel,
    modelLoading,
    fetchData,
    addModelDocument,
    selectModel,
  };
});

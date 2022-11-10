import type { Model, ModelProps } from '@/types'
import { computed, ref, toRef, type Ref } from "vue";
import { pullDataInto, addDocument, modelPolicyConverter } from "@/stores/utils";
import { defineStore } from "pinia";
import { collection, getDocs, QueryDocumentSnapshot, type SnapshotOptions, type DocumentData, type DocumentChange, type DocumentReference, type Query } from 'firebase/firestore';
import { db } from '@/plugins/firebase'
// import collect from 'collect.js';



export const useModelStore = defineStore('models', () => {

  const models = ref([]) as Ref<Model[]>;

  let model: Ref<Model> | undefined = undefined;

  const selectedModel = computed({
    get() {
      if (model === undefined) return undefined;
      return model.value;
    },

    set(newModel) {
      const newName = newModel?.name || undefined;
      selectModel(newName);
    }
  })
  const modelLoading = ref(false)

  async function init() {
    modelLoading.value = true;
    await pullDataInto(collection(db, 'models'), models)

    if (model === undefined)
      selectFirstAvailable();

    modelLoading.value = false;
  }

  async function addModelDocument(model: ModelProps) {
    return addDocument(model, "models");
  }

  function selectFirstAvailable() {
    model = undefined;
    if (models.value.length)
      model = toRef(models.value, 0);
  }

  function selectModel(name: string | undefined) {
    if (name === undefined) model = undefined;
    model = toRef(models.value, models.value.findIndex((e) => e.name === name))
  }

  return { init, models, selectedModel, modelLoading, pullDataInto, addModelDocument, selectModel }
});

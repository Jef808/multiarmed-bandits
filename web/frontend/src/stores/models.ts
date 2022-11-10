import type { Model, ModelProps } from '@/types'
import { computed, ref, toRef, type Ref } from "vue";
import { addDocument } from "@/stores/utils";
import { defineStore } from "pinia";
import { collection, getDocs } from 'firebase/firestore';
import { db } from '@/plugins/firebase'

export const useModelStore = defineStore('models', () => {
  const models = ref([{ name: "", label: "", parameters: [] }] as Model[]);
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

  async function fetchModels() {
    modelLoading.value = true
    models.value = []

    const querySnapshot = await getDocs(collection(db, "models"));
    querySnapshot.forEach((doc) => {
      console.log(`${doc.id} => ${doc.data()}`);
      console.log(JSON.stringify(doc.data()))
    })

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

  return { models, selectedModel, modelLoading, fetchModels, addModelDocument, selectModel }
});

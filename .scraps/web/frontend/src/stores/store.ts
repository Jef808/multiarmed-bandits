import { defineStore } from "pinia";
import { useUserModels, useUserPolicies, useUserQueryOptions } from "@/utils";
import type { Model, Policy, QueryOption } from "@/types"
import axios from 'axios'
import { ref } from 'vue'

export const useStore = defineStore("dataStore", async () => {

  const models = ref([] as Model[]);
  const policies = ref([] as Policy[]);
  const queryOptions = ref([] as QueryOption[]);

  return {
    models,
    policies,
    queryOptions,
  };
});

import type {
  DocumentData,
  QueryDocumentSnapshot,
  SnapshotOptions,
  FirestoreDataConverter,
} from 'firebase/firestore'

export const modelPolicyConverter = {

  toFireStore(item: Partial<Model | Policy>) {
    if (!!item.name && !!item.label && !!item.parameters)
      return {
        name: item.name,
        label: item.label,
        parameters: item.parameters
      } as DocumentData;
  },
  fromFireStore(snapshot: QueryDocumentSnapshot, options: SnapshotOptions): Model | Policy {
    const { name, label, parameters } = snapshot.data(options)!;
    return { name, label, parameters };
  }
} as unknown as FirestoreDataConverter<Model | Policy>;

import uniqueId from "lodash.uniqueid";
import type { Ref } from 'vue'
import { type FirestoreDataConverter, Query, getDocs, type DocumentChange, type SetOptions, writeBatch, setDoc, doc, type QueryDocumentSnapshot, type SnapshotOptions, type DocumentData } from 'firebase/firestore';
import { db } from '@/plugins/firebase';
import type { Model, ModelProps, Policy, PolicyProps } from '@/types'
import collect from 'collect.js'

export type ParameterProps = {
  name: string;
  label: string;
  defaultValue: number;
  idPrefix: string;
  min?: number;
  max?: number;
  step?: number;
};

export function defineParameter(props: ParameterProps) {
  const min: number = props.min ?? -Infinity;
  const max: number = props.max ?? Infinity;
  const step: number = props.step ?? 1;
  const value: number = props.defaultValue;

  return {
    id: uniqueId(props.idPrefix),
    name: props.name,
    label: props.label,
    min,
    max,
    step,
    value,
  };
}

export function defineItem(name: string, label: string, params: ParameterProps[]) {
  return {
    name,
    label,
    parameters: params.map((param) => defineParameter(param)),
  } as Model | Policy;
}

export function defineQueryOptions(params: ParameterProps[]) {
  return params.map((param) => defineParameter(param));
}

export async function addDocument(props: ModelProps | PolicyProps, collection: "models" | "policies") {
  const batch = writeBatch(db);

  const docId = `${collection}-${String(props.name)}`;
  const docRef = doc(db, collection, docId);
  batch.set(docRef, {
    name: props.name,
    label: props.label
  });

  props.parameters.forEach((param, idx) => {
    const paramId = `${collection}-${String(props.name)}-param-${idx}`;
    const paramDocRef = doc(db, "category", docId, "parameters", paramId);

    batch.set(paramDocRef, {
      name: param.name,
      label: param.label,
      defaultValue: param.value,
      min: param.min ?? -Infinity,
      max: param.max ?? Infinity,
      step: param.step ?? 1
    });
  });

  await batch.commit();

  console.log("New document written at path", `${collection}/${docId}`);
  console.log("Added the 'parameters' collection to policy ", docId);
}

export async function fetchData(q: Query, target: Ref<Model[] | Policy[]>) {
  const querySnapshot = await getDocs(q.withConverter(modelPolicyConverter));

  collect(querySnapshot
    .docChanges())
    .map((item: DocumentChange) => item.doc.data())
    .merge(target.value);
}

import uniqueId from "lodash.uniqueid";
import type { Store } from "pinia";
import { writeBatch, setDoc, doc } from 'firebase/firestore';
import { db } from '@/plugins/firebase';
import type { Model, ModelProps, Policy, PolicyProps, List } from '@/types'

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

export function getItem(store: Store, id: string) { }

export async function addDocument(props: ModelProps | PolicyProps, collection: "models" | "policies") {
  const docId = `${collection}-${String(props.name)}`;
  await setDoc(doc(db, 'models', docId), {
    name: props.name,
    label: props.label
  });
  console.log("New document written with ID: ", docId);

  const batch = writeBatch(db);

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
  console.log("Added the 'parameters' collection to policy ", docId);
}

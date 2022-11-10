import { computed, ref, toRef, type Ref } from "vue";
import { defineStore } from "pinia";
import type { Policy, PolicyProps } from '@/types'
import { addDocument } from "@/stores/utils";
import { collection, getDocs } from 'firebase/firestore';
import { db } from '@/plugins/firebase'

export const usePolicyStore = defineStore('policies', () => {

  const policies = ref([{ name: "", label: "", parameters: [] }] as Policy[]);

  let policy: Ref<Policy> | undefined = undefined;

  const selectedPolicy = computed({
    get() {
      if (policy === undefined) return undefined;
      return policy.value;
    },

    set(newPolicy) {
      const newName = newPolicy?.name || undefined;
      selectPolicy(newName);
    }
  })
  const policyLoading = ref(false)


  async function fetchPolicies() {
    policyLoading.value = true
    policies.value = []

    const querySnapshot = await getDocs(collection(db, "policies"));
    querySnapshot.forEach((doc) => {
      console.log(`${doc.id} => ${doc.data()}`);
      console.log(JSON.stringify(doc.data()))
    })

    selectFirstAvailable();
    policyLoading.value = false
  }

  async function addPolicyDocument(policy: PolicyProps) {
    return addDocument(policy, "policies");
  }

  function selectFirstAvailable() {
    policy = undefined;
    if (policies.value.length)
      policy = toRef(policies.value, 0);
  }

  function selectPolicy(name: string | undefined) {
    if (name === undefined) policy = undefined;
    policy = toRef(policies.value, policies.value.findIndex((e) => e.name === name))
  }

  return { policies, selectedPolicy, policyLoading, fetchPolicies, addPolicyDocument, selectPolicy }
});

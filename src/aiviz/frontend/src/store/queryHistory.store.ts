import _, { uniq, sortBy } from 'underscore';
import { defineStore } from 'pinia';
import { reactive, ref } from 'vue';
import type { Model, Policy, Options, QueryForm } from '@/data/types';

const areEqual = (a: QueryForm, b: QueryForm) => {
    return (a.modelName === b.modelName
        && a.policyName === b.policyName
        && Object.entries(a.modelParameters)
            .every((kv) => b.modelParameters[kv[0] as keyof Model] === kv[1])
        && Object.entries(a.policyParameters)
            .every((kv) => (b.policyParameters[kv[0] as keyof Policy] === kv[1])));
}

export const useQueryHistoryStore = defineStore('queryHistoryStore', () => {
    const queryHistory: QueryForm[] = reactive([]);

    function addQuery(query: QueryForm) {
        if (queryHistory.find((q) => areEqual(q, query)) !== undefined)
            return;
        queryHistory.push(query);
    }

    function removeDuplicates() {
        _.sortBy(queryHistory, 'id');
        return _.uniq(queryHistory, true, (a: QueryForm) => a.id);
    }

    return {
        queryHistory,
        addQuery,
        removeDuplicates
    }
});

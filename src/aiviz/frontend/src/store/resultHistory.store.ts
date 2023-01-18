import _, { uniq, sortBy } from 'underscore';
import { reactive } from 'vue';
import { defineStore } from 'pinia';
import type { QueryResult } from '@/data/types';

export const useResultHistoryStore = defineStore('resultHistoryStore', () => {
    const resultHistory: QueryResult[] = reactive([]);

    function addResult(result: QueryResult) {
        resultHistory.push(result);
    }

    // NOTE: underscore.sortBy is a stable algorithm (respects inside original
    // array for equivalent elements).
    function removeDuplicates() {
        _.sortBy(resultHistory, (a: QueryResult) => a.id);
        return _.uniq(resultHistory, true, (a: QueryResult) => a.id);
    }

    return {
        resultHistory,
        addResult,
        removeDuplicates
    };
});

import { computed, readonly, reactive, ref, toRef } from "vue";
import { defineStore } from "pinia";
import { items as _models } from "../data/models.json" assert { type: "json" };
import { items as _policies } from "../data/policies.json" assert { type: "json" };
import { items as _options } from "../data/options.json" assert { type: "json" };
import { Category } from '../data/types';
import type { CategoryName, DataModel, ModelName, PolicyName, Option } from '../data/types';


/**
 * Pinia store holding the form data for queries.
 */
export const useFormStore = defineStore("formStore", () => {

    const models = reactive(_models as DataModel[]);
    const policies = reactive(_policies as DataModel[]);
    const options = reactive(_options as Option[]);

    const selectedModelName = ref(models[0].name)
    const selectedPolicyName = ref(policies[0].name)

    const selectedModel = computed(() => {
        return models.find((m) => m.name === selectedModelName.value) as DataModel;
    });
    const selectedPolicy = computed(() => {
        return policies.find((p) => p.name === selectedPolicyName.value) as DataModel;
    });
    // let selectedModel = ref(models[0]);
    // let selectedPolicy = ref(policies[0]);
    const selected = {
        model: ref(models[0]),
        policy: ref(policies[0])
    };

    function updateModel(values: number[]) {
        values.forEach((val, idx) => {
            selected.model.value.parameters[idx].value = val;
        });
    }

    function updatePolicy(values: number[]) {
        values.forEach((val, idx) => {
            selected.policy.value.parameters[idx].value = val;
        });
    }

    function updateOptions(values: number[]) {
        values.forEach((val, idx) => {
            options[idx].value = val;
        });
    }

    function onUpdate(category: CategoryName,
                      values: number[]) {
        switch (category) {
            case 'models':
                updateModel(values);
                break;
            case 'policies':
                updatePolicy(values);
                break;
            case 'options':
                updateOptions(values);
                break;
            default:
                throw Error(`form.store.upUpdate method unimplemented for category ${category}`);
        }
    }

    // ON SELECTION
    function selectModel(name: ModelName) {
        selectedModelName.value = name;
    }
    function selectPolicy(name: PolicyName) {
        selectedPolicyName.value = name;
    }
    function onSelect(category: CategoryName, name: string) {
        switch (category) {
            case Category.models:
                selectModel(name as ModelName);
                break;
            case Category.policies:
                selectPolicy(name as PolicyName);
                break;
        }
    }

    return {
        models: readonly(models),
        policies: readonly(policies),
        model: selectedModel,
        policy: selectedPolicy,
        options,
        onUpdate,
        onSelect
    };
});

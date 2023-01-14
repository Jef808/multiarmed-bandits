import { computed, reactive, readonly, ref } from "vue";
import { defineStore } from "pinia";
import modelsData from "../data/models.json" assert { type: "json" };
import policiesData from "../data/policies.json" assert { type: "json" };
import optionsData from "../data/options.json" assert { type: "json" };
import type { Model, Policy, Options } from "@/data/types";

export const useFormStore = defineStore("formStore", () => {
    const models = reactive(modelsData);
    const policies = reactive(policiesData);
    const options = reactive(optionsData);

    // SELECTED DATA
    const selected = reactive({
        model: models[0],
        policy: policies[0],
    } as { model: Model | undefined; policy: Policy | undefined });

    const currentModel = ref(models[0].name);
    const currentPolicy = ref(policies[0].name);

    // SELECTION DATA
    const modelSelectionItems = models.map(({ name, label }) => ({
        name,
        label,
    }));
    const policySelectionItems = policies.map(({ name, label }) => ({
        name,
        label,
    }));

    // ON SELECTION
    function onSelectModel(name: string) {
        currentModel.value = name;
        selected.model = models.find((e) => e.name === name) as Model;
    }
    function onSelectPolicy(name: string) {
        currentPolicy.value = name;
        selected.policy = policies.find((e) => e.name === name) as Policy;
    }

    // PARAMETERS GETTERS
    const modelParameters = computed(() => selected.model?.parameters);
    const policyParameters = computed(() => selected.policy?.parameters);
    const optionsParameters = readonly(options[0].parameters);

    /**
     * To be able to work with no metadata excepted the parameter values here, we enforce throughout
     * that the `values` argument below represents properties in the `SAME` order as they
     * appear in the `SelectionItems` objects.
     *
     * Since selections are the only events invalidating the `ordering` of those `ParametersProps`,
     * and since in any case a change of selection should invalidate the partial filling of a form
     * by a user, we can enforce this condition almost for free, without need to implement order checks everywhere.
     */
    function updateCurrentModel(values: number[]) {
        values.forEach((val, idx) => {
            (selected.model as Model).parameters[idx].value = val;
        });
    }
    function updateCurrentPolicy(values: number[]) {
        values.forEach((val, idx) => {
            (selected.policy as Policy).parameters[idx].value = val;
        });
    }

    function updateCurrentOptions(values: number[]) {
        values.forEach((val, idx) => {
            options[0].parameters[idx].value = val;
        });
    }

    function onUpdate(
        category: "model" | "policy" | "options",
        values: number[]
    ) {
        switch (category) {
            case "model":
                updateCurrentModel(values);
                break;
            case "policy":
                updateCurrentPolicy(values);
                break;
            case "options":
                updateCurrentOptions(values);
                break;
        }
    }

    const currentData = computed(() => {
        return {
            model: selected.model as Model,
            policy: selected.policy as Policy,
            options: options[0] as Options,
        };
    });

    const showRaw = computed(
        () =>
            "models: " +
            JSON.stringify(models, null, 2) +
            "\npolicies: " +
            JSON.stringify(policies, null, 2) +
            "\noptions: " +
            JSON.stringify(options, null, 2)
    );

    return {
        currentData,
        initialModelName: models[0].name,
        initialPolicyName: policies[0].name,
        modelSelectionItems,
        policySelectionItems,
        onSelectModel,
        onSelectPolicy,
        currentModel,
        currentPolicy,
        modelParameters,
        policyParameters,
        optionsParameters,
        onUpdate,
        showRaw,
    };
});

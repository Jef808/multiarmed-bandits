import type {
    Model,
    Policy,
    QueryOption
} from '@/types'
import axios from 'axios'
import morgan from 'morgan'

function onError<E extends Error>(error: E) {
    if (axios.isAxiosError(error)) {
        console.warn(`Server error during get request: ${error}`)
        return {} as Model;
    }

    else {
        console.error(`Unexpected error during get request: ${error}`)
        return undefined;
    }
}

const config = new Map<string, string>([
    ['models', "models.json"],
    ['policies', "policies.json"],
    ['queryOptions', "queryOptions.json"]
]);

// type Config = {
//     modelsSrc: "models.json",
//     policiesSrc: "policies.json",
//     queryOptionsSrc: "queryOptions.json"
// };

async function fetch() {

    // Array of 3 Promise objects
    const dataPromises = Array.from(['models', 'policies', 'queryOptions'], (itemCat, idx) => {
        const itemSrc = config.get(itemCat);
        return !!itemSrc ? axios.get(itemSrc) : undefined;
    })

    Promise.all(dataPromises)
        .then(([modelsRes, policiesRes, queryOptionsRes]) => {
            models: modelsRes?.data;
            policies: policiesRes?.data;
            queryOptions: queryOptionsRes?.data;
        })
        .catch(onError);

    // let modelsRes: Model[];
    // let policiesRes: Policy[];
    // let queryOptionsRes: QueryOption[];

    // console.log("Fetching models from", modelsSrc);
    // console.log("Fetching policies from", policiesSrc)
    // console.log("Fetching queryOptions from", queryOptionsSrc)

    // await Promise.all([
    //     noThrow(axios.get(modelsSrc)),
    //     noThrow(axios.get(policiesSrc)),
    //     noThrow(axios.get(queryOptionsSrc))
    // ]).then((items) => {
    //     items.filter((item) => item.ok === true)
    //         .map(item => item.data)
    // })
    //     .catch({

    //     })

    return { models, policies, queryOptions };
}



type ParameterProps = {
    name: string;
    label: string;
    defaultValue: number;
    idPrefix?: string;
    min?: number;
    max?: number;
    step?: number;
};

function defineParameter(props: ParameterProps) {
    const min = props.min ?? -Infinity;
    const max = props.max ?? Infinity;
    const step = props.step ?? 1;
    const value = props.defaultValue;

    return {
        id: uniqueId(props.idPrefix) ?? null,
        name: props.name,
        label: props.label,
        min,
        max,
        step,
        value,
    };
}

function defineItem(name: string, label: string, params: ParameterProps[]) {
    return {
        name,
        label,
        parameters: params.map((param) => defineParameter(param)),
    } as Model | Policy;
}

export default async function(dataSources: string) {

    const  = await fetch<Config>(dataSources)



    async function useUserModels(url: string) {
        return await fetchData(url) as Model[];
    }

    async function useUserPolicies(url: string) {
        return await fetchData(url) as Policy[];
    }

    async function useUserQueryOptions(url: string) {
        return await fetchData(url) as QueryOption[];
    }
}

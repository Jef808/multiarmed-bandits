export type Parameter = {
    name: string;
    value: number;
    min?: number;
    max?: number;
    step?: number;
};

export type QueryOptions = {
    numberOfSteps: number;
    numberOfRepeats: number;
}

export interface QueryForm {
    id: string;
    modelName: string;
    modelParameters: {
        [name: string]: number;
    };
    policyName: string;
    policyParameters: {
        [name: string]: number;
    };
    options: QueryOptions;
}

export type Series = {step: number, value: number}[];

export interface QueryResult {
    id: string;
    data: Series[];
}

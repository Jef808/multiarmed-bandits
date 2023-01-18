export type Parameter = {
    name: string;
    value: number;
    min?: number;
    max?: number;
    step?: number;
};

export type DataModel = {
    name: string;
    label: string;
    parameters: Parameter[];
};

export type Model = DataModel;
export type Policy = DataModel;
export type Options = DataModel;

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
    options: Options;
}

export type Series = {step: number, value: number}[];

export interface QueryResult {
    id: string;
    data: Series[];
}

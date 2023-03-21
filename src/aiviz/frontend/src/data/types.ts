export type Parameter = {
    name: string;
    label: string;
    value: number;
    min: number;
    max: number;
    step: number;
};

export enum Category {
    models = 'models',
    policies = 'policies',
    options = 'options'
};
export type CategoryName = 'models' | 'policies' | 'options';

export type ModelName = 'mab';
export type PolicyName = 'epsilonGreedy' | 'ucb';

export type Models = ['mab'];
export type Policies = ['epsilonGreedy', 'ucb'];

// export type ModelName = Models.mab;
// export type PolicyName = Policies.epsilonGreedy | Policies.ucb

export type DataModel = {
    name: string;
    label: string;
    parameters: Parameter[];
};

export type Model = DataModel;
export type Policy = DataModel;
export type Option = Parameter;

export interface QueryForm {
    model: Model;
    policy: Policy;
    options: Option[];
}

export interface Query {
    id: string;
    modelName: string;
    modelParameters: { [paramName: string]: number };
    policyName: string;
    policyParameters: { [paramName: string]: number };
    options: { [optionName: string]: number };
}

export type Series = {step: number, value: number}[];

export type QueryResult = {
    name: string,
    values: Series
}[];

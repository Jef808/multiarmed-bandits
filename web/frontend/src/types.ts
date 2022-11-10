export enum Category {
    Model = "Model",
    Policy = "Policy",
    QueryOption = "QueryOption",
}

export type Parameter = {
    name: string;
    label: string;
    id: string;
    min: number;
    max: number;
    step: number;
    value: number;
};

export type ParameterProps = {
    name: string;
    label: string;
    id?: number;
    min?: number;
    max?: number;
    step?: number;
    value: number;
}

export type List<T> = T[];
export type Label = string;
export type WithLabel = { label: Label };
export type Name = string;
export type WithName = { name: Name };
export type Id = string;
export type WithId = { id: Id };
export type WithParameters = { parameters: List<Parameter> };
export type WithParametersProps = { parameters: List<ParameterProps> };

export type Model = WithName & WithLabel & WithParameters;
export type ModelProps = WithName & WithLabel & WithParametersProps;
export type Policy = WithName & WithLabel & WithParameters;
export type PolicyProps = WithName & WithLabel & WithParametersProps;
export type QueryOption = List<Parameter>;
export type QueryOptionProps = List<ParameterProps>;

export interface ParameterDocument {
    min: number;
    max: number;
    step: number;
    name: string,
    label: string,
    id: string,
    defaultValue: number,
}

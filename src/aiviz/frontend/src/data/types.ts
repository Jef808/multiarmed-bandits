export type Parameter = {
  name: string;
  label: string;
  value: number;
  min: number;
  max: number;
  step: number;
};

// export enum Category {
//   models = "model",
//   policies = "policy",
//   options = "options",
// }
export type Category = "model" | "policy" | "options";

export type ModelName = "mab";
export type PolicyName = "epsilonGreedy" | "ucb";

export type DataModel = {
  name: string;
  label: string;
  parameters: Parameter[];
};

export type Model = DataModel;
export type Policy = DataModel;
export type Options = DataModel;

export type QueryForm = DataModel[];

export interface Query {
  id: string;
  modelName: string;
  modelParameters: { [paramName: string]: number };
  policyName: string;
  policyParameters: { [paramName: string]: number };
  optionsName: string;
  optionsParameters: { [optionName: string]: number };
}

export type Series = { step: number; value: number }[];

export type QueryResult = {
  name: string;
  values: Series;
}[];

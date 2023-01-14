export type Parameter = {
    name: string;
    label: () => name; // lodash.autoLabel(name);
    value: number;
    min: Nullable<number>;
    max: Nullable<number>;
    step: Nullable<number>;
};

export type QueryOptions = {
    numberOfSteps: number;
}

export interface QueryFormModel {
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

export interface QueryResult {
    id: string;
    data: Series<{action: string}>[];
}

export type SeriesValue<V> = V & {
    step?: number, value: number
};

export type SeriesOptions = {
    name: string;
    label: () => name;
    color: Nullable<string>;
}

export type Series<V> = {
    id: string;
    data: SeriesValue<V>[];
    options: Nullable<SeriesOptions>;
}

export type ChartOptions = {
    // TODO: Represent data with collection of series Id
    id: string;
    label: () => name;
    backgroundColor?: string;
}

export type ChartData<PointMeta = {}> = {
    datasets: Series<PointMeta>[];
    options?: ChartOptions;
}

export type WithId<T> = T & { id: string };

export interface Option {
    category: string;
    name: string;
    label: string;
    parameters: Parameter[];
}

export interface Parameter {
    id: string;
    name: string;
    label: string;
    value: number;
    min?: number;
    max?: number;
    steps?: number;
}

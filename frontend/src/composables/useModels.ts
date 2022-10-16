import { reactive, computed } from 'vue';


export enum ModelName {
    "MultiarmedBandit",
}

export interface MultiarmedBanditParameters {
    numberOfArms: number;
}

export type ModelParameters = MultiarmedBanditParameters;

export interface Props {
    name: ModelName;
    parameters: ModelParameters;
}

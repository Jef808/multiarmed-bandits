import type { ParameterInfo } from "@/types";

export type QueryOptionName = "numberOfSteps" | "numberOfEpisodes";
export type QueryOption = Record<QueryOptionName, ParameterInfo>;

export default () => ();

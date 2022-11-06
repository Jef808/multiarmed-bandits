const ParameterArgsDefaults = {
  min: -Infinity,
  max: Infinity,
  sliderStep: 1,
};

export const withDefaultParams = (p: any) => {
  return { ...ParameterArgsDefaults, ...p }
}

export function withDefaultParameters<T extends { parameters: object[] }>(e: T) {
  e.parameters = e.parameters.map(p => withDefaultParams(p));
  return e;
}

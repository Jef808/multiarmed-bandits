const ParameterArgsDefaults = {
  min: -Infinity,
  max: Infinity,
  sliderStep: 1,
};

export function asParameterWithDefaults<T extends object>(parameter: T) {
  return { ...ParameterArgsDefaults, ...parameter };
}

export function withDefaultParameters<T extends { parameters: object[] }>(entity: T) {
  entity.parameters = entity.parameters.map(p => asParameterWithDefaults(p))
  return entity
}

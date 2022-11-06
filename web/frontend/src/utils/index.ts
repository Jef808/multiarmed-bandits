import uniqueId from 'lodash.uniqueid'

const ParameterArgsDefaults = {
  min: -Infinity,
  max: Infinity,
  sliderStep: 1,
};

export function useDefaults<T extends object>(params: T[]) {
  params.forEach((param) => {
    for (const [key, value] of Object.entries(ParameterArgsDefaults)) {
      if (!param.hasOwnProperty(key)) {
        Object.defineProperty(param, key, {
          value: value,
          writable: false
        })
      }
    }
  });
  return params
}

export function useDefaultParameters<T extends { parameters: object[] }>(objs: T[]) {
  objs.forEach((obj) => useDefaults(obj.parameters));
  return objs;
}

// Add/replace each parameter's id with a unique Id with given prefix.
export function useUniqueIds<T extends object>(params: T[], prefix: string) {
  params.forEach((param) => {
    Object.defineProperty(param, 'id', {
      value: uniqueId(prefix),
      writable: false
    })
  });
  return params;
}

export function useUniqueParamsIds<T extends { parameters: object[] }>(objs: T[], prefix: string) {
  objs.forEach((obj) => useUniqueIds(obj.parameters, prefix))
  return objs
}

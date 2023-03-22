export enum ValidationResult {
  Ok = "Ok",
  RequiredPropMissing = "A required property is missing",
  InvalidData = 'Invalid "data" object',
  InvalidJSON = "Invalid JSON",
  UnknownError = "Unknown Error",
}

function each_data_has_name<T extends { data: object[] }>(obj: T) {
  return obj.data.every((d: any) => {
    return "name" in d;
  });
}
function eachDataHasValidValues<T extends { data: object[] }>(obj: T) {
  const isValidValue = (v: any): boolean => {
    return ["action", "step", "value"].every((prop) => {
      return prop in v;
    });
  };
  return obj.data.every((d: any) => {
    return (
      "values" in d && Array.isArray(d.values) && d.values.every(isValidValue)
    );
  });
}

/**
 * Simply verify that the needed property names are present (without type checking)
 * i.e. 'id', 'model': { 'name', ... }, 'policy': { 'name', ... }, 'options'
 */
// export function validateMsgFront2Back(query: Query) {
//     try {
//         const request = JSON.stringify(query);
//         const requiredProps = [
//             'id',
//             'modelName',
//             'modelParameters',
//             'policyName',
//             'policyParameters',
//             'options'
//         ];
//         return {
//             validation: requiredProps.every(prop => prop in query)
//                 ? ValidationResult.Ok : ValidationResult.RequiredPropMissing,
//             request
//         };
//     }
//     catch (e) {
//         return {
//             validation: ValidationResult.UnknownError,
//             request: ""
//         };
//     }
// }

/**
 * Verify that message can be parsed into a valid QueryResult object,
 * with well-defined (named) values formatted as pairs of numbers
 */
export function validateMsgBack2Front(message: string) {
  try {
    const response = JSON.parse(message);
    const requiredProps = ["id", "data"];
    return {
      validation: requiredProps.every((prop) => prop in response)
        ? each_data_has_name(response) && eachDataHasValidValues(response)
          ? ValidationResult.Ok
          : ValidationResult.InvalidData
        : ValidationResult.RequiredPropMissing,
      response,
    };
  } catch (e) {
    return {
      validation:
        (e as Error).name === "SyntaxError"
          ? ValidationResult.InvalidJSON
          : ValidationResult.UnknownError,
      response: {},
    };
  }
}

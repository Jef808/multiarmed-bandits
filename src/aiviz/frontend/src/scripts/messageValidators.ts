function has_id<T extends object>(obj: T) { return 'id' in obj; }
function has_data<T extends object>(obj: T) { return 'data' in obj && Array.isArray(obj.data); }
function each_data_has_name<T extends { data: object[] }>(obj: T) {
    return obj.data.every((d: any) => {
        return 'name' in d;
    })
}
function isValidValue(obj: any) {
    return ['action', 'step', 'value'].every(prop => {
        return prop in obj;
    });
}
function eachDataHasValidValues<T extends { data: object[] }>(obj: T) {
    return obj.data.every((d: any) => {
        return 'values' in d
            && Array.isArray(d.values)
            && d.values.every(isValidValue);
    });
}


export enum ValidationResult {
    Ok = 'Ok',
    RequiredPropMissing = 'A required property is missing',
    InvalidData = 'Invalid "data" object',
    InvalidJSON = 'Invalid JSON',
    UnknownError = 'Unknown Error'
}

/**
 * Simply verify that the needed property names are present (without type checking)
 * i.e. 'id', 'model': { 'name', ... }, 'policy': { 'name', ... }, 'options'
 */
export function validateMsgFront2Back(message: string) {
    try {
        const msg = JSON.parse(message);
        const requiredProps = ['id', 'modelName', 'modelParameters', 'policyName', 'policyParameters', 'options'];
        const isOk = requiredProps.every(prop => prop in msg);
        //const is_ok = has_id(msg) && has_named_model(msg) && has_named_policy(msg) && has_options(msg);
        return isOk ? ValidationResult.Ok : ValidationResult.RequiredPropMissing;
    }
    catch (e) {
        if ((e as Error).name == 'SyntaxError') {
            return ValidationResult.InvalidJSON;
        }
    }
    return ValidationResult.UnknownError;
}

/**
 * Verify that message can be parsed into a valid QueryResult object,
 * with well-defined (named) values formatted as pairs of numbers
 */
export function validateMsgBack2Front(msg: QueryResult) {
    try {
        const requiredProps = ['id', 'data'];
        const is_ok = has_id(msg) && has_data(msg);
        const is_values_ok = is_ok && each_data_has_name(msg) && eachDataHasValidValues(msg);
        return is_ok ? is_values_ok ? ValidationResult.Ok : ValidationResult.InvalidData : ValidationResult.RequiredPropMissing;
    }
    catch (e) {
        if ((e as Error).name == 'SyntaxError') {
            return ValidationResult.InvalidJSON;
        }
    }
    return ValidationResult.UnknownError;
}

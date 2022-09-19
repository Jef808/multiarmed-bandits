export namespace Types {
    export type ObjectDescriptor<D, M> = {
        data?: D;
        methods?: M & ThisType<D & M>;
    };

    export function makeObject<D, M>(desc: ObjectDescriptor<D, M>): D & M {
        let data: object = desc.data || {};
        let methods: object = desc.methods || {};
        return { ...data, ...methods } as D & M;
    }

    export type Permissive<Type> = {
        [Property in keyof Type]+?: Type[Property];
    };
    export type Strict<Type> = {
        [Property in keyof Type]-?: Type[Property];
    };
}

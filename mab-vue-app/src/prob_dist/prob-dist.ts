import { Types } from "./../scripts/utils";

interface IParams<D> {
    data: D;
    callback: (d: D) => number;
}

interface IBuilder<D> {
    data(d: D): this;
    callback(cb: (d: D) => number): this;
    finalize(): Required<IParams<D>>;
}

function callbackDefault(): number {
    return NaN;
}

class ParamsDefaults<D> implements IParams<D> {
    constructor(
        public data: D,
        public callback: (d: D) => number = callbackDefault
    ) {
        const validate: boolean = !!(() => {
            const check: boolean | null = <D extends Required<D> ? boolean : null>true;
            return check;
        })();

        if (!validate) throw "Invalid default data";

        this.data = data;
    }
}

class Builder<DistData> implements IBuilder<DistData> {
    public params!: IParams<DistData>;

    constructor(defs: DistData) {
        this.params = new ParamsDefaults(defs);
    }
    data(d: DistData) {
        this.params.data = d;
        return this;
    }

    callback(cb: (d: DistData) => number) {
        this.params.callback = cb;
        return this;
    }

    finalize() {
        return <Types.Strict<IParams<DistData>>>this.params;
    }
}

function extractSampler<D>(params: IParams<D>): () => number {
    return () => params.callback(params.data);
}

export default function construct_distribution<DistData>(
    defs: DistData,
    data: DistData,
    cb: (d: DistData) => number
): () => number {
    return extractSampler(new Builder(defs).data(data).callback(cb).finalize());
}

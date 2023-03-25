import {reactive} from "vue";
import {items as models} from "./models.json" assert {type: "json"};
import {items as policies} from "./policies.json" assert {type: "json"};
import {items as options} from "./options.json" assert {type: "json"};
import type {DataModel} from "./types";

export function useData() {
    return reactive([
        { title: "model", items: models },
        { title: "policy", items: policies },
        { title: "options", items: options },
    ] as { title: string; items: DataModel[] }[]);
}

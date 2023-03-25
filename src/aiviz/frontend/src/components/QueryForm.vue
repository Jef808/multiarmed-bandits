<script lang="ts">
export default {
  name: "QueryForm",
  components: {ParametersForm},
};
</script>

<script setup lang="ts">
import {reactive, ref, unref, onBeforeMount, type Ref} from "vue";
import {onKeyStroke} from "@vueuse/core";
import ParametersForm from "./ParametersForm.vue";
import {useData} from "../data/data";
import type {DataModel} from "../data/types";

// TODO Extract a single column into a formSection component
// and make the call to useDataModel() here, passing each column
// item to the formSection components as props.

export interface Props {
  handleSubmit: (dms: DataModel[]) => void;
}

interface Emits {
  (e: "newQuery"): void;
}

const props = defineProps<Props>();
const emit = defineEmits<Emits>();

let sections = [] as {title: string; items: DataModel[]}[];

const selectable = reactive([] as boolean[]);
const selected = reactive([] as Ref<DataModel>[]);
const updated = reactive([] as boolean[]);

const bgColorNonUpdated = "#fff";
const bgColorUpdated = "grey";

function sectionBgColor(idx: number) {
  return updated[idx] ? bgColorUpdated : bgColorNonUpdated;
}

onBeforeMount(() => {
  sections = useData();
  sections.forEach((section) => {
    const _selectable = section.items?.length > 1 || false;
    selectable.push(_selectable);
    selected.push(ref(section.items[0]));
    updated.push(false);
  });
});

onKeyStroke(["s", "S", "Enter"], (e) => {
  onSubmit();
});

// Mock form submission
function onSubmit() {
  console.log("newQuery");
  props.handleSubmit(selected.map((dataModel) => unref(dataModel)));
  updated.length = 0;
  sections.forEach((_, idx) => {
    updated[idx] = false;
  });
  emit("newQuery");
}
</script>

<template>
  <v-form @submit.prevent="onSubmit">
    <v-row>
      <v-col
        cols="4"
        style="border: 1px solid"
        v-for="(section, idx) in sections"
        :key="idx"
      >
        <v-card
          :title="section.title.toUpperCase()"
          :color="sectionBgColor(idx)"
        >
          <v-card-text>
            <v-row>
              <v-spacer></v-spacer>

              <v-col cols="8">
                <v-select
                  v-model="selected[idx].value"
                  :items="section.items"
                  item-title="label"
                  item-value="name"
                  returnObject
                  :disabled="!selectable[idx]"
                ></v-select>
              </v-col>

              <v-spacer></v-spacer>
            </v-row>
            <v-row>
              <v-spacer></v-spacer>
              <v-col cols="8">
                <ParametersForm
                  :name="selected[idx].value.name"
                  :label="selected[idx].value.label"
                  v-model="selected[idx].value.parameters"
                  @change="updated[idx] = true"
                ></ParametersForm>

                <v-spacer></v-spacer>
              </v-col>
              <v-spacer />
            </v-row>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
    <v-row>
      <v-spacer></v-spacer>

      <v-col cols="8">
        <v-btn :disabled="loading" :loading="loading" type="submit" block
          ><b><u>S</u></b
          >ubmit</v-btn
        >
      </v-col>

      <v-spacer></v-spacer>
    </v-row>
  </v-form>
</template>

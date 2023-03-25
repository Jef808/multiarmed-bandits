<script lang="ts">
export default {
  name: "QueryForm",
  components: {ParametersForm},
};
</script>

<script setup lang="ts">
import {reactive, ref, unref, onBeforeMount, type Ref} from "vue";
import ParametersForm from "./ParametersForm.vue";
import {useData} from "../data/data";
import type {DataModel} from "../data/types";

// TODO Extract a single column into a formSection component
// and make the call to useDataModel() here, passing each column
// item to the formSection components as props.

export interface Props {
  handleSubmit: (dms: DataModel[]) => void;
}

const props = defineProps<Props>();

let sections = [] as {title: string; items: DataModel[]}[];

const loading = ref(false);
const selectable = reactive([] as boolean[]);
const selected = reactive([] as Ref<DataModel>[]);

onBeforeMount(() => {
  sections = useData();
  sections.forEach((section) => {
    selected.push(ref(section.items[0]));
    const _selectable = section.items?.length > 1 || false;
    selectable.push(_selectable);
  });
});

// Mock form submission
function onSubmit() {
  console.log("submit");
  props.handleSubmit(selected.map((dataModel) => unref(dataModel)));
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
        <v-card :title="section.title.toUpperCase()">
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
          >Submit</v-btn
        >
      </v-col>

      <v-spacer></v-spacer>
    </v-row>
  </v-form>
</template>

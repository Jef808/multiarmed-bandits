<script setup lang="ts">
import {reactive, ref, onMounted} from "vue";
import ParametersForm from "./ParametersForm.vue";
import {useData} from "../data/data";
import type {DataModel} from "../data/types";
// TODO Extract a single column into a formSection component
// and make the call to useDataModel() here, passing each column
// item to the formSection components as props.

const sections = useData();

const loading = ref(false);
const selectable = reactive([]);
const selected = reactive([]);

onMounted(() => {
  sections.forEach((section) => {
    selected.push(ref(section.items[0]));
    selectable.push(section.items.length > 1 ? true : false);
  });
});

// Mock form submission
function handleSubmit() {
  loading.value = true;
  setTimeout(() => {
    loading.value = false;
  }, 1000);
}
</script>

<template>
  <v-form @submit.prevent="handleSubmit">
    <v-row>
      <v-col
        cols="4"
        style="border: 1px solid"
        v-for="(section, idx) in sections"
        :key="idx"
      >
        <v-card :title="section.title.toUpperCase()">
          <v-card-text>
            <v-row v-if="selectable[idx]">
              <v-spacer></v-spacer>

              <v-col cols="8">
                <!-- Put select element here -->
                <v-select
                  v-model="selected[idx]"
                  :items="formSection.items"
                  item-title="label"
                  item-value="name"
                  returnObject
                >
                </v-select>
              </v-col>

              <v-spacer></v-spacer>
            </v-row>
            <v-row>
              <v-spacer></v-spacer>
              <v-col cols="8">
                <!-- Put selected parameters input here -->
                <ParametersForm
                  :name="selected.name"
                  :label="selected.label"
                  v-model="selected.parameters"
                >
                </ParametersForm>
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
        <v-btn :disabled="loading" :loading="loading" type="submit" block>
          Submit
        </v-btn>
      </v-col>

      <v-spacer></v-spacer>
    </v-row>
  </v-form>
</template>

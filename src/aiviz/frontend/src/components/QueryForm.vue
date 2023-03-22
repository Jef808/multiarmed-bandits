<script setup lang="ts">
import { useFormStore } from "../store/form.store";
import { storeToRefs } from "pinia";
import { computed, ref } from "vue";
import ParametersForm from "./ParametersForm.vue";
import { useQueryStore } from "../store/query.store";

const { options, onUpdate, onSelect } = useFormStore();
const { models, policies, model, policy } = storeToRefs(useFormStore());

const selectedModel = ref(models.value[0].name);
const selectedPolicy = ref(policies.value[0].name);

function onSubmit() {
  useQueryStore().submit();
}
</script>

<template>
  <div class="form-container" style="border: 1px solid">
    <v-form @submit.prevent="onSubmit">
      <v-expansion-panels v-model="panel" v-click-outside="onCancel">
        <v-expansion-panel class="text-grey" value="model" title="MODEL">
          <v-expansion-panel-subtitle>
            <h3>{{model.label}}</h3>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-subtitle v-for="param in model.parameters" :key="param.name">
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <v-select
              v-model="selectedModel"
              @update:modelValue="onSelectModel"
              :items="models"
              item-title="label"
              item-value="name"
            ></v-select>
            <ParametersForm
              :dataName="selectedModel"
              :items="model.parameters"
              @change="onUpdateModel"
              @cancel="onCancel"
            ></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
        <v-expansion-panel class="text-grey" value="policy" title="POLICY">
          <v-expansion-panel-subtitle>
            <h3>{{policy.label}}</h3>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-subtitle v-for="param in policy.parameters" :key="param.name">
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <v-select
              v-model="selectedPolicy"
              @update:modelValue="onSelectPolicy"
              :items="policies"
              item-title="label"
              item-value="name"
            ></v-select>
            <ParametersForm
              :dataName="selectedPolicy"
              :items="policy.parameters"
              @change="onUpdatePolicy"
              @cancel="onCancel"
            ></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
        <v-expansion-panel class="text-grey" value="options" title="OPTIONS">
          <v-expansion-panel-subtitle v-for="param in options" :key="param.name">
            <h4>{{ param.label }}: {{ param.value }}</h4>
          </v-expansion-panel-subtitle>
          <v-expansion-panel-text class="text-primary">
            <ParametersForm :items="options" @change="onUpdateOptions" @cancel="onCancel"></ParametersForm>
          </v-expansion-panel-text>
        </v-expansion-panel>
      </v-expansion-panels>
      <v-container>
        <v-btn type="submit" color="primary" block>Submit</v-btn>
      </v-container>
    </v-form>
  </div>
</template>

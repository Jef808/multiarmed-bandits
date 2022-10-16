<script setup lang="ts">
import { defineComponent, defineProps, PropType } from "vue";
import type { Model } from "../../views/HomeView.vue";

const models = ref([
  {
    name: "Multiarmed Bandit",
    parameters: {
      numberOfArms: 0,
    },
  },
]);

const selected: ShallowRef<Model> = shallowRef({ name: "", parameters: {} });

function onInput() {
  selected.value = selected.value ?? { name: "", parameters: {} };
}

const parametersComponent = computed(() => {
  return hidden.value
    ? `${selected.value.name.replace(/ /g, "")}Parameters`
    : "MultiarmedBanditParameters";
});

const hidden = computed(() => {
  return selected.value != null && selected.value.name.length > 0;
});
</script>

<template>
  <div>
    <v-select
      v-model="selected"
      :options="models"
      label="name"
      @input="onInput()"
    >
    </v-select>
  </div>
  <div>
    <component :is="parametersComponent" :hidden="hidden"></component>
  </div>
</template>

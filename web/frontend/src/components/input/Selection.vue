<script setup lang="ts">
import { computed, ref, onMounted, type Ref } from "vue";
import { Option, Parameter } from "./index";
import InputParameters from "./Parameters.vue";

const props = defineProps<{ options: Option[] }>();

const selected: Ref<Option> = ref({
  category: "None",
  name: "",
  label: "",
  parameters: [],
});

const category = computed(() => {
  return props.options[0].category;
});

const emptyMsg = computed(() => {
  return `Select ${category.value}`;
});

function getId(option: Option) {
  return `input-${option.category}`;
}

const id = computed(() => {
  return `input-${category.value}`;
});

onMounted(() => {
  console.log("Mounted");
  console.log(
    "Category: ",
    props.options[0].category,
    "\nName: ",
    props.options[0].name,
    "\nlabel: ",
    props.options[0].label,
    "\nparameters: ",
    props.options[0].parameters
  );
});
</script>

<template>
  <div class="box">
    <h2 class="label-wrapper">
      <label for="id">{{ options[0].category }}: </label>
    </h2>
  </div>
  <div>
    <v-select
      :options="options"
      :clearable="false"
      :placeholder="emptyMsg"
      :getOptionKey="(opt: Option) => opt.name"
      :inputId="id"
      v-model="selected"
    ></v-select>
  </div>
  <div class="flex">
    <h3 class="label-wrapper">
      <label>Parameters: </label>
    </h3>
  </div>
  <div class="flex">
    <InputParameters :parameters="selected.parameters" />
  </div>
</template>

<style scoped>
.box {
  display: flex;
  flex-flow: column nowrap;
}
</style>

<template>
  <div id="container">
    <div class="title">
    <span class="text-gray-700">{{ title }}</span>
    </div>
    <div>
      <select class="form-select px-4 py-3 rounded-full">
        <option v-for="items.?value"
                v-model="selected">"Multiarmed Bandit"</option>
      </select>
    </div>
  </div>
</template>

<script setup lang="ts">
  import { defineProps, ref } from 'vue';

  interface Item {
    name: string,
    parameters: object,
  }

  defineProps<{items: Item | Item[]}>();

  const selected = ref({name: "", parameters: {}});
</script>

<template>
<v-container class="fill-height">
  <v-data-iterator :items="models" sort-by="label" v-bind="$attrs">
    <template v-slot:default>
      <v-row>
        <v-col
          v-for="model in models"
          :key="model.name"
          cols="12"
          sm="6"
        >
          <ModelOrPolicy :valueModel="model" />
        </v-col>
      </v-row>
    </template>
  </v-data-iterator>
  <v-data-iterator :items="policies" sort-by="label" v-bind="$attrs">
    <template v-slot:default>
      <v-row>
        <v-col
          v-for="policy in policies"
          :key="policy.name"
          cols="12"
          sm="6"
        >
          <ModelOrPolicy :valueModel="policy" />
        </v-col>
      </v-row>
    </template>
  </v-data-iterator>
</v-container>
</v-container>
</template>

<script setup lang="ts">
 import ModelOrPolicyView
</script>

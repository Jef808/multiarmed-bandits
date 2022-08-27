<template>
  <div>
    <select v-model="selected" name="name">
      <option disabled value="">Make a selection</option>
      <option v-for="option in options" :key="option.id" :label="option.label" :value="option.value"></option>
    </select>
  </div>
</template>
<script>
export default {
  props: {
    id: { required: true, type: String },
    options: { required: true, type: Array },
  },
  data() {
    return {
      selected: ''
    }
  },
};
</script>

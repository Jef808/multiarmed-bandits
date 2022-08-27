<template>
  <p>Files in {{ dir }}:</p>
  <div id="file-list">
    <ul>
      <li v-for="file in files" :key="file">{{ file }}</li>
    </ul>
  </div>
</template>
<script>
import axios from "axios";
export default {
  props: {
    dir: { required: true, type: String },
  },
  data() {
    return {
      files: [],
    };
  },
  async mounted() {
    this.files = await axios
      .get(this.dir)
      .then((res) => res.data.filter((d) => d.endsWith(".json")));
    console.log(this.files);
  },
};
</script>

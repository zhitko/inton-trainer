<template>
  <md-list class="md-triple-line">
    <CatalogSection v-for="item in files_list" v-bind:key="item.uuid" :item="item" />
  </md-list>
</template>

<script>
import CatalogSection from "../components/CatalogSection.vue";
import Api from "../Api";

export default {
  name: "Catalog",
  data: () => ({
    records_count: 15,
    files_list: [],
  }),
  components: {
    CatalogSection
  },
  created: function() {
    let self = this;
    Api.getRecords().then(data => {
      self.files_list = data;
    });
  }
};
</script>

<template>
    <md-list-item md-expand :md-expanded.sync="expand">
      <md-subheader>{{item.title}}</md-subheader>

      <md-list slot="md-expand">
        <md-list-item
            v-for="record in item.files"
            v-bind:key="record.uuid">
            <md-button class="md-fab" :to="{ name: 'training-record', params: { uuid: record.uuid }}">
                <md-icon class="md-size-2x">mic</md-icon>
            </md-button>

            <div class="md-list-item-text">
                <span>{{record.title}}</span>
                <p>{{record.text}}</p>
            </div>

            <md-button class="md-icon-button md-list-action">
                <md-icon class="md-size-2x">play_circle_filled</md-icon>
            </md-button>
        </md-list-item>
      </md-list>
    </md-list-item>
</template>

<style lang="scss" scoped>
.md-list-expand .md-list-item:hover {
    background-color: rgba(0, 0, 0, 0.08);
}
</style>

<script>
export default {
  name: 'CatalogSection',
  data: () => ({
    expand: false,
  }),
  components: {
  },
  props: ['item'],
};
</script>

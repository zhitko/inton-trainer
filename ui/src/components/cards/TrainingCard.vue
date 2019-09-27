<template>
  <md-card md-ratio="16:9">
    <md-card-media>
      <img src="@/assets/logo.svg" alt="Inton@trainer">
    </md-card-media>

    <md-card-area>
      <md-card-header>
        <span class="md-title">{{ $t('home-page.welcome-message') }}</span>
        <span class="md-subhead">
          {{ $t('home-page.records-count', {count: records_count}) }}
        </span>
      </md-card-header>

      <md-card-actions>
        <md-button to="/catalog">
          {{ $t('home-page.start') }}
        </md-button>
      </md-card-actions>
    </md-card-area>
  </md-card>
</template>

<style lang="scss" scoped>
  .md-card-media img {
    height: 100px;
  }
  .md-card {
    width: 400px;
    margin: 4px;
    padding: 10px;
    display: inline-block;
    vertical-align: top;
  }
</style>

<script>
export default {
  name: 'TrainingCard',
  data: () => ({
    records_count: 15,
  }),
  components: {
  },
};
</script>

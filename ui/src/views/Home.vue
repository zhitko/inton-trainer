<template>
  <div>
    <TrainingCard />
    <RecordsCard />
  </div>
</template>

<style lang="scss" scoped>
</style>

<script>
import TrainingCard from '@/components/cards/TrainingCard.vue';
import RecordsCard from '@/components/cards/RecordsCard.vue';

export default {
  name: 'home',
  data: () => ({
    records_count: 15,
  }),
  components: {
    TrainingCard,
    RecordsCard,
  },
};
</script>

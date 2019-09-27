<template>
  <div class="container">
    <span class="md-title">{{ title }}</span>
    <md-list>
        <md-list-item md-expand>
          <span class="md-list-item-text">{{ $t('training-record.options') }}</span>

          <md-list slot="md-expand">
            <md-switch v-model="pitchMode">
              {{ pitchMode?$t('training-record.pitch-mode'):$t('training-record.source-mode') }}
            </md-switch>
            <md-switch v-model="manualMarkout">
              {{ manualMarkout?$t('training-record.manual-marking'):$t('training-record.auto-marking') }}
            </md-switch>
            <md-switch v-model="showLegend">
              {{ $t('training-record.show-legend') }}
            </md-switch>
          </md-list>
        </md-list-item>
    </md-list>
    <svg id="wave"></svg>
  </div>
</template>

<style lang="scss" scoped>
.container {
  height: 100%;
  width: 100%;
}
svg {
  height: 100%;
  width: 100%;
}
</style>

<script>
import Api from "../Api";
import Wave from "../visualization/wave";
import Pitch from "../visualization/pitch";
import Segments from "../visualization/segments";
import * as d3 from "d3";
import _ from "lodash";

const SEGM_N_COLOR = '#005AC8';
const SEGM_T_COLOR = '#0AB45A';
const SEGM_P_COLOR = '#FA7850';
const ORIG_PITCH_COLOR = '#005AC8';
const WAVE_COLOR = '#000000';

export default {
  name: 'TrainingRecord',
  data: () => ({
    text: '',
    title: '',
    path: '',
    showLegend: true,
    manualMarkout: true,
    pitchMode: true,
    state: {
      showWave: undefined,
      showPitch: undefined,
      showSegmentsP: undefined,
      showSegmentsN: undefined,
      showSegmentsT: undefined,
    },
  }),
  components: {
  },
  watch: {
    'showLegend': 'refreshView',
    'pitchMode': 'refreshView',
    'manualMarkout': 'refreshView',
    'state.showWave': 'drawWave',
    'state.showPitch': 'drawPitch',
    'state.showSegmentsP': 'drawSegmentsP',
    'state.showSegmentsN': 'drawSegmentsN',
    'state.showSegmentsT': 'drawSegmentsT',
  },
  props: ['uuid'],
  created: function() {
    const self = this;
    Api.getRecordInfo(self.uuid).then(data => {
      self.text = data.text;
      self.title = data.title;
      self.path = data.path;
    });

    self.refreshView();
  },
  methods: {
    refreshView() {
      const self = this;
      console.warn("refreshView");
      _.assignIn(self.state, {
          showWave: false,
          showPitch: false,
          showSegmentsP: false,
          showSegmentsN: false,
          showSegmentsT: false,
        });
      setTimeout(function() {
        if(self.pitchMode) {
          _.assignIn(self.state, {
            showWave: true,
            showPitch: true,
            showSegmentsP: true,
            showSegmentsN: true,
            showSegmentsT: true,
          });
        } else {
          _.assignIn(self.state, {
            showWave: false,
            showPitch: false,
            showSegmentsP: false,
            showSegmentsN: false,
            showSegmentsT: false,
          });
        }
      }, 0);
    },
    drawWave: function() {
      const self = this;
      console.warn("drawWave", self.state.showWave);
      if (self.state.showWave) {
        Api.getRecordWave(self.uuid).then(data => {
          Wave.draw(data, "#wave", "wave", {
            width: window.innerWidth - 85,
            height: 150,
            stroke: WAVE_COLOR,
            strokeWidth: "0.1"
          });
        });
      } else {
        Wave.clean("#wave", "wave")
      }
    },
    drawPitch: function() {
      const self = this;
      console.warn("drawPitch", self.state.showPitch);
      if (self.state.showPitch) {
        Api.getRecordPitch(self.uuid, self.manualMarkout).then(data => {
          Pitch.draw(data, "#wave", "pitch", {
            marginTop: 150, 
            width: window.innerWidth - 85,
            height: 150,
            stroke: ORIG_PITCH_COLOR,
            strokeWidth: 3
          });
          if (self.showLegend) {
            Pitch.legend("#wave", "pitch", {
              marginTop: 310, 
              stroke: ORIG_PITCH_COLOR,
              strokeWidth: 3,
              text: "F0"
            });
          }
        });
      } else {
        Pitch.clean("#wave", "pitch")
      }
    },
    drawSegmentsP: function() {
      const self = this;
      console.warn("drawSegmentsP", self.state.showSegmentsP);
      if (self.state.showSegmentsP) {
        Api.getRecordSegmentsP(self.uuid, self.manualMarkout).then(data => {
          Segments.draw(data, "#wave", "SegmentsP", {
            width: window.innerWidth - 85,
            height: 300,
            fill: SEGM_P_COLOR,
            fillOpacity: 0.3
          });
          if (self.showLegend) {
            Segments.legend("#wave", "SegmentsP", {
              marginTop: 310, 
              marginLeft: 50, 
              fill: SEGM_P_COLOR,
              fillOpacity: 0.3,
              text: "Pre-nucleus"
            });
          }
        });
      } else {
        Segments.clean("#wave", "SegmentsP");
      }
    },
    drawSegmentsN: function() {
      const self = this;
      console.warn("drawSegmentsN", self.state.showSegmentsN);
      if (self.state.showSegmentsN) {
        Api.getRecordSegmentsN(self.uuid, self.manualMarkout).then(data => {
          Segments.draw(data, "#wave", "SegmentsN", {
            width: window.innerWidth - 85,
            height: 300,
            fill: SEGM_N_COLOR,
            fillOpacity: 0.3
          });
          if (self.showLegend) {
            Segments.legend("#wave", "SegmentsN", {
              marginTop: 310, 
              marginLeft: 160, 
              fill: SEGM_N_COLOR,
              fillOpacity: 0.3,
              text: "Nucleus"
            });
          }
        });
      } else {
        Segments.clean("#wave", "SegmentsN");
      }
    },
    drawSegmentsT: function() {
      const self = this;
      console.warn("drawSegmentsT", self.state.showSegmentsT);
      if (self.state.showSegmentsT) {
        Api.getRecordSegmentsT(self.uuid, self.manualMarkout).then(data => {
          Segments.draw(data, "#wave", "SegmentsT", {
            width: window.innerWidth - 85,
            height: 300,
            fill: SEGM_T_COLOR,
            fillOpacity: 0.3
          });
          if (self.showLegend) {
            Segments.legend("#wave", "SegmentsT", {
              marginTop: 310, 
              marginLeft: 240, 
              fill: SEGM_T_COLOR,
              fillOpacity: 0.3,
              text: "Nucleus"
            });
          }
        });
      } else {
        Segments.clean("#wave", "SegmentsT");
      }
    },
  },
};
</script>

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
import Octaves from "../visualization/octaves";
import Pitch from "../visualization/pitch";
import Ump from "../visualization/ump";
import Segments from "../visualization/segments";
import UMPMask from "../visualization/ump_mask";
import * as d3 from "d3";
import _ from "lodash";

const SEGM_OCTAVES_COLOR = '#AA0A3C';
const SEGM_UMP_COLOR = '#005AC8';
const SEGM_N_COLOR = '#005AC8';
const SEGM_T_COLOR = '#0AB45A';
const SEGM_P_COLOR = '#FA7850';
const ORIG_PITCH_COLOR = '#005AC8';
const ORIG_UMP_COLOR = '#005AC8';
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
      showOctaves: undefined,
      showUMP: undefined,
      showUMPMask: undefined,
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
    'state.showOctaves': 'drawOctaves',
    'state.showUMP': 'drawUMP',
    'state.showUMPMask': 'drawUMPMask',
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
          showOctaves: false,
          showUMP: false,
          showUMPMask: false,
          showSegmentsP: false,
          showSegmentsN: false,
          showSegmentsT: false,
        });
      setTimeout(function() {
        if(self.pitchMode) {
          _.assignIn(self.state, {
            showWave: true,
            showPitch: true,
            showOctaves: false,
            showUMP: false,
            showUMPMask: false,
            showSegmentsP: true,
            showSegmentsN: true,
            showSegmentsT: true,
          });
        } else {
          _.assignIn(self.state, {
            showWave: false,
            showPitch: false,
            showOctaves: true,
            showUMP: true,
            showUMPMask: true,
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
        Wave.clean("#wave", "wave");
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
        Pitch.clean("#wave", "pitch");
      }
    },
    drawOctaves: function() {
      const self = this;
      console.warn("drawOctaves", self.state.showOctaves);
      if (self.state.showOctaves) {
        Api.getRecordOctavesRange(self.uuid, self.manualMarkout).then(data => {
          Octaves.draw(data, "#wave", "octaves", {
            width: 80,
            height: 300,
            fill: SEGM_OCTAVES_COLOR,
            fillOpacity: 0.5,
          });
          if (self.showLegend) {
            Octaves.legend("#wave", "octaves", {
              marginTop: 310,
              marginLeft: 200,
              fill: SEGM_OCTAVES_COLOR,
              fillOpacity: 0.5,
              text: "Octaves Range"
            });
          }
        });
      } else {
        Octaves.clean("#wave", "octaves");
      }
    },
    drawUMP: function() {
      const self = this;
      console.warn("drawUMP", self.state.showUMP);
      if (self.state.showUMP) {
        Api.getRecordUPM(self.uuid, self.manualMarkout).then(data => {
          Ump.draw(data, "#wave", "ump", {
            marginLeft: 200, 
            width: window.innerWidth - 285,
            height: 300,
            stroke: ORIG_UMP_COLOR,
            strokeWidth: 3
          });
          if (self.showLegend) {
            Ump.legend("#wave", "ump", {
              marginTop: 310, 
              stroke: ORIG_UMP_COLOR,
              strokeWidth: 3,
              text: "UMP"
            });
          }
        });
      } else {
        Ump.clean("#wave", "ump");
      }
    },
    drawUMPMask: function() {
      const self = this;
      console.warn("drawUMPMask", self.state.showUMPMask);
      if (self.state.showUMPMask) {
        Api.getRecordUPMMask(self.uuid, self.manualMarkout).then(data => {
          UMPMask.draw(data, "#wave", "UMPMask", {
            marginLeft: 200, 
            width: window.innerWidth - 285,
            height: 300,
            fill: SEGM_UMP_COLOR,
            fillOpacity: 0.3,
            stroke: SEGM_UMP_COLOR,
            strokeWidth: 0.3,
          });
          if (self.showLegend) {
            UMPMask.legend("#wave", "UMPMask", {
              marginTop: 310, 
              marginLeft: 90, 
              fill: SEGM_UMP_COLOR,
              fillOpacity: 0.3,
              text: "Nucleus"
            });
          }
        });
      } else {
        UMPMask.clean("#wave", "UMPMask");
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
            fill: SEGM_N_COLOR,
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

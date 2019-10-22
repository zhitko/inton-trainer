import Stubs from './stubs';

export default {
  api: null,
  getApi: function() {
    const self = this;
    return new Promise(function(resolve, reject) {
      if (!!self.api) {
        resolve(self.api);
      } else {
        let script = document.createElement("script");
        script.onload = function() {
          console.log("QWebChannel are loaded");
          new QWebChannel(qt.webChannelTransport, channel => {
            self.api = channel.objects.jshelper;
            resolve(self.api);
          });
        };
        script.onerror = function() {
          console.warn("Error load QT QWebChannel, will be used stubs");
          self.api = Stubs;
          resolve(self.api);
        };
        script.src = "qrc:///qtwebchannel/qwebchannel.js";
        document.head.appendChild(script);
      }
    });
  },
  getRecords: function() {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getRecords(data => {
          // console.warn("getRecords");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordInfo: function(uuid) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getRecordInfo(uuid, data => {
          // TODO: add check for error response
          // console.warn("getRecordInfo");
          // console.warn(data);
          console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordWave: function(uuid) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getRecordWave(uuid, data => {
          // TODO: add check for error response
          // console.warn("getRecordWave");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordPitch: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getRecordPitch(uuid, !!manual, data => {
          // TODO: add check for error response
          // console.warn("getRecordPitch");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordOctavesRange: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getRecordOctavesRange(uuid, !!manual, data => {
          // TODO: add check for error response
          console.warn("getRecordOctavesRange");
          console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordUPM: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getUPM(uuid, !!manual, data => {
          // TODO: add check for error response
          // console.warn("getRecordUPM");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordUPMMask: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getUPMMask(uuid, !!manual, data => {
          // TODO: add check for error response
          console.warn("getRecordUPMMask");
          console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordSegmentsP: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getSegmentsP(uuid, !!manual, data => {
          // TODO: add check for error response
          // console.warn("getRecordSegmentsP");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordSegmentsN: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getSegmentsN(uuid, !!manual, data => {
          // TODO: add check for error response
          // console.warn("getRecordSegmentsN");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
  getRecordSegmentsT: function(uuid, manual) {
    const self = this;
    return new Promise((resolve, reject) => {
      self.getApi().then(api => {
        api.getSegmentsT(uuid, !!manual, data => {
          // TODO: add check for error response
          // console.warn("getRecordSegmentsT");
          // console.warn(data);
          resolve(JSON.parse(data));
        });
      });
    });
  },
};

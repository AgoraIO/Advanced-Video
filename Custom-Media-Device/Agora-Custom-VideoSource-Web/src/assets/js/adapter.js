import * as $ from 'jquery';
import {
  APP_ID as appID,
  clientConfig,
  channelName
} from './config';
import AgoraRTC from 'agora-rtc-sdk';

export const Sender = {
  client: null,
  stream: null,
  uid: 2,
  init: function () {
    var client = this.client = AgoraRTC.createClient(clientConfig);
    client.on('error', function(err) {
      console.log("Got error msg:", err.reason);
      if (err.reason === 'DYNAMIC_KEY_TIMEOUT') {
        client.renewChannelKey(channelKey, function(){
          console.log("Renew channel key successfully");
        }, function(err){
          console.log("Renew channel key failed: ", err);
        });
      }
    });
  
    client.on('stream-added', function (evt) {
      var stream = evt.stream;
      console.log("New stream added: " + stream.getId());
      console.log("Subscribe ", stream);
      client.subscribe(stream, function (err) {
        console.log("Subscribe stream failed", err);
      });
    });
  
    client.on('stream-subscribed', function (evt) {
      var stream = evt.stream;
      console.log("Subscribe remote stream successfully: " + stream.getId());
    });
  
    client.on('stream-removed', function (evt) {
      var stream = evt.stream;
      console.log("Remote stream is removed " + stream.getId());
    });
  
    client.on('peer-leave', function (evt) {
      var stream = evt.stream;
      if (stream) {
        stream.stop();
        $('#agora_remote' + stream.getId()).remove();
        console.log(evt.uid + " leaved from this channel");
      }
    });
    return new Promise( (resolve) => {
      client.init(appID, () => {
        client.join(appID, channelName, this.uid, (uid) => {
          this.uid = uid;
          AgoraRTC.getDevices((devices) => {
            console.log('devices', devices);
            var stream = this.stream = AgoraRTC.createStream({
              cameraId: devices.find(e => e.kind == "videoinput").deviceId,
              microphoneId: devices.find(e => e.kind == "audioinput").deviceId,
              streamId: uid,
              audio: true,
              video: true
            });
            stream.on("accessAllowed", function() {
              console.log("accessAllowed");
            });
    
            // The user has denied access to the camera and mic.
            stream.on("accessDenied", function() {
              console.log("accessDenied");
            });
  
            stream.init(() => {
              stream.play('local_agora', this.uid);
              resolve(true);
              client.on('stream-published', function (evt) {
                console.log("Publish local stream successfully");
              });
            }, function (err) {
              console.log("getUserMedia failed", err);
            });
    
            console.log('teacher joined');
          });
        }, function (err) {
          console.error('teacher join failed', err);
        });
      })
    })
    
  },
  start: function () {
    this.publish();
  },
  publish: function () {
    this.client.publish(this.stream, function (err) {
      console.error(err);
    });
    console.log('teacher published');
  },
}

export const VideoSource = {
  client: null,
  stream: null,
  uid: 1,
  init: function (canvas) {
    var client = this.client = AgoraRTC.createClient(clientConfig);

    client.on('error', function(err) {
      console.log("Got error msg:", err.reason);
      if (err.reason === 'DYNAMIC_KEY_TIMEOUT') {
        client.renewChannelKey(channelKey, function(){
          console.log("Renew channel key successfully");
        }, function(err){
          console.log("Renew channel key failed: ", err);
        });
      }
    });
  
  
    client.on('stream-added', function (evt) {
      console.log('stream added')
      var stream = evt.stream;
      console.log("New stream added: " + stream.getId());
      console.log("Subscribe ", stream);
    });
  
    client.on('stream-subscribed', function (evt) {

    });
  
    client.on('stream-removed', function (evt) {
      var stream = evt.stream;
      stream.stop();
      $('#agora_remote' + stream.getId()).remove();
      console.log("Remote stream is removed " + stream.getId());
    });
  
    client.on('peer-leave', function (evt) {
      var stream = evt.stream;
      if (stream) {
        stream.stop();
        $('#agora_remote' + stream.getId()).remove();
        console.log(evt.uid + " leaved from this channel");
      }
    });

    client.init(appID, () => {
      this.client.join(appID, channelName, this.uid, (uid) => {
        this.uid = uid;
        var canvasStream = canvas.captureStream(60);
        var videoSource = canvasStream.getVideoTracks()[0];
        var stream = this.stream = AgoraRTC.createStream({
          streamId: uid,
          videoSource,
          audio: false,
          video: false
        });
        stream.on("accessAllowed", function() {
          console.log("accessAllowed");
        });

        // The user has denied access to the camera and mic.
        stream.on("accessDenied", function() {
          console.log("accessDenied");
        });
          stream.init(() => {
            console.log("getUserMedia successfully");
            stream.play('canvasPlayer', this.uid);
            client.publish(stream, function (err) {
              console.error(err);
            });
  
            client.on('stream-published', function (evt) {
              console.log("Publish local stream successfully");
            });
          }, function (err) {
            console.log("getUserMedia failed", err);
          });

        console.log('canvas joined');
      });
    }, function (err) {
      console.error('canvas join failed', err);
    });
    
  },
  start: function () {
    this.publish();
  },
  publish: function () {

    console.log('canvas published');
  },
}

export const Receiver = {
  client: null,
  stream: null,
  uid: 3,
  init: function () {
    var client = this.client = AgoraRTC.createClient(clientConfig);
    client.on('error', function(err) {
      console.log("Got error msg:", err.reason);
      if (err.reason === 'DYNAMIC_KEY_TIMEOUT') {
        client.renewChannelKey(channelKey, function(){
          console.log("Renew channel key successfully");
        }, function(err){
          console.log("Renew channel key failed: ", err);
        });
      }
    });
  
    client.on('stream-added', (evt) => {
      var stream = evt.stream;
      console.log("New stream added: " + stream.getId());
      if (stream.getId() != this.uid) {
        console.log("Subscribe ", stream);
        client.subscribe(stream, function (err) {
          console.log("Subscribe stream failed", err);
        });
      }
    });
  
    client.on('stream-subscribed', function (evt) {
      var stream = evt.stream;
      console.log("Subscribe remote stream successfully: " + stream.getId());
      if ($('div#video #agora_remote'+stream.getId()).length === 0) {
        $('div#video').append('<div id="agora_remote'+stream.getId()+'" style="float:left; width:480px;height:360px;display:inline-block;"></div>');
      }
      stream.play('agora_remote' + stream.getId());
    });
  
    client.on('stream-removed', function (evt) {
      var stream = evt.stream;
      stream.stop();
      $('#agora_remote' + stream.getId()).remove();
      console.log("Remote stream is removed " + stream.getId());
    });
  
    client.on('peer-leave', function (evt) {
      var stream = evt.stream;
      if (stream) {
        stream.stop();
        $('#agora_remote' + stream.getId()).remove();
        console.log(evt.uid + " leaved from this channel");
      }
    });
    client.init(appID, () => {
      client.join(appID, channelName, this.uid, (uid) => {
        console.log('receiver joined');
      }, function (err) {
        console.error('receiver join failed', err);
      });
    })
    
  },
  start: function () {
    this.publish();
  },
  publish: function () {
    this.client.publish(this.stream, function (err) {
      console.error(err);
    });
    console.log('receiver published');
  },
}


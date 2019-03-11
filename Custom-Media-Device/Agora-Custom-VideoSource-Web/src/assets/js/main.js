'use strict';

import renderCanvas from './canvas';
import {Sender, VideoSource, Receiver} from './adapter';

var timers = []
window.isActivated = true
window.location.href.split("?")[1] !== 'receiver' && renderCanvas(window.isActivated, timers);

document.addEventListener("visibilitychange", function() {
    console.log('visibilitychange', document.visibilityState, 'activated', window.isActivated);
    if (document.visibilityState === 'hidden') {
        window.isActivated = false;
    } else {
        window.isActivated = true;
    }
    console.log("visibilitychange", window.isActivated);
    window.location.href.split("?")[1] !== 'receiver' && renderCanvas(window.isActivated, timers);
});

document.addEventListener('DOMContentLoaded', function () {
    var publishSender = document.querySelector('#publishSender');
    var myCanvas = document.querySelector("#myCanvas");
    if (window.location.href.split("?")[1] === 'receiver') {
      // Receiver side
      var $sender = document.querySelector(".sender");
      var $canvas = document.querySelector(".canvas");
      var $senderCanvas = document.querySelector(".sender-canvas");
      $canvas.parentNode.removeChild($canvas);
      $sender.parentNode.removeChild($sender);
      $senderCanvas.parentNode.removeChild($senderCanvas);
      if (window.chrome) {
        navigator.mediaDevices.getUserMedia(
          {video: true, audio: true}
        ).then(function(mediaStream){
          Receiver.init();
        })
      } else {
        Receiver.init();
      }
    } else {
      var remotes = document.querySelector(".remotes");
      remotes.parentNode.removeChild(remotes);
      Sender.init().then(e => {
        VideoSource.init(myCanvas);
      });
      publishSender.addEventListener('click', function () {
        Sender.publish();
      });
    }
  });
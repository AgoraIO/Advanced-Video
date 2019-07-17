import RTCClient from './rtc-client';
import SketchPad, {colors, widths} from './sketchpad';
import {getDevices, serializeFormData, validator, resolutions, Toast} from './common';

import "./assets/style.scss";
import * as M from 'materialize-css';

document.addEventListener("visibilitychange", () => {
  if (document.visibilityState === 'hidden') {
    SketchPad.activate = false;
  } else {
    SketchPad.activate = true;
  }
  SketchPad.autoRender(SketchPad.activate);
})

$(() => {
  SketchPad.mount("#local_canvas");
  
  getDevices(function (devices) {
    devices.audios.forEach(function (audio) {
      $('<option/>', {
        value: audio.value,
        text: audio.name,
      }).appendTo("#microphoneId");
    })
    devices.videos.forEach(function (video) {
      $('<option/>', {
        value: video.value,
        text: video.name,
      }).appendTo("#cameraId");
    })
    resolutions.forEach(function (resolution) {
      $('<option/>', {
        value: resolution.value,
        text: resolution.name
      }).appendTo("#cameraResolution");
    })
    M.AutoInit();
  })

  const fields = ['appID', 'channel'];


  let lineWidthCount = 1;
  $("#lineWidth").on("click", function (e) {
    e.preventDefault();
    $("#lineWidthProgress").removeClass(widths[lineWidthCount])
    let widthIdx = ++lineWidthCount % 3;
    SketchPad.width = widthIdx;
    $("#lineWidthProgress").addClass(widths[widthIdx]);
  });

  let colorCount = 0;
  $("#color").on("click", function (e) {
    e.preventDefault();
    $("#colorProgress").removeClass(colors[colorCount])
    let colorIdx = ++colorCount % 3;
    SketchPad.color = colorIdx;
    $("#colorProgress").addClass(colors[colorIdx]);
  });

  $("#clear").on("click", function (e) {
    e.preventDefault();
    SketchPad.clear();
  })

  let rtc = new RTCClient();

  $("#show_quality").on("change", function (e) {
    e.preventDefault();
    if (!rtc._joined) {
      $(this).removeAttr("checked");
      return false;
    }
    rtc.setNetworkQualityAndStreamStats(this.checked);
  })

  $("#join").on("click", function (e) {
    e.preventDefault();
    console.log("join")
    const params = serializeFormData();
    if (validator(params, fields)) {
      rtc.join(params).then(() => {
        rtc.publish();
      })
    }
  })

  const selectVals = [ "camera", "canvas", "./assets/sample.mp4"];
  const domIds = [ 'local_stream', 'local_canvas', 'local_video' ];

  $("#stream").on("change", function (e) {
    e.preventDefault();
    if (!rtc._joined) {
      Toast.error("Please Join First!");
      return;
    }
    const idx = selectVals.indexOf($(this).val());
    rtc._currentStreamIdx = idx;
    const currentDomId = domIds[idx];

    for (let dom of domIds) {
      if (dom == currentDomId) {
        $("#"+currentDomId).hasClass("hide") && $("#"+currentDomId).removeClass("hide");
        continue;
      }
      if (!$("#"+dom).hasClass("hide")) {
        $("#"+dom).addClass("hide");
      }
    }
  })

  $("#switch_track").on("click", function (e) {
    e.preventDefault();
    if (!rtc._joined) {
      Toast.error("Please Join First!");
      return;
    }
    const currentDomId = domIds[rtc._currentStreamIdx];
    if (currentDomId == "local_video") {
      $("#sample_video")[0].play().then(() => {
        console.log("play video success");
      })
    }
    const params = serializeFormData();
    if (validator(params, fields)) {
      rtc.replaceTrack(params, () => {
        Toast.notice("switch success");
      })
    }
  })

  $("#leave").on("click", function (e) {
    e.preventDefault();
    console.log("leave")
    const params = serializeFormData();
    if (validator(params, fields)) {
      rtc.leave();
    }
  });
})
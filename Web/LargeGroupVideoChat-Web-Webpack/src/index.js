import RTCClient from './rtc-client';
import {getDevices, serializeFormData, objectToUrlString, validator, resolutions, Toast} from './common';
import "./assets/style.scss";
import * as M from 'materialize-css';

function getUrlParameters () {
  const urlstr = window.location.href.split("?")[1];
  if (!urlstr) return;
  const _urlParams = urlstr.split("&");
  const urlParams = {};
  for (let item of _urlParams) {
    let [key, value] = item.split("=");
    urlParams[key] = value;
  }
  return urlParams;
}

function fillForm(urlParams) {
  if (!urlParams || urlParams.role != 'audience') return;

  if (!validator(urlParams, ['appID', 'channel', 'role'])) {
    return;
  }

  $("#join").text("JOIN AS AUDIENCE");

  $("#local_video_view").remove();

  const data = {
    appID: urlParams.appID,
    channel: urlParams.channel,
    role: urlParams.role,
  }

  $("#appID").val(data.appID);
  $("#channel").val(data.channel);
  $("#role")[0].innerText = data.role;

  if (urlParams.token) {
    data.token = urlParams.token;
    $("#token").val(data.token);
  }

  if (urlParams.mode) {
    if (urlParams.mode === 'live') {
      $("#live").prop("checked", true);
      $("#rtc").prop("checked", false);
    }
    if (urlParams.mode === 'rtc') {
      $("#live").prop("checked", false);
      $("#rtc").prop("checked", true);
    }
    data.mode = urlParams.mode;
  }
  
  if (urlParams.codec) {
    if (urlParams.codec === 'live') {
      $("#h264").prop("checked", true);
      $("#vp8").prop("checked", false);
    }
    if (urlParams.codec === 'rtc') {
      $("#h264").prop("checked", false);
      $("#vp8").prop("checked", true);
    }
    data.codec = urlParams.codec;
  }

  return data;
}

$(() => {

  let rtc = new RTCClient();

  $(".autoplay-fallback").on("click", function (e) {
    e.preventDefault()
    const id = e.target.getAttribute("id").split("video_autoplay_")[1]
    console.log("autoplay fallback")
    if (id === 'local') {
      rtc._localStream.resume().then(() => {
        Toast.notice("local resume")
        $(e.target).addClass("hide")
      }).catch((err) => {
        Toast.error("resume failed, please open console see more details")
        console.error(err)
      })
      return;
    }
    const remoteStream = rtc._remoteStreams.find((item) => `${item.getId()}` == id)
    if (remoteStream) {
      remoteStream.resume().then(() => {
        Toast.notice("remote resume")
        $(e.target).addClass("hide")
      }).catch((err) => {
        Toast.error("resume failed, please open console see more details")
        console.error(err)
      })
    }
  })

  $("#share").on("click", function (e) {
    e.preventDefault();
    let formData = serializeFormData();
    formData.role = 'audience';
    let urlStr = objectToUrlString(formData, ['appID', 'channel', 'token', 'mode', 'codec', 'role']);
    $("#share_link").val(window.location.href.split("?")[0] + "?" + urlStr);
    $("#share_link")[0].select();
    document.execCommand("copy");
    Toast.notice("copy success");
  })

  const modal = M.Modal.init($("#warn")[0]);


  $("#sure").on("click", () => {
    modal.close();
  })

  $("#never_show").on("click", () => {
    modal.close();
    localStorage.setItem("large_group_video_chat", true)
  })

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

    if (localStorage.getItem("large_group_video_chat") != "true") {
      M.Modal.init($("#warn")[0], {
        dismissible: false,
      }).open()
    }
    
    let urlData = fillForm(getUrlParameters());
    if (urlData && !rtc._joined) {
      rtc.join(urlData).then(() => {
        // join success
      })
    }
    
  })

  const fields = ['appID', 'channel'];
  $("#show_profile").on("change", function (e) {
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
    console.log("params", params);
    if (validator(params, fields)) {
      rtc.join(params).then(() => {
        rtc.publish();
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
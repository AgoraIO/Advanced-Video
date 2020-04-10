import RTCClient from './rtc-client'
import SketchPad, {colors, widths} from './sketchpad'
import {getDevices, serializeFormData, validator, resolutions, Toast} from './common'

import './assets/style.css'
import * as M from 'materialize-css'

// handle current tab or window inactive scenario
// If current tab or window inactive `visibilitychange` would occurs and we would change `activate` state so that it will switch to another async render way 
document.addEventListener('visibilitychange', () => {
  if (document.visibilityState === 'hidden') {
    SketchPad.activate = false
  } else {
    SketchPad.activate = true
  }
  SketchPad.autoRender(SketchPad.activate)
})

$(() => {
  SketchPad.mount('#local_canvas')
  
  getDevices(function (devices) {
    devices.audios.forEach(function (audio) {
      $('<option/>', {
        value: audio.value,
        text: audio.name,
      }).appendTo('#microphoneId')
    })
    devices.videos.forEach(function (video) {
      $('<option/>', {
        value: video.value,
        text: video.name,
      }).appendTo('#cameraId')
    })
    resolutions.forEach(function (resolution) {
      $('<option/>', {
        value: resolution.value,
        text: resolution.name
      }).appendTo('#cameraResolution')
    })
    M.AutoInit()

    if (localStorage.getItem('custom_videosource') != 'true') {
      M.Modal.init($('#warn')[0], {
        dismissible: false,
      }).open()
    }
  })

  const fields = ['appID', 'channel']
  
  const modal = M.Modal.init($('#warn')[0])

  $('#sure').on('click', () => {
    modal.close()
  })

  $('#never_show').on('click', () => {
    modal.close()
    localStorage.setItem('custom_videosource', true)
  })

  let lineWidthCount = 1
  $('#lineWidth').on('click', function (e) {
    e.preventDefault()
    $('#lineWidthProgress').removeClass(widths[lineWidthCount])
    let widthIdx = ++lineWidthCount % 3
    SketchPad.width = widthIdx
    $('#lineWidthProgress').addClass(widths[widthIdx])
  })

  let colorCount = 0
  $('#color').on('click', function (e) {
    e.preventDefault()
    $('#colorProgress').removeClass(colors[colorCount])
    let colorIdx = ++colorCount % 3
    SketchPad.color = colorIdx
    $('#colorProgress').addClass(colors[colorIdx])
  })

  $('#clear').on('click', function (e) {
    e.preventDefault()
    SketchPad.clear()
  })

  let rtc = new RTCClient()

  $('.autoplay-fallback').on('click', function (e) {
    e.preventDefault()
    const id = e.target.getAttribute('id').split('video_autoplay_')[1]
    console.log('autoplay fallback')
    if (id === 'local') {
      rtc._localStream.resume().then(() => {
        Toast.notice('local resume')
        $(e.target).addClass('hide')
      }).catch((err) => {
        Toast.error('resume failed, please open console see more details')
        console.error(err)
      })
      return
    }
    const remoteStream = rtc._remoteStreams.find((item) => `${item.getId()}` == id)
    if (remoteStream) {
      remoteStream.resume().then(() => {
        Toast.notice('remote resume')
        $(e.target).addClass('hide')
      }).catch((err) => {
        Toast.error('resume failed, please open console see more details')
        console.error(err)
      })
    }
  })

  $('#show_profile').on('change', function (e) {
    e.preventDefault()
    if (!rtc._joined) {
      $(this).removeAttr('checked')
      return false
    }
    rtc.setNetworkQualityAndStreamStats(this.checked)
  })

  $('#join').on('click', function (e) {
    e.preventDefault()
    console.log('join')
    const params = serializeFormData()
    if (validator(params, fields)) {
      rtc.join(params).then(() => {
        rtc.publish()
      })
    }
  })

  const selectVals = [ 'camera', 'canvas', './assets/sample.mp4']
  const domIds = [ 'local_stream', 'local_canvas', 'local_video' ]

  $('#stream').on('change', function (e) {
    e.preventDefault()
    if (!rtc._joined) {
      Toast.error('Please Join First!')
      return
    }
    const idx = selectVals.indexOf($(this).val())
    rtc._currentStreamIdx = idx
    const currentDomId = domIds[idx]

    for (let dom of domIds) {
      if (dom == currentDomId) {
        $('#'+currentDomId).hasClass('hide') && $('#'+currentDomId).removeClass('hide')
        continue
      }
      if (!$('#'+dom).hasClass('hide')) {
        $('#'+dom).addClass('hide')
      }
    }
  })

  $('#switch_track').on('click', function (e) {
    e.preventDefault()
    if (!rtc._joined) {
      Toast.error('Please Join First!')
      return
    }
    const currentDomId = domIds[rtc._currentStreamIdx]
    if (currentDomId == 'local_video') {
      $('#sample_video')[0].play().then(() => {
        console.log('play video success')
      })
    }
    const params = serializeFormData()
    if (validator(params, fields)) {
      rtc.replaceTrack(params, () => {
        Toast.notice('switch success')
      })
    }
  })

  $('#leave').on('click', function (e) {
    e.preventDefault()
    console.log('leave')
    const params = serializeFormData()
    if (validator(params, fields)) {
      rtc.leave()
    }
  })
})
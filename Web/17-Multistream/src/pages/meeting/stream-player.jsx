import React, {useMemo, useState, useEffect} from 'react';
import PropTypes from 'prop-types';

StreamPlayer.propTypes = {
  stream: PropTypes.object,
}

export default function StreamPlayer (props) {
  const {stream, domId, uid} = props;

  const [resume, changeResume] = useState(false);

  const [autoplay, changeAutoplay] = useState(false);

  const handleClick = () => {
    if (autoplay && !resume) {
      stream.resume();
      changeResume(true);
    }
  }

  const handleDoubleClick = (evt) => {
    evt.stopPropagation();
    props.onDoubleClick(stream);
  }

  const [state, setState] = useState({
    accessDelay: 0,
    fps: 0,
    resolution: 0,    
  });

  const analytics = useMemo(() => state, [state]);

  useEffect(() => {
    if (!stream) return () => {};

    const timer = setInterval(() => {
      stream.getStats((stats) => {
        const width = props.local ? stats.videoSendResolutionWidth : stats.videoReceiveResolutionWidth;
        const height = props.local ? stats.videoSendResolutionHeight : stats.videoReceiveResolutionHeight;
        const fps = props.local ? stats.videoSendFrameRate : stats.videoReceiveFrameRate;
        setState({
          accessDelay: `${stats.accessDelay ? stats.accessDelay : 0}`,
          resolution: `${width}x${height}`,
          fps: `${fps ? fps : 0}`,
        })
      })
    }, 500);

    return () => {
      clearInterval(timer);
    }
  }, [stream]);

  useEffect(() => {
    if (!stream) return () => {};

    if (!stream.isPlaying()) {
      stream.play(domId, {fit: 'cover'}, (errState) => {
        if (errState && errState.status !== 'aborted') {
          console.log("stream-player play failed ", domId)
          changeAutoplay(true);
        } else {
          console.log(`>>>> ${stream.getId()}#played`);
        }
      });
    } else {
      stream.stop();
    }
    return () => {
      if (stream.isPlaying()) {
        console.log(`>>>> ${stream.getId()}#stop >>>> ${domId}`)
        stream.stop();
      } else {
        stream.play(domId, {fit: 'cover'}, (errState) => {
          if (errState && errState.status !== 'aborted') {
            console.log("stream-player play failed ", domId)
            changeAutoplay(true);
          } else {
            console.log(`>>>> ${stream.getId()}#played`);
          }
        });
      }
    }
  }, [stream, domId]);

  return (
    stream ?
      <div style={props.style} className={`stream-player grid-player ${(props.main ? 'main-stream-player' : '')} ${autoplay ? "autoplay": ''}`} id={domId} onClick={uid ? handleClick : null} onDoubleClick={props.onDoubleClick ? handleDoubleClick : null}>
        {props.showProfile ? 
          <div className={props.className ? props.className : (props.main ? 'main-stream-profile' : '')}>
            <span>SD-RTN delay: {analytics.accessDelay}ms</span>
            <span>Video: {analytics.fps}fps {analytics.resolution}</span>
          </div>
        : null}
        {props.showUid && uid ? <div className='stream-uid'>UID: {uid}</div> : null }
      </div>
    :
      <div style={props.style} className={`stream-player cover-media grid-player ${(props.main ? 'main-stream-player' : '')}`}></div>
  )
}
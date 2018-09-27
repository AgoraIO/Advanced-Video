import AgoraRTC from 'agora-rtc-sdk'
import {Logger} from './utils'
import { EventEmitter } from 'events';
import { StreamType, DualType } from '../utils/constants'


// wrapper of Stream Object
class StreamWrapper {
    constructor(stream, type, uid) {
        // websdk stream object
        this.stream = stream;
        // type - remote or local
        this.type = type || StreamType.Local;
        // uid of the stream
        this.uid = uid;
    }
}

// wrapper of agora rtc sdk client
class RtcClient extends EventEmitter{
    constructor(appid) {
        super();
        this.appid = appid;
        this.rtc = AgoraRTC.createClient({ mode: 'live', codec: 'h264' });
        this.streams = [];
        this.uid = null;
        this.videoProfile = "480p_1";
        this.dynamicKey = null;

        // this.subscribeWindowResizeEvent();
        this.subscribeStreamEvents();
    }

    //life cycle
    init(channelName, publish) {
        return new Promise(async (resolve, reject) => {
            let appid = this.appid;
            let client = this.rtc;
            try {
                //dynamic key used to join channel
                //leave it undefined/null if app certificate is not on
                let key = await this.getDynamicKey(channelName);
                //init sdk client with your appid
                client.init(appid, () => {
                    //join channel, a random uid will be given once joined
                    client.join(key, channelName, undefined, uid => {
                        // if you are an audience, you may not need to continue
                        if(!publish) {
                            // immediately resolve if audience
                            return resolve();
                        }

                        // prepare to publish, init local stream opts
                        let options = {
                            streamID: uid,
                            audio: true,
                            video: true,
                            screen: false
                        };
                        this.uid = uid;
                        let localStream = AgoraRTC.createStream(options);
                        // this decides the video resolution/fps
                        // for all values go to: https://docs.agora.io/cn/2.4/product/Voice/API%20Reference/communication_web_audio?platform=Web
                        localStream.setVideoProfile(this.videoProfile);
                        
                        // initialize localstream
                        localStream.init(() => {
                            //adding this stream into stream list and notify stream list update
                            this.addStream(new StreamWrapper(localStream, StreamType.Local, uid))
                            //enable dual stream mode so we have big/small streams
                            client.enableDualStream(() => {
                                //publish stream so that others will receive your video
                                client.publish(localStream);
                                resolve();
                            }, err => {
                                reject(err);
                            });
                        });
                    }, err => {
                        reject(err);
                    });
                });
            } catch(e){
                throw e;
            };
        });
    }


    getDynamicKey(channelName) {
        // if dynamic not enabled
        return Promise.resolve(undefined);

        // if dynamic key enabled
        // return $.ajax({
        //     url: 'service url to get your dynamic key'
        // })
    }

    //subscribe necessary RTC events
    subscribeStreamEvents() {
        let client = this.rtc;
        // indicate a new user has joined the channel, you will not be able to see him unless you subscribe him.
        client.on('stream-added', evt => {
            let stream = evt.stream;
            Logger.log("New stream added: " + stream.getId());
            Logger.log("Timestamp: " + Date.now());
            Logger.log("Subscribe ", stream);
            client.subscribe(stream, function (err) {
                Logger.log("Subscribe stream failed", err);
            });
        });

        // indicate a user has left current channel
        client.on('peer-leave', evt => {
            Logger.log("Peer has left: " + evt.uid);
            Logger.log("Timestamp: " + Date.now());
            Logger.log(evt);
            this.removeStream(evt.uid);
        });

        // indicate a user has been subscribed, you may play the live video now
        client.on('stream-subscribed', evt => {
            let stream = evt.stream;
            Logger.log("Got stream-subscribed event");
            Logger.log("Timestamp: " + Date.now());
            Logger.log("Subscribe remote stream successfully: " + stream.getId());
            Logger.log(evt);

            let streamObj = new StreamWrapper(stream, StreamType.Remote, stream.getId());
            this.addStream(streamObj);
        });

        // indicate a user's stream has been removed
        client.on("stream-removed", evt => {
            let stream = evt.stream;
            Logger.log("Stream removed: " + evt.stream.getId());
            Logger.log("Timestamp: " + Date.now());
            Logger.log(evt);

            this.removeStream(stream.getId());
        });

        client.on('stream-published', () => {
            this.published = true;
            Logger.log(`stream published`);
        });
    }

    addStream(stream) {
        if(!stream instanceof StreamWrapper) {
            console.error(`only type of streamWrapper can be added to list`);
            return;
        }
        this.streams.push(stream);
        this.emit('streamlist-update', this.streams);
    }

    removeStream(uid) {
        this.streams = this.streams.filter(stream => `${uid}` !== `${stream.uid}`);
        this.emit('streamlist-update', this.streams);
    }

    setRemoteStreamType(stream, dualType) {
        if(!stream instanceof StreamWrapper) {
            console.error(`only type of streamWrapper can be added to list`);
            return;
        }
        let client = this.rtc;
        client.setRemoteVideoStreamType(stream.stream, dualType);
    }
}

export default RtcClient;
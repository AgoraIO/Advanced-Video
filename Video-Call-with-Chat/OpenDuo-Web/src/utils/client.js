import SignalingClient from './signalingClient';
import RtcClient from './rtcClient';
import { Howl } from 'howler';
import $ from 'jquery';
import { Logger, Message } from './utils'

class Client {
    //construct a meeting client with signal client and rtc client
    constructor(appid) {
        this.appid = appid;
        this.channelName = Math.random() * 10000 + "";
        this.signal = new SignalingClient(appid);
        this.rtc = new RtcClient(appid);

        //ring tones resources
        this.sound_ring = new Howl({
            src: [require('../assets/media/basic_ring.mp3')],
            loop: true
        });

        this.sound_tones = new Howl({
            src: [require('../assets/media/basic_tones.mp3')],
            loop: true
        });

        this.signal.on("inviteReceived", call => {this.onInviteReceived(call)});
        this.signal.on("inviteEndByPeer", () => {this.onInviteEndByPeer()});

        this.subscribeEvents();
    }

    init(localAccount) {
        return this.signal.login(localAccount);
    }


    //return a promise resolves a remote account name
    requestRemoteAccount() {
        return new Promise((resolve) => {
            let dialog = $(".remoteAccountModal");
            let localAccount = this.localAccount;

            dialog.find(".callBtn").off("click").on("click", () => {
                let accountField = dialog.find(".remoteAccountField");
                //dialog confirm
                let account = accountField.val();

                if (!account) {
                    accountField.siblings(".invalid-feedback").html("Valid account should be a non-empty numeric value.")
                    accountField.removeClass("is-invalid").addClass("is-invalid");
                } else if (`${account}` === `${localAccount}`) {
                    accountField.siblings(".invalid-feedback").html("You can't call yourself.")
                    accountField.removeClass("is-invalid").addClass("is-invalid");
                } else {
                    $(".startCallBtn").hide();
                    dialog.modal('hide');
                    resolve(account);
                }
            });

            //start modal
            dialog.modal({ backdrop: "static", focus: true });
        });
    }

    //return a promise resolves a signaling call result
    call(channelName, account, requirePeerOnline) {
        return new Promise((resolve, reject) => {
            let dialog = $(".callingModal");
            dialog.find(".callee").html(account);
            let signal = this.signal;

            signal.call(channelName, account, requirePeerOnline).then(() => {
                dialog.modal('hide');
                resolve();
            }).catch(err => {
                Message.show(err.reason);
                reject();
            });
        });
    }

    //end given call object, passive means the call is ended by peer
    endCall(call, passive) {
        let signal = this.signal;
        let rtc = this.rtc;
        let btn = $(".toolbar .muteBtn");

        $(".startCallBtn").show();

        rtc.muted = true;
        btn.removeClass("btn-info").addClass("btn-secondary");
        btn.find("i").html("mic");
        //end rtc
        rtc.end();
        //end signal call
        signal.endCall(call, passive);
        return Promise.resolve();
    }

    //ring when calling someone else
    ringCalling(play) {
        if (play) {
            this.sound_ring.play();
        } else {
            this.sound_ring.stop();
        }
    }
    //ring when being called by someone else
    ringCalled(play) {
        if (play) {
            this.sound_tones.play();
        } else {
            this.sound_tones.stop();
        }
    }

    //events
    subscribeEvents() {
        let signal = this.signal;
        //toolbar end call btn
        $(".toolbar .endCallBtn").off("click").on("click", () => {
            this.ringCalling(false);
            this.endCall(signal.call_active || signal.call_holding, false);
        });

        //toolbar mute btn
        $(".toolbar .muteBtn").off("click").on("click", e => {
            let btn = $(e.currentTarget);
            let rtc = this.rtc;
            rtc.toggleMute();
            if (rtc.muted) {
                btn.removeClass("btn-secondary").addClass("btn-info");
                btn.find("i").html("mic_off");
            } else {
                btn.removeClass("btn-info").addClass("btn-secondary");
                btn.find("i").html("mic");
            }
        });

        $(".startCallBtn").off("click").on("click", () => {
            let channelName = this.channelName;
            this.requestRemoteAccount().then(remoteAccount => {
                //start calling via signal
                if (remoteAccount !== "") {
                    this.ringCalling(true);
                    this.rtc.init(channelName, false).then(stream => {
                        this.call(channelName, remoteAccount, true).then(() => {
                            this.ringCalling(false);
                            this.rtc.rtc.publish(stream);
                        }).catch(() => {
                            this.ringCalling(false);
                            this.endCall(signal.call_active || signal.call_holding, false);
                        });
                    }).catch(() => {
                        this.ringCalling(false);
                        this.endCall(signal.call_active || signal.call_holding, false);
                    });
                }
            });
        });
    }

    //delegate callback when receiving call
    onInviteReceived(call) {
        let dialog = $(".calledModal");
        let signal = this.signal;
        let rtc = this.rtc;

        dialog.find(".caller").html(call.peer);
        dialog.find(".declineBtn").off("click").on("click", () => {
            dialog.modal('hide');
            this.ringCalled(false);
            signal.rejectCall(call, 0);
        });

        dialog.find(".acceptBtn").off("click").on("click", () => {
            dialog.modal('hide');
            $(".startCallBtn").hide();
            this.ringCalled(false);
            signal.acceptCall(call).then(call => {
                rtc.init(call.channelName, true);
            }).catch(err => {
                Logger.log(`Accept call failed: ${err}`);
            });
        });

        this.ringCalled(true);
        dialog.modal({ backdrop: "static" });
    }

    //delegate callback called when call end by peer
    onInviteEndByPeer() {
        let signal = this.signal;
        $(".calledModal").modal('hide');
        this.ringCalled(false);
        this.endCall(signal.call_active || signal.call_holding, true);
    }
}

export default Client;
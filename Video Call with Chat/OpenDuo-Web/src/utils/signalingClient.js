import { Logger } from './utils'
import { EventEmitter } from 'events';

class SignalingClient {
    constructor(appid) {
        this.signal = Signal(appid);
        this.call_holding = null;
        this.call_active = null;
        this.channel = null;
        this.appid = appid;
        this.uid = null;
        this.events = new EventEmitter();
    }

    login(account) {
        return new Promise((resolve, reject) => {
            Logger.log('Logging in ' + account);
            //starts login
            let session = this.signal.login(account, "_no_need_token");

            //if success
            session.onLoginSuccess = uid => {
                Logger.log('login success ' + uid);
                this.uid = uid;
                resolve();
            };

            //if fail
            session.onLoginFailed = ecode => {
                Logger.log('login failed ' + ecode);
                this.session = null;
                reject();
            };

            session.onInviteReceived = (...args) => {this._onInviteReceived(...args)};
            this.session = session;
        });
    }

    call(channelName, peer, require_peer_online) {
        return new Promise((resolve, reject) => {
            let extra = {};

            if (require_peer_online) {
                extra["_require_peer_online"] = 1;
            }

            let extra_msg = JSON.stringify(extra);

            Logger.log('call ' + peer + ' , channelName : ' + channelName + ', extra : ' + extra_msg);

            let call = this.session.channelInviteUser2(channelName, peer, extra_msg);

            call.onInviteAcceptedByPeer = extra => {
                this.call_active = this.call_holding;
                this.call_holding = null;
                this.join(call.channelName).then(() => {
                    Logger.log('call.onInviteAcceptedByPeer ' + extra);
                    resolve();
                });
            };

            call.onInviteRefusedByPeer = extra => {
                Logger.log(`call.onInviteRefusedByPeer ${extra}`);
                let status = JSON.parse(extra).status;
                reject({ reason: `Call refused. ${this.statusText(status)}` });
            };

            call.onInviteFailed = extra => {
                Logger.log(`call.onInviteFailed ${extra}`);
                reject({ reason: `Invite failed: ${JSON.parse(extra).reason}` });
            };

            call.onInviteEndByPeer = (...args) => {this._onInviteEndByPeer(...args)};

            this.call_holding = call;
        });
    }

    join(channelName) {
        return new Promise((resolve, reject) => {
            Logger.log(`Joining channel ${channelName}`);

            let channel = this.session.channelJoin(channelName);
            channel.onChannelJoined = () => {
                Logger.log('channel.onChannelJoined');
                resolve();
            };

            channel.onChannelJoinFailed = ecode => {
                Logger.log(`channel.onChannelJoinFailed ${ecode}`);
                reject(ecode);
            };

            this.channel = channel;
        });
    }

    leave() {
        let channel = this.channel;
        if (channel === null) {
            return Promise.resolve();
        }
        return new Promise((resolve) => {
            channel.onChannelLeaved = () => {
                Logger.log('channel.onChannelLeaved');
                this.channel = null;
                resolve();
            };
            channel.channelLeave();
        });
    }

    acceptCall(call) {
        return new Promise((resolve, reject) => {
            this.call_active = this.call_holding;
            this.call_holding = null;

            this.join(call.channelName).then(() => {
                call.channelInviteAccept();
                resolve({
                    peer: call.peer,
                    channelName: call.channelName
                });
            }).catch(err => {
                reject(err);
            });
        });
    }

    rejectCall(call, status) {
        status = status || 0;
        call.channelInviteRefuse(JSON.stringify({ status: status }));
        return Promise.resolve();
    }

    endCall(call, passive) {
        call.onInviteEndByMyself = extra => {
            Logger.log('call.onInviteEndByMyself ' + extra);
            this.call_holding = (this.call_holding === call) ? null : this.call_holding;
            this.call_active = (this.call_active === call) ? null : this.call_active;
            this.leave();
        };

        if (!passive) {
            call.channelInviteEnd();
        } else {
            this.call_active = null;
            this.call_holding = null;
        }
        return Promise.resolve();
    }

    statusText(status) {
        switch (status) {
        case 0:
            return "Peer rejected.";
        case 1:
            return "Peer is busy.";
        }
    }

    on(event, callback) {
        this.events.on(event, callback);
    }

    //session events delegate
    _onInviteReceived(call) {
        Logger.log(`recv invite from ${call.peer}, ${call.channelName}, ${call.extra}`);

        //incoming call for accept or refuse
        if (this.call_active !== null) {
            //busy
            this.rejectCall(call, 1);
        } else {
            call.onInviteEndByPeer = (...args) => {this._onInviteEndByPeer(...args)};
            this.call_holding = call;
            this.events.emit("inviteReceived", call);
        }
    }

    //call events delegate
    _onInviteEndByPeer(extra) {
        Logger.log('call.onInviteEndByPeer ' + extra);
        this.events.emit("inviteEndByPeer");
    }
}

export default SignalingClient;
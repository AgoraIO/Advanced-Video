import { Logger } from './utils'
import { EventEmitter } from 'events';
import Signal from 'agora-signaling-sdk'

// wrapper of agora signaling sdk client
class SignalClient extends EventEmitter {
    constructor(appid) {
        super();
        this.signal = Signal(appid);
        this.session = null;
        this.channel = null;
    }

    async init(account, channelName) {
        try {
            await this.login(account);
            await this.joinChannel(channelName);
            Logger.log(`signal initialized`);
        } catch(e) {
            throw(e);
        }
    }

    login(account) {
        return new Promise((resolve, reject) => {
            // login signaling with given account, token needs to be provided if app certificate is on
            let session = this.signal.login(account, '_no_need_token');
            session.onLoginSuccess = () => {
                resolve();
            };
            session.onLoginFailed = ecode => {
                reject(`login failed: ${ecode}`);
            };
            this.session = session;
        });
    }

    joinChannel(channelName) {
        return new Promise((resolve, reject) => {
            // join a channel with given name, users in the same channel can communicate with each other with channel functions
            let channel = this.session.channelJoin(channelName);
            channel.onChannelJoined = () => {
                channel.onMessageChannelReceive = (account, uid, msg) => {
                    // received channle message
                    this.emit("channel-message", {account, msg});
                };
                resolve();
            };
            channel.onChannelJoinFailed = ecode => {
                reject(`join channel ${channelName} failed: ${ecode}`);
            };
            this.channel = channel;
        });
    }

    broadcast(msg) {
        // broadcast msgs to users in current channel
        this.channel.messageChannelSend(msg);
    }
}

export default SignalClient;
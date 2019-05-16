import { Logger } from './utils'
import { EventEmitter } from 'events'

export default class RTMClient {
    constructor(appid) {
        this.client = AgoraRTM.createInstance(appid)
        this.channel = null
        this.uid = null
        this.appid = appid
        this.remoteInvitation = null
        this.localInvitation = null
        this.events = new EventEmitter()

        this.client.on("MessageFromPeer", ({text}, peerId) => {
            if (text == 'endCall') this.events.emit("inviteEndByPeer", {text, peerId})
            Logger.log("[rtm] MessageFromPeer " + text + " peerId " + peerId)
        })

        this.client.on("ConnectionStateChanged", (state, reason) => {
            Logger.log("[rtm] ConnectionStateChanged state: " + state + " reason: " + reason)
        })
        this.client.on("RemoteInvitationReceived", remoteInvitation => {
            Logger.log("[rtm] RemoteInvitationReceived ", )
            console.log(remoteInvitation)
            this.remoteInvitation = remoteInvitation
            this.events.emit("RemoteInvitationReceived", remoteInvitation.callerId)
            remoteInvitation.on("RemoteInvitationAccepted", () => {
                Logger.log("[rtm] RemoteInvitationAccepted ")
                this.events.emit("RemoteInvitationAccepted", {
                    uid: this.uid,
                    channel: remoteInvitation.content,
                    state: remoteInvitation.state
                })
            })
            remoteInvitation.on("RemoteInvitationCanceled", () => {
                Logger.log("[rtm] RemoteInvitationCanceled ")
                this.events.emit("RemoteInvitationCanceled", {
                    uid: this.uid,
                    channel: this.channel,
                    state: remoteInvitation.state
                })
            })
            remoteInvitation.on("RemoteInvitationFailure", (reason) => {
                Logger.log("[rtm] RemoteInvitationFailure ", reason)
                this.events.emit("RemoteInvitationFailure", {
                    uid: this.uid,
                    channel: this.channel,
                    state: remoteInvitation.state
                })
            })
            remoteInvitation.on("RemoteInvitationRefused", () => {
                Logger.log("[rtm] RemoteInvitationRefused ")
                this.events.emit("RemoteInvitationRefused", {
                    uid: this.uid,
                    channel: this.channel,
                    state: remoteInvitation.state
                })
            })
        })
    }

    async login(uid) {
      try {
        Logger.log('Logging in ' + uid)
        await this.client.login({uid})
        this.uid = uid
        Logger.log('[rtm] login suscess' + uid)
        return true
      } catch (e) {
        Logger.log('[rtm] login failed ' + e)
      }
    }

    _createChannel (channelName) {
      this.channel = this.client.createChannel(channelName)

      this.channel.on('MemberJoined', (memberId) => {
        Logger.log('[rtm] channel.MemberJoined')
        this.events.emit("MemberJoined", memberId)
      })

      this.channel.on("MemberLeft", (memberId) => {
        Logger.log('[rtm] channel.MemberLeft')
        this.events.emit("MemberLeft", memberId)
      })
    }

    async join(channelName) {
      this._createChannel(channelName)
      await this.channel.join()
    }

    async leave() {
      await this.channel.leave()
      this.channel = null
    }

    _createLocalInvitation(calleeId) {
        this.localInvitation = this.client.createLocalInvitation(calleeId)
    }

    async invite(channelName, peerId) {
        if (!this.channel) {
            await this.join(channelName)
        }
        this._createLocalInvitation(peerId)
        this.localInvitation.on("LocalInvitationAccepted", (res) => {
            Logger.log("[rtm] LocalInvitationAccepted ", res)
            this.events.emit("LocalInvitationAccepted", {peerId: this.peerId, res, state: this.localInvitation.state})
        })
        this.localInvitation.on("LocalInvitationRefused", (res) => {
            Logger.log("[rtm] LocalInvitationRefused ", res)
            this.events.emit("LocalInvitationRefused", {res, state: this.localInvitation.state})
        })
        this.localInvitation.on("LocalInvitationCanceled", () => {
            Logger.log("[rtm] LocalInvitationCanceled ", res)
            this.events.emit("LocalInvitationCanceled", {state: this.localInvitation.state})
        })
        this.localInvitation.on("LocalInvitationFailure", (reason) => {
            Logger.log("[rtm] LocalInvitationFailure ", reason)
            this.events.emit("LocalInvitationFailure", {reason, state: this.localInvitation.state})
            this.leave()

        })
        this.localInvitation.on("LocalInvitationReceivedByPeer", () => {
            Logger.log("[rtm] LocalInvitationReceivedByPeer ")
            this.events.emit("LocalInvitationReceivedByPeer", {state: this.localInvitation.state})
        })
        this.localInvitation.content = channelName
        this.peerId = peerId
        return this.localInvitation.send()
    }

    accept() {
        this.remoteInvitation.accept()
    }

    refuse() {
        this.remoteInvitation.refuse()
    }

    async endCall() {
        Logger.log("[rtm endCall]")
        if (this.localInvitation) {
            this.client.sendMessageToPeer({text: "endCall"}, this.peerId)
        } else {
            this.client.sendMessageToPeer({text: "endCall"}, this.remoteInvitation.callerId)
        }
    }

    on(event, callback) {
        this.events.on(event, callback)
    }
}
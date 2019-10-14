import { WRCKeyboardEvent } from '../types'

const robotKeyValues = [
  'backspace',
  'delete',
  'enter',
  'tab',
  'escape',
  'up',
  'down',
  'right',
  'left',
  'home',
  'end',
  'pageup',
  'pagedown',
  'f1',
  'f2',
  'f3',
  'f4',
  'f5',
  'f6',
  'f7',
  'f8',
  'f9',
  'f10',
  'f11',
  'f12',
  'command',
  'alt',
  'control',
  'shift',
  'right_shift',
  'space',
  'printscreen',
  'insert',
  'audio_mute',
  'audio_vol_down',
  'audio_vol_up',
  'audio_play',
  'audio_stop',
  'audio_pause',
  'audio_prev',
  'audio_next',
  'audio_rewind',
  'audio_forward',
  'audio_repeat',
  'audio_random'
]
export class KeyboardRemoteControl {
  private onKeyboardEvent: (event: WRCKeyboardEvent) => any

  constructor(onKeyboardEvent: (e: WRCKeyboardEvent) => any) {
    this.onKeyboardEvent = onKeyboardEvent

    document.addEventListener('keydown', this.handleKeyDown)
    document.addEventListener('keyup', this.handleKeyUp)
  }

  public stop(): void {
    document.removeEventListener('keydown', this.handleKeyDown)
    document.removeEventListener('keyup', this.handleKeyUp)
  }

  private getRobotKey(k: string): string | null {
    let key = k.toLocaleLowerCase()

    switch (key) {
      case 'meta':
        return 'command'
      case 'mediaplay':
        return 'audio_play'
      case 'mediapause':
        return 'audio_pause'
      case 'audiovolumemute':
        return 'audio_mute'
      case 'audiovolumeup':
        return 'audio_vol_up'
      case 'audiovolumedown':
        return 'audio_vol_down'
      default:
        break
    }

    if (key.startsWith('arrow')) {
      key = key.slice(5, key.length)
    }

    if (key.length > 1 && robotKeyValues.indexOf(key) === -1) return null
    return key
  }

  private handleKeyDown = (e: KeyboardEvent) => {
    const key = this.getRobotKey(e.key)
    if (!key) {
      console.warn('key value', e.key, 'is not supported')
      return
    }
    this.onKeyboardEvent({
      state: 'down',
      key
    })
  }

  private handleKeyUp = (e: KeyboardEvent) => {
    const key = this.getRobotKey(e.key)
    if (!key) {
      console.warn('key value', e.key, 'is not supported')
      return
    }
    this.onKeyboardEvent({
      state: 'up',
      key
    })
  }
}

import {
  WRCDeviceInfo,
  WRCMouseStateEvent,
  WRCMouseMoveEvent,
  WRCMouseScrollEvent
} from '../types'
import debounce from 'lodash.debounce'

export class MouseState {
  private onMouseEvent!: (event: WRCMouseStateEvent) => any
  private _state: 'down' | 'up' = 'up'
  private _button: 'left' | 'middle' | 'right'

  constructor(button: 'left' | 'middle' | 'right', callback: (e: WRCMouseStateEvent) => any) {
    this.onMouseEvent = callback
    this._button = button
  }

  public get state(): 'down' | 'up' {
    return this._state
  }

  public set state(newState: 'down' | 'up') {
    if (this._state === newState) return
    this._state = newState
    this.onMouseEvent({ button: this._button, down: newState })
  }
}

export class MouseRemoteControl {
  private onMouseMoveEvent: (event: WRCMouseMoveEvent) => any
  private onMouseStateEvent: (event: WRCMouseStateEvent) => any
  private onMouseScrollEvent: (event: WRCMouseScrollEvent) => any

  private element: HTMLVideoElement
  private info: WRCDeviceInfo
  private currentMouseState: {
    left: MouseState
    mid: MouseState
    right: MouseState
  }

  private lastTouchStart: number = 0;

  public constructor(
    element: HTMLVideoElement,
    info: WRCDeviceInfo,
    onMouseMove: (e: WRCMouseMoveEvent) => any,
    onMouseState: (e: WRCMouseStateEvent) => any,
    onMouseScroll: (e: WRCMouseScrollEvent) => any
  ) {
    this.onMouseMoveEvent = onMouseMove
    this.onMouseStateEvent = onMouseState
    this.onMouseScrollEvent = onMouseScroll
    this.element = element
    this.info = info
    this.currentMouseState = {
      left: new MouseState('left', e => this.onMouseStateEvent(e)),
      mid: new MouseState('middle', e => this.onMouseStateEvent(e)),
      right: new MouseState('right', e => this.onMouseStateEvent(e))
    }
    this.init()
  }

  private getCurrentVideoOffsetInfo(): { scale: number; offsetX: number; offsetY: number } {
    const domWidth = this.element.clientWidth
    const domHeight = this.element.clientHeight
    const videoWidth = this.element.videoWidth
    const videoHeight = this.element.videoHeight
    let info = {
      scale: 1,
      offsetX: 0,
      offsetY: 0
    }
    if (videoHeight / videoWidth < domHeight / domWidth) {
      info.scale = videoWidth / domWidth
      info.offsetY = (domHeight - videoHeight / info.scale) / 2
    } else {
      info.scale = videoHeight / domHeight
      info.offsetX = (domWidth - videoWidth / info.scale) / 2
    }
    if (this.info.width / this.info.height !== videoWidth / videoHeight) {
      console.warn('origin resolution is not match current video resolution')
      console.log('screen resolution', this.info.width, this.info.height)
      console.log('video resolution', videoWidth, videoHeight)
    }
    const videoOriginScale = this.info.width / videoWidth
    info.scale = info.scale * videoOriginScale

    return info
  }

  private handleMouseDown = (e: MouseEvent) => {
    e.preventDefault()
    switch (e.buttons) {
      case 1: {
        this.currentMouseState.left.state = 'down'
        break
      }
      case 2: {
        this.currentMouseState.right.state = 'down'
        break
      }
      case 3: {
        this.currentMouseState.left.state = 'down'
        this.currentMouseState.right.state = 'down'
        break
      }
      case 4: {
        this.currentMouseState.mid.state = 'down'
        break
      }
      case 5: {
        this.currentMouseState.left.state = 'down'
        this.currentMouseState.mid.state = 'down'
        break
      }
      case 6: {
        this.currentMouseState.right.state = 'down'
        this.currentMouseState.mid.state = 'down'
        break
      }
      case 7: {
        this.currentMouseState.left.state = 'down'
        this.currentMouseState.right.state = 'down'
        this.currentMouseState.mid.state = 'down'
        break
      }
      default: {
        console.log('unsupport btn')
        return
      }
    }
  }

  private handleMouseUp = (e: MouseEvent) => {
    e.preventDefault()
    switch (e.buttons) {
      case 0: {
        this.currentMouseState.left.state = 'up'
        this.currentMouseState.right.state = 'up'
        this.currentMouseState.mid.state = 'up'
        break
      }
      case 1: {
        this.currentMouseState.right.state = 'up'
        this.currentMouseState.mid.state = 'up'
        break
      }
      case 2: {
        this.currentMouseState.left.state = 'up'
        this.currentMouseState.mid.state = 'up'
        break
      }
      case 3: {
        this.currentMouseState.mid.state = 'up'
        break
      }
      case 4: {
        this.currentMouseState.left.state = 'up'
        this.currentMouseState.right.state = 'up'
        break
      }
      case 5: {
        this.currentMouseState.right.state = 'up'
        break
      }
      case 6: {
        this.currentMouseState.left.state = 'up'
        break
      }
    }
  }

  private handleMouseScroll = (e: WheelEvent) => {
    const deltaMode = e.deltaMode
    if (deltaMode !== 0x00) {
      console.warn('unsupport wheel event')
      return
    }
    const info = this.getCurrentVideoOffsetInfo()

    this.onMouseScrollEvent({ x: e.deltaX * info.scale, y: e.deltaY * info.scale })
  }

  private handleMouseMove = (e: MouseEvent) => {
    const info = this.getCurrentVideoOffsetInfo()
    this.onMouseMoveEvent({
      x: Math.min(this.info.width, Math.max(0, (e.clientX - info.offsetX) * info.scale)),
      y: Math.min(this.info.height, Math.max(0, (e.clientY - info.offsetY) * info.scale))
    })
  }

  private handleTouchStart = (e: TouchEvent) => {
    e.preventDefault();
    const touch = e.touches.item(0);
    if (!touch) return;
    const info = this.getCurrentVideoOffsetInfo();
    const now = Date.now();
    if (now - this.lastTouchStart > 200) {
      this.onMouseMoveEvent({
        x: Math.min(this.info.width, Math.max(0, (touch.clientX - info.offsetX) * info.scale)),
        y: Math.min(this.info.height, Math.max(0, (touch.clientY - info.offsetY) * info.scale)),
      });
    }
    this.lastTouchStart = now;

    this.currentMouseState.left.state = "down";
  }

  private handleTouchMove = (e: TouchEvent) => {
    e.preventDefault();
    const touch = e.touches.item(0);
    if (!touch) return;

    const info = this.getCurrentVideoOffsetInfo();
    this.onMouseMoveEvent({
      x: Math.min(this.info.width, Math.max(0, (touch.clientX - info.offsetX) * info.scale)),
      y: Math.min(this.info.height, Math.max(0, (touch.clientY - info.offsetY) * info.scale)),
    });
  }

  private handleTouchEnd = (e: TouchEvent) => {
    e.preventDefault();

    this.currentMouseState.left.state = "up";
  }

  private init(): void {
    this.element.addEventListener('mousedown', this.handleMouseDown)
    this.element.addEventListener('mouseup', this.handleMouseUp)
    this.element.addEventListener('mousemove', this.handleMouseMove)
    this.element.addEventListener('wheel', this.handleMouseScroll)

    this.element.addEventListener("touchstart", this.handleTouchStart);
    this.element.addEventListener("touchmove", this.handleTouchMove);
    this.element.addEventListener("touchend", this.handleTouchEnd);

    document.addEventListener('contextmenu', event => event.preventDefault())
  }

  public destroy(): void {};
}

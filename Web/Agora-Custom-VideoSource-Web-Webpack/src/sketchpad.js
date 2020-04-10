import * as Sketch from 'sketch-js'

export const colors = [
  'red',
  'blue',
  'green'
]

export const widths = [
  'low',
  'mid',
  'high',
]

export const LINE_WIDTH = [
  1,
  5,
  10
]

const COLOURS = [
  '#FF3333',
  '#0066FF',
  '#33CC66'
]

export default class SketchPad {

  static set activate (val) {
    this._activate = val
  }

  static get activate () {
    return this._activate
  }

  static mount (domID) {
    let pad = new SketchPad()
    this._color = COLOURS[0]
    this.shared = pad.attach(domID)
    this.shared.start()
    this.autoRender()
  }

  static clear() {
    if (this.shared) {
      const canvas = $('.sketch')[0]
      const ctx = canvas.getContext('2d')
      this.shared.clear()
      ctx.fillStyle = '#fff'
      ctx.fillRect(0, 0, canvas.offsetWidth, canvas.offsetHeight)
    }
  }

  static createInterval (cb, elapsed_ms) {
    if (!this._interval) {
      this._interval = setInterval(cb, elapsed_ms) 
    }
  }

  static clearInterval() {
    if (this._interval) {
      clearInterval(this._interval)
      this._interval = null
    }
  }

  static autoRender (flag) {
    if ($('.sketch').length > 0) {
      const renderFrame = (flag) => {
        const canvas = $('.sketch')[0]
        const ctx = canvas.getContext('2d')
        if (flag == false) {
          // use interval for canvas framerate
          this.createInterval(() => {
            ctx.drawImage(canvas, 0, 0)
          }, 50)
        } else {
          // use requestAnimationFrame for canvas framerate
          this.clearInterval()
          requestAnimationFrame(() => {
            ctx.drawImage(canvas, 0, 0)
            renderFrame(flag)
          })
        }
      }
      renderFrame(flag)
    }
  }
  
  constructor () {
    this._radius = 0
  }

  static set color (val) {
    this._color = COLOURS[val]
  }

  static get color () {
    return this._color
  }

  static set width (val) {
    this._width = LINE_WIDTH[val]
  }

  static get width () {
    return this._width
  }

  attach(domID) {
    const touches = []

    return Sketch.create({
      container: document.querySelector(domID),
      autoclear: false,
      fullscreen: false,
      width: 480,
      height: 360,
      id: 'agora_drawing',
      autopause: false,
      retina: false,

      setup ()  {
        // set canvas background color;
        this.fillStyle = '#fff'
        this.fillRect(0, 0, 480, 360)
      },
    
      keydown () {
        if ( this.keys.C ) {
          SketchPad.clear()
        }
      },

      touchstart () {
        this._render = true
      },

      draw () {
      },
  
      touchmove () {
        if (this._render) {  
          for ( let i = this.touches.length - 1, touch; i >= 0; i-- ) {
            touch = this.touches[i]
    
            this.lineCap = 'round'
            this.lineJoin = 'round'
            this.fillStyle = this.strokeStyle = SketchPad.color
            this.lineWidth = SketchPad.width
    
            this.beginPath()
            this.moveTo( touch.ox, touch.oy )
            this.lineTo( touch.x, touch.y )
            this.stroke()
          }
        }
      },

      touchend () {
        this._render = false
      }
    })
  }
}

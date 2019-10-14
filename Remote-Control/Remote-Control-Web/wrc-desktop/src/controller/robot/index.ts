const os = require('os');

export default class RobotCore {
  private isLeftButtonDown: boolean;
  private robot: any;
  public constructor(robot: any) {
    this.robot = robot;
    this.isLeftButtonDown = false;
    robot.setMouseDelay(0);
  }

  public getScreenSize(): { width: number; height: number } {
    return this.robot.getScreenSize();
  }

  public getPlatform(): NodeJS.Platform {
    return os.platform();
  }

  public mouseMove(x: number, y: number): void {
    if (this.isLeftButtonDown) {
      this.robot.dragMouse(x, y);
    } else {
      this.robot.moveMouse(x, y);
    }
  }

  public toggleMouseState(button: 'left' | 'middle' | 'right', state: 'up' | 'down') {
    this.robot.mouseToggle(state, button);
    if (button === 'left') {
      if (state === 'up') {
        this.isLeftButtonDown = false;
      } else {
        this.isLeftButtonDown = true;
      }
    }
  }

  public mouseScroll(x: number, y: number): void {
    setImmediate(() => {
      this.robot.scrollMouse(x, y);
    })
  }

  public toggleKey(key: string, state: 'down' | 'up'): void {
    try {
      this.robot.keyToggle(key, state);
    } catch (e) {
      console.log("key toggle error", e);
    }
  }
}

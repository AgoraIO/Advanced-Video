export default function renderCanvas(actived, timers) {
  const theCanvas = document.getElementById('myCanvas');
  const ctx = theCanvas.getContext('2d');
  const mix = 6000;
  let canvas_width = 480;
  let canvas_height = 360;
  let points = [];
  //  requestAnimationFrame
  function draw() {
    ctx.clearRect(0, 0, canvas_width, canvas_height);
    let i, pi, x_dist, y_dist;
    points.forEach((p, index) => {
      p.x += p.xa,
        p.y += p.ya,
        p.xa *= p.x > canvas_width || p.x < 0 ? -1 : 1,
        p.ya *= p.y > canvas_height || p.y < 0 ? -1 : 1,
        ctx.fillRect(p.x - 0.5, p.y - 0.5, 1, 1);
      for (i = index + 1; i < points.length; i++) {
        pi = points[i];
        x_dist = p.x - pi.x;
        y_dist = p.y - pi.y;
        let dist = x_dist * x_dist + y_dist * y_dist;
        if (dist < mix) {
          ctx.beginPath();
          ctx.moveTo(p.x, p.y);
          ctx.lineTo(pi.x, pi.y);
          ctx.strokeStyle = 'green';
          ctx.stroke();
        }
      }
    });

    var AnimationFrame = function (state, callback) {
      if (state) {
        timers.forEach(timer => clearTimeout(timer));
        requestAnimationFrame(callback);
      } else {
        timers.push(setTimeout(callback, 50));
      }
    }

    AnimationFrame(actived, function () {
      draw();
    });
  }
  for (let i = 0; i < 100; i++) {
    let x = Math.random() * canvas_width,
      y = Math.random() * canvas_height,
      xa = 2 * Math.random() - 1,
      ya = 2 * Math.random() - 1;
    points[i] = {
      x,
      y,
      xa,
      ya
    };
  }
  draw();
}
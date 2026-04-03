const SCR_WIDTH = 400;
const SCR_HEIGHT = 300;

const CANVAS = document.getElementById("canvas");
const CTX = CANVAS.getContext("2d");

CTX.canvas.width = SCR_WIDTH;
CTX.canvas.height = SCR_HEIGHT;


const TILE_MAP = [
  [1, 1, 1, 1, 1, 1, 1, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 1, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 1, 1, 1, 1, 1, 1, 1]
];

const MAP_SIZE = 8;
const TILE_SIZE = 32;

function loop(time) {
  CTX.clearRect(0, 0, SCR_WIDTH, SCR_HEIGHT);

  CTX.fillStyle = "red";
  for (let row = 0; row < MAP_SIZE; ++row) {
    for (let col = 0; col < MAP_SIZE; ++col) {
      if (TILE_MAP[row][col] == 1) CTX.fillRect(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }
  }

  requestAnimationFrame(loop);
}

requestAnimationFrame(loop);

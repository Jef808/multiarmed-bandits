export function assert(condition: boolean, message: string): void {
  if (!condition) {
    throw new Error("Assertion failed: " + (message || ""));
  }
}

export function smoothen(points: Point[], ratio: number): Point[] {
  assert(ratio > 0.0001 && ratio < 0.999, `ratio out of bounds: ${ratio}`);
  assert(
    Boolean(points) && points.length > 1,
    "smoothen needs points array of length at least 2"
  );

  const nbSteps = Math.ceil(points.length * ratio);
  const stepSize = getStepSize(points.length, nbSteps);
  const intervals: Interval[] = getIntervals(points.length, stepSize);

  const intervalsIsValid = intervals.every(
    (inter) =>
      Boolean(inter) && inter.beg >= 0 && inter.end <= points.length - 1
  );

  console.log("Intervals:", intervals);
  assert(intervalsIsValid, "Failed to build intervals array");

  return Array.from(intervals, (inter) => {
    let values = points.map((p) => p.y).slice(inter.beg, inter.end + 1);
    assert(Boolean(values), "map.slice values failed");
    return {
      x: inter.beg,
      y: values.reduce((a, b) => a + b, 0) / (inter.end - inter.beg),
    };
  });
}

type Interval = {
  beg: number;
  end: number;
};

type Point = {
  x: number;
  y: number;
};

function getStepSize(arrLength: number, nbSteps: number) {
  assert(nbSteps > 0, "nbSteps should be at least 1");

  return arrLength / nbSteps;
}

function getNbSteps(arrLength: number, stepSize: number) {
  assert(stepSize > 0, "invalid step size"),
    assert(arrLength > 0, "Empty array");

  return arrLength / stepSize;
}

const getIntervals = function(
  pointsLength: number,
  stepSize: number
): NonNullable<Interval[]> {
  const nbSteps: number = getNbSteps(pointsLength, stepSize);
  const xInters: number[] = Array.from(
    { length: nbSteps },
    (v, i) => i * stepSize
  );
  assert(
    Boolean(xInters),
    "Array.from() trick failed in creation of intervals' index array"
  );
  //assert(xInters.length && xInters.length > 0, "xInters empty");
  console.log("xInters:", xInters);
  return xInters.map((x) => {
    return {
      beg: x,
      end: Math.min(pointsLength - 1, x + stepSize),
    };
  });
};

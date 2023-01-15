
class MAB {
  constructor(numberOfArms = 10, sampleNoiseStdDev = 1) {
    this.numberOfArms = numberOfArms;
    this.sampleNoiseStdDev = sampleNoiseStdDev;
  }
}

export class AgentBase {
  constructor(episodes = 500, steps = 500, stepsize = 10) {
    this.episodes = episodes;
    this.steps = steps;
    this.stepsize = stepsize;
  }
}

export class Ucb extends AgentBase {
  static name = "ucb";

  constructor(episodes, steps, stepsize, explorationCst = 0.4) {
    super(episodes, steps, stepsize);
    this.explorationCst = explorationCst;
  }
}

export class EpsGreedy extends AgentBase {
  static name = "epsGreedy";

  constructor(episodes, steps, stepsize, epsilon = 0.1) {
    super(episodes, steps, stepsize);
    this.epsilon = epsilon;
  }
}

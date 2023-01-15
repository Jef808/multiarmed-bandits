import * as Config from './objects'

function validateSampleRequest(body) {
  const success = true;
  const message = "";

  success &&= ["serverUrl"].contains(body.action)
    || message.concat("No URL for end target was found\n");

  success &&= ["sample"].contains(body.action)
    || message.concat("No valid action found\n");

  success &&= ["mab"].contains(body.name)
    || message.concat("No valid model found\n");

  success &&= ["ucb", "epsGreedy"].contains(body.name)
    || message.concat("No valid agent found\n");

  return { success, message };
}


export class PostSampleRequest {
  constructor(serverUrl, action, model, agent) {
    this.serverUrl = serverUrl;
    this.action = action;
    this.model = model;
    this.agent = agent;
  }

  makeRequest() {
    const body = JSON.stringify(this);

    const { success, message } = validateSampleRequest(body);

    if (!success) {
      throw "Invalid request parsed:\n" + message;
    }

    return body
  }
}

export class GetSampleRequest {
  constructor(body) {
    this.body = body;
  }

  prepareForwarding() {
    const { success, message } = validateSampleRequest(body);

    if (!success) {
      throw "Invalid request parsed:\n" + message;
    }

    return new PostSampleRequest(body.serverUrl, body.action, body.model, body.agent);
  }
}

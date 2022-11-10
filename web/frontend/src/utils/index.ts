// interface PollArgs {

//   validate: (r: R) => {};
//   interval: number;
//   maxAttempts: number;
// }

export function poll({ fn, validate, interval, maxAttempts }: { fn: () => any, validate: () => boolean, interval: number, maxAttempts: number }) {
  let attempts = 0;

  const executePoll = async (resolve: (a: any) => R, reject: (e: Error) => void) {
    const result = await fn();
    ++attempts;

    if (validate(result)) {
      return resolve(result);
    }
    else if (maxAttempts && attempts === maxAttempts) {
      return reject(new Error('Exceeded max attempts'));
    }
    else {
      setTimeout(executePoll, interval, resolve, reject);
    }
  };

  return new Promise(executePoll);
}

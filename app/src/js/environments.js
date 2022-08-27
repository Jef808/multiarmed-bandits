export default function () {
  return [
    {
      id: "mab",
      label: "Multi Armed Bandit",
      value: {
        params: [
          {
            id: "actions",
            label: "number of actions",
            default: 10,
            type: Number,
          },
          {
            id: "noiseMean",
            label: "sample noise mean",
            default: 0.0,
            type: Number,
          },
          {
            id: "noiseStdDev",
            label: "sample noise standard deviation",
            default: 1.0,
            type: Number,
          },
        ],
      },
    },
  ];
}

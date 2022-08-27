export default function () {
  return [
    {
      id: "random",
      label: "Random",
      value: {
        params: [],
      },
    },
    {
      id: "eg",
      label: "Epsilon Greedy",
      value: {
        params: [
          { id: "epsilon", label: "epsilon", default: 0.1, type: Number },
        ],
      },
    },
    {
      id: "ucb",
      label: "UCB",
      value: {
        params: [
          {
            id: "exploration",
            label: "exploration constant",
            default: 0.4,
            type: Number,
          },
        ],
      },
    },
  ];
}

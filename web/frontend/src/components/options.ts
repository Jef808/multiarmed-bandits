export function models() {

    return [
        {
            category: "Model",
            name: "multiarmedBandit",
            label: "Multiarmed Bandit",
            parameters: [
                {
                    name: "numberOfArms",
                    label: "Number Of Arms",
                    value: 2,
                    min: 1,
                },
            ],
        },
    ];
};

export function policies() {
    return [
        {
            category: "Policy",
            name: "epsilonGreedy",
            label: "Epsilon Greedy",
            parameters: [
                {
                    name: "epsilon",
                    label: "Epsilon",
                    value: 0.1,
                    min: 0.0,
                    max: 1.0,
                    steps: 0.05,
                },
            ],
        },
        {
            category: "Policy",
            name: "ucb",
            label: "Ucb",
            parameters: [
                {
                    name: "exploration",
                    label: "Exploration",
                    value: 0.7,
                    min: 0.0,
                    steps: 0.05,
                },
            ],
        },
    ];
}

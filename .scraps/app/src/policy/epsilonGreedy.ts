export default function(epsilon: number) {
    return {
        name: "epsilonGreedy",
        parameters: {
            epsilon: epsilon,
        },
    };
}

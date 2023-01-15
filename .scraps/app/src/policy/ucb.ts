export default function(exploration: number) {
    return {
        name: "ucb",
        parameters: {
            exploration: exploration,
        },
    };
}

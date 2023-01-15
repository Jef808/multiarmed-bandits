import axios from "axios";
import type { Method, AxiosInstance } from "axios";

axios.defaults.headers.common["Content-Type"] = "application/json";

const axiosInstance = axios.create({
    baseURL: "http://127.0.0.1:3000",
    withCredentials: false, // Default
    transformRequest: [
        function(data) {
            return JSON.stringify(data);
        },
    ],
    headers: {
        Accept: "application/json",
    },
});

export default function(url: string) {
    axiosInstance({
        url: url,
        data: {
            model: {
                name: "mab",
                parameters: {
                    numberOfArms: 10,
                },
            },

            status: 1,
            message: "Hello",
        },
    })
        .then(function(response) {
            console.log(response.data);
        })
        .catch(function(error) {
            console.log("Error, ", error.toJSON());
        });
}

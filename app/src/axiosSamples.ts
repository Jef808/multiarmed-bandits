import axios from 'axios';
import type { Method, AxiosInstance } from 'axios';

axios.defaults.headers.common['Content-Type'] = 'application/json';

const axiosInstance = axios.create({
    baseURL: 'http://localhost:3000',
    withCredentials: false,  // Default
    transformRequest: [function(data, headers) {
        return JSON.stringify(data);
    },],
    headers: {
        'Accept': 'application/json',
    },
});


export default function(url: string, method: Method = 'get') {
    axiosInstance({
        url: url,
        method: method,
        data: {
            status: 1,
            message: "Hello",
        },
    }).then(function(response) {
        console.log(response.data);
    }).catch(function(error) {
        console.log("Error, ", error.toJSON());
    });
}

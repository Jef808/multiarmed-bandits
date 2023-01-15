// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getFirestore } from 'firebase/firestore'

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional

// Initialize Firebase
const firebase = initializeApp({
    apiKey: "AIzaSyCnQGe-vNF1ZR4f992NF91V7YTlQQSj_2Y",
    authDomain: "rfview-fa298.firebaseapp.com",
    projectId: "rfview-fa298",
    storageBucket: "rfview-fa298.appspot.com",
    messagingSenderId: "865069080414",
    appId: "1:865069080414:web:a4d5605e369a9cb90879cf",
    measurementId: "G-B2V1TPHB0G"
});

export const db = getFirestore(firebase)

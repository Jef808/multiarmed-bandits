/** @type {import('tailwindcss').Config} */
module.exports = {
  darkMode: false,
  content: ['./index.html',
    './src/**/*.{vue,js,ts}',
    './node_modules/flowbite-vue/**/*.{js,jsx,ts,tsx}'],
  theme: {
    extend: {},
  },
  plugins: [
    require('flowbite/plugin')youtuf
  ],
}

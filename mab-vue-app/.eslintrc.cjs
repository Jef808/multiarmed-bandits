module.exports = {
    "root": true,
    "env": {
        "browser": true,
        "es2021": true,
        "node": true
    },
    "extends": [
        "plugin:vue/vue3-recommended",
        "eslint:recommended",
        "@vue/typescript/recommended"
        "plugin:@typescript-eslint/recommended",
        "plugin:@typescript-eslint/recommended-requiring-type-checking",
    ],
    ignorePatterns: [
        "node_modules/",
        "dist/",
        "build/",
    ],
    "parser": "@typescript-eslint/parser",
    "parserOptions": {
        "ecmaVersion": "latest",
        "sourceType": "module"
    },
    "plugins": [ "@typescript-eslint" ],
        "vue/comment-directive":
        [
            "error", { "reportUnusedDisableDirectives": false }
        ],
        "plugin:typescript-eslint"
    ],
    "rules": {
    }
}

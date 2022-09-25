import express from 'express'

const router = express.Router();

/* GET users listing. */
router.get('/', function (req, res, next) {
  res.json({ status: true, message: "app is working!" });
});



export default router;

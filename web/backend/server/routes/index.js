import express, { json } from 'express';
import items from './../../scripts/items';

const router = express.Router();

/* GET home page. */
router.get('/', function (req, res, next) {
  res.json({ status: true, message: "app is working!" });
});

router.get('/items', function (req, res, next) {
  res.json({ status: true, message: "Fetched all items", data: items })
})

export default router;

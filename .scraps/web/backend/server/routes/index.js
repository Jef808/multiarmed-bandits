import express, { json } from 'express';
import morgan from 'morgan';
import axios from 'axios';

import * as SampleAPIRequest from './../MAB/sampleRequest';

const router = express.Router();
router.use(express.json());
router.use(morgan('dev'));


router.get('/', function (req, res, next) {
  console.log(req);

  res.json({ status: true, message: "World!" });

  res.send();
});




router.post('/', function (req, res, next) {
  console.log(req);

  res.json({ status: true, message: "World!" });

  res.send();
});



router.get('/mab', function (req, res) {
  morgan(req.body);


  try {
    const sampleReq = new SampleAPIRequest.GetSampleRequest(req.body);
    const forwardedReq = SampleAPIRequest.getSampleReq.prepareForwarding();
  }
  catch (e) {
    console.debug('Error: ', e);
  }


  axios.get({
    url: forwardedReq.serverUrl,
    data: forwardedReq,
  })
    .then(function (response) {

      console.log("Success, ", response.data);
      res.json().send(response.data);

    })
    .catch(function (error) {

      console.log("Error, ", error.toJSON());

    })
    .then(function () {

      console.log()

    });
})

export default router;

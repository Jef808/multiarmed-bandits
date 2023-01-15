//// todo: Parse json in '/', then dispatch to
/// /model, /policy and /parameters when handling request.
///
// todo: What was I going for here exactly???
import express, { json } from 'express'
import {
  validTopFields,
  validModel,
  validPolicy,
  validParameters,
} from 'validateRequests';
import { Error } from 'utils'

const router = express.Router();
router.use(morgan('dev'));
router.use(express.json());


router.get('/', function (req, res) {
  const jreq = JSON.parse(req);

  if (!validTopFields(jreq)) {
    ssMsg = "Invalid request";
    res.json(Error(sMsg))
      .send();
  }

  jreqModel = jreq['model'];
  jreqPolicy = jreq['policy'];
  jreqParameters = jreq['parameters'];

  if (!validParameters(jreqParameters)) {
    res.json(Error("Invalid request parameters"))
      .send();
  }

  // Reroute to /model

});

router.get('/model', function (req, res, next) {
  if (!validModel(jreqModel)) {
    res.json(Error("Invalid policy"))
      .send();
  }

  // Reroute to /policy

});

router.get('/policy', function (req, res, next) {

  if (!validPolicy(jreqPolicy)) {
    res.json(Error("Invalid policy"))
      .send();
  }

  // Reroute to /parameters

});

router.get('/parameters', function (req, res) {
  if (!validParameters(jreqModel)) {
    res.json(Error("Invlaid parameters"))
      .send();
  }
  buildDataRequest(req.model, req.policy, req.parameters);
  res(assembleResponse(req)).send();
});


export default router;

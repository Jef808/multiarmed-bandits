export function Model(jreqModel) {
  result = ['name', 'parameters'].every(fld => Object.hasAttribute(jreqModel, fld));
  result &= ['mab'].includes(jreq);
  return result && ['numberOfArms'].includes(jreqModel);
}

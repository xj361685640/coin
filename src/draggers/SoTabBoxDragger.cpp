/**************************************************************************\
 *
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class SoTabBoxDragger SoTabBoxDragger.h Inventor/draggers/SoTabBoxDragger.h
  \brief The SoTabBoxDragger class is a box dragger you can translate and scale. 
  \ingroup draggers
  
  FIXME: write class doc
*/

#include <Inventor/draggers/SoTabBoxDragger.h>
#include <Inventor/draggers/SoTabPlaneDragger.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <coindefs.h> // COIN_STUB()


SO_KIT_SOURCE(SoTabBoxDragger);


void
SoTabBoxDragger::initClass(void)
{
  SO_KIT_INTERNAL_INIT_CLASS(SoTabBoxDragger);
}

SoTabBoxDragger::SoTabBoxDragger(void)
{
  SO_KIT_INTERNAL_CONSTRUCTOR(SoTabBoxDragger);

  SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, TRUE, topSeparator, tabPlane1Sep, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane1Sep, SoSeparator, FALSE, topSeparator, tabPlane2Sep, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane1Xf, SoTransform, TRUE, tabPlane1Sep, tabPlane1, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane1, SoTabPlaneDragger, TRUE, tabPlane1Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane2Sep, SoSeparator, FALSE, topSeparator, tabPlane3Sep, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane2Xf, SoTransform, TRUE, tabPlane2Sep, tabPlane2, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane2, SoTabPlaneDragger, TRUE, tabPlane2Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane3Sep, SoSeparator, FALSE, topSeparator, tabPlane4Sep, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane3Xf, SoTransform, TRUE, tabPlane3Sep, tabPlane3, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane3, SoTabPlaneDragger, TRUE, tabPlane3Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane4Sep, SoSeparator, FALSE, topSeparator, tabPlane5Sep, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane4Xf, SoTransform, TRUE, tabPlane4Sep, tabPlane4, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane4, SoTabPlaneDragger, TRUE, tabPlane4Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane5Sep, SoSeparator, FALSE, topSeparator, tabPlane6Sep, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane5Xf, SoTransform, TRUE, tabPlane5Sep, tabPlane5, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane5, SoTabPlaneDragger, TRUE, tabPlane5Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane6Sep, SoSeparator, FALSE, topSeparator, geomSeparator, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane6Xf, SoTransform, TRUE, tabPlane6Sep, tabPlane6, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(tabPlane6, SoTabPlaneDragger, TRUE, tabPlane6Sep, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(boxGeom, SoSeparator, TRUE, geomSeparator, "", TRUE);

  if (SO_KIT_IS_FIRST_INSTANCE()) {
    SoInteractionKit::readDefaultParts("tabBoxDragger.iv", NULL, 0);
  }

  SO_NODE_ADD_FIELD(translation, (0.0f, 0.0f, 0.0f));
  SO_NODE_ADD_FIELD(scaleFactor, (1.0f, 1.0f, 1.0f));

  SO_KIT_INIT_INSTANCE();

  // create subdraggers
  SO_GET_ANY_PART(this, "tabPlane1", SoTabPlaneDragger);
  SO_GET_ANY_PART(this, "tabPlane2", SoTabPlaneDragger);
  SO_GET_ANY_PART(this, "tabPlane3", SoTabPlaneDragger);
  SO_GET_ANY_PART(this, "tabPlane4", SoTabPlaneDragger);
  SO_GET_ANY_PART(this, "tabPlane5", SoTabPlaneDragger);
  SO_GET_ANY_PART(this, "tabPlane6", SoTabPlaneDragger);

  this->setPartAsDefault("boxGeom", "tabBoxBoxGeom");

  this->initTransformNodes();
  
  this->addValueChangedCallback(SoTabBoxDragger::valueChangedCB);
  
  this->scaleFieldSensor = new SoFieldSensor(SoTabBoxDragger::fieldSensorCB, this);
  this->translFieldSensor = new SoFieldSensor(SoTabBoxDragger::fieldSensorCB, this);
  this->setUpConnections(TRUE, TRUE);
}



SoTabBoxDragger::~SoTabBoxDragger()
{
  delete this->scaleFieldSensor;
  delete this->translFieldSensor;
}

SbBool
SoTabBoxDragger::setUpConnections(SbBool onoff, SbBool doitalways)
{
  if (!doitalways && this->connectionsSetUp == onoff) return onoff;

  SbString str;
  if (onoff) {
    inherited::setUpConnections(onoff, doitalways);
    SoDragger *child;
    for (int i = 1; i <= 6; i++) {
      str.sprintf("tabPlane%d", i);
      child = (SoDragger*)this->getAnyPart(str.getString(), FALSE);
      child->setPartAsDefault("translator", "tabBoxTranslator");
      child->setPartAsDefault("scaleTabMaterial", "tabBoxScaleTabMaterial");
      child->setPartAsDefault("scaleTabHints", "tabBoxScaleTabHints");
      child->addStartCallback(SoTabBoxDragger::invalidateSurroundScaleCB, this);
      child->addFinishCallback(SoTabBoxDragger::invalidateSurroundScaleCB, this);
      this->registerChildDragger(child);
    } 
    
    if (this->translFieldSensor->getAttachedField() != &this->translation) {
      this->translFieldSensor->attach(&this->translation);
    }
    if (this->scaleFieldSensor->getAttachedField() != &this->scaleFactor) {
      this->scaleFieldSensor->attach(&this->scaleFactor);
    }
  }
  else {
    SoDragger *child;
    for (int i = 1; i <= 6; i++) {
      str.sprintf("tabPlane%d", i);
      child = (SoDragger*)this->getAnyPart(str.getString(), FALSE);
      child->removeStartCallback(SoTabBoxDragger::invalidateSurroundScaleCB, this);
      child->removeFinishCallback(SoTabBoxDragger::invalidateSurroundScaleCB, this);
      this->unregisterChildDragger(child);      
    }

    if (this->translFieldSensor->getAttachedField() != NULL) {
      this->translFieldSensor->detach();
    }
    if (this->scaleFieldSensor->getAttachedField() != NULL) {
      this->scaleFieldSensor->detach();
    }
    inherited::setUpConnections(onoff, doitalways);
  }
  return !(this->connectionsSetUp = onoff);
}

void
SoTabBoxDragger::setDefaultOnNonWritingFields(void)
{
  COIN_STUB();
  inherited::setDefaultOnNonWritingFields();
}

void
SoTabBoxDragger::fieldSensorCB(void * d, SoSensor *)
{
  SoTabBoxDragger *thisp = (SoTabBoxDragger*)d;
  SbMatrix matrix = thisp->getMotionMatrix();
  thisp->workFieldsIntoTransform(matrix);
  thisp->setMotionMatrix(matrix);
}

void
SoTabBoxDragger::valueChangedCB(void *, SoDragger *d)
{
  SoTabBoxDragger *thisp = (SoTabBoxDragger*)d;
  
  const SbMatrix &matrix = thisp->getMotionMatrix();
  SbVec3f t, s;
  SbRotation r, so;
  matrix.getTransform(t, r, s, so);
  
  thisp->translFieldSensor->detach();
  if (thisp->translation.getValue() != t) {
    thisp->translation = t;
  }
  thisp->translFieldSensor->attach(&thisp->translation);
  
  thisp->scaleFieldSensor->detach();
  if (thisp->scaleFactor.getValue() != s) {
    thisp->scaleFactor = s;
  }
  thisp->scaleFieldSensor->attach(&thisp->scaleFactor);
}

/*!
  \e surroundScale is invalidated every time a child dragger is 
  activated/deactivated using this callback
*/
void
SoTabBoxDragger::invalidateSurroundScaleCB(void * d, SoDragger *)
{
  SoTabBoxDragger *thisp = (SoTabBoxDragger*)d;
  SoSurroundScale *ss = SO_CHECK_PART(thisp, "surrounddScale", SoSurroundScale);
  if (ss) ss->invalidate();
}

void 
SoTabBoxDragger::initTransformNodes(void)
{
  SoTransform *tf;
  tf = SO_GET_ANY_PART(this, "tabPlane1Xf", SoTransform);
  tf->translation = SbVec3f(0.0f, 0.0f, 1.0f);
  tf = SO_GET_ANY_PART(this, "tabPlane2Xf", SoTransform);
  tf->translation = SbVec3f(0.0f, 0.0f, -1.0f);
  tf->rotation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), M_PI);
  tf = SO_GET_ANY_PART(this, "tabPlane3Xf", SoTransform);
  tf->translation = SbVec3f(1.0f, 0.0f, 0.0f);
  tf->rotation = SbRotation(SbVec3f(0.0f, 1.0f, 0.0f), M_PI*0.5f);
  tf = SO_GET_ANY_PART(this, "tabPlane4Xf", SoTransform);
  tf->translation = SbVec3f(-1.0f, 0.0f, 0.0f);
  tf->rotation = SbRotation(SbVec3f(0.0f, 1.0f, 0.0f), -M_PI*0.5f);
  tf = SO_GET_ANY_PART(this, "tabPlane5Xf", SoTransform);
  tf->translation = SbVec3f(0.0f, 1.0f, 0.0f);
  tf->rotation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), -M_PI*0.5f);
  tf = SO_GET_ANY_PART(this, "tabPlane6Xf", SoTransform);
  tf->translation = SbVec3f(0.0f, -1.0f, 0.0f);
  tf->rotation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), M_PI*0.5f);
}


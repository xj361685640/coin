#ifndef COIN_SCXMLONENTRYELT_H
#define COIN_SCXMLONENTRYELT_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2008 by Kongsberg SIM.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Kongsberg SIM about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Kongsberg SIM, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

#include <Inventor/scxml/ScXMLElt.h>
#include <Inventor/tools/SbPimplPtr.h>

class ScXMLExecutableElt;
class ScXMLEvent;
class ScXMLStateMachine;

class COIN_DLL_API ScXMLOnEntryElt : public ScXMLElt {
  typedef ScXMLElt inherited;
  SCXML_ELEMENT_HEADER(ScXMLOnEntryElt);

public:
  static void initClass(void);
  static void cleanClass(void);

  ScXMLOnEntryElt(void);
  virtual ~ScXMLOnEntryElt(void);

  // executable content
  virtual int getNumExecutables(void) const;
  virtual ScXMLExecutableElt * getExecutable(int idx) const;
  virtual void addExecutable(ScXMLExecutableElt * exectable);
  virtual void removeExecutable(ScXMLExecutableElt * exectable);
  virtual void clearAllExecutables(void);

  virtual void copyContents(const ScXMLElt * rhs);

  virtual const ScXMLElt * search(const char * attrname, const char * attrvalue) const;

  virtual void execute(ScXMLStateMachine * statemachine) const;

private:
  ScXMLOnEntryElt(const ScXMLOnEntryElt & rhs); // N/A
  ScXMLOnEntryElt & operator = (const ScXMLOnEntryElt & rhs); // N/A

  class PImpl;
  SbPimplPtr<PImpl> pimpl;

}; // ScXMLOnEntryElt

#endif // !COIN_SCXMLONENTRYELT_H
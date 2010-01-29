/*
* Copyright (c) 2010 Ixonos Plc.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - Initial contribution
*
* Contributors:
* Ixonos Plc
*
* Description: 
* Represents an N-bit bit field.
*
*/


#ifndef BITFIELD_H
#define BITFIELD_H

#include <e32std.h>

/**  Class name */
_LIT(KClassName, "BitField");

/**  Max size */
const TUint32 KSize = 32;


/**
 *  Represents an N-bit bit field
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ImageEditorUtils.lib
 *  @since S60 v5.0
 */
class TBitField 
{

public:


  /** : Should be changed to private and NewL used */
	IMPORT_C TBitField ();

  /**
  * Destructor.
  */
	IMPORT_C ~TBitField ();

  /**
  * Copy constructor, disabled
  *
  * @since S60 v5.0
  * @param rhs reference to TBitField
  * @return -
  */
	IMPORT_C TBitField (const TBitField & rhs);

	/** 
	* Assignment operator, disabled 
	*
  * @since S60 v5.0
	*	@param rhs reference to TBitField
	*	@return reference to the copied TBitField 
	*/
	IMPORT_C TBitField & operator= (const TBitField & rhs);

	/** 
	* Reset
	*
  * @since S60 v5.0
	*	@param -
	*	@return - 
	*/
	IMPORT_C void Reset ();

	/** 
	* Set bit
	*
  * @since S60 v5.0
	*	@param aIndex bit field position
	*	@return - 
	*/
	IMPORT_C void SetBit (const TUint32 aIndex);

	/** 
	* Get bit
	*
  * @since S60 v5.0
	*	@param aIndex bit field position
	*	@return bit value in TInt
	*/
	IMPORT_C TInt GetBit (const TUint32 aIndex) const;


protected:

private:

 /**
  * Data
  */
	TUint8			iData[(KSize >> 3) + 1];

};

#endif // BITFIELD_H

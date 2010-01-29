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
*
*/



#ifndef EDITORIMAGE_H
#define EDITORIMAGE_H

//  INCLUDES
#include <e32base.h>
#include <fbs.h>

//  FORWARD DECLARATIONS


/*	CLASS: CEditorImage
*
*	CEditorImage class represents an image in Image Editor. The image can
*	be loaded with RAJPEG engine (JPEG) or Symbian MMF (other formats).
*	When image is loaded with RAJPEG, the image is represented as file name.
*	When image is loaded with MMF, the image is represented as CFbsBitmap.
*/
class CEditorImage : public CBase
{

public:


/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CEditorImage object
	*/
	static CEditorImage * NewL ();

	/** NewLC factory method, does not pop cleanupstack
	*
	*	@param -
	*	@return pointer to created CEditorImage object
	*/
	static CEditorImage * NewLC ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CEditorImage ();

	/** GetPreviewData
	*
	*	@param -
	*	@return pointer to preview bitmap
	*/
	CFbsBitmap *& GetPreviewData();

	/** GetFullData
	*
	*	@param -
	*	@return pointer to full sized bitmap
	*/
	CFbsBitmap *& GetFullData();

    /** GetImageName
	*
	*	@param -
	*	@return pointer to bitmap
	*/
	TFileName & GetImageName();

	/** LockHeapLC
	*
	*	Locks bitmap heap
	*
	*	@param -
	*	@return -
	*/
	void LockHeapLC() const;

	/** LockHeapL
	*
	*	Releases bitmap heap.
	*
	*	@param -
	*	@return -
	*/
	void ReleaseHeap() const;


//@}

protected:

/** @name Methods:*/
//@{

//@}

/** @name Members:*/
//@{

//@}

private:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CEditorImage ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();
//@}

/** @name Members:*/
//@{
	/// Preview
	CFbsBitmap *		iPreview;
	/// Full 
	CFbsBitmap *		iFull;
	/// File name
	TFileName 			iFileName;
//@}

};


#endif

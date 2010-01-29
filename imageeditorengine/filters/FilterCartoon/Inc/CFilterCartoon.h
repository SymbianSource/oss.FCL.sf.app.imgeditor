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


#ifndef __CFilterCartoon_H__
#define __CFilterCartoon_H__

#include <e32base.h>
#include "MImageFilter.h"

typedef RArray<TUint32> RPalette;

// Class: Octree node
//=============================================================================
class CNode : public CBase
{

public:

    CNode();
    virtual ~CNode();

public:
    /// If ETrue, has no leaves
    TBool   iIsLeaf;
    /// Number of pixels represented by this leaf
    TInt    iPixels;
    /// Sum of red components
    TInt    iRedSum;
    /// Sum of green components
    TInt    iGreenSum;
    /// Sum of blue components
    TInt    iBlueSum;
    /// Children
    CNode * iChild[8];
    /// Next reducible node
    CNode * iNext;
};
    
// Class: Octree 
//=============================================================================
class COTreeQuant : public CBase 
{

public:

    COTreeQuant();
    virtual ~COTreeQuant();

    void AddColorL (TUint32 aRgb);
    TInt GetColorCount() const;
    void GetColorTable ();
    void ReduceTree ();

public:

    /// Amount of leaves
    TInt        iLeaves;
    /// Tree struct
    CNode *     iTree;
    /// Reducible nodes
    CNode *     iReducibleNodes[9];
    /// Palette
    RPalette    iPalette;
    
private:

    void DoAddColorL (CNode ** aNode, TUint32 aRgb, TInt aLevel);
    void GetPaletteColors (const CNode & aNode);

};

// Class: Cartoon filter
//=============================================================================
class CFilterCartoon
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterCartoon();
	
	public: // MImageFilter
		virtual TRect Rect();
        virtual TReal Scale();
        virtual TSize ViewPortSize();
		virtual TBlock * GetBlockL ( const TRect & aRect );
		virtual void SetParent ( MImageFilter * aParent );
		virtual void SetChild ( MImageFilter * aChild );
		virtual TInt CmdL( const TDesC16& aCmd );
		virtual const char* Type();

    private:
		static CFilterCartoon* NewL();
		CFilterCartoon();
		void ConstructL();
        void CreateOctreePaletteL();

    private: // data     

		TInt*		i16BitIndLut;
		TUint32*	iRGB;
		TInt		iCount;
	};

#endif

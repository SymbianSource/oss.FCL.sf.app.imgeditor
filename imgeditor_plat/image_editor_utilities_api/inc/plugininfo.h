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
* CPluginInfo class represents information about editor plug-in.
*
*/


#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <e32base.h>
#include <badesca.h>
#include <eikmenup.h>

/**  Array for menu items */
typedef CArrayFixSeg<CEikMenuPaneItem::SData> CMenuItemArray;


class CFbsBitmap;
class RReadStream;
class RWriteStream;

/**
 *  Represents information about editor plug-in.
 *	The class encapsulates the following info:
 *
 *	PLUG-IN DLL INFO:
 *
 *		plug-in DLL name 
 *		plug-in DLL UID 2 (API UID)
 *		plug-in DLL UID 3
 *		plug-in UI type
 *		plug-in filter type
 *
 *	PLUG-IN UI INFO:
 *
 *    plug-in run-time ID
 *		plug-in name 
 *		icon file name
 *       icon bitmap
 *       icon mask bitmap
 *		parameter array
 *		softkey 1 items
 *		softkey 2 items
 *		menu items
 *  
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ImageEditorUtils.lib
 *  @since S60 v5.0
 */
class CPluginInfo :	public CBase
{

public:

    /**
     * Comparison method for CPluginInfo instances based on RID
     *
     * @since S60 v5.0
     * @param aItem1 - first item to be compared
     * @param aItem2 - second item to be compared
     * @return -1      if aItem1 < aItem2
     *	         1      if aItem1 > aItem2
     *	         0      otherwise
     */
	   IMPORT_C static TInt ComparePluginInfo (
        const CPluginInfo & aItem1,
        const CPluginInfo & aItem2
        );

    /**
     * Comparison method for CPluginInfo instances based on plug-in order
     *
     * @since S60 v5.0
     * @param aItem1 - first item to be compared
     * @param aItem2 - second item to be compared
     * @return -1      if aItem1 < aItem2
     *	         1      if aItem1 > aItem2
     *	         0      otherwise
     */
	   IMPORT_C static TInt ComparePluginOrder (
        const CPluginInfo & aItem1,
        const CPluginInfo & aItem2
        );


    /**
     * Two-phased constructor.
     * @param -
     * @return pointer to created CPluginInfo object
     */
     IMPORT_C static CPluginInfo * NewL ();

    /** 
     * Two-phased constructor.	
	   * Leaves the object in cleanup stack.
	   * @param -
	   *	@return pointer to created CPluginInfo object
	   */
     IMPORT_C static CPluginInfo * NewLC ();

    /**
     * Destructor.
     */
     IMPORT_C ~CPluginInfo ();

    /**
     * Getter for plug-in DLL file name.
     *
     * @since S60 v5.0
     * @param -
     *	@return plug-in DLL name
     */
     IMPORT_C HBufC *& PluginDll ();

    /**
     * Getter for plug-in DLL file name, const.
     *
     * @since S60 v5.0
     * @param -
     *	@return plug-in DLL name
     */
     IMPORT_C const TDesC & PluginDll () const;

    /**
     * Getter for plug-in DLL UID2, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in DLL UID2
     */
     IMPORT_C TUid & Uid2 ();

    /**
     * Getter for plug-in DLL UID2, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in DLL UID2
     */
	   IMPORT_C const TUid & Uid2 () const;

    /**
     * Getter for plug-in DLL UID3, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in DLL UID3
     */
	   IMPORT_C TUid & Uid3 ();

    /**
     * Getter for plug-in DLL UID3, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in DLL UID3
     */
     IMPORT_C const TUid & Uid3 () const;

    /**
     * Getter for plug-in UI type, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in UI type
     */
	   IMPORT_C TInt & PluginUiType();

    /**
     * Getter for plug-in UI type, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in UI type
     */
	   IMPORT_C const TInt & PluginUiType() const;

    /**
     * Getter for plug-in filter type, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in filter type
     */
	   IMPORT_C TInt & PluginFilterType();

    /**
     * Getter for plug-in filter type, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in filter type
     */
	   IMPORT_C const TInt & PluginFilterType() const;

    /**
     * Getter for plug-in scope, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in scope
     */
     IMPORT_C TInt & PluginScope();

    /**
     * Getter for plug-in scope, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in scope
     */
     IMPORT_C const TInt & PluginScope() const;

    /**
     * Getter for plug-in display order, non-const.
     * Determines the order in which the plugins are 
     * displayed in the user interface.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in order
     */
     IMPORT_C TInt & PluginDisplayOrder();

    /**
     * Getter for plug-in display order, const.
     * Determines the order in which the plugins are 
     * displayed in the user interface.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in order
     */
     IMPORT_C const TInt & PluginDisplayOrder() const;

    /**
     * Getter for plug-in RID, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in RID
     */
     IMPORT_C TInt & PluginRID();

    /**
     * Getter for plug-in RID, const.
     *
     * @since S60 v5.0
     * @param -
     * @return plug-in RID
     */
     IMPORT_C const TInt & PluginRID() const;

    /** 
     * Getter for plug-in name, non-const.
     *
     *	@param -
     *	@return plug-in name
     */
	   IMPORT_C HBufC *& PluginName();

    /** 
     * Getter for plug-in name, const.
     *
     * @param -
     * @return plug-in name
     */
     IMPORT_C const TDesC & PluginName() const;

    /**
     * Getter for plug-in group icon file name, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return icon file name
     */
     IMPORT_C HBufC *& IconFile();

    /**
     * Getter for plug-in group icon file name, const.
     *
     * @since S60 v5.0
     * @param -
     * @return icon file name
     */
     IMPORT_C const TDesC & IconFile() const;

    /**
     * Getter for plug-in group icon bitmap, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return icon bitmap
     */
     IMPORT_C CFbsBitmap *& Icon();

    /**
     * Getter for plug-in group icon bitmap, const.
     *
     * @since S60 v5.0
     * @param -
     * @return icon bitmap
     */
     IMPORT_C const CFbsBitmap * Icon() const;

    /**
     * Getter for plug-in mask bitmap, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return mask bitmap
     */
     IMPORT_C CFbsBitmap *& Mask();

    /**
     * Getter for plug-in mask bitmap, const.
     *
     * @since S60 v5.0
     * @param -
     * @return mask bitmap
     */
     IMPORT_C const CFbsBitmap * Mask() const;

    /**
     * Getter for parameter descriptor array, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return pointer to descriptor array
     */
     IMPORT_C CDesCArray & Parameters();

    /**
     * Getter for parameter descriptor array, const.
     *
     * @since S60 v5.0
     * @param -
     * @return pointer to descriptor array
     */
     IMPORT_C const CDesCArray & Parameters() const;

    /**
     * Getter for soft key 1 command ID, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 1
     */
     IMPORT_C CArrayFix<TInt> & Sk1Cmds();

    /**
     * Getter for soft key 1 command ID, const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 1
     */
     IMPORT_C const CArrayFix<TInt> & Sk1Cmds() const;

    /**
     * Getter for soft key 1 texts, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C CDesCArray &  Sk1Texts();

    /**
     * Getter for soft key 1 texts, const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C const CDesCArray & Sk1Texts() const;

    /**
     * Getter for soft key 2 command ID, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 2
     */
     IMPORT_C CArrayFix<TInt> & Sk2Cmds();

    /**
     * Getter for soft key 2 command ID, const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 2
     */
     IMPORT_C const CArrayFix<TInt> & Sk2Cmds() const;

    /**
     * Getter for soft key 2 texts, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C CDesCArray & Sk2Texts();

    /**
     * Getter for soft key 1 texts, const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C const CDesCArray & Sk2Texts() const;
    
    /**
     * Getter for middle soft key command ID, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 2
     */
     IMPORT_C CArrayFix<TInt> & MSKCmds();

    /**
     * Getter for middle soft key command ID, const.
     *
     * @since S60 v5.0
     * @param -
     * @return command ID for soft key 2
     */
     IMPORT_C const CArrayFix<TInt> & MSKCmds() const;

    /**
     * Getter for middle soft key texts, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C CDesCArray & MSKTexts();

    /**
     * Getter for middle soft key texts, const.
     *
     * @since S60 v5.0
     * @param -
     * @return soft key texts
     */
     IMPORT_C const CDesCArray & MSKTexts() const;
    
    /**
     * Getter for menu items, non-const.
     *
     * @since S60 v5.0
     * @param -
     * @return reference to menu item array
     */
     IMPORT_C CMenuItemArray & MenuItems();

    /**
     * Getter for menu items, const.
     *
     * @since S60 v5.0
     * @param -
     * @return reference to menu item array
     */
     IMPORT_C const CMenuItemArray & MenuItems() const;

    /**
     * Externalises the object.
     * All the member variables are serialized into the stream, 
     * except for the CFbsBitmap members iIcon and iMask. For those,
     * only file path is stored, and the icons need to be created 
     * from file when internalizing the object.
     *
     * @since S60 v5.0
     * @param aStream - write stream
     * @return -
     */
     IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

    /** InternalizeL
     *
     * Internalises the object.
     * All the member variables are serialized into the stream, 
     * except for the CFbsBitmap members iIcon and iMask. For those,
     * only file path is stored, and the icons need to be created 
     * from file when internalizing the object.
     *	
     * @since S60 v5.0	
     * @param aStream - read stream
     * @return -
     */
     IMPORT_C void InternalizeL(RReadStream& aStream);


protected:
private:

     CPluginInfo ();

     void ConstructL ();

private: // data

    /**
     * Plug-in DLL name
     */
     HBufC *				    iPluginDllName;
   
    /**
     * Plug-in DLL UID 2
     */     
     TUid				    iUID2;
	
    /**
     * Plug-in DLL UID 3
     */	
     TUid				    iUID3;
	
    /**
     * Plug-in UI type
     */	
     TInt				    iPluginUiType;
	
    /**
     * Plug-in filter type
     */		
     TInt				    iPluginFilterType;
	
    /**
     * Plug-in scope
     */		
     TInt				    iScope;
	
    /**
     * Plug-in run-time ID (RID)
     */		
     TInt				    iPluginRID;
	
    /**
     * Plug-in display order
     */		
     TInt				    iPluginDisplayOrder;
	
    /**
     * Plug-in name
     */		
     HBufC *				    iPluginName;
	
    /**
     * Icon name
     */		
     HBufC *				    iIconFileName;
	
    /**
     * Icon bitmap
     */		
     CFbsBitmap *		    iIcon;
	
    /**
     * Icon mask bitmap
     */		
     CFbsBitmap *		    iMask;
	
    /**
     * Parameter array
     */		
     CDesCArray *	        iParams; 
	
    /**
     * Soft key 1 command ID
     */		
     CArrayFix<TInt> *       iSk1Cmds;
    
    /**
     * Soft key 1 text
     */	    
     CDesCArray *            iSk1Texts;
    
    /**
     * Soft key 2 command ID
     */	    
     CArrayFix<TInt> *       iSk2Cmds;
    
    /**
     * Soft key 2 text
     */	    
     CDesCArray *            iSk2Texts;
    
    /**
     * Middle soft key command ID
     */	    
     CArrayFix<TInt> *       iMSKCmds;
    
    /**
     * Middle soft key text
     */	    
     CDesCArray *            iMSKTexts;

     
    /**
     * Menu items
     */	    
     CMenuItemArray *        iMenuItems;

};


#endif // PLUGININFO_H

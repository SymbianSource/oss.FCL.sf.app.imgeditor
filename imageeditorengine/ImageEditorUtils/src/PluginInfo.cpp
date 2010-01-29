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
* Utils class handling the information about the plugins.
*
*/



#include <fbs.h>

#include "PluginInfo.h"
#include "EditorVersion.h"

//=============================================================================
EXPORT_C TInt CPluginInfo::ComparePluginInfo (
    const CPluginInfo & aItem1,
    const CPluginInfo & aItem2
    )
{
    if (aItem1.iPluginRID < aItem2.iPluginRID)
    {
        return -1;
    }
    else if (aItem1.iPluginRID > aItem2.iPluginRID)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//=============================================================================
EXPORT_C TInt CPluginInfo::ComparePluginOrder(
    const CPluginInfo & aItem1,
    const CPluginInfo & aItem2
    )
{
    if (aItem1.iPluginDisplayOrder < aItem2.iPluginDisplayOrder)
    {
        return -1;
    }
    else if (aItem1.iPluginDisplayOrder > aItem2.iPluginDisplayOrder)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//=============================================================================
EXPORT_C CPluginInfo * CPluginInfo::NewL ()
{
    CPluginInfo * self = CPluginInfo::NewLC();
    CleanupStack::Pop(); // self
    return self;
}

//=============================================================================
EXPORT_C CPluginInfo * CPluginInfo::NewLC ()
{
    CPluginInfo * self = new (ELeave) CPluginInfo;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

//=============================================================================
EXPORT_C CPluginInfo::~CPluginInfo ()
{
    delete iPluginDllName;
    delete iPluginName;
    delete iIconFileName;
    delete iIcon;
    delete iMask;

    if (iParams)
    {
        iParams->Reset();
    }
    delete iParams;

    if (iSk1Cmds)
    {
        iSk1Cmds->Reset();
    }
    delete iSk1Cmds;

    if (iSk1Texts)
    {
        iSk1Texts->Reset();
    }
    delete iSk1Texts;

    if (iSk2Cmds)
    {
        iSk2Cmds->Reset();
    }
    delete iSk2Cmds;

    if (iSk2Texts)
    {
        iSk2Texts->Reset();
    }
    delete iSk2Texts;

    if (iMSKCmds)
    {
        iMSKCmds->Reset();
    }
    delete iMSKCmds;

    if (iMSKTexts)
    {
        iMSKTexts->Reset();
    }
    delete iMSKTexts;
    if (iMenuItems)
    {
        iMenuItems->Reset();
    }
    delete iMenuItems;
}

//=============================================================================
EXPORT_C HBufC *& CPluginInfo::PluginDll ()
{
    return iPluginDllName;
}

//=============================================================================
EXPORT_C const TDesC & CPluginInfo::PluginDll () const
{
    return *iPluginDllName;
}

//=============================================================================
EXPORT_C TUid & CPluginInfo::Uid2 ()
{
    return iUID2;
}

//=============================================================================
EXPORT_C const TUid & CPluginInfo::Uid2 () const
{
    return iUID2;
}

//=============================================================================
EXPORT_C TUid & CPluginInfo::Uid3 ()
{
    return iUID3;
}

//=============================================================================
EXPORT_C const TUid & CPluginInfo::Uid3 () const
{
    return iUID3;
}

//=============================================================================
EXPORT_C TInt & CPluginInfo::PluginUiType()
{
    return iPluginUiType;
}

//=============================================================================
EXPORT_C const TInt & CPluginInfo::PluginUiType() const
{
    return iPluginUiType;
}

//=============================================================================
EXPORT_C TInt & CPluginInfo::PluginFilterType()
{
    return iPluginFilterType;
}

//=============================================================================
EXPORT_C const TInt & CPluginInfo::PluginFilterType() const
{
    return iPluginFilterType;
}

//=============================================================================
EXPORT_C TInt & CPluginInfo::PluginScope()
{
    return iScope;
}

//=============================================================================
EXPORT_C const TInt & CPluginInfo::PluginScope() const
{
    return iScope;
}

//=============================================================================
EXPORT_C TInt & CPluginInfo::PluginDisplayOrder()
{
    return iPluginDisplayOrder;
}

//=============================================================================
EXPORT_C const TInt & CPluginInfo::PluginDisplayOrder() const
{
    return iPluginDisplayOrder;
}

//=============================================================================
EXPORT_C TInt & CPluginInfo::PluginRID()
{
    return iPluginRID;
}

//=============================================================================
EXPORT_C const TInt & CPluginInfo::PluginRID() const
{
    return iPluginRID;
}

//=============================================================================
EXPORT_C HBufC *& CPluginInfo::PluginName()
{
    return iPluginName;
}

//=============================================================================
EXPORT_C const TDesC & CPluginInfo::PluginName() const
{
    return *iPluginName;
}
//=============================================================================
EXPORT_C HBufC *& CPluginInfo::IconFile()
{
    return iIconFileName;
}

//=============================================================================
EXPORT_C const TDesC & CPluginInfo::IconFile() const
{
    return *iIconFileName;
}

//=============================================================================
EXPORT_C CFbsBitmap *& CPluginInfo::Icon()
{
    return iIcon;
}

//=============================================================================
EXPORT_C const CFbsBitmap * CPluginInfo::Icon() const
{
    return iIcon;
}

//=============================================================================
EXPORT_C CFbsBitmap *& CPluginInfo::Mask()
{
    return iMask;
}

//=============================================================================
EXPORT_C const CFbsBitmap * CPluginInfo::Mask() const
{
    return iMask;
}

//=============================================================================
EXPORT_C CDesCArray & CPluginInfo::Parameters()
{
	return *iParams;
}

//=============================================================================
EXPORT_C const CDesCArray & CPluginInfo::Parameters() const
{
	return *iParams;
}

//=============================================================================
EXPORT_C CArrayFix<TInt> & CPluginInfo::Sk1Cmds()
{
    return *iSk1Cmds;
}

//=============================================================================
EXPORT_C const CArrayFix<TInt> & CPluginInfo::Sk1Cmds() const
{
    return *iSk1Cmds;
}

//=============================================================================
EXPORT_C CDesCArray & CPluginInfo::Sk1Texts()
{
    return *iSk1Texts;
}

//=============================================================================
EXPORT_C const CDesCArray & CPluginInfo::Sk1Texts() const
{
    return *iSk1Texts;
}

//=============================================================================
EXPORT_C CArrayFix<TInt> & CPluginInfo::Sk2Cmds()
{
    return *iSk2Cmds;
}

//=============================================================================
EXPORT_C const CArrayFix<TInt> & CPluginInfo::Sk2Cmds() const
{
    return *iSk2Cmds;
}

//=============================================================================
EXPORT_C CDesCArray & CPluginInfo::Sk2Texts()
{
    return *iSk2Texts;
}

//=============================================================================
EXPORT_C const CDesCArray & CPluginInfo::Sk2Texts() const
{
    return *iSk2Texts;
}

//=============================================================================
EXPORT_C CArrayFix<TInt> & CPluginInfo::MSKCmds()
{
    return *iMSKCmds;
}

//=============================================================================
EXPORT_C const CArrayFix<TInt> & CPluginInfo::MSKCmds() const
{
    return *iMSKCmds;
}

//=============================================================================
EXPORT_C CDesCArray & CPluginInfo::MSKTexts()
{
    return *iMSKTexts;
}

//=============================================================================
EXPORT_C const CDesCArray & CPluginInfo::MSKTexts() const
{
    return *iMSKTexts;
}
//=============================================================================
EXPORT_C CMenuItemArray & CPluginInfo::MenuItems()
{
    return *iMenuItems;
}

//=============================================================================
EXPORT_C const CMenuItemArray & CPluginInfo::MenuItems() const
{
    return *iMenuItems;
}

//=============================================================================
EXPORT_C void CPluginInfo::ExternalizeL ( RWriteStream& aStream ) const
{
    TInt i = 0;

    // Current UI language
    TLanguage lang = User::Language();
    aStream.WriteInt32L (lang);

    // Editor version (from EditorVersion.h)
    aStream.WriteInt32L (my_version_major); 
    aStream.WriteInt32L (my_version_minor);
    aStream.WriteInt32L (my_version_build);

    // HBufC * iPluginDllName
    if ( iPluginDllName )
        {
        TPtr pluginDllNamePtr = iPluginDllName->Des();
        aStream.WriteInt32L (pluginDllNamePtr.Length());
        aStream << pluginDllNamePtr;            
        }
	
    // TUid iUID2
    aStream << iUID2;

    // TUid iUID3
    aStream << iUID3;

    // TInt iPluginUiType
    aStream.WriteInt32L( iPluginUiType );

    // TInt iPluginFilterType
    aStream.WriteInt32L( iPluginFilterType );

    // TInt iScope
    aStream.WriteInt32L( iScope );

    //TInt iPluginDisplayOrder
    aStream.WriteInt32L( iPluginDisplayOrder );

    // TInt iPluginRID
    aStream.WriteInt32L( iPluginRID );

    // HBufC * iPluginName
    if ( iPluginName )
        {
        TPtr pluginNamePtr = iPluginName->Des();
        aStream.WriteInt32L (pluginNamePtr.Length());
        aStream << pluginNamePtr;            
        }

    // HBufC * iIconFileName
    if ( iIconFileName )
        {
        TPtr iconFileNamePtr = iIconFileName->Des();
        aStream.WriteInt32L (iconFileNamePtr.Length());
        aStream << iconFileNamePtr;            
        }
	
    // CDesCArray * iParams
    TInt paramCount = iParams->MdcaCount();
    aStream.WriteInt32L( paramCount );
    for ( i=0; i<paramCount; i++ )
    {
        TPtrC des = iParams->MdcaPoint(i);
        aStream.WriteInt32L( des.Length() );
        aStream << des;
    }

    // CArrayFix<TInt> * iSk1Cmds
    TInt sk1CmdCount = iSk1Cmds->Count();
    aStream.WriteInt32L( sk1CmdCount );
    for ( i=0; i<sk1CmdCount; i++ )
    {
        aStream.WriteInt32L( iSk1Cmds->At(i) );
    }

    // CDesCArray * iSk1Texts
    TInt sk1TextCount = iSk1Texts->MdcaCount();
    aStream.WriteInt32L( sk1TextCount );
    for ( i=0; i<sk1TextCount; i++ )
    {
        TPtrC des = iSk1Texts->MdcaPoint(i);
        aStream.WriteInt32L( des.Length() );
        aStream << des;
    }

    // CArrayFix<TInt> * iSk2Cmds
    TInt sk2CmdCount = iSk2Cmds->Count();
    aStream.WriteInt32L( sk2CmdCount );
    for ( i=0; i<sk2CmdCount; i++ )
    {
        aStream.WriteInt32L( iSk2Cmds->At(i) );
    }

    // CDesCArray * iSk2Texts
    TInt sk2TextCount = iSk2Texts->MdcaCount();
    aStream.WriteInt32L( sk2TextCount );
    for ( i=0; i<sk2TextCount; i++ )
    {
        TPtrC des = iSk2Texts->MdcaPoint(i);
        aStream.WriteInt32L( des.Length() );
        aStream << des;
    }
    
    // CArrayFix<TInt> * iMSKCmds
    TInt mskCmdCount = iMSKCmds->Count();
    aStream.WriteInt32L( mskCmdCount );
    for ( i=0; i<mskCmdCount; i++ )
    {
        aStream.WriteInt32L( iMSKCmds->At(i) );
    }

    // CDesCArray * iMSKTexts
    TInt mskTextCount = iMSKTexts->MdcaCount();
    aStream.WriteInt32L( mskTextCount );
    for ( i=0; i<mskTextCount; i++ )
    {
        TPtrC des = iMSKTexts->MdcaPoint(i);
        aStream.WriteInt32L( des.Length() );
        aStream << des;
    }
    
    // CMenuItemArray * iMenuItems
    TInt menuItemCount = iMenuItems->Count();
    aStream.WriteInt32L( menuItemCount );
    for ( i=0; i<menuItemCount; i++ )
    {
        CEikMenuPaneItem::SData& item = iMenuItems->At(i);

        // TInt iCommandId;
        aStream.WriteInt32L( item.iCommandId );
        // TInt iCascadeId;
        aStream.WriteInt32L( item.iCascadeId );
        // TInt iFlags;
        aStream.WriteInt32L( item.iFlags );
        // TBuf<ENominalTextLength> iText;
        aStream << item.iText;
        // TBuf<1> iExtraText;
        aStream << item.iText;
    }
}

//=============================================================================
EXPORT_C void CPluginInfo::InternalizeL ( RReadStream& aStream )
{
    TInt i = 0;

    // Check that the current UI language is the same as stored
    // in the ini file. If not, reject the stored information.
    TLanguage storedLang = (TLanguage) aStream.ReadInt32L();
    if (storedLang != User::Language())
    {
        User::Leave (KErrCorrupt);
    }

    // If the editor version has changed, also reject the stored data.
    TInt major = aStream.ReadInt32L(); 
    TInt minor = aStream.ReadInt32L();
    TInt build = aStream.ReadInt32L();
    if (major != my_version_major ||
        minor != my_version_minor ||
        build != my_version_build 
        )
    {
        User::Leave (KErrCorrupt);
    }

    // HBufC * iPluginDllName
    TInt pluginDllNameLength = aStream.ReadInt32L();
    iPluginDllName = HBufC::NewL(pluginDllNameLength);
    TPtr pluginDllNamePtr = iPluginDllName->Des();
    aStream >> pluginDllNamePtr;

    // TUid iUID2
    aStream >> iUID2;

    // TUid iUID3
    aStream >> iUID3;

    // TInt iPluginUiType
    iPluginUiType = aStream.ReadInt32L();

    // TInt iPluginFilterType
    iPluginFilterType = aStream.ReadInt32L();

    // TInt iScope
    iScope = aStream.ReadInt32L();

    // TInt iPluginDisplayOrder
    iPluginDisplayOrder = aStream.ReadInt32L();

    // TInt iPluginRID
    iPluginRID = aStream.ReadInt32L();

    // HBufC * iPluginName
    TInt pluginNameLength = aStream.ReadInt32L();
    iPluginName = HBufC::NewL(pluginNameLength);
    TPtr pluginNamePtr = iPluginName->Des();
    aStream >> pluginNamePtr;

    // HBufC * iIconFileName
    TInt iconFileNameLength = aStream.ReadInt32L();
    iIconFileName = HBufC::NewL(iconFileNameLength);
    TPtr iconFileNamePtr = iIconFileName->Des();
    aStream >> iconFileNamePtr;

    // CDesCArray *	iParams
    TInt paramCount = aStream.ReadInt32L();
    for ( i=0; i<paramCount; i++ )
    {
        TInt paramLength = aStream.ReadInt32L();
        HBufC* paramBuf = HBufC::NewLC( paramLength );
        TPtr paramPtr = paramBuf->Des();
        aStream >> paramPtr;
        iParams->AppendL( paramPtr );
        CleanupStack::PopAndDestroy( paramBuf );
    }

    // CArrayFix<TInt> * iSk1Cmds;
    TInt sk1CmdCount = aStream.ReadInt32L();
    for ( i=0; i<sk1CmdCount; i++ )
    {
        TInt cmd = aStream.ReadInt32L();
        iSk1Cmds->AppendL( cmd );
    }

    // CDesCArray * iSk1Texts;
    TInt sk1TextCount = aStream.ReadInt32L();
    for ( i=0; i<sk1TextCount; i++ )
    {
        TInt sk1TextLength = aStream.ReadInt32L();
        HBufC* sk1TextBuf = HBufC::NewLC( sk1TextLength );
        TPtr sk1TextPtr = sk1TextBuf->Des();
        aStream >> sk1TextPtr;
        iSk1Texts->AppendL( sk1TextPtr );
        CleanupStack::PopAndDestroy( sk1TextBuf );
    }

    // CArrayFix<TInt> * iSk2Cmds;
    TInt sk2CmdCount = aStream.ReadInt32L();
    for ( i=0; i<sk2CmdCount; i++ )
    {
        TInt cmd = aStream.ReadInt32L();
        iSk2Cmds->AppendL( cmd );
    }

    // CDesCArray * iSk2Texts;
    TInt sk2TextCount = aStream.ReadInt32L();
    for ( i=0; i<sk2TextCount; i++ )
    {
        TInt sk2TextLength = aStream.ReadInt32L();
        HBufC* sk2TextBuf = HBufC::NewLC( sk2TextLength );
        TPtr sk2TextPtr = sk2TextBuf->Des();
        aStream >> sk2TextPtr;
        iSk2Texts->AppendL( sk2TextPtr );
        CleanupStack::PopAndDestroy( sk2TextBuf );
    }
    
    // CArrayFix<TInt> * iMSKCmds;
    TInt mskCmdCount = aStream.ReadInt32L();
    for ( i=0; i<mskCmdCount; i++ )
    {
        TInt cmd = aStream.ReadInt32L();
        iMSKCmds->AppendL( cmd );
    }

    // CDesCArray * iMSKTexts;
    TInt mskTextCount = aStream.ReadInt32L();
    for ( i=0; i<mskTextCount; i++ )
    {
        TInt mskTextLength = aStream.ReadInt32L();
        HBufC* mskTextBuf = HBufC::NewLC( mskTextLength );
        TPtr mskTextPtr = mskTextBuf->Des();
        aStream >> mskTextPtr;
        iMSKTexts->AppendL( mskTextPtr );
        CleanupStack::PopAndDestroy( mskTextBuf );
    }
    
    // CMenuItemArray * iMenuItems;
    TInt menuItemCount = aStream.ReadInt32L();
    for ( i=0; i<menuItemCount; i++ )
    {
        CEikMenuPaneItem::SData item;

        // TInt iCommandId;
        item.iCommandId = aStream.ReadInt32L();
        // TInt iCascadeId;
        item.iCascadeId = aStream.ReadInt32L();
        // TInt iFlags;
        item.iFlags = aStream.ReadInt32L();
        // TBuf<ENominalTextLength> iText;
        aStream >> item.iText;
        // TBuf<1> iExtraText;
        aStream >> item.iText;

        iMenuItems->AppendL( item );
    }
}

//=============================================================================
void CPluginInfo::ConstructL()
{
	iParams = new (ELeave) CDesCArraySeg(16);
	iSk1Cmds= new (ELeave) CArrayFixSeg<TInt>(4);
	iSk1Texts = new (ELeave) CDesCArraySeg(4);
	iSk2Cmds= new (ELeave) CArrayFixSeg<TInt>(4);
	iSk2Texts = new (ELeave) CDesCArraySeg(4);
	iMSKCmds= new (ELeave) CArrayFixSeg<TInt>(4);
	iMSKTexts = new (ELeave) CDesCArraySeg(4);
    iMenuItems = new (ELeave) CMenuItemArray (8);
}

//=============================================================================
CPluginInfo::CPluginInfo ()
{

}

// End of File

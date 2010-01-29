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



#include <barsread.h> 
#include <bautils.h>
#include <eikenv.h> 
#include <coneresloader.h> 


#include <ImageEditorPluginBase.rsg>
#include "iepbres.h"
#include "ImageEditorPluginBase.hrh"

//=============================================================================
CPluginResource * CPluginResource::NewL (
    const TDesC &   aResourcePath,
    const TDesC &   aResourceFile
    )
{
    CPluginResource * self = new (ELeave) CPluginResource;
    CleanupStack::PushL (self);
    self->ConstructL (aResourcePath, aResourceFile);
    CleanupStack::Pop (); // self;
    return self;
}

//=============================================================================
CPluginResource::~CPluginResource ()
{
	delete iPluginName;
	delete iPluginIconFileName;
	delete iPluginFilterDllName;
	
    if (iParameters)
	{
		iParameters->Reset();
	}
	delete iParameters;

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
const TPluginInt & CPluginResource::PluginUID () const
{
    return iPluginUID;
}

//=============================================================================
const TPluginUiType & CPluginResource::PluginUiType() const
{
    return iPluginUIType;
}

//=============================================================================
const TPluginFilterType & CPluginResource::PluginFilterType() const
{
    return iPluginFilterType;
}

//=============================================================================
const TPluginScope & CPluginResource::PluginScope() const
{
    return iPluginScope;
}

//=============================================================================
const TPluginInt & CPluginResource::PluginDisplayOrder() const
{
    return iPluginDisplayOrder;
}

//=============================================================================
const TPtrC CPluginResource::PluginName () const
{
    return iPluginName->Des();
}

//=============================================================================
const TPtrC CPluginResource::IconFileName() const
{
    return iPluginIconFileName->Des();
}

//=============================================================================
const TPtrC CPluginResource::FilterFileName () const
{
    return iPluginFilterDllName->Des();
}

//=============================================================================
const CDesCArray * CPluginResource::GetParameters () const
{
	return (CDesCArray *)iParameters;
}

//=============================================================================
const CDesCArray * CPluginResource::Sk1Texts () const
{
    return iSk1Texts;
}

//=============================================================================
const CArrayFix<TInt> * CPluginResource::Sk1CommandIds() const
{
    return iSk1Cmds;
}

//=============================================================================
const CDesCArray * CPluginResource::Sk2Texts () const
{
    return iSk2Texts;
}

//=============================================================================
const CArrayFix<TInt> * CPluginResource::Sk2CommandIds() const
{
    return iSk2Cmds;
}

//=============================================================================
const CDesCArray * CPluginResource::MSKTexts () const
{
    return iMSKTexts;
}

//=============================================================================
const CArrayFix<TInt> * CPluginResource::MSKCommandIds() const
{
    return iMSKCmds;
}

//=============================================================================
const CMenuItemArray * CPluginResource::MenuItems() const
{
    return iMenuItems;
}

//=============================================================================
CPluginResource::CPluginResource()
{

}

//=============================================================================
void CPluginResource::ConstructL (
    const TDesC &   aResourcePath,
    const TDesC &   aResourceFile
    )
{

	CEikonEnv* env = CEikonEnv::Static();

    /**
    *
    *   PLUG-IN INFO
    *
    */
  
    //  Create resource utility
	TFileName file_name;
	file_name.Copy (aResourcePath);
	file_name.Append (aResourceFile);
    BaflUtils::NearestLanguageFile( env->FsSession(), file_name );

    RConeResourceLoader res_loader ( *env );
    CleanupClosePushL (res_loader);
	res_loader.OpenL ( file_name );

    //  Construct resource reader
    HBufC8 * res8 = env->AllocReadResourceAsDes8LC (R_PLUGIN_INFO);
    TResourceReader resource;
    resource.SetBuffer (res8);

    //  Read plug-in UID
    iPluginUID = resource.ReadInt32();    

    //  Read plug-in UID
    iPluginUIType = (TPluginUiType)resource.ReadInt32();    

    //  Read plug-in UID
    iPluginFilterType = (TPluginFilterType)resource.ReadInt32();    

    //  Read plug-in UID
    iPluginScope = (TPluginScope)resource.ReadInt32();

    //  Read plug-in display priority
    iPluginDisplayOrder = resource.ReadInt32();

    //  Read plug-in name
    iPluginName = resource.ReadHBufCL();
    if ( !iPluginName )
    {
        User::Leave (KErrGeneral);
    }

    //  Read icon file name
    iPluginIconFileName = resource.ReadHBufCL();
    if ( !iPluginIconFileName )
    {
        User::Leave (KErrGeneral);
    }

    //  Read filter file name
    iPluginFilterDllName = resource.ReadHBufCL();

    CleanupStack::PopAndDestroy(); // res8


    /**
    *
    *   PLUG-IN PARAMETERS
    *
    */

	//	Read resource to descriptor
	res8 = env->AllocReadResourceAsDes8LC (R_PLUGIN_PARS);

	//	Set resource reader buffer
	resource.SetBuffer (res8);

	//	Read extra parameter count
	TInt16 arraycount = (TInt16)resource.ReadInt16();

	if (arraycount > 0)
	{
		//	Construct a new descriptor array
		if (!iParameters)
		{
			iParameters = new (ELeave) CDesCArraySeg(16);
		}

		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
			//	Construct a flat descriptor array and read all the parameter names 
			HBufC * parameter = resource.ReadHBufCL();
            CleanupStack::PushL(parameter);
			TPtrC ptr = parameter->Des();
			iParameters->AppendL (ptr);
            CleanupStack::PopAndDestroy(); // parameter
		}
    }
    CleanupStack::PopAndDestroy(); // res8


    /**
    *
    *   SOFT KEY 1
    *
    */

	//	Read resource to descriptor
	res8 = env->AllocReadResourceAsDes8LC (R_SK1_ITEMS);
    
	//	Set resource reader buffer
	resource.SetBuffer (res8);

	//	Read menu item 
	arraycount = (TInt16)resource.ReadInt16();

	if (arraycount > 0)
	{
        
		//	Construct a new descriptor array
		if (!iSk1Cmds)
		{
			iSk1Cmds = new (ELeave) CArrayFixSeg<TInt>(8);
		}
		if (!iSk1Texts)
		{
			iSk1Texts = new (ELeave) CDesCArraySeg(8);
		}

		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
            //  Read command ID
            TInt cmd = resource.ReadInt32();
            iSk1Cmds->AppendL(cmd);

            //  Read the menu item text
            HBufC * menutext = resource.ReadHBufCL();
            CleanupStack::PushL (menutext);
            TPtrC ptr = menutext->Des();
            iSk1Texts->AppendL ( ptr );
            CleanupStack::PopAndDestroy(); // menutext
		}
    }
    CleanupStack::PopAndDestroy(); // res8

    /**
    *
    *   SOFT KEY 2
    *
    */

	//	Read resource to descriptor
	res8 = env->AllocReadResourceAsDes8LC (R_SK2_ITEMS);
    
	//	Set resource reader buffer
	resource.SetBuffer (res8);

	//	Read menu item 
	arraycount = (TInt16)resource.ReadInt16();

	if (arraycount > 0)
	{
        
		//	Construct a new descriptor array
		if (!iSk2Cmds)
		{
			iSk2Cmds = new (ELeave) CArrayFixSeg<TInt>(8);
		}
		if (!iSk2Texts)
		{
			iSk2Texts = new (ELeave) CDesCArraySeg(8);
		}

		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
            //  Read command ID
            TInt cmd = resource.ReadInt32();
            iSk2Cmds->AppendL(cmd);

            //  Read the menu item text
            HBufC * menutext = resource.ReadHBufCL();
            CleanupStack::PushL (menutext);
            TPtrC ptr = menutext->Des();
            iSk2Texts->AppendL ( ptr );
            CleanupStack::PopAndDestroy(); // menutext
		}
    }
    CleanupStack::PopAndDestroy(); // res8

    /**
    *
    *   MIDDLE SOFTKEY
    *
    */
    
	//	Read resource to descriptor
	res8 = env->AllocReadResourceAsDes8LC (R_MSK_ITEMS);
    
	//	Set resource reader buffer
	resource.SetBuffer (res8);

	//	Read menu item 
	arraycount = (TInt16)resource.ReadInt16();

	if (arraycount > 0)
	{
        
		//	Construct a new descriptor array
		if (!iMSKCmds)
		{
			iMSKCmds = new (ELeave) CArrayFixSeg<TInt>(8);
		}
		if (!iMSKTexts)
		{
			iMSKTexts = new (ELeave) CDesCArraySeg(8);
		}

		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
            //  Read command ID
            TInt cmd = resource.ReadInt32();
            iMSKCmds->AppendL(cmd);

            //  Read the menu item text
            HBufC * menutext = resource.ReadHBufCL();
            CleanupStack::PushL (menutext);
            TPtrC ptr = menutext->Des();
            iMSKTexts->AppendL ( ptr );
            CleanupStack::PopAndDestroy(); // menutext
		}
    }
    CleanupStack::PopAndDestroy(); // res8
    
    /**
    *
    *   MENU ITEMS
    *
    */

    //	Read resource to descriptor
	res8 = env->AllocReadResourceAsDes8LC (R_MENU_ITEMS);

	//	Set resource reader buffer
	resource.SetBuffer (res8);

	//	Read menu item 
	arraycount = (TInt16)resource.ReadInt16();

	if (arraycount > 0)
	{
        
		//	Construct a new descriptor array
		if (!iMenuItems)
		{
			iMenuItems = new (ELeave) CMenuItemArray(8);
		}

		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
			//	Construct and initialize a new menu item
            CEikMenuPaneItem::SData item;
            item.iFlags = 0;
            item.iCascadeId = 0;

            //  Read command ID
            item.iCommandId = resource.ReadInt32();

            //  Read the menu item text
            HBufC * menutext = resource.ReadHBufCL();
            CleanupStack::PushL(menutext);
            item.iText.Copy ( menutext->Des() );

            //  Add menu item to array
            iMenuItems->AppendL (item);
            CleanupStack::PopAndDestroy(menutext);
		}

    }

	CleanupStack::PopAndDestroy (2); // res8, res_loader

}

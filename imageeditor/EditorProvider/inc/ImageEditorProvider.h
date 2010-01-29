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


#ifndef IMEGEEDITORPROVIDER_H
#define IMEGEEDITORPROVIDER_H

// INCLUDES
#include <AiwServiceIfMenu.h>
#include <apparc.h>
#include <ConeResLoader.h> 
#include <AknServerApp.h> 


// FORWARD DECLARATIONS

class MAiwNotifyCallback;
class CAknOpenFileService;
class TDataType;

/** CEditorProvider
*
*   CEditorProvider acts as a service provider of Image Editor in
*   Series 60 2.8 platform. It implements the base and menu service classes
*   as an ECom component.
*
*/ 
class CEditorProvider : public CAiwServiceIfMenu, public MAknServerAppExitObserver
{	

public:

    /** New factory method
	*
	*	
    *	@param -
	*	@return - pointer to a new instance of CEditorProvider
    */ 
	static CEditorProvider * NewL();

    /** Destructor
	*	
    *	@param -
	*	@return - 
    */ 
	virtual ~CEditorProvider();

    /** InitialiseL
    *
    *   @see CAiwServiceIfBase
    */ 
    virtual void InitialiseL (
		MAiwNotifyCallback &			aFrameworkCallback,
		const RCriteriaArray &			aInterest
		);

    /** HandleServiceCmdL
    *
    *   @see CAiwServiceIfBase
    */ 
    virtual void HandleServiceCmdL (
        const TInt &                    aCmdId,
        const CAiwGenericParamList &    aInParamList,
        CAiwGenericParamList &          aOutParamList,
        TUint                           aCmdOptions,
        const MAiwNotifyCallback *      aCallback
        );

    /** InitializeMenuPaneL
    *
    *   @see CAiwServiceIfMenu
    */ 
    virtual void InitializeMenuPaneL (
        CAiwMenuPane &                  aMenuPane,
        TInt                            aIndex,
        TInt                            aCascadeId,
        const CAiwGenericParamList &    aInParamList
        );

    /** HandleMenuCmdL
    *
    *   @see CAiwServiceIfMenu
    */ 
    virtual void HandleMenuCmdL (
        TInt                            aMenuCmdId,
        const CAiwGenericParamList &	aInParamList,
        CAiwGenericParamList &          aOutParamList,
        TUint                           aCmdOptions,
        const MAiwNotifyCallback *      aCallback
        );

    /** HandleServerAppExit
    *
    *   @see MAknServerAppExitObserver
    */ 
    virtual void HandleServerAppExit (TInt aReason);

protected:

    /** Default constructor
	*	
    *	@param -
	*	@return - 
    */ 
	CEditorProvider();

    /** 
    *   @param aInParamList
    *   @return CDesCArray*
    */
	CDesCArray* CheckInputFilesLC ( 
	    const CAiwGenericParamList &    aInParamList
		);

    /** 
    *   @param aOutParamList
    *   @param aCallback
    *   @return -
    */
	void CheckAiwCallBackL ( 
	    CAiwGenericParamList &          aOutParamList,
	    const MAiwNotifyCallback *      aCallback
		);

    /** 
    *   @param aFileName
    *   @param aFsSession
    *   @return -
    */
    void LaunchImageEditorL( 
		const TDesC & 				 aFileName,
	    const CAiwGenericParamList & aInParamList
	    );

    
    /** CheckDRMProtectionL 
	*
	*	Checks if file is DRM protected.
	*
	*	@param aFileName - file name
	*	@return - ETrue if protected, otherwise EFalse, 
	*/
    TBool CheckDRMProtectionL(const TDesC& aFileName) const;

    /** IsSupportedImageFile
    *
    *   @param aDataType
    *   @return  TBool
    */
    TBool IsSupportedImageFile (const TDataType& aDataType) const;

    /** IsJpeg
    *
    *   @param aDataType
    *   @return  TBool
    */
    TBool IsJpeg (const TDataType& aDataType) const;

    /** CheckDiskSpaceL 
	*
	*	Checks if there is enough space in the disk (MMC/PhoneMemory).
	*
    *   @param aFsSession - file server session
	*	@param aFileName - file name
	*	@return - ETrue if enough free memory, otherwise EFalse 
	*/
    TBool CheckDiskSpaceL(RFs& aFsSession, const TDesC& aFileName) const;

    /** CheckDiskSpaceL 
	*
	*	Checks if there is enough space in the disk (MMC/PhoneMemory).
	*
    *   @param aFsSession - file server session
	*	@param aFileList - list of file names
	*	@return - ETrue if enough free memory, otherwise EFalse 
	*/
    TBool CheckDiskSpaceL(RFs& aFsSession, const CDesCArray* aFileList) const;
  
    /** HandleCmdsL
    *
    *   Handle menu and service commands
    *
    *   @see HandleMenuCmdL
    *   @see HandleServiceCmdL
    */ 
    void HandleCmdsL (
        TInt                            aMenuCmdId,
        const CAiwGenericParamList &	aInParamList,
        CAiwGenericParamList &          aOutParamList,
        TUint                           aCmdOptions,
        const MAiwNotifyCallback *      aCallback
        );

private:

    TFileName					iResourceFile;
    RConeResourceLoader			iResLoader;
   	TBool						iResLoaderOpen;
   	TBool						iFileServerConnected;
   	TBool						iUseCallBack;
   	CAknOpenFileService *		iOpenFileService;
   	RFs 						iSharableFS;
    
    const MAiwNotifyCallback*	iCallback;
    CAiwGenericParamList*		iInParamList;
    CAiwGenericParamList*		iOutParamList;

    CDesCArray*					iInputFileList;
    CDesCArray*					iTargetFileList;

};


#endif // EOF EditorServiceProvider.h

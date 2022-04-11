#include <spl/types.h>
#include <spl/debug.h>
#include <spl/stringbuffer.h>
#include "request.h"

#ifdef __TANDEM
#include <sql.h>
#endif

extern bool _writeError(stringbuffer* xml, const char* cp1, const char* cp2);
extern bool _writeErrorCode(stringbuffer* xml, const char* cp1, const int i);

static bool ReportingTypeValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void ReportingType(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TraNotepadPgFwdCursorValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsSqlNotepadInquiryDate")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlNotepadInquiryDate")->m_type == 'I'
	;
}

static void TraNotepadPgFwdCursor(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int32 WsSqlWorkFieldsSqlNotepadSequenceNum = requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_data.m_int;
	int32 WsSqlWorkFieldsSqlNotepadInquiryDate = requestGetItem(req, "WsSqlWorkFieldsSqlNotepadInquiryDate")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TraNotepadPgBwdCursorValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_type == 'I'
	;
}

static void TraNotepadPgBwdCursor(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int32 WsSqlWorkFieldsSqlNotepadSequenceNum = requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TraNpHighSeqNumCursorValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TraNpHighSeqNumCursor(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TraActivityCursorValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TraActivityCursor(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool SicCodeCursorValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void SicCodeCursor(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TrafileSeasnlSel64ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TrafileSeasnlSel64(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 TraRecTraId = 0;
	int64 TraRecLeId = 0;
	int64 TraRecTraMultLocationsRptgInd = 0;
	int64 TraRecTraUseTaxTypeCode = 0;
	int64 TraRecTraPrimarySicCode = 0;
	int64 TraRecTraPrimaryNaicsCode = 0;
	int64 TraRecTraOpenDate = 0;
	int64 TraRecTraCloseDate = 0;
	int64 TraRecTraAdminClosureProcessDate = 0;
	char TraRecTraInitialTaxReturnPrd[3];
	int64 TraRecTraInitialTaxReturnYr = 0;
	char TraRecTraReportingFrequencyCode[2];
	char TraRecTraPrevRptgFreqCode[2];
	int64 TraRecTraRptgFreqEffectiveDate = 0;
	int64 TraRecTraNoAutoFreqChangeInd = 0;
	char TraRecTraMailingLine1Addr[31];
	char TraRecTraMailingLine2Addr[31];
	char TraRecTraMailingCityName[22];
	char TraRecTraMailingAbbrvStateName[3];
	char TraRecTraMailingZipCode[10];
	char TraRecTraMailZipCdCarrierRtNum[5];
	int64 TraRecTraMailingCountryCode = 0;
	int64 TraRecTraAreaCode = 0;
	int64 TraRecTraPhonePrefix = 0;
	int64 TraRecTraPhone = 0;
	int64 TraRecTraPhoneExt = 0;
	int64 TraRecTraFaxAreaCode = 0;
	int64 TraRecTraFaxPhonePrefix = 0;
	int64 TraRecTraFaxPhone = 0;
	int64 TraRecTraAvgTaxRptdAmt = 0;
	int64 TraRecTraLsuTaxCode = 0;
	int64 TraRecTraCountyCityLocCode = 0;
	int64 TraRecTraAppealInProgressInd = 0;
	int64 TraRecTraLitigationInProgressInd = 0;
	char TraRecTraHotelMotelTaxCode[2];
	int64 TraRecTraTaxExclusionCode = 0;
	int64 TraRecTraPermFilingExtensionInd = 0;
	int64 TraRecTraGovFilingExtensionInd = 0;
	int64 TraRecTraRevocationDate = 0;
	int64 TraRecTraHwgFeeInclusionInd = 0;
	int64 TraRecTraPollutionControlProgInd = 0;
	int64 TraRecTraCoGenerationProgInd = 0;
	int64 TraRecTraBusAndOccuptnTaxCrInd = 0;
	int64 TraRecTraNewManufacturerProgInd = 0;
	int64 TraRecTraDistressedAreaProgInd = 0;
	int64 TraRecTraEconomicAssistActInd = 0;
	int64 TraRecTraEftBeginDate = 0;
	int64 TraRecTraEftEndDate = 0;
	int64 TraRecTraFishTaxLicensePrd = 0;
	int64 TraRecTraFishDealerNum = 0;
	int64 TraRecTraCarRentalTaxInd = 0;
	int64 TraRecTraTaxDeductDtlInd = 0;
	int64 TraRecTraInvlntryRegInd = 0;
	int64 TraRecTraHighTechDefrlProgInd = 0;
	int64 TraRecTraDistrsAreaExmptProgInd = 0;
	int64 TraRecTraRideShareCrProgInd = 0;
	int64 TraRecTraHighTechCrProgInd = 0;
	int64 TraRecTraCmputrGenRtnUsersInd = 0;
	int64 TraRecTraTempBusnActvyInd = 0;
	int64 TraRecTraKingCntyFoodTaxInd = 0;
	int64 TraRecTraAluminumSmelterTaxInd = 0;
	int64 TraRecTraElectrlyProcessExmptInd = 0;
	int64 TraRecTraElfBeginDate = 0;
	int64 TraRecTraElfEndDate = 0;
	char TraRecTraElfBusnAuthCode[9];
	int64 TraRecTraStDrctPayPermitInd = 0;
	int64 TraRecTraEnergyAsstnCrProgInd = 0;
	int64 TraRecTraDrctServIndstryCrInd = 0;
	int64 TraRecTraNoInitUseTaxDueInd = 0;
	int64 TraRecTraElecUtilPutCrInd = 0;
	int64 TraRecTraNoLitterTaxDueInd = 0;
	int64 TraRecTraSmallHarvInd = 0;
	int64 TraRecTraEnhncd911TaxSwtchInd = 0;
	int64 TraRecTraEnhncd911TaxWrlssInd = 0;
	int64 TraRecTraPetroleumProdTaxInd = 0;
	int64 TraRecTraQltyMaintFeeInd = 0;
	int64 TraRecTraMotorVhclSaleLeaseInd = 0;
	int64 TraRecTraIntlServActvyBnoCrInd = 0;
	int64 TraRecTraAltFieldBurnBnoCrInd = 0;
	int64 TraRecTraPutOthrTaxAddenInd = 0;
	int64 TraRecTraLsuTaxAddenInd = 0;
	int64 TraRecTraParimutuelWageringInd = 0;
	int64 TraRecTraGmblngFiftykOrMoreInd = 0;
	int64 TraRecTraSolarEnrgySysInd = 0;
	int64 TraRecTraPutInvestRcvryCrInd = 0;
	int64 TraRecRecUpdateNum = 0;
	int64 TraSeasnlRecTraSeasnlTaxRtnPrd1 = 0;
	short WsNullSeasnlRtnPrd1Ind = 0;
	int64 TraSeasnlRecTraSeasnlTaxRtnPrd2 = 0;
	short WsNullSeasnlRtnPrd2Ind = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             TRA.TRA_ID
           , LE_ID
           , TRA_MULT_LOCATIONS_RPTG_IND
           , TRA_USE_TAX_TYPE_CODE
           , TRA_PRIMARY_SIC_CODE
           , TRA_PRIMARY_NAICS_CODE
           , TRA_OPEN_DATE
           , TRA_CLOSE_DATE
           , TRA_ADMIN_CLOSURE_PROCESS_DATE
           , TRA_INITIAL_TAX_RETURN_PRD
           , TRA_INITIAL_TAX_RETURN_YR
           , TRA_REPORTING_FREQUENCY_CODE
           , TRA_PREV_RPTG_FREQ_CODE
           , TRA_RPTG_FREQ_EFFECTIVE_DATE
           , TRA_NO_AUTO_FREQ_CHANGE_IND
           , TRA_MAILING_LINE_1_ADDR
           , TRA_MAILING_LINE_2_ADDR
           , TRA_MAILING_CITY_NAME
           , TRA_MAILING_ABBRV_STATE_NAME
           , TRA_MAILING_ZIP_CODE
           , TRA_MAIL_ZIP_CD_CARRIER_RT_NUM
           , TRA_MAILING_COUNTRY_CODE
           , TRA_AREA_CODE
           , TRA_PHONE_PREFIX
           , TRA_PHONE
           , TRA_PHONE_EXT
           , TRA_FAX_AREA_CODE
           , TRA_FAX_PHONE_PREFIX
           , TRA_FAX_PHONE
           , TRA_AVG_TAX_RPTD_AMT
           , TRA_LSU_TAX_CODE
           , TRA_COUNTY_CITY_LOC_CODE
           , TRA_APPEAL_IN_PROGRESS_IND
           , TRA_LITIGATION_IN_PROGRESS_IND
           , TRA_HOTEL_MOTEL_TAX_CODE
           , TRA_TAX_EXCLUSION_CODE
           , TRA_PERM_FILING_EXTENSION_IND
           , TRA_GOV_FILING_EXTENSION_IND
           , TRA_REVOCATION_DATE
           , TRA_HWG_FEE_INCLUSION_IND
           , TRA_POLLUTION_CONTROL_PROG_IND
           , TRA_CO_GENERATION_PROG_IND
           , TRA_BUS_AND_OCCUPTN_TAX_CR_IND
           , TRA_NEW_MANUFACTURER_PROG_IND
           , TRA_DISTRESSED_AREA_PROG_IND
           , TRA_ECONOMIC_ASSIST_ACT_IND
           , TRA_EFT_BEGIN_DATE
           , TRA_EFT_END_DATE
           , TRA_FISH_TAX_LICENSE_PRD
           , TRA_FISH_DEALER_NUM
           , TRA_CAR_RENTAL_TAX_IND
           , TRA_TAX_DEDUCT_DTL_IND
           , TRA_INVLNTRY_REG_IND
           , TRA_HIGH_TECH_DEFRL_PROG_IND
           , TRA_DISTRS_AREA_EXMPT_PROG_IND
           , TRA_RIDE_SHARE_CR_PROG_IND
           , TRA_HIGH_TECH_CR_PROG_IND
           , TRA_CMPUTR_GEN_RTN_USERS_IND
           , TRA_TEMP_BUSN_ACTVY_IND
           , TRA_KING_CNTY_FOOD_TAX_IND
           , TRA_ALUMINUM_SMELTER_TAX_IND
           , TRA_ELECTRLY_PROCESS_EXMPT_IND
           , TRA_ELF_BEGIN_DATE
           , TRA_ELF_END_DATE
           , TRA_ELF_BUSN_AUTH_CODE
           , TRA_ST_DRCT_PAY_PERMIT_IND
           , TRA_ENERGY_ASSTN_CR_PROG_IND
           , TRA_DRCT_SERV_INDSTRY_CR_IND
           , TRA_NO_INIT_USE_TAX_DUE_IND
           , TRA_ELEC_UTIL_PUT_CR_IND
           , TRA_NO_LITTER_TAX_DUE_IND
           , TRA_SMALL_HARV_IND
           , TRA_ENHNCD_911_TAX_SWTCH_IND
           , TRA_ENHNCD_911_TAX_WRLSS_IND
           , TRA_PETROLEUM_PROD_TAX_IND
           , TRA_QLTY_MAINT_FEE_IND
           , TRA_MOTOR_VHCL_SALE_LEASE_IND
           , TRA_INTL_SERV_ACTVY_BNO_CR_IND
           , TRA_ALT_FIELD_BURN_BNO_CR_IND
           , TRA_PUT_OTHR_TAX_ADDEN_IND
           , TRA_LSU_TAX_ADDEN_IND
           , TRA_PARIMUTUEL_WAGERING_IND
           , TRA_GMBLNG_FIFTYK_OR_MORE_IND
           , TRA_SOLAR_ENRGY_SYS_IND
           , TRA_PUT_INVEST_RCVRY_CR_IND
           , REC_UPDATE_NUM
           , TRA_SEASNL_TAX_RTN_PRD_1
           , TRA_SEASNL_TAX_RTN_PRD_2
         INTO
             :TraRecTraId                         
           , :TraRecLeId                          
           , :TraRecTraMultLocationsRptgInd    
           , :TraRecTraUseTaxTypeCode          
           , :TraRecTraPrimarySicCode           
           , :TraRecTraPrimaryNaicsCode         
           , :TraRecTraOpenDate                  
           , :TraRecTraCloseDate                 
           , :TraRecTraAdminClosureProcessDate 
           , :TraRecTraInitialTaxReturnPrd     
           , :TraRecTraInitialTaxReturnYr      
           , :TraRecTraReportingFrequencyCode   
           , :TraRecTraPrevRptgFreqCode        
           , :TraRecTraRptgFreqEffectiveDate   
           , :TraRecTraNoAutoFreqChangeInd    
           , :TraRecTraMailingLine1Addr        
           , :TraRecTraMailingLine2Addr        
           , :TraRecTraMailingCityName          
           , :TraRecTraMailingAbbrvStateName   
           , :TraRecTraMailingZipCode           
           , :TraRecTraMailZipCdCarrierRtNum 
           , :TraRecTraMailingCountryCode       
           , :TraRecTraAreaCode                  
           , :TraRecTraPhonePrefix               
           , :TraRecTraPhone                      
           , :TraRecTraPhoneExt                  
           , :TraRecTraFaxAreaCode              
           , :TraRecTraFaxPhonePrefix           
           , :TraRecTraFaxPhone                  
           , :TraRecTraAvgTaxRptdAmt           
           , :TraRecTraLsuTaxCode               
           , :TraRecTraCountyCityLocCode       
           , :TraRecTraAppealInProgressInd     
           , :TraRecTraLitigationInProgressInd 
           , :TraRecTraHotelMotelTaxCode       
           , :TraRecTraTaxExclusionCode         
           , :TraRecTraPermFilingExtensionInd  
           , :TraRecTraGovFilingExtensionInd   
           , :TraRecTraRevocationDate            
           , :TraRecTraHwgFeeInclusionInd      
           , :TraRecTraPollutionControlProgInd 
           , :TraRecTraCoGenerationProgInd     
           , :TraRecTraBusAndOccuptnTaxCrInd 
           , :TraRecTraNewManufacturerProgInd  
           , :TraRecTraDistressedAreaProgInd   
           , :TraRecTraEconomicAssistActInd    
           , :TraRecTraEftBeginDate             
           , :TraRecTraEftEndDate               
           , :TraRecTraFishTaxLicensePrd       
           , :TraRecTraFishDealerNum            
           , :TraRecTraCarRentalTaxInd         
           , :TraRecTraTaxDeductDtlInd         
           , :TraRecTraInvlntryRegInd           
           , :TraRecTraHighTechDefrlProgInd   
           , :TraRecTraDistrsAreaExmptProgInd 
           , :TraRecTraRideShareCrProgInd     
           , :TraRecTraHighTechCrProgInd      
           , :TraRecTraCmputrGenRtnUsersInd   
           , :TraRecTraTempBusnActvyInd        
           , :TraRecTraKingCntyFoodTaxInd     
           , :TraRecTraAluminumSmelterTaxInd   
           , :TraRecTraElectrlyProcessExmptInd 
           , :TraRecTraElfBeginDate             
           , :TraRecTraElfEndDate               
           , :TraRecTraElfBusnAuthCode         
           , :TraRecTraStDrctPayPermitInd     
           , :TraRecTraEnergyAsstnCrProgInd   
           , :TraRecTraDrctServIndstryCrInd   
           , :TraRecTraNoInitUseTaxDueInd    
           , :TraRecTraElecUtilPutCrInd       
           , :TraRecTraNoLitterTaxDueInd      
           , :TraRecTraSmallHarvInd             
           , :TraRecTraEnhncd911TaxSwtchInd   
           , :TraRecTraEnhncd911TaxWrlssInd   
           , :TraRecTraPetroleumProdTaxInd     
           , :TraRecTraQltyMaintFeeInd         
           , :TraRecTraMotorVhclSaleLeaseInd  
           , :TraRecTraIntlServActvyBnoCrInd 
           , :TraRecTraAltFieldBurnBnoCrInd  
           , :TraRecTraPutOthrTaxAddenInd     
           , :TraRecTraLsuTaxAddenInd          
           , :TraRecTraParimutuelWageringInd    
           , :TraRecTraGmblngFiftykOrMoreInd  
           , :TraRecTraSolarEnrgySysInd        
           , :TraRecTraPutInvestRcvryCrInd    
           , :TraRecRecUpdateNum                 
           , :TraSeasnlRecTraSeasnlTaxRtnPrd1       
              INDICATOR :WsNullSeasnlRtnPrd1Ind
           , :TraSeasnlRecTraSeasnlTaxRtnPrd2       
              INDICATOR :WsNullSeasnlRtnPrd2Ind
         FROM
             =TRAFILE  TRA
         LEFT JOIN =SEASNL SSNL  ON TRA.TRA_ID = SSNL.TRA_ID
                                 AND TRA_SEASNL_ACTV_IND = 1
         WHERE
             TRA.TRA_ID = :CiTraDetailMsgTraId 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' TraRecTraId='");
	stringbufferAppendLong(xml, TraRecTraId);
	stringbufferAppend(xml, "' TraRecLeId='");
	stringbufferAppendLong(xml, TraRecLeId);
	stringbufferAppend(xml, "' TraRecTraMultLocationsRptgInd='");
	stringbufferAppendLong(xml, TraRecTraMultLocationsRptgInd);
	stringbufferAppend(xml, "' TraRecTraUseTaxTypeCode='");
	stringbufferAppendLong(xml, TraRecTraUseTaxTypeCode);
	stringbufferAppend(xml, "' TraRecTraPrimarySicCode='");
	stringbufferAppendLong(xml, TraRecTraPrimarySicCode);
	stringbufferAppend(xml, "' TraRecTraPrimaryNaicsCode='");
	stringbufferAppendLong(xml, TraRecTraPrimaryNaicsCode);
	stringbufferAppend(xml, "' TraRecTraOpenDate='");
	stringbufferAppendLong(xml, TraRecTraOpenDate);
	stringbufferAppend(xml, "' TraRecTraCloseDate='");
	stringbufferAppendLong(xml, TraRecTraCloseDate);
	stringbufferAppend(xml, "' TraRecTraAdminClosureProcessDate='");
	stringbufferAppendLong(xml, TraRecTraAdminClosureProcessDate);
	stringbufferAppend(xml, "' TraRecTraInitialTaxReturnPrd='");
	stringbufferAppend(xml, TraRecTraInitialTaxReturnPrd);
	stringbufferAppend(xml, "' TraRecTraInitialTaxReturnYr='");
	stringbufferAppendLong(xml, TraRecTraInitialTaxReturnYr);
	stringbufferAppend(xml, "' TraRecTraReportingFrequencyCode='");
	stringbufferAppend(xml, TraRecTraReportingFrequencyCode);
	stringbufferAppend(xml, "' TraRecTraPrevRptgFreqCode='");
	stringbufferAppend(xml, TraRecTraPrevRptgFreqCode);
	stringbufferAppend(xml, "' TraRecTraRptgFreqEffectiveDate='");
	stringbufferAppendLong(xml, TraRecTraRptgFreqEffectiveDate);
	stringbufferAppend(xml, "' TraRecTraNoAutoFreqChangeInd='");
	stringbufferAppendLong(xml, TraRecTraNoAutoFreqChangeInd);
	stringbufferAppend(xml, "' TraRecTraMailingLine1Addr='");
	stringbufferAppend(xml, TraRecTraMailingLine1Addr);
	stringbufferAppend(xml, "' TraRecTraMailingLine2Addr='");
	stringbufferAppend(xml, TraRecTraMailingLine2Addr);
	stringbufferAppend(xml, "' TraRecTraMailingCityName='");
	stringbufferAppend(xml, TraRecTraMailingCityName);
	stringbufferAppend(xml, "' TraRecTraMailingAbbrvStateName='");
	stringbufferAppend(xml, TraRecTraMailingAbbrvStateName);
	stringbufferAppend(xml, "' TraRecTraMailingZipCode='");
	stringbufferAppend(xml, TraRecTraMailingZipCode);
	stringbufferAppend(xml, "' TraRecTraMailZipCdCarrierRtNum='");
	stringbufferAppend(xml, TraRecTraMailZipCdCarrierRtNum);
	stringbufferAppend(xml, "' TraRecTraMailingCountryCode='");
	stringbufferAppendLong(xml, TraRecTraMailingCountryCode);
	stringbufferAppend(xml, "' TraRecTraAreaCode='");
	stringbufferAppendLong(xml, TraRecTraAreaCode);
	stringbufferAppend(xml, "' TraRecTraPhonePrefix='");
	stringbufferAppendLong(xml, TraRecTraPhonePrefix);
	stringbufferAppend(xml, "' TraRecTraPhone='");
	stringbufferAppendLong(xml, TraRecTraPhone);
	stringbufferAppend(xml, "' TraRecTraPhoneExt='");
	stringbufferAppendLong(xml, TraRecTraPhoneExt);
	stringbufferAppend(xml, "' TraRecTraFaxAreaCode='");
	stringbufferAppendLong(xml, TraRecTraFaxAreaCode);
	stringbufferAppend(xml, "' TraRecTraFaxPhonePrefix='");
	stringbufferAppendLong(xml, TraRecTraFaxPhonePrefix);
	stringbufferAppend(xml, "' TraRecTraFaxPhone='");
	stringbufferAppendLong(xml, TraRecTraFaxPhone);
	stringbufferAppend(xml, "' TraRecTraAvgTaxRptdAmt='");
	stringbufferAppendLong(xml, TraRecTraAvgTaxRptdAmt);
	stringbufferAppend(xml, "' TraRecTraLsuTaxCode='");
	stringbufferAppendLong(xml, TraRecTraLsuTaxCode);
	stringbufferAppend(xml, "' TraRecTraCountyCityLocCode='");
	stringbufferAppendLong(xml, TraRecTraCountyCityLocCode);
	stringbufferAppend(xml, "' TraRecTraAppealInProgressInd='");
	stringbufferAppendLong(xml, TraRecTraAppealInProgressInd);
	stringbufferAppend(xml, "' TraRecTraLitigationInProgressInd='");
	stringbufferAppendLong(xml, TraRecTraLitigationInProgressInd);
	stringbufferAppend(xml, "' TraRecTraHotelMotelTaxCode='");
	stringbufferAppend(xml, TraRecTraHotelMotelTaxCode);
	stringbufferAppend(xml, "' TraRecTraTaxExclusionCode='");
	stringbufferAppendLong(xml, TraRecTraTaxExclusionCode);
	stringbufferAppend(xml, "' TraRecTraPermFilingExtensionInd='");
	stringbufferAppendLong(xml, TraRecTraPermFilingExtensionInd);
	stringbufferAppend(xml, "' TraRecTraGovFilingExtensionInd='");
	stringbufferAppendLong(xml, TraRecTraGovFilingExtensionInd);
	stringbufferAppend(xml, "' TraRecTraRevocationDate='");
	stringbufferAppendLong(xml, TraRecTraRevocationDate);
	stringbufferAppend(xml, "' TraRecTraHwgFeeInclusionInd='");
	stringbufferAppendLong(xml, TraRecTraHwgFeeInclusionInd);
	stringbufferAppend(xml, "' TraRecTraPollutionControlProgInd='");
	stringbufferAppendLong(xml, TraRecTraPollutionControlProgInd);
	stringbufferAppend(xml, "' TraRecTraCoGenerationProgInd='");
	stringbufferAppendLong(xml, TraRecTraCoGenerationProgInd);
	stringbufferAppend(xml, "' TraRecTraBusAndOccuptnTaxCrInd='");
	stringbufferAppendLong(xml, TraRecTraBusAndOccuptnTaxCrInd);
	stringbufferAppend(xml, "' TraRecTraNewManufacturerProgInd='");
	stringbufferAppendLong(xml, TraRecTraNewManufacturerProgInd);
	stringbufferAppend(xml, "' TraRecTraDistressedAreaProgInd='");
	stringbufferAppendLong(xml, TraRecTraDistressedAreaProgInd);
	stringbufferAppend(xml, "' TraRecTraEconomicAssistActInd='");
	stringbufferAppendLong(xml, TraRecTraEconomicAssistActInd);
	stringbufferAppend(xml, "' TraRecTraEftBeginDate='");
	stringbufferAppendLong(xml, TraRecTraEftBeginDate);
	stringbufferAppend(xml, "' TraRecTraEftEndDate='");
	stringbufferAppendLong(xml, TraRecTraEftEndDate);
	stringbufferAppend(xml, "' TraRecTraFishTaxLicensePrd='");
	stringbufferAppendLong(xml, TraRecTraFishTaxLicensePrd);
	stringbufferAppend(xml, "' TraRecTraFishDealerNum='");
	stringbufferAppendLong(xml, TraRecTraFishDealerNum);
	stringbufferAppend(xml, "' TraRecTraCarRentalTaxInd='");
	stringbufferAppendLong(xml, TraRecTraCarRentalTaxInd);
	stringbufferAppend(xml, "' TraRecTraTaxDeductDtlInd='");
	stringbufferAppendLong(xml, TraRecTraTaxDeductDtlInd);
	stringbufferAppend(xml, "' TraRecTraInvlntryRegInd='");
	stringbufferAppendLong(xml, TraRecTraInvlntryRegInd);
	stringbufferAppend(xml, "' TraRecTraHighTechDefrlProgInd='");
	stringbufferAppendLong(xml, TraRecTraHighTechDefrlProgInd);
	stringbufferAppend(xml, "' TraRecTraDistrsAreaExmptProgInd='");
	stringbufferAppendLong(xml, TraRecTraDistrsAreaExmptProgInd);
	stringbufferAppend(xml, "' TraRecTraRideShareCrProgInd='");
	stringbufferAppendLong(xml, TraRecTraRideShareCrProgInd);
	stringbufferAppend(xml, "' TraRecTraHighTechCrProgInd='");
	stringbufferAppendLong(xml, TraRecTraHighTechCrProgInd);
	stringbufferAppend(xml, "' TraRecTraCmputrGenRtnUsersInd='");
	stringbufferAppendLong(xml, TraRecTraCmputrGenRtnUsersInd);
	stringbufferAppend(xml, "' TraRecTraTempBusnActvyInd='");
	stringbufferAppendLong(xml, TraRecTraTempBusnActvyInd);
	stringbufferAppend(xml, "' TraRecTraKingCntyFoodTaxInd='");
	stringbufferAppendLong(xml, TraRecTraKingCntyFoodTaxInd);
	stringbufferAppend(xml, "' TraRecTraAluminumSmelterTaxInd='");
	stringbufferAppendLong(xml, TraRecTraAluminumSmelterTaxInd);
	stringbufferAppend(xml, "' TraRecTraElectrlyProcessExmptInd='");
	stringbufferAppendLong(xml, TraRecTraElectrlyProcessExmptInd);
	stringbufferAppend(xml, "' TraRecTraElfBeginDate='");
	stringbufferAppendLong(xml, TraRecTraElfBeginDate);
	stringbufferAppend(xml, "' TraRecTraElfEndDate='");
	stringbufferAppendLong(xml, TraRecTraElfEndDate);
	stringbufferAppend(xml, "' TraRecTraElfBusnAuthCode='");
	stringbufferAppend(xml, TraRecTraElfBusnAuthCode);
	stringbufferAppend(xml, "' TraRecTraStDrctPayPermitInd='");
	stringbufferAppendLong(xml, TraRecTraStDrctPayPermitInd);
	stringbufferAppend(xml, "' TraRecTraEnergyAsstnCrProgInd='");
	stringbufferAppendLong(xml, TraRecTraEnergyAsstnCrProgInd);
	stringbufferAppend(xml, "' TraRecTraDrctServIndstryCrInd='");
	stringbufferAppendLong(xml, TraRecTraDrctServIndstryCrInd);
	stringbufferAppend(xml, "' TraRecTraNoInitUseTaxDueInd='");
	stringbufferAppendLong(xml, TraRecTraNoInitUseTaxDueInd);
	stringbufferAppend(xml, "' TraRecTraElecUtilPutCrInd='");
	stringbufferAppendLong(xml, TraRecTraElecUtilPutCrInd);
	stringbufferAppend(xml, "' TraRecTraNoLitterTaxDueInd='");
	stringbufferAppendLong(xml, TraRecTraNoLitterTaxDueInd);
	stringbufferAppend(xml, "' TraRecTraSmallHarvInd='");
	stringbufferAppendLong(xml, TraRecTraSmallHarvInd);
	stringbufferAppend(xml, "' TraRecTraEnhncd911TaxSwtchInd='");
	stringbufferAppendLong(xml, TraRecTraEnhncd911TaxSwtchInd);
	stringbufferAppend(xml, "' TraRecTraEnhncd911TaxWrlssInd='");
	stringbufferAppendLong(xml, TraRecTraEnhncd911TaxWrlssInd);
	stringbufferAppend(xml, "' TraRecTraPetroleumProdTaxInd='");
	stringbufferAppendLong(xml, TraRecTraPetroleumProdTaxInd);
	stringbufferAppend(xml, "' TraRecTraQltyMaintFeeInd='");
	stringbufferAppendLong(xml, TraRecTraQltyMaintFeeInd);
	stringbufferAppend(xml, "' TraRecTraMotorVhclSaleLeaseInd='");
	stringbufferAppendLong(xml, TraRecTraMotorVhclSaleLeaseInd);
	stringbufferAppend(xml, "' TraRecTraIntlServActvyBnoCrInd='");
	stringbufferAppendLong(xml, TraRecTraIntlServActvyBnoCrInd);
	stringbufferAppend(xml, "' TraRecTraAltFieldBurnBnoCrInd='");
	stringbufferAppendLong(xml, TraRecTraAltFieldBurnBnoCrInd);
	stringbufferAppend(xml, "' TraRecTraPutOthrTaxAddenInd='");
	stringbufferAppendLong(xml, TraRecTraPutOthrTaxAddenInd);
	stringbufferAppend(xml, "' TraRecTraLsuTaxAddenInd='");
	stringbufferAppendLong(xml, TraRecTraLsuTaxAddenInd);
	stringbufferAppend(xml, "' TraRecTraParimutuelWageringInd='");
	stringbufferAppendLong(xml, TraRecTraParimutuelWageringInd);
	stringbufferAppend(xml, "' TraRecTraGmblngFiftykOrMoreInd='");
	stringbufferAppendLong(xml, TraRecTraGmblngFiftykOrMoreInd);
	stringbufferAppend(xml, "' TraRecTraSolarEnrgySysInd='");
	stringbufferAppendLong(xml, TraRecTraSolarEnrgySysInd);
	stringbufferAppend(xml, "' TraRecTraPutInvestRcvryCrInd='");
	stringbufferAppendLong(xml, TraRecTraPutInvestRcvryCrInd);
	stringbufferAppend(xml, "' TraRecRecUpdateNum='");
	stringbufferAppendLong(xml, TraRecRecUpdateNum);
	stringbufferAppend(xml, "' TraSeasnlRecTraSeasnlTaxRtnPrd1='");
	stringbufferAppendLong(xml, TraSeasnlRecTraSeasnlTaxRtnPrd1);
	stringbufferAppend(xml, "' TraSeasnlRecTraSeasnlTaxRtnPrd2='");
	stringbufferAppendLong(xml, TraSeasnlRecTraSeasnlTaxRtnPrd2);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool CigTbcoLicValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void CigTbcoLic(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;

	EXEC SQL END DECLARE SECTION;

#else
#endif
}

static bool TradateSel66ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "TRADATE_RECTrdttypTypeCode")
		&& requestGetItem(req, "TRADATE_RECTrdttypTypeCode")->m_type == 'L'
	;
}

static void TradateSel66(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 TRADATE_RECTrdttypTypeCode = requestGetItem(req, "TRADATE_RECTrdttypTypeCode")->m_data.m_long;
	int64 TRADATE_RECTradateValueDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT TRADATE_VALUE_DATE
           INTO :TRADATE_RECTradateValueDate 
           FROM =TRADATE
          WHERE TRA_ID = :CiTraDetailMsgTraId 
            AND TRDTTYP_TYPE_CODE = :TRADATE_RECTrdttypTypeCode 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' TRADATE_RECTrdttypTypeCode='");
	stringbufferAppendLong(xml, TRADATE_RECTrdttypTypeCode);
	stringbufferAppend(xml, "' TRADATE_RECTradateValueDate='");
	stringbufferAppendLong(xml, TRADATE_RECTradateValueDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool LefileSel67ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void LefileSel67(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 LeRecLeId = 0;
	int64 LeRecLeUbiAssignedDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             LE_ID
           , LE_UBI_ASSIGNED_DATE
         INTO
             :LeRecLeId                 
           , :LeRecLeUbiAssignedDate  
         FROM
             =LEFILE
         WHERE
             LE_ID = :CiTraDetailMsgTraId 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' LeRecLeId='");
	stringbufferAppendLong(xml, LeRecLeId);
	stringbufferAppend(xml, "' LeRecLeUbiAssignedDate='");
	stringbufferAppendLong(xml, LeRecLeUbiAssignedDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool AudasgndSel68ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void AudasgndSel68(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 AudasgndRecAudasgndAuditNum = 0;
	int64 AudasgndRecTreqActivityJtsDate = 0;
	int64 AudasgndRecAudasgndDate = 0;
	int64 AudasgndRecAudasgndByUnitCode = 0;
	int64 AudasgndRecAudasgndToSfaNum = 0;
	int64 AudasgndRecAudasgndToAudtrId = 0;
	int64 AudasgndRecAudasgndExamFromPrd = 0;
	int64 AudasgndRecAudasgndExamThruPrd = 0;
	int64 AudasgndRecAudasgndConsultExprnDate = 0;
	int64 AudasgndRecAudasgndConsultInd = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
       SELECT
               AUDASGND_AUDIT_NUM
             , TREQ_ACTIVITY_JTS_DATE
             , AUDASGND_DATE
             , AUDASGND_BY_UNIT_CODE
             , AUDASGND_TO_SFA_NUM
             , AUDASGND_TO_AUDTR_ID
             , AUDASGND_EXAM_FROM_PRD
             , AUDASGND_EXAM_THRU_PRD
             , AUDASGND_CONSULT_EXPRN_DATE
             , AUDASGND_CONSULT_IND
         INTO
               :AudasgndRecAudasgndAuditNum          
             , :AudasgndRecTreqActivityJtsDate      
             , :AudasgndRecAudasgndDate               
             , :AudasgndRecAudasgndByUnitCode       
             , :AudasgndRecAudasgndToSfaNum         
             , :AudasgndRecAudasgndToAudtrId        
             , :AudasgndRecAudasgndExamFromPrd      
             , :AudasgndRecAudasgndExamThruPrd      
             , :AudasgndRecAudasgndConsultExprnDate 
             , :AudasgndRecAudasgndConsultInd        
         FROM =AUDASGND
        WHERE TRA_ID = :CiTraDetailMsgTraId 
        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' AudasgndRecAudasgndAuditNum='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndAuditNum);
	stringbufferAppend(xml, "' AudasgndRecTreqActivityJtsDate='");
	stringbufferAppendLong(xml, AudasgndRecTreqActivityJtsDate);
	stringbufferAppend(xml, "' AudasgndRecAudasgndDate='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndDate);
	stringbufferAppend(xml, "' AudasgndRecAudasgndByUnitCode='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndByUnitCode);
	stringbufferAppend(xml, "' AudasgndRecAudasgndToSfaNum='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndToSfaNum);
	stringbufferAppend(xml, "' AudasgndRecAudasgndToAudtrId='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndToAudtrId);
	stringbufferAppend(xml, "' AudasgndRecAudasgndExamFromPrd='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndExamFromPrd);
	stringbufferAppend(xml, "' AudasgndRecAudasgndExamThruPrd='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndExamThruPrd);
	stringbufferAppend(xml, "' AudasgndRecAudasgndConsultExprnDate='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndConsultExprnDate);
	stringbufferAppend(xml, "' AudasgndRecAudasgndConsultInd='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndConsultInd);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool DoruservSel69ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "AudasgndRecAudasgndToAudtrId")
		&& requestGetItem(req, "AudasgndRecAudasgndToAudtrId")->m_type == 'L'
	;
}

static void DoruservSel69(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 AudasgndRecAudasgndToAudtrId = requestGetItem(req, "AudasgndRecAudasgndToAudtrId")->m_data.m_long;
	char WsAuditorNameEmpFirstName[21];
	char WsAuditorNameEmpLastName[21];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
            DORUSER_FIRST_NAME,
            DORUSER_LAST_NAME
         INTO
           :WsAuditorNameEmpFirstName ,
           :WsAuditorNameEmpLastName  
         FROM =DORUSERV
         WHERE DORUSER_ID = :AudasgndRecAudasgndToAudtrId 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "AudasgndRecAudasgndToAudtrId='");
	stringbufferAppendLong(xml, AudasgndRecAudasgndToAudtrId);
	stringbufferAppend(xml, "' WsAuditorNameEmpFirstName='");
	stringbufferAppend(xml, WsAuditorNameEmpFirstName);
	stringbufferAppend(xml, "' WsAuditorNameEmpLastName='");
	stringbufferAppend(xml, WsAuditorNameEmpLastName);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool SeasnlSel70ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraRecTraId")
		&& requestGetItem(req, "TraRecTraId")->m_type == 'L'
	;
}

static void SeasnlSel70(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraRecTraId = requestGetItem(req, "TraRecTraId")->m_data.m_long;
	int64 TraSeasnlRecTraSeasnlTaxRtnPrd1 = 0;
	int64 TraSeasnlRecTraSeasnlTaxRtnPrd2 = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
              TRA_SEASNL_TAX_RTN_PRD_1,
              TRA_SEASNL_TAX_RTN_PRD_2
         INTO
             :TraSeasnlRecTraSeasnlTaxRtnPrd1       ,
             :TraSeasnlRecTraSeasnlTaxRtnPrd2       
          FROM
              =SEASNL
         WHERE
              TRA_ID = :TraRecTraId  
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraRecTraId='");
	stringbufferAppendLong(xml, TraRecTraId);
	stringbufferAppend(xml, "' TraSeasnlRecTraSeasnlTaxRtnPrd1='");
	stringbufferAppendLong(xml, TraSeasnlRecTraSeasnlTaxRtnPrd1);
	stringbufferAppend(xml, "' TraSeasnlRecTraSeasnlTaxRtnPrd2='");
	stringbufferAppendLong(xml, TraSeasnlRecTraSeasnlTaxRtnPrd2);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TrarptgSel71ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TrarptgSel71(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 WsSqlWorkFieldsSqlExciseTaxCnt = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             COUNT (*)
        INTO
             :WsSqlWorkFieldsSqlExciseTaxCnt
        FROM
             =TRARPTG
        WHERE
             TRA_ID = :CiTraDetailMsgTraId 
        AND
             TRA_REPORTING_TYPE_CODE = 1

        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsSqlExciseTaxCnt='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsSqlExciseTaxCnt);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TrarptgSel72ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TrarptgSel72(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 WsSqlWorkFieldsSqlLeaseholdTaxCnt = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             COUNT (*)
        INTO
             :WsSqlWorkFieldsSqlLeaseholdTaxCnt
        FROM
             =TRARPTG
        WHERE
             TRA_ID = :CiTraDetailMsgTraId 
        AND
             TRA_REPORTING_TYPE_CODE = 5

        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsSqlLeaseholdTaxCnt='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsSqlLeaseholdTaxCnt);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TradateSel73ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TradateSel73(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 TRADATE_RECTradateValueDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT TRADATE_VALUE_DATE
           INTO :TRADATE_RECTradateValueDate 
           FROM =TRADATE
          WHERE TRA_ID = :CiTraDetailMsgTraId 
            AND TRDTTYP_TYPE_CODE =  17
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' TRADATE_RECTradateValueDate='");
	stringbufferAppendLong(xml, TRADATE_RECTradateValueDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool SstpSel74ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsCurrentDateAreaWsCurrentDateComp")
		&& requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsWsHighestSeqNum")
		&& requestGetItem(req, "WsSqlWorkFieldsWsHighestSeqNum")->m_type == 'I'
	;
}

static void SstpSel74(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int32 WsCurrentDateAreaWsCurrentDateComp = requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_data.m_int;
	int32 WsSqlWorkFieldsWsHighestSeqNum = requestGetItem(req, "WsSqlWorkFieldsWsHighestSeqNum")->m_data.m_int;
	char SstpRecSstpId[10];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
              SSTP_ID
         INTO
              :SstpRecSstpId 
         FROM =SSTP
         WHERE
              SSTP.TRA_ID = :CiTraDetailMsgTraId 
         AND
              (SSTP_END_DATE = 0 OR
               SSTP_END_DATE >= :WsCurrentDateAreaWsCurrentDateComp)
         AND
              SSTP_SEQ_NUM = :WsSqlWorkFieldsWsHighestSeqNum

       FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsCurrentDateAreaWsCurrentDateComp='");
	stringbufferAppendInt(xml, WsCurrentDateAreaWsCurrentDateComp);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsHighestSeqNum='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsHighestSeqNum);
	stringbufferAppend(xml, "' SstpRecSstpId='");
	stringbufferAppend(xml, SstpRecSstpId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool SstpSel75ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void SstpSel75(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 SstpRecSstpSeqNum = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
               SELECT
                    SSTP_SEQ_NUM
                INTO
                   :SstpRecSstpSeqNum  
                FROM
                   =SSTP
               WHERE
                    SSTP.TRA_ID = :CiTraDetailMsgTraId 
                AND
                    SSTP_SEQ_NUM = ( SELECT
                                         MAX (SSTP_SEQ_NUM)
                                            FROM
                                                =SSTP
                                            WHERE
                                                SSTP.TRA_ID = :CiTraDetailMsgTraId 
                                            FOR BROWSE ACCESS )
               FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' SstpRecSstpSeqNum='");
	stringbufferAppendLong(xml, SstpRecSstpSeqNum);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool LefileSel76ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraRecLeId")
		&& requestGetItem(req, "TraRecLeId")->m_type == 'L'
	;
}

static void LefileSel76(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraRecLeId = requestGetItem(req, "TraRecLeId")->m_data.m_long;
	int64 LeRecLeTypeCode = 0;
	int64 LeRecLeUbiAssignedDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             LE_TYPE_CODE
           , LE_UBI_ASSIGNED_DATE
         INTO
             :LeRecLeTypeCode                   
           , :LeRecLeUbiAssignedDate           
         FROM
             =LEFILE
         WHERE
             LE_ID = :TraRecLeId 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraRecLeId='");
	stringbufferAppendLong(xml, TraRecLeId);
	stringbufferAppend(xml, "' LeRecLeTypeCode='");
	stringbufferAppendLong(xml, LeRecLeTypeCode);
	stringbufferAppend(xml, "' LeRecLeUbiAssignedDate='");
	stringbufferAppendLong(xml, LeRecLeUbiAssignedDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool LetypeSel77ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "LeRecLeTypeCode")
		&& requestGetItem(req, "LeRecLeTypeCode")->m_type == 'L'
	;
}

static void LetypeSel77(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 LeRecLeTypeCode = requestGetItem(req, "LeRecLeTypeCode")->m_data.m_long;
	char LetypeRecLetypeLongLit[18];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT LETYPE_LONG_LIT
            INTO :LetypeRecLetypeLongLit 
            FROM =LETYPE
           WHERE LE_TYPE_CODE = :LeRecLeTypeCode 
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "LeRecLeTypeCode='");
	stringbufferAppendLong(xml, LeRecLeTypeCode);
	stringbufferAppend(xml, "' LetypeRecLetypeLongLit='");
	stringbufferAppend(xml, LetypeRecLetypeLongLit);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool UncollSel78ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraRecTraId")
		&& requestGetItem(req, "TraRecTraId")->m_type == 'L'
	;
}

static void UncollSel78(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraRecTraId = requestGetItem(req, "TraRecTraId")->m_data.m_long;
	int64 UncollRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             TRA_ID
        INTO
             :UncollRecTraId       
        FROM
             =UNCOLL
        WHERE
             TRA_ID = :TraRecTraId  
          AND
             UNCOLL_TO_SQLDTS > CURRENT
        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraRecTraId='");
	stringbufferAppendLong(xml, TraRecTraId);
	stringbufferAppend(xml, "' UncollRecTraId='");
	stringbufferAppendLong(xml, UncollRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TraemailSel79ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraRecTraId")
		&& requestGetItem(req, "TraRecTraId")->m_type == 'L'
	;
}

static void TraemailSel79(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraRecTraId = requestGetItem(req, "TraRecTraId")->m_data.m_long;
	char TraEmailRecTraEmailAddr[251];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             TRA_EMAIL_ADDR
        INTO
             :TraEmailRecTraEmailAddr    
        FROM
             =TRAEMAIL
        WHERE
             TRA_ID = :TraRecTraId  
        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraRecTraId='");
	stringbufferAppendLong(xml, TraRecTraId);
	stringbufferAppend(xml, "' TraEmailRecTraEmailAddr='");
	stringbufferAppend(xml, TraEmailRecTraEmailAddr);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool BrcrfileSel80ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsWsSqlBrcrId")
		&& requestGetItem(req, "WsSqlWorkFieldsWsSqlBrcrId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsWsSqlBrcrSourceEntityCode")
		&& requestGetItem(req, "WsSqlWorkFieldsWsSqlBrcrSourceEntityCode")->m_type == 'L'
	;
}

static void BrcrfileSel80(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsWsSqlBrcrId = requestGetItem(req, "WsSqlWorkFieldsWsSqlBrcrId")->m_data.m_int;
	int64 WsSqlWorkFieldsWsSqlBrcrSourceEntityCode = requestGetItem(req, "WsSqlWorkFieldsWsSqlBrcrSourceEntityCode")->m_data.m_long;
	char BrcrRecBrcrName[46];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             BRCR_NAME
        INTO
             :BrcrRecBrcrName    
        FROM
             =BRCRFILE  BRCR
        WHERE
             BRCR.BRCR_ID = :WsSqlWorkFieldsWsSqlBrcrId
          AND
             BRCR.BRCR_SOURCE_ENTITY_CODE = :WsSqlWorkFieldsWsSqlBrcrSourceEntityCode
        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsWsSqlBrcrId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsSqlBrcrId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsSqlBrcrSourceEntityCode='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsWsSqlBrcrSourceEntityCode);
	stringbufferAppend(xml, "' BrcrRecBrcrName='");
	stringbufferAppend(xml, BrcrRecBrcrName);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool SictitleSel81ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "SicRecSicAlphaCode")
		&& requestGetItem(req, "SicRecSicAlphaCode")->m_type == 'S'
	;
}

static void SictitleSel81(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	char SicRecSicAlphaCode[5];
	char SicRecSicCodeShortTitle[41];

	EXEC SQL END DECLARE SECTION;

	StrCpyLen(SicRecSicAlphaCode, requestGetItem(req, "SicRecSicAlphaCode")->m_data.m_str->m_cstr, 5);
	EXEC SQL 
         SELECT
              SIC_CODE_SHORT_TITLE
         INTO
             :SicRecSicCodeShortTitle   
         FROM
              =SICTITLE
         WHERE
              SIC_ALPHA_CODE = :SicRecSicAlphaCode 
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "SicRecSicAlphaCode='");
	stringbufferAppend(xml, SicRecSicAlphaCode);
	stringbufferAppend(xml, "' SicRecSicCodeShortTitle='");
	stringbufferAppend(xml, SicRecSicCodeShortTitle);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool VldnaicsSel82ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "ValidNaicsCodeRecNaicsCode")
		&& requestGetItem(req, "ValidNaicsCodeRecNaicsCode")->m_type == 'L'
	;
}

static void VldnaicsSel82(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 ValidNaicsCodeRecNaicsCode = requestGetItem(req, "ValidNaicsCodeRecNaicsCode")->m_data.m_long;
	char ValidNaicsCodeRecNaicsCodeDesc[141];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
              NAICS_CODE_DESC
         INTO
             :ValidNaicsCodeRecNaicsCodeDesc        
         FROM
              =VLDNAICS
         WHERE
              NAICS_CODE  = :ValidNaicsCodeRecNaicsCode    
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "ValidNaicsCodeRecNaicsCode='");
	stringbufferAppendLong(xml, ValidNaicsCodeRecNaicsCode);
	stringbufferAppend(xml, "' ValidNaicsCodeRecNaicsCodeDesc='");
	stringbufferAppend(xml, ValidNaicsCodeRecNaicsCodeDesc);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool PermitPermitapSel83ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
		&& requestHasItem(req, "WsCurrentDateAreaWsCurrentDate")
		&& requestGetItem(req, "WsCurrentDateAreaWsCurrentDate")->m_type == 'L'
	;
}

static void PermitPermitapSel83(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 WsCurrentDateAreaWsCurrentDate = requestGetItem(req, "WsCurrentDateAreaWsCurrentDate")->m_data.m_long;
	int32 WsSqlWorkFieldsWsPermitCount = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
            COUNT (*)  INTO  :WsSqlWorkFieldsWsPermitCount
          FROM
             =PERMIT  PMT
            ,=PERMITAP PMTAP
          WHERE
             PMT.PERMITAPP_ID = PMTAP.PERMITAPP_ID
             AND
              TRA_ID = :WsSqlWorkFieldsSqlTraId
             AND
                PERMIT_EFCTV_DATE <= :WsCurrentDateAreaWsCurrentDate
             AND
                PERMIT_EXPRN_DATE >= :WsCurrentDateAreaWsCurrentDate
             AND
                PERMIT_REVOCATION_DATE = 0

         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' WsCurrentDateAreaWsCurrentDate='");
	stringbufferAppendLong(xml, WsCurrentDateAreaWsCurrentDate);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsPermitCount='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsPermitCount);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool ActfileSel84ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraActivityRecTraActivityCode")
		&& requestGetItem(req, "TraActivityRecTraActivityCode")->m_type == 'L'
	;
}

static void ActfileSel84(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraActivityRecTraActivityCode = requestGetItem(req, "TraActivityRecTraActivityCode")->m_data.m_long;
	char CiActivityRecCiActivityDesc[19];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              CI_ACTIVITY_DESC
          INTO
              :CiActivityRecCiActivityDesc 
          FROM
              =ACTFILE
          WHERE
              CI_ACTIVITY_CODE = :TraActivityRecTraActivityCode 
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraActivityRecTraActivityCode='");
	stringbufferAppendLong(xml, TraActivityRecTraActivityCode);
	stringbufferAppend(xml, "' CiActivityRecCiActivityDesc='");
	stringbufferAppend(xml, CiActivityRecCiActivityDesc);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TraeftSel85ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void TraeftSel85(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 TraEftMailingRecTraEftPymtCode = 0;
	int64 TraEftMailingRecTraEftVlntryParticInd = 0;
	int64 TraEftMailingRecTraEftAuthDate = 0;
	int64 TraEftMailingRecTraEftRfndCode = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
               TRA_EFT_PYMT_CODE
              ,TRA_EFT_VLNTRY_PARTIC_IND
              ,TRA_EFT_AUTH_DATE
              ,TRA_EFT_RFND_CODE
          INTO
               :TraEftMailingRecTraEftPymtCode             
              ,:TraEftMailingRecTraEftVlntryParticInd     
              ,:TraEftMailingRecTraEftAuthDate             
              ,:TraEftMailingRecTraEftRfndCode             
          FROM
              =TRAEFT  TRAEF
          WHERE
              TRAEF.TRA_ID = :CiTraDetailMsgTraId 
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' TraEftMailingRecTraEftPymtCode='");
	stringbufferAppendLong(xml, TraEftMailingRecTraEftPymtCode);
	stringbufferAppend(xml, "' TraEftMailingRecTraEftVlntryParticInd='");
	stringbufferAppendLong(xml, TraEftMailingRecTraEftVlntryParticInd);
	stringbufferAppend(xml, "' TraEftMailingRecTraEftAuthDate='");
	stringbufferAppendLong(xml, TraEftMailingRecTraEftAuthDate);
	stringbufferAppend(xml, "' TraEftMailingRecTraEftRfndCode='");
	stringbufferAppendLong(xml, TraEftMailingRecTraEftRfndCode);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TranotesSel86ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_type == 'I'
	;
}

static void TranotesSel86(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int32 WsSqlWorkFieldsSqlNotepadSequenceNum = requestGetItem(req, "WsSqlWorkFieldsSqlNotepadSequenceNum")->m_data.m_int;
	int64 TraNotepadRecTraNotepadSequenceNum = 0;
	char TraNotepadRecTraNotepadText[79];
	int64 TraNotepadRecTraNotepadActivityUserId = 0;
	int64 TraNotepadRecTraNotepadActivityDate = 0;
	int64 TraNotepadRecTraNotepadActivityUnitCode = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             TRA_NOTEPAD_SEQUENCE_NUM,
             TRA_NOTEPAD_TEXT,
             TRA_NOTEPAD_ACTIVITY_USER_ID,
             TRA_NOTEPAD_ACTIVITY_DATE,
             TRA_NOTEPAD_ACTIVITY_UNIT_CODE
         INTO
             :TraNotepadRecTraNotepadSequenceNum       ,
             :TraNotepadRecTraNotepadText               ,
             :TraNotepadRecTraNotepadActivityUserId   ,
             :TraNotepadRecTraNotepadActivityDate      ,
             :TraNotepadRecTraNotepadActivityUnitCode 
         FROM
             =TRANOTES
         WHERE
             TRA_ID = :CiTraDetailMsgTraId  
             AND
             TRA_NOTEPAD_SEQUENCE_NUM = :WsSqlWorkFieldsSqlNotepadSequenceNum
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsSqlNotepadSequenceNum='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlNotepadSequenceNum);
	stringbufferAppend(xml, "' TraNotepadRecTraNotepadSequenceNum='");
	stringbufferAppendLong(xml, TraNotepadRecTraNotepadSequenceNum);
	stringbufferAppend(xml, "' TraNotepadRecTraNotepadText='");
	stringbufferAppend(xml, TraNotepadRecTraNotepadText);
	stringbufferAppend(xml, "' TraNotepadRecTraNotepadActivityUserId='");
	stringbufferAppendLong(xml, TraNotepadRecTraNotepadActivityUserId);
	stringbufferAppend(xml, "' TraNotepadRecTraNotepadActivityDate='");
	stringbufferAppendLong(xml, TraNotepadRecTraNotepadActivityDate);
	stringbufferAppend(xml, "' TraNotepadRecTraNotepadActivityUnitCode='");
	stringbufferAppendLong(xml, TraNotepadRecTraNotepadActivityUnitCode);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool AuditsfaSel87ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsWsSqlTraAuditToSfaNum")
		&& requestGetItem(req, "WsSqlWorkFieldsWsSqlTraAuditToSfaNum")->m_type == 'L'
	;
}

static void AuditsfaSel87(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 WsSqlWorkFieldsWsSqlTraAuditToSfaNum = requestGetItem(req, "WsSqlWorkFieldsWsSqlTraAuditToSfaNum")->m_data.m_long;
	int64 AsfaRecAsfaNum = 0;
	char AsfaRecAsfaEmployeeName[46];

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             ASFA_NUM
           , ASFA_EMPLOYEE_NAME
         INTO
             :AsfaRecAsfaNum            
           , :AsfaRecAsfaEmployeeName  
         FROM
             =AUDITSFA
         WHERE
             ASFA_NUM = :WsSqlWorkFieldsWsSqlTraAuditToSfaNum
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsWsSqlTraAuditToSfaNum='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsWsSqlTraAuditToSfaNum);
	stringbufferAppend(xml, "' AsfaRecAsfaNum='");
	stringbufferAppendLong(xml, AsfaRecAsfaNum);
	stringbufferAppend(xml, "' AsfaRecAsfaEmployeeName='");
	stringbufferAppend(xml, AsfaRecAsfaEmployeeName);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TragrpSel88ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsWsSqlTraSpecialGroupCode")
		&& requestGetItem(req, "WsSqlWorkFieldsWsSqlTraSpecialGroupCode")->m_type == 'I'
	;
}

static void TragrpSel88(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int32 WsSqlWorkFieldsWsSqlTraSpecialGroupCode = requestGetItem(req, "WsSqlWorkFieldsWsSqlTraSpecialGroupCode")->m_data.m_int;
	int64 TraSpecialGroupRecTraSpecialGroupCode = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             TRA_SPECIAL_GROUP_CODE
         INTO
             :TraSpecialGroupRecTraSpecialGroupCode  
         FROM
             =TRAGRP  GRP
         WHERE
             GRP.TRA_ID = :CiTraDetailMsgTraId 
          AND
             GRP.TRA_SPECIAL_GROUP_CODE = :WsSqlWorkFieldsWsSqlTraSpecialGroupCode
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsSqlTraSpecialGroupCode='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsSqlTraSpecialGroupCode);
	stringbufferAppend(xml, "' TraSpecialGroupRecTraSpecialGroupCode='");
	stringbufferAppendLong(xml, TraSpecialGroupRecTraSpecialGroupCode);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TracrSel89ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
		&& requestHasItem(req, "WsHighValueDate")
		&& requestGetItem(req, "WsHighValueDate")->m_type == 'S'
	;
}

static void TracrSel89(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	char WsHighValueDate[27];
	double TracrRecTraCreditProgEfctvSqldts = 0;
	double TracrRecTraCreditProgExpirSqldts = 0;

	EXEC SQL END DECLARE SECTION;

	StrCpyLen(WsHighValueDate, requestGetItem(req, "WsHighValueDate")->m_data.m_str->m_cstr, 27);
	
	EXEC SQL 
         SELECT
               TRA_CREDIT_PROG_EFCTV_SQLDTS,
               TRA_CREDIT_PROG_EXPIR_SQLDTS
         INTO
               :TracrRecTraCreditProgEfctvSqldts  TYPE AS TIMESTAMP,
               :TracrRecTraCreditProgExpirSqldts  TYPE AS TIMESTAMP
         FROM
             =TRACR
         WHERE
             TRA_ID = :CiTraDetailMsgTraId 
           AND
             TRA_CREDIT_PROG_CODE = 0850
           AND
             TRA_CREDIT_PROG_EXPIR_SQLDTS = :WsHighValueDate  TYPE AS TIMESTAMP
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsHighValueDate='");
	stringbufferAppend(xml, WsHighValueDate);
	stringbufferAppend(xml, "' TracrRecTraCreditProgEfctvSqldts='");
	stringbufferAppendReal(xml, TracrRecTraCreditProgEfctvSqldts);
	stringbufferAppend(xml, "' TracrRecTraCreditProgExpirSqldts='");
	stringbufferAppendReal(xml, TracrRecTraCreditProgExpirSqldts);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool AudasgndSel90ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void AudasgndSel90(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 WsAudasgndConsultInd = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             AUDASGND_CONSULT_IND
         INTO
             :WsAudasgndConsultInd
         FROM
             =AUDASGND
         WHERE
             TRA_ID = :CiTraDetailMsgTraId 
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsAudasgndConsultInd='");
	stringbufferAppendLong(xml, WsAudasgndConsultInd);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool OrfileSel91ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "TraRecTraId")
		&& requestGetItem(req, "TraRecTraId")->m_type == 'L'
	;
}

static void OrfileSel91(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int64 TraRecTraId = requestGetItem(req, "TraRecTraId")->m_data.m_long;
	int64 OrRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
               TRA_ID
         INTO
               :OrRecTraId 
         FROM
               =ORFILE
         WHERE
               TRA_ID = :TraRecTraId 
          AND
               OR_REFERRED_DATE > 0
         FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "TraRecTraId='");
	stringbufferAppendLong(xml, TraRecTraId);
	stringbufferAppend(xml, "' OrRecTraId='");
	stringbufferAppendLong(xml, OrRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool EftexmptSel92ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void EftexmptSel92(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 TraEftexmptRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             TRA_ID
         INTO
             :TraEftexmptRecTraId 
         FROM
             =EFTEXMPT
         WHERE
             TRA_ID = :CiTraDetailMsgTraId 
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' TraEftexmptRecTraId='");
	stringbufferAppendLong(xml, TraEftexmptRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool EppastatSel93ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "CiTraDetailMsgTraId")
		&& requestGetItem(req, "CiTraDetailMsgTraId")->m_type == 'I'
	;
}

static void EppastatSel93(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 CiTraDetailMsgTraId = requestGetItem(req, "CiTraDetailMsgTraId")->m_data.m_int;
	int64 WsSqlWorkFieldsWsEppastatCount = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
       SELECT
         COUNT(*)
       INTO
         :WsSqlWorkFieldsWsEppastatCount
       FROM
         =EPPASTAT A
       WHERE
             TRA_ID = :CiTraDetailMsgTraId 
       AND
         EPPASTAT_STATUS_LIT IN ("ACTIVE","PENDING")
       AND
         EPPASTAT_STATUS_SQLDTS = (SELECT
                                     MAX(EPPASTAT_STATUS_SQLDTS)
                                   FROM
                                     =EPPASTAT B
                                   WHERE
                                     A.TRA_ID = B.TRA_ID
                                   AND A.INV_CTR = B.INV_CTR
                                   FOR BROWSE ACCESS)
       FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "CiTraDetailMsgTraId='");
	stringbufferAppendInt(xml, CiTraDetailMsgTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsEppastatCount='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsWsEppastatCount);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TraindSel94ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
		&& requestHasItem(req, "WsSqlWorkFieldsSqlTrindtypTypeCode")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTrindtypTypeCode")->m_type == 'L'
	;
}

static void TraindSel94(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 WsSqlWorkFieldsSqlTrindtypTypeCode = requestGetItem(req, "WsSqlWorkFieldsSqlTrindtypTypeCode")->m_data.m_long;
	int64 TraindRecTraId = 0;
	int64 TraindRecTrindtypTypeCode = 0;
	int64 TraindRecTraindValueNum = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
        SELECT
             TRA_ID,
             TRINDTYP_TYPE_CODE,
             TRAIND_VALUE_NUM
        INTO
          :TraindRecTraId                     
         ,:TraindRecTrindtypTypeCode         
         ,:TraindRecTraindValueNum           
        FROM
             =TRAIND
        WHERE
             TRA_ID = :WsSqlWorkFieldsSqlTraId
          AND
             TRINDTYP_TYPE_CODE = :WsSqlWorkFieldsSqlTrindtypTypeCode
          AND
             TRAIND_VALUE_NUM  = 1
        FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsSqlTrindtypTypeCode='");
	stringbufferAppendLong(xml, WsSqlWorkFieldsSqlTrindtypTypeCode);
	stringbufferAppend(xml, "' TraindRecTraId='");
	stringbufferAppendLong(xml, TraindRecTraId);
	stringbufferAppend(xml, "' TraindRecTrindtypTypeCode='");
	stringbufferAppendLong(xml, TraindRecTrindtypTypeCode);
	stringbufferAppend(xml, "' TraindRecTraindValueNum='");
	stringbufferAppendLong(xml, TraindRecTraindValueNum);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool FrpexmptSel95ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void FrpexmptSel95(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 TraCertRecTraId = 0;
	int64 TraCertRecTraFrpCertExpiredDate = 0;
	int64 TraCertRecTraFrpCertRevokedInd = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
             TRA_ID,
             TRA_FRP_CERT_EXPIRED_DATE,
             TRA_FRP_CERT_REVOKED_IND
         INTO
             :TraCertRecTraId                     ,
             :TraCertRecTraFrpCertExpiredDate  ,
             :TraCertRecTraFrpCertRevokedInd  
         FROM
             =FRPEXMPT
         WHERE
             TRA_ID = :WsSqlWorkFieldsSqlTraId
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' TraCertRecTraId='");
	stringbufferAppendLong(xml, TraCertRecTraId);
	stringbufferAppend(xml, "' TraCertRecTraFrpCertExpiredDate='");
	stringbufferAppendLong(xml, TraCertRecTraFrpCertExpiredDate);
	stringbufferAppend(xml, "' TraCertRecTraFrpCertRevokedInd='");
	stringbufferAppendLong(xml, TraCertRecTraFrpCertRevokedInd);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool LneexmptSel96ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void LneexmptSel96(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 TraLneRecTraId = 0;
	char TraLneRecTraLneCertId[13];
	int64 TraLneRecTraLneCertEffectiveDate = 0;
	int64 TraLneRecTraLneCertExpiredDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              TRA_ID,
              TRA_LNE_CERT_ID,
              TRA_LNE_CERT_EFFECTIVE_DATE,
              TRA_LNE_CERT_EXPIRED_DATE
          INTO
              :TraLneRecTraId                       ,
              :TraLneRecTraLneCertId              ,
              :TraLneRecTraLneCertEffectiveDate  ,
              :TraLneRecTraLneCertExpiredDate    
          FROM
             =LNEEXMPT
          WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' TraLneRecTraId='");
	stringbufferAppendLong(xml, TraLneRecTraId);
	stringbufferAppend(xml, "' TraLneRecTraLneCertId='");
	stringbufferAppend(xml, TraLneRecTraLneCertId);
	stringbufferAppend(xml, "' TraLneRecTraLneCertEffectiveDate='");
	stringbufferAppendLong(xml, TraLneRecTraLneCertEffectiveDate);
	stringbufferAppend(xml, "' TraLneRecTraLneCertExpiredDate='");
	stringbufferAppendLong(xml, TraLneRecTraLneCertExpiredDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool AdeexmptSel97ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void AdeexmptSel97(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 TraAdeRecTraId = 0;
	char TraAdeRecTraAdeCertId[13];
	int64 TraAdeRecTraAdeCertEffectiveDate = 0;
	int64 TraAdeRecTraAdeCertExpiredDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              TRA_ID,
              TRA_ADE_CERT_ID,
              TRA_ADE_CERT_EFFECTIVE_DATE,
              TRA_ADE_CERT_EXPIRED_DATE
          INTO
              :TraAdeRecTraId                       ,
              :TraAdeRecTraAdeCertId              ,
              :TraAdeRecTraAdeCertEffectiveDate  ,
              :TraAdeRecTraAdeCertExpiredDate    
          FROM
             =ADEEXMPT
          WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' TraAdeRecTraId='");
	stringbufferAppendLong(xml, TraAdeRecTraId);
	stringbufferAppend(xml, "' TraAdeRecTraAdeCertId='");
	stringbufferAppend(xml, TraAdeRecTraAdeCertId);
	stringbufferAppend(xml, "' TraAdeRecTraAdeCertEffectiveDate='");
	stringbufferAppendLong(xml, TraAdeRecTraAdeCertEffectiveDate);
	stringbufferAppend(xml, "' TraAdeRecTraAdeCertExpiredDate='");
	stringbufferAppendLong(xml, TraAdeRecTraAdeCertExpiredDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TraelfSel98ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void TraelfSel98(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 TraElfRecTraId = 0;
	int64 TraElfRecTraelfEfileRequiredYrMth = 0;
	int64 TraElfRecTraelfMndtyEfileEpayYrMth = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              TRA_ID,
              TRAELF_EFILE_REQUIRED_YR_MTH,
              TRAELF_MNDTY_EFILE_EPAY_YR_MTH
          INTO


              :TraElfRecTraId                           ,
              :TraElfRecTraelfEfileRequiredYrMth     ,
              :TraElfRecTraelfMndtyEfileEpayYrMth   

          FROM
             =TRAELF
          WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId

          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' TraElfRecTraId='");
	stringbufferAppendLong(xml, TraElfRecTraId);
	stringbufferAppend(xml, "' TraElfRecTraelfEfileRequiredYrMth='");
	stringbufferAppendLong(xml, TraElfRecTraelfEfileRequiredYrMth);
	stringbufferAppend(xml, "' TraElfRecTraelfMndtyEfileEpayYrMth='");
	stringbufferAppendLong(xml, TraElfRecTraelfMndtyEfileEpayYrMth);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TradateSel99ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void TradateSel99(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 TraDateRecTraId = 0;
	int64 TraDateRecTradateValueDate = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              TRA_ID
             ,TRADATE_VALUE_DATE

          INTO
              :TraDateRecTraId                       
             ,:TraDateRecTradateValueDate           

        FROM   =TRADATE

          WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId
             AND
              TRDTTYP_TYPE_CODE   = 16

          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' TraDateRecTraId='");
	stringbufferAppendLong(xml, TraDateRecTraId);
	stringbufferAppend(xml, "' TraDateRecTradateValueDate='");
	stringbufferAppendLong(xml, TraDateRecTradateValueDate);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TrafileSel100ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsWsSqlLeId")
		&& requestGetItem(req, "WsSqlWorkFieldsWsSqlLeId")->m_type == 'I'
	;
}

static void TrafileSel100(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsWsSqlLeId = requestGetItem(req, "WsSqlWorkFieldsWsSqlLeId")->m_data.m_int;
	int32 WsSqlWorkFieldsWsSqlTraCount = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
          SELECT
              COUNT (*)
          INTO
              :WsSqlWorkFieldsWsSqlTraCount
          FROM
              =TRAFILE
          WHERE
              LE_ID = :WsSqlWorkFieldsWsSqlLeId
           AND
              TRA_CLOSE_DATE = 0
          FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsWsSqlLeId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsSqlLeId);
	stringbufferAppend(xml, "' WsSqlWorkFieldsWsSqlTraCount='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsWsSqlTraCount);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool CspSel101ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
	;
}

static void CspSel101(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int64 CspRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
                TRA_ID
         INTO
              :CspRecTraId 
         FROM =CSP
         WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId

       FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' CspRecTraId='");
	stringbufferAppendLong(xml, CspRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool SstpSel102ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
		&& requestHasItem(req, "WsCurrentDateAreaWsCurrentDateComp")
		&& requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_type == 'I'
	;
}

static void SstpSel102(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int32 WsCurrentDateAreaWsCurrentDateComp = requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_data.m_int;
	int64 SstpRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
                TRA_ID
         INTO
              :SstpRecTraId 
         FROM =SSTP SSTP
         WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId
              AND
              SSTP_RELIEF_END_DATE >=  :WsCurrentDateAreaWsCurrentDateComp
       FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' WsCurrentDateAreaWsCurrentDateComp='");
	stringbufferAppendInt(xml, WsCurrentDateAreaWsCurrentDateComp);
	stringbufferAppend(xml, "' SstpRecTraId='");
	stringbufferAppendLong(xml, SstpRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

static bool TracsprlSel103ValidateRequest(request *req, stringbuffer* xml)
{
	return
		requestHasItem(req, "WsSqlWorkFieldsSqlTraId")
		&& requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_type == 'I'
		&& requestHasItem(req, "WsCurrentDateAreaWsCurrentDateComp")
		&& requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_type == 'I'
	;
}

static void TracsprlSel103(request *req, stringbuffer* xml)
{
#ifdef __TANDEM
	EXEC SQL BEGIN DECLARE SECTION;

	short sqlcode = 0;
	int32 WsSqlWorkFieldsSqlTraId = requestGetItem(req, "WsSqlWorkFieldsSqlTraId")->m_data.m_int;
	int32 WsCurrentDateAreaWsCurrentDateComp = requestGetItem(req, "WsCurrentDateAreaWsCurrentDateComp")->m_data.m_int;
	int64 SstpRecTraId = 0;

	EXEC SQL END DECLARE SECTION;

	EXEC SQL 
         SELECT
                TRA_ID
         INTO
              :SstpRecTraId 
         FROM
              =TRACSPRL RL
         WHERE
              TRA_ID = :WsSqlWorkFieldsSqlTraId
              AND
               TRACSPRL_RELIEF_END_DATE >=  :WsCurrentDateAreaWsCurrentDateComp
       FOR BROWSE ACCESS
	;
	if (sqlcode != 0)
	{
		_writeErrorCode(xml, "SQL ERROR ", sqlcode);
		return;
	}

	stringbufferAppend(xml, "<row ");
	stringbufferAppend(xml, "WsSqlWorkFieldsSqlTraId='");
	stringbufferAppendInt(xml, WsSqlWorkFieldsSqlTraId);
	stringbufferAppend(xml, "' WsCurrentDateAreaWsCurrentDateComp='");
	stringbufferAppendInt(xml, WsCurrentDateAreaWsCurrentDateComp);
	stringbufferAppend(xml, "' SstpRecTraId='");
	stringbufferAppendLong(xml, SstpRecTraId);
	stringbufferAppend(xml, "' />\r\n");
#else
#endif
}

void serviceRequest2(request *req, stringbuffer* xml)
{
	if (stringIsEqual(req->m_name, "ReportingType", true))
	{
		if (! ReportingTypeValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		ReportingType(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraNotepadPgFwdCursor", true))
	{
		if (! TraNotepadPgFwdCursorValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraNotepadPgFwdCursor(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraNotepadPgBwdCursor", true))
	{
		if (! TraNotepadPgBwdCursorValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraNotepadPgBwdCursor(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraNpHighSeqNumCursor", true))
	{
		if (! TraNpHighSeqNumCursorValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraNpHighSeqNumCursor(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraActivityCursor", true))
	{
		if (! TraActivityCursorValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraActivityCursor(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SicCodeCursor", true))
	{
		if (! SicCodeCursorValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SicCodeCursor(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TrafileSeasnlSel64", true))
	{
		if (! TrafileSeasnlSel64ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TrafileSeasnlSel64(req, xml);
	}
	else if (stringIsEqual(req->m_name, "CigTbcoLic", true))
	{
		if (! CigTbcoLicValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		CigTbcoLic(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TradateSel66", true))
	{
		if (! TradateSel66ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TradateSel66(req, xml);
	}
	else if (stringIsEqual(req->m_name, "LefileSel67", true))
	{
		if (! LefileSel67ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		LefileSel67(req, xml);
	}
	else if (stringIsEqual(req->m_name, "AudasgndSel68", true))
	{
		if (! AudasgndSel68ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		AudasgndSel68(req, xml);
	}
	else if (stringIsEqual(req->m_name, "DoruservSel69", true))
	{
		if (! DoruservSel69ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		DoruservSel69(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SeasnlSel70", true))
	{
		if (! SeasnlSel70ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SeasnlSel70(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TrarptgSel71", true))
	{
		if (! TrarptgSel71ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TrarptgSel71(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TrarptgSel72", true))
	{
		if (! TrarptgSel72ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TrarptgSel72(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TradateSel73", true))
	{
		if (! TradateSel73ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TradateSel73(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SstpSel74", true))
	{
		if (! SstpSel74ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SstpSel74(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SstpSel75", true))
	{
		if (! SstpSel75ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SstpSel75(req, xml);
	}
	else if (stringIsEqual(req->m_name, "LefileSel76", true))
	{
		if (! LefileSel76ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		LefileSel76(req, xml);
	}
	else if (stringIsEqual(req->m_name, "LetypeSel77", true))
	{
		if (! LetypeSel77ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		LetypeSel77(req, xml);
	}
	else if (stringIsEqual(req->m_name, "UncollSel78", true))
	{
		if (! UncollSel78ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		UncollSel78(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraemailSel79", true))
	{
		if (! TraemailSel79ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraemailSel79(req, xml);
	}
	else if (stringIsEqual(req->m_name, "BrcrfileSel80", true))
	{
		if (! BrcrfileSel80ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		BrcrfileSel80(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SictitleSel81", true))
	{
		if (! SictitleSel81ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SictitleSel81(req, xml);
	}
	else if (stringIsEqual(req->m_name, "VldnaicsSel82", true))
	{
		if (! VldnaicsSel82ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		VldnaicsSel82(req, xml);
	}
	else if (stringIsEqual(req->m_name, "PermitPermitapSel83", true))
	{
		if (! PermitPermitapSel83ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		PermitPermitapSel83(req, xml);
	}
	else if (stringIsEqual(req->m_name, "ActfileSel84", true))
	{
		if (! ActfileSel84ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		ActfileSel84(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraeftSel85", true))
	{
		if (! TraeftSel85ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraeftSel85(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TranotesSel86", true))
	{
		if (! TranotesSel86ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TranotesSel86(req, xml);
	}
	else if (stringIsEqual(req->m_name, "AuditsfaSel87", true))
	{
		if (! AuditsfaSel87ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		AuditsfaSel87(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TragrpSel88", true))
	{
		if (! TragrpSel88ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TragrpSel88(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TracrSel89", true))
	{
		if (! TracrSel89ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TracrSel89(req, xml);
	}
	else if (stringIsEqual(req->m_name, "AudasgndSel90", true))
	{
		if (! AudasgndSel90ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		AudasgndSel90(req, xml);
	}
	else if (stringIsEqual(req->m_name, "OrfileSel91", true))
	{
		if (! OrfileSel91ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		OrfileSel91(req, xml);
	}
	else if (stringIsEqual(req->m_name, "EftexmptSel92", true))
	{
		if (! EftexmptSel92ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		EftexmptSel92(req, xml);
	}
	else if (stringIsEqual(req->m_name, "EppastatSel93", true))
	{
		if (! EppastatSel93ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		EppastatSel93(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraindSel94", true))
	{
		if (! TraindSel94ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraindSel94(req, xml);
	}
	else if (stringIsEqual(req->m_name, "FrpexmptSel95", true))
	{
		if (! FrpexmptSel95ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		FrpexmptSel95(req, xml);
	}
	else if (stringIsEqual(req->m_name, "LneexmptSel96", true))
	{
		if (! LneexmptSel96ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		LneexmptSel96(req, xml);
	}
	else if (stringIsEqual(req->m_name, "AdeexmptSel97", true))
	{
		if (! AdeexmptSel97ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		AdeexmptSel97(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TraelfSel98", true))
	{
		if (! TraelfSel98ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TraelfSel98(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TradateSel99", true))
	{
		if (! TradateSel99ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TradateSel99(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TrafileSel100", true))
	{
		if (! TrafileSel100ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TrafileSel100(req, xml);
	}
	else if (stringIsEqual(req->m_name, "CspSel101", true))
	{
		if (! CspSel101ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		CspSel101(req, xml);
	}
	else if (stringIsEqual(req->m_name, "SstpSel102", true))
	{
		if (! SstpSel102ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		SstpSel102(req, xml);
	}
	else if (stringIsEqual(req->m_name, "TracsprlSel103", true))
	{
		if (! TracsprlSel103ValidateRequest(req, xml))
		{
			_writeError(xml, "invalid parameter(s)", "");
			return;
		}
		TracsprlSel103(req, xml);
	}
	else
	{
		_writeError(xml, "unknown procedure ", req->m_name->m_cstr);
	}
}

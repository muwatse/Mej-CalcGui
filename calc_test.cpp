/*******************************************************************************
 * Method      : DoAnd
 * Description : Bitwise AND the value of one data item with another and place
 *               result in another location
 * And source data 1, source data 2, target data, format
 ******************************************************************************/
TStatus::ETestStatus RTSManager::DoAnd(DynamicArray<String> &params)
{
    std::map<String, int> BitLength;

    // initialise bitLength MAP
    BitLength.clear();
    BitLength[Script_Constants::sHEX8.UpperCase()]   = 8;
    BitLength[Script_Constants::sHEX16.UpperCase()]  = 16;
    BitLength[Script_Constants::sHEX32.UpperCase()]  = 32;
    BitLength[Script_Constants::sUBYTE.UpperCase()]  = 8;
    BitLength[Script_Constants::sSBYTE.UpperCase()]  = 8;
    BitLength[Script_Constants::sSWORD.UpperCase()]  = 16;
    BitLength[Script_Constants::sUWORD.UpperCase()]  = 16;

    TStatus::ETestStatus OverallResult = TStatus::TESTFAIL;

    try
    {
        int pos1 = StrToInt(params[0]);
        int pos2 = StrToInt(params[1]);
        int dest = StrToInt(params[2]);

        if (params[3].Length() > 0)
        {
            union lint
            {
                float    fli;
                int32_t  sli;
                uint32_t uli;
                int16_t  si[2];
                uint16_t ui[2];
                uint8_t  uc[4];
                int8_t   sc[4];
            };

            lint parm1;
            lint parm2;
            lint result;

            parm1.uli = 0;
            parm2.uli = 0;
            result.uli = 0;

            // Make sure the global data buffer is big enough
            if (RTSForm->GetDatabase()->m_ucUUTResults.Length < dest + sizeof(result))
            {
                RTSForm->GetDatabase()->m_ucUUTResults.Length =
                    dest + sizeof(result);
            }

            // Number of bytes for this format (0 for unsupported formats,
            // e.g. Single, which leaves the result as TESTFAIL)
            const int numBytes = BitLength[params[3].UpperCase()] / 8;

            if (numBytes > 0)
            {
                const bool isHex =
                    Pos(Script_Constants::sHexFormat, params[3].UpperCase()) > 0;

                // Read both operands, one byte at a time
                for (int i = 0; i < numBytes; i++)
                {
                    if (isHex)
                    {
                        parm1.uc[i] =
                            (uint8_t)HexStringToUnsignedInt(
                                RTSForm->GetDatabase()->m_ucUUTResults[pos1 + i]);

                        parm2.uc[i] =
                            (uint8_t)HexStringToUnsignedInt(
                                RTSForm->GetDatabase()->m_ucUUTResults[pos2 + i]);
                    }
                    else
                    {
                        parm1.uc[i] =
                            (uint8_t)StrToInt(
                                RTSForm->GetDatabase()->m_ucUUTResults[pos1 + i]);

                        parm2.uc[i] =
                            (uint8_t)StrToInt(
                                RTSForm->GetDatabase()->m_ucUUTResults[pos2 + i]);
                    }
                }

                // Unused upper bytes are zero, so one 32-bit AND covers
                // the 8, 16 and 32 bit cases
                result.uli = parm1.uli & parm2.uli;

                for (int i = 0; i < numBytes; i++)
                {
                    RTSForm->GetDatabase()->m_ucUUTResults[dest + i] =
                        result.uc[i];
                }

                OverallResult = TStatus::TESTPASS;
            }
        }

        if (OverallResult == TStatus::TESTPASS)
        {
            RTSForm->GetDatabase()->SetDataReceivedStatus(
                RTSCommsTypes::RECEIVED_DATA_GOOD);
        }
    }
    catch (Exception &e)
    {
        LOGERROR(TEXT("Exception caught: During DoAnd"));
        RTSForm->GetErrorHandler()->WriteError(GTM_INVALID_COMMAND_DATA);
        RTSForm->RTSDisplayOperatorPrompt(e.Message);
    }

    return OverallResult;
}

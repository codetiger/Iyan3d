package com.smackall.animator.Helper;

import android.content.Context;


public class CheckLicense {

    protected static String CERTIFICATE = "A4";
    protected static String CERTIFICATE1 = "A5";
    protected static String CERTIFICATE2 = "6C";
    protected static String CERTIFICATE3 = "78";
    protected static String CERTIFICATE4 = "8E";
    protected static String CERTIFICATE5 = "8E";
    protected static String CERTIFICATE6 = "FE";
    protected static String CERTIFICATE7 = "67";
    protected static String CERTIFICATE8 = "C6";
    protected static String CERTIFICATE9 = "9C";
    protected static String CERTIFICATE10 = "EB";
    protected static String CERTIFICATE11 = "06";
    protected static String CERTIFICATE12 = "70";
    protected static String CERTIFICATE13 = "3E";
    protected static String CERTIFICATE14 = "D9";
    protected static String CERTIFICATE15 = "FC";
    protected static String CERTIFICATE16 = "6C";
    protected static String CERTIFICATE17 = "58";
    protected static String CERTIFICATE18 = "4D";
    protected static String CERTIFICATE19 = "87";

    public static boolean checkLicense(Context mContext) {
        String CERTIFICATE_ORG = CERTIFICATE + ":" + CERTIFICATE1 + ":" + CERTIFICATE2 + ":" + CERTIFICATE3 + ":" + CERTIFICATE4 + ":" + CERTIFICATE5 + ":" + CERTIFICATE6 + ":"
                + CERTIFICATE7 + ":" + CERTIFICATE8 + ":" + CERTIFICATE9 + ":" + CERTIFICATE10 + ":" + CERTIFICATE11 + ":" + CERTIFICATE12 + ":" + CERTIFICATE13 + ":" + CERTIFICATE14 + ":" + CERTIFICATE15 + ":"
                + CERTIFICATE16 + ":" + CERTIFICATE17 + ":" + CERTIFICATE18 + ":" + CERTIFICATE19;
        return CERTIFICATE_ORG.equals(CheckCertificate.getCertificateSHA1Fingerprint(mContext));
    }
}

